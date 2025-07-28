//
// Created by rose on 2025/1/31.
//

#ifndef LSPOSED_TOOLS_ART_METHOD_H
#define LSPOSED_TOOLS_ART_METHOD_H

//
// Created by rose on 2025/1/31.
//
#include <jni.h>
#include <shadowhook.h>
#include <string>


class MyArtMethod {
public:
    typedef std::string(*PrettyMethodType)(void *thiz, bool with_signature);

    static std::string PrettyMethod(void *thiz, bool with_signature) {
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
        return !(MyArtMethod::PrettyMethodSym = reinterpret_cast<MyArtMethod::PrettyMethodType>(shadowhook_dlsym(
                handler, "_ZN3art9ArtMethod12PrettyMethodEPS0_b"))) &&
               !(MyArtMethod::PrettyMethodSym = reinterpret_cast<MyArtMethod::PrettyMethodType>(shadowhook_dlsym(
                       handler, "_ZN3art12PrettyMethodEPNS_9ArtMethodEb"))) &&
               !(MyArtMethod::PrettyMethodSym = reinterpret_cast<MyArtMethod::PrettyMethodType>(shadowhook_dlsym(
                       handler, "_ZN3art12PrettyMethodEPNS_6mirror9ArtMethodEb")));
    }

private:
    static std::string (*PrettyMethodSym)(void*thiz, bool with_signature);

};

MyArtMethod::PrettyMethodType MyArtMethod::PrettyMethodSym = nullptr;



#endif //LSPOSED_TOOLS_ART_METHOD_H
