#include <string>
#include "jni.h"
#include "shadowhook.h"
#include <stack>
#include <array>
#include <unistd.h>
#include "faster/utils.h"
#include "faster/lock_free_queue.h"
#include "faster/logger_entry.h"
#include <sys/prctl.h>
#include <pthread.h>
#include "faster/tracer.h"
#include <time.h>


//PERFETTO_DEFINE_CATEGORIES(
//        perfetto::Category(TAG)
//                .SetDescription("Events about the all the java trace"));
//
//PERFETTO_TRACK_EVENT_STATIC_STORAGE();


static void initTracer();

class MyArtMethod {
public:
    typedef std::string(*PrettyMethodType)(MyArtMethod *thiz, bool with_signature);

    inline std::string PrettyMethod(MyArtMethod *thiz, bool with_signature) {
        if (thiz == nullptr)
            return "null";
        else if (PrettyMethodSym)
            return PrettyMethodSym(thiz, with_signature);
        else
            return "null sym";
    }

    typedef const char *(*GetMethodShortyType)(JNIEnv *env, jmethodID mid);

    const char *(*GetMethodShortySym)(JNIEnv *env, jmethodID mid);

    const char *GetMethodShorty(JNIEnv *env, jmethodID mid) {
        if (GetMethodShortySym)
            return GetMethodShortySym(env, mid);
        return nullptr;
    }

    static bool Init(void *handler) {
        initTracer();
        return !(MyArtMethod::PrettyMethodSym = reinterpret_cast<MyArtMethod::PrettyMethodType>(shadowhook_dlsym(
                handler, "_ZN3art9ArtMethod12PrettyMethodEPS0_b"))) &&
               !(MyArtMethod::PrettyMethodSym = reinterpret_cast<MyArtMethod::PrettyMethodType>(shadowhook_dlsym(
                       handler, "_ZN3art12PrettyMethodEPNS_9ArtMethodEb"))) &&
               !(MyArtMethod::PrettyMethodSym = reinterpret_cast<MyArtMethod::PrettyMethodType>(shadowhook_dlsym(
                       handler, "_ZN3art12PrettyMethodEPNS_6mirror9ArtMethodEb")));
    }

private:
    static std::string (*PrettyMethodSym)(MyArtMethod *thiz, bool with_signature);

};

static void initTracer() {
    InitTrace();
}

MyArtMethod::PrettyMethodType MyArtMethod::PrettyMethodSym = nullptr;

void (*SetJavaDebuggable)(bool debug);

void * (*beginSelection)(const char *sectionName);
void * (*endSelection)();

//void endSelection(void) {
//    TRACE_EVENT_END(TAG);
//}
//
//
//void beginSelection(const char *sectionName) {
//    TRACE_EVENT_BEGIN(TAG, perfetto::DynamicString{sectionName});
//}


void
method_before(MyArtMethod *artMethod, bool &trace, long &start, std::string &method, timeval &tv);

void method_after(bool trace, long start, const std::string &method, timeval &tv);

void initPerfetto();

typedef void *(*fp_ATrace_beginSection)(const char *sectionName);

typedef void *(*fp_ATrace_endSection)(void);

typedef void (*executeSwitchImplAsm_type)(void *, void *, void *);

struct shadowframet {
    void *link_;
    void *method_;
};

struct SwitchImplContext {
    void *self;
    void *accessor;
    shadowframet *shadow_frame;
};
void *executeSwitchImplAsm_orig = NULL;
void *executeSwitchImplAsm_stub = NULL;

void *executeMterpImpl_orig = NULL;
void *executeMterpImpl_stub = NULL;
void *executeNterpImpl_stub = NULL;

extern "C" {
   void *executeNterpImpl_orgi = NULL;
}


thread_local std::stack<std::string> words;
thread_local std::stack<bool> key;
static std::string filter_key = "com.wy";
static int filter_tid = -1;
static int filter_depth = 10;
static bool filter_debug = false;


inline long getNativeTimestamp() {
    struct timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec;
}

void
method_before(MyArtMethod *artMethod, bool &trace, long &start, std::string &method, timeval &tv) {
    trace = false;
    start = 0l;
    method = "";//    0 == gettimeofday(&tv, nullptr) && (tv.tv_sec * 1000 + tv.tv_usec / 1000) % 100 >= 0
    if (filter_tid < 0 || gettid() == filter_tid) {
        method = artMethod-> PrettyMethod(artMethod, false);
//        if (key.empty() && method.find(filter_key) != std::string::npos) {
//            key.push(true);
//        }
//        trace = (words.size() < filter_depth) && !key.empty();
        trace = true;
        if (true) {
            words.push(method.c_str());
            beginSelection(method.c_str());
            // get buff
            char* buff = new char[100];
            pthread_getname_np(pthread_self(), buff, 100);
            LogEntry logEntry{
                .pname = reinterpret_cast<const char*>(buff),
                .timestamp = getNativeTimestamp(),
                .methodName = method.c_str(),
                .type = TRACE_BEGIN
            };
//            LOGE("producerLoop Addr %p", &lock_free_ringbuffer);
            producerLoop<LogEntry>(lock_free_ringbuffer, logEntry);
//            LOGE("%s", method.c_str());
//            if (filter_debug && 0 == gettimeofday(&tv, nullptr)) {
//                start = tv.tv_sec * 1000 + tv.tv_usec / 1000;
//            }
        }
    }
}

void method_after(bool trace, long start, const std::string &method, timeval &tv) {
    if (trace) {
        long word_size = words.size();
//        if (word_size == 1 && !key.empty()) {
//            key.pop();
//        }
        if (/*filter_debug && 0 == gettimeofday(&tv, nullptr)*/false) {
            long cost = tv.tv_sec * 1000 + tv.tv_usec / 1000 - start;
            LOGE("%lu, %s %lu ms", word_size, method.c_str(), cost);
        }
        words.pop();
        endSelection();
    }
}


void executeSwitchImplAsm_proxy(SwitchImplContext *ctx, void *a, void *jvalue) {
    // do something
    MyArtMethod *artMethod = reinterpret_cast<MyArtMethod *>(ctx->shadow_frame->method_);
    bool trace;
    long start;
    std::string method;
    struct timeval tv;
    method_before(artMethod, trace, start, method, tv);
    ((executeSwitchImplAsm_type) executeSwitchImplAsm_orig)(ctx, a, jvalue);
    method_after(trace, start, method, tv);

}


typedef bool (*type_t2)(void *, void *, void *a, void *b);

typedef void (*type_t3)();

bool executeMterpImpl_proxy(void *thread, void *shadowframe, void *a, void *b) {
    struct shadowframet *shadowframet1 = (shadowframet *) (a);
    MyArtMethod *artMethod = reinterpret_cast<MyArtMethod *>(shadowframet1->method_);
//    LOGE("%s", artMethod->PrettyMethod(artMethod,false).c_str());
    bool trace;
    long start;
    std::string method;
    struct timeval tv;
    method_before(artMethod, trace, start, method, tv);
    bool res = ((type_t2) executeMterpImpl_orig)(thread, shadowframe, a, b);
    method_after(trace, start, method, tv);
    return res;
}


bool hook_success;

extern "C" void executeNTerpImpl_proxy();

void do_hook() {
    void *handler = shadowhook_dlopen("libart.so");
    if (handler == nullptr) {
        LOGE("hook error  dlopen libart failed");
        return;
    }
    // 初始化
    // initPerfetto();
    MyArtMethod::Init(handler);
    shadowhook_dlclose(handler);
    void *libandroid = shadowhook_dlopen("libandroid.so");
    if (libandroid == nullptr) {
        LOGE("hook error  dlopen libandroid failed");
        return;
    }
    beginSelection = reinterpret_cast<fp_ATrace_beginSection>(
            shadowhook_dlsym(libandroid, "ATrace_beginSection"));
    endSelection = reinterpret_cast<fp_ATrace_endSection>(
            shadowhook_dlsym(libandroid, "ATrace_endSection"));
    shadowhook_dlclose(libandroid);


    executeSwitchImplAsm_stub = shadowhook_hook_sym_name(
            "libart.so",
            "ExecuteSwitchImplAsm",
            (void *) executeSwitchImplAsm_proxy,
            (void **) &executeSwitchImplAsm_orig);
    executeMterpImpl_stub = shadowhook_hook_sym_name(
            "libart.so",
            "ExecuteMterpImpl",
            (void *) executeMterpImpl_proxy,
            (void **) &executeMterpImpl_orig);
//    executeNterpImpl_stub = shadowhook_hook_sym_name(
//            "libart.so",
//            "ExecuteNterpImpl",
//            (void *) executeNTerpImpl_proxy,
//            (void **) &executeNterpImpl_orgi);

    bool hookSucess = true;
    if (executeSwitchImplAsm_stub == NULL) {
        int err_num = shadowhook_get_errno();
        const char *err_msg = shadowhook_to_errmsg(err_num);
//        LOGE("executeSwitchImplAsm hook error %d - %s", err_num, err_msg);
        hookSucess = false;
    }
    if (executeMterpImpl_stub == NULL) {
        int err_num = shadowhook_get_errno();
        const char *err_msg = shadowhook_to_errmsg(err_num);
        LOGE("executeMterpImpl hook error %d - %s", err_num, err_msg);
        hookSucess = false;
    }
    if (hookSucess) {
        LOGE("hook success10");
        hook_success = true;
    }

}


// 初始化 Perfetto
// 初始化参数为当前 Process 内部kInProcessBackend
/**
 *  1) The in-process backend only records within the app itself.
  args.backends |= perfetto::kInProcessBackend;

   2) The system backend writes events into a system Perfetto daemon,
      allowing merging app and system events (e.g., ftrace) on the same
      timeline. Requires the Perfetto `traced` daemon to be running (e.g.,
      on Android Pie and newer).
  args.backends |= perfetto::kSystemBackend;
 */
//void initPerfetto() {
//    perfetto::TracingInitArgs args;
//    args.backends |= perfetto::kSystemBackend;
//    perfetto::Tracing::Initialize(args);
//    perfetto::TrackEvent::Register();
//
//    // Give a custom name for the traced process.
////    perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current();
////    perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();
////    desc.mutable_process()->set_process_name("Example");
////    perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);
//}

void *useNterp_orig = nullptr;
void *useNterp_stup = nullptr;

bool useNterp() {
    return false;
}

void **instance_;

template<typename T>
int findOffset(void *start, int regionStart, int regionEnd, T value) {

    if (NULL == start || regionEnd <= 0 || regionStart < 0) {
        return -1;
    }
    char *c_start = (char *) start;

    for (int i = regionStart; i < regionEnd; i += 4) {
        T *current_value = (T *) (c_start + i);
        if (value == *current_value) {
            return i;
        }
    }
    return -2;
}

extern "C" {

JNIEXPORT  void JNICALL
Java_com_wy_lib_wytrace_ArtMethodTrace_disableNterp(JNIEnv *env,
                                                    jclass clazz) {
    useNterp_stup = shadowhook_hook_sym_name(
            "libart.so",
            "_ZN3art11interpreter18CanRuntimeUseNterpEv",
            (void *) useNterp,
            (void **) &useNterp_orig);
    if (useNterp_stup != nullptr) {
        LOGE("disableNterp success ");
    }

}


JNIEXPORT  void JNICALL
Java_com_wy_lib_wytrace_ArtMethodTrace_methodHook(JNIEnv *env,
                                                  jclass clazz, jstring methodName, jint tid,
                                                  jint depth, jboolean debug) {
    filter_tid = tid;
    filter_depth = depth;
    filter_debug = debug;
    const char *c_method_name = env->GetStringUTFChars(methodName, nullptr);
    filter_key = std::string(c_method_name);
    do_hook();
    (env)->ReleaseStringUTFChars(methodName, c_method_name);

}


JNIEXPORT  void JNICALL
Java_com_wy_lib_wytrace_ArtMethodTrace_methodUnHook(JNIEnv *env, jclass clazz) {
    if (!hook_success) {
        return;
    }
    if (executeMterpImpl_stub != NULL) {
        int err1 = shadowhook_unhook(executeMterpImpl_stub);
        LOGE("unhook executeMterpImpl_stub %d ", err1);

    }
    if (executeSwitchImplAsm_stub != NULL) {
        int err2 = shadowhook_unhook(executeSwitchImplAsm_stub);
        LOGE("unhook executeMterpImpl_stub %d ", err2);
        hook_success = false;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_wy_lib_wytrace_ArtMethodTrace_foreInterpretor(JNIEnv *env, jclass clazz) {

    void *handler = shadowhook_dlopen("libart.so");
    // art::Runtime.instance_
    void **instance_ = static_cast<void **>(shadowhook_dlsym(handler,
                                                             "_ZN3art7Runtime9instance_E"));
    if (instance_ == nullptr) {
        LOGE("foreInterpretor instance_ hook error");
        return;
    }

    jobject
    (*DeoptimizeBootImage)(void *runtime) =reinterpret_cast<jobject (*)(void *)>((jobject (*)(
            void *runtime)) shadowhook_dlsym(handler,

                                             "_ZN3art7Runtime19DeoptimizeBootImageEv"));
    if (DeoptimizeBootImage == nullptr) {
        LOGE("error DeoptimizeBootImage");
    }
    LOGE("foreInterpretor success");

//    jobject
//    (*getSystemThreadGroup)(void *runtime) =(jobject (*)(void *runtime)) shadowhook_dlsym(handler,
//
//                                                                                          "_ZNK3art7Runtime20GetSystemThreadGroupEv");
//    // art::instrumentation::Instrumentation.UpdateEntrypointsForDebuggable
//    void
//    (*UpdateEntrypointsForDebuggable)(void *instrumentation) = (void (*)(void *i)) shadowhook_dlsym(
//            handler,
//            "_ZN3art15instrumentation15Instrumentation30UpdateEntrypointsForDebuggableEv");
//    if (getSystemThreadGroup == nullptr || UpdateEntrypointsForDebuggable == nullptr) {
//        LOGE("getSystemThreadGroup  failed ");
//        shadowhook_dlclose(handler);
//        return;
//    }
//    jobject thread_group = getSystemThreadGroup(*instance_);
//    int vm_offset = findOffset(*instance_, 0, 4000, thread_group);
//    if (vm_offset < 0) {
//        LOGE("vm_offset not found ");
//        shadowhook_dlclose(handler);
//        return;
//    }
//
//    void (*setRuntimeDebugState)(void *instance_, int r) =(void (*)(void *runtime,
//                                                                    int r)) shadowhook_dlsym(
//            handler, "_ZN3art7Runtime20SetRuntimeDebugStateENS0_17RuntimeDebugStateE");
//    if (setRuntimeDebugState != nullptr) {
//        setRuntimeDebugState(*instance_, 2);
//    }
//    LOGE("vm_offset %d", vm_offset);
//    void *instrumentation = reinterpret_cast<void *>(reinterpret_cast<char *>(*instance_) +
//                                                     vm_offset - 368 );
//
//    DeoptimizeBootImage(instrumentation);
//    LOGE("DeoptimizeBootImage, %p", DeoptimizeBootImage);
//
//    UpdateEntrypointsForDebuggable(instrumentation);
//    shadowhook_dlclose(handler);
//
//    LOGE("foreInterpretor success");

}


JNIEXPORT  void JNICALL
Java_com_wy_lib_wytrace_ArtMethodTrace_bootImageNterp(JNIEnv *env,
                                                      jclass clazz) {
    void *handler = shadowhook_dlopen("libart.so");

    // art::Runtime.instance_
    instance_ = static_cast<void **>(shadowhook_dlsym(handler, "_ZN3art7Runtime9instance_E"));

    // art::Runtime.GetSystemThreadGroup
    jobject
    (*getSystemThreadGroup)(void *runtime) =(jobject (*)(void *runtime)) shadowhook_dlsym(handler,

                                                                                          "_ZNK3art7Runtime20GetSystemThreadGroupEv");

    //  art::instrumentation::Instrumentation.UpdateEntrypointsForDebuggable
    void
    (*UpdateEntrypointsForDebuggable)(void *instrumentation) = (void (*)(void *i)) shadowhook_dlsym(
            handler,
            "_ZN3art15instrumentation15Instrumentation30UpdateEntrypointsForDebuggableEv");
    if (getSystemThreadGroup == nullptr || UpdateEntrypointsForDebuggable == nullptr) {
        LOGE("getSystemThreadGroup  failed ");
        shadowhook_dlclose(handler);
        return;
    }
    jobject thread_group = getSystemThreadGroup(*instance_);
    int vm_offset = findOffset(*instance_, 0, 4000, thread_group);
    if (vm_offset < 0) {
        LOGE("vm_offset not found ");
        shadowhook_dlclose(handler);
        return;
    }


    void (*setRuntimeDebugState)(void *instance_, int r) =(void (*)(void *runtime,
                                                                    int r)) shadowhook_dlsym(
            handler, "_ZN3art7Runtime20SetRuntimeDebugStateENS0_17RuntimeDebugStateE");
//    if (setRuntimeDebugState != nullptr) {
//        setRuntimeDebugState(*instance_, 0);
//    }
    if (setRuntimeDebugState != nullptr) {
        setRuntimeDebugState(*instance_, 0);
    }


    void *instrumentation = reinterpret_cast<void *>(reinterpret_cast<char *>(*instance_) +
                                                     vm_offset - 368 );

    UpdateEntrypointsForDebuggable(instrumentation);
    setRuntimeDebugState(*instance_, 2);
    shadowhook_dlclose(handler);
    LOGE("bootImageNterp success");

}


}















extern "C"
JNIEXPORT void JNICALL
Java_com_wy_lib_wytrace_ArtMethodTrace_deoptimizedEverything(JNIEnv *env, jclass clazz) {

    void *handler = shadowhook_dlopen("libart.so");

    // art::Runtime.instance_
    instance_ = static_cast<void **>(shadowhook_dlsym(handler, "_ZN3art7Runtime9instance_E"));

    // art::Runtime.GetSystemThreadGroup
    jobject
    (*getSystemThreadGroup)(void *runtime) =(jobject (*)(void *runtime)) shadowhook_dlsym(handler,

                                                                                          "_ZNK3art7Runtime20GetSystemThreadGroupEv");
    if (getSystemThreadGroup == nullptr) {
        LOGE("getSystemThreadGroup  failed ");
        shadowhook_dlclose(handler);
        return;
    }
    jobject thread_group = getSystemThreadGroup(*instance_);
    int vm_offset = findOffset(*instance_, 0, 4000, thread_group);
    if (vm_offset < 0) {
        LOGE("vm_offset not found ");
        shadowhook_dlclose(handler);
        return;
    }


    void *instrumentation = reinterpret_cast<void *>(reinterpret_cast<char *>(*instance_) +
                                                     vm_offset - 368 );
    auto* deoptimizeEverything =  (void (*)(void*,const char *))(shadowhook_dlsym(handler,

                                                                                            "_ZN3art15instrumentation15Instrumentation20DeoptimizeEverythingEPKc"));
    std::string str = "";
    deoptimizeEverything((void*)instrumentation, str.c_str());
    shadowhook_dlclose(handler);
    LOGE("bootImageNterp success");

}