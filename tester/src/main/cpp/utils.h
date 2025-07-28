//
// Created by rose on 2025/1/30.
//

#ifndef LSPOSED_TOOLS_UTILS_H
#define LSPOSED_TOOLS_UTILS_H
#include <android/log.h>
#include "thread-announcement.h"
#define TAG "HookerTester"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define SHARED_LOCKABLE SHARED_CAPABILITY("mutex")

#define ATTRIBUTE_UNUSED __attribute__((__unused__))


#endif //LSPOSED_TOOLS_UTILS_H
