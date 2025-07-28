//
// Created by rose on 2025/1/30.
//
#include <jni.h>
#include "shadowhook.h"
#include "utils.h"
#include "mListener.h"
#include "virlistener.h"
#include "mock-listener.h"
#include "art_method.h"
#include "atrace.h"



void doHook();

int findOffset(void * instance, int begin,int end,jobject thread);


void * (*beginSelection)(const char *sectionName);
void * (*endSelection)();

typedef void *(*fp_ATrace_beginSection)(const char *sectionName);

typedef void *(*fp_ATrace_endSection)(void);

void ** instance_;

void doHook() {

    // find Atrace begin
    void *libandroid = shadowhook_dlopen("libandroid.so");
    ATrace::Init(libandroid);
    shadowhook_dlclose(libandroid);
    // find Atrace   end

    void *handler = shadowhook_dlopen("libart.so");
    instance_ = static_cast<void **>(shadowhook_dlsym(handler, "_ZN3art7Runtime9instance_E"));
    jobject
    (*getSystemThreadGroup)(void *runtime) =(jobject (*)(void *runtime)) shadowhook_dlsym(handler,
                                                                                          "_ZNK3art7Runtime20GetSystemThreadGroupEv");

    if (getSystemThreadGroup == nullptr) {
        LOGE("getSystemThreadGroup  failed ");
        shadowhook_dlclose(handler);
        return;
    }
    jobject thread_group = getSystemThreadGroup(*instance_);
    int vm_offset = (uint64_t)findOffset(*instance_, 0, 4000, thread_group);
    if (vm_offset < 0) {
        LOGE("vm_offset not found ");
        shadowhook_dlclose(handler);
        return;
    }
    void (*setRuntimeDebugState)(void *instance_, int r) =(void (*)(void *runtime,
                                                                    int r)) shadowhook_dlsym(
            handler, "_ZN3art7Runtime20SetRuntimeDebugStateENS0_17RuntimeDebugStateE");
    if (setRuntimeDebugState != nullptr) {
        setRuntimeDebugState(*instance_, 0);
    }
    void *instrumentation = reinterpret_cast<void *>(reinterpret_cast<char *>(*instance_) +
                                                     vm_offset - 368 );
    void (*AddListener)(void *instrumentation,void* listener, uint32_t r) =reinterpret_cast<void (*)(
            void *, void *, uint32_t)>((void (*)(void *runtime,
                                                 int r)) shadowhook_dlsym(
            handler,
            "_ZN3art15instrumentation15Instrumentation11AddListenerEPNS0_23InstrumentationListenerEj"));

    MyArtMethod::Init(handler);

    AddListener(instrumentation, new art::instrumentation::MListener(), kMethodEntered | kMethodExited);


    // Deoptimize
    void (*DeoptimizeEverything)(void *instrumentation,const char* key) =reinterpret_cast<void (*)(void *instrumentation,const char* )>
            (shadowhook_dlsym(
            handler,
            "_ZN3art15instrumentation15Instrumentation20DeoptimizeEverythingEPKc"));

    const char * deopt = "test";
    DeoptimizeEverything(instrumentation, deopt);

    if (setRuntimeDebugState != nullptr) {
        setRuntimeDebugState(*instance_, 0);
    }
    shadowhook_dlclose(handler);
    LOGE("bootImageNterp success");

}

int findOffset(void * instance, int begin,int end,jobject thread) {
    for(int i = begin;i <= end; i++) {
        jobject * net = (jobject *)((char *)(instance) + i);
        if (thread == *net) {
            return i;
        }
    }
    return -1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_hook_HookAllMethod_addListener(JNIEnv *env, jobject thiz) {
    doHook();
}