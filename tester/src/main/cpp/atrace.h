//
// Created by rose on 2025/1/31.
//

#ifndef LSPOSED_TOOLS_ATRACE_H
#define LSPOSED_TOOLS_ATRACE_H

#include <shadowhook.h>
#include "utils.h"

class ATrace {

    static void * (*beginSelection)(const char *sectionName);
    static void * (*endSelection)();

    typedef void *(*fp_ATrace_beginSection)(const char *sectionName);

    typedef void *(*fp_ATrace_endSection)(void);

public:
    static bool Init(void * libandroid) {
        if (libandroid == nullptr) {
            LOGE("hook error  dlopen libandroid failed");
            return false;
        }
        beginSelection = reinterpret_cast<fp_ATrace_beginSection>(
                shadowhook_dlsym(libandroid, "ATrace_beginSection"));
        endSelection = reinterpret_cast<fp_ATrace_endSection>(
                shadowhook_dlsym(libandroid, "ATrace_endSection"));
        return true;
    }

    static void TraceBegin(const char *sectionName) {
        beginSelection(sectionName);
    }

    static void TraceEnd() {
        endSelection();
    }

};

ATrace::fp_ATrace_beginSection ATrace::beginSelection = nullptr;
ATrace::fp_ATrace_endSection ATrace::endSelection = nullptr;

#endif //LSPOSED_TOOLS_ATRACE_H
