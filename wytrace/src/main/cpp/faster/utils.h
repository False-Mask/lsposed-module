//
// Created by rose on 2025/8/4.
//

#ifndef LSPOSED_TOOLS_UTILS_H
#define LSPOSED_TOOLS_UTILS_H

#include <android/log.h>

#define TAG "TesterHooker"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

#endif //LSPOSED_TOOLS_UTILS_H
