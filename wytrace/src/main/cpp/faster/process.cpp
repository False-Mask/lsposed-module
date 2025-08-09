//
// Created by rose on 2025/8/5.
//

#include "process.h"
#include "lock_free_queue.h"
#include <string>
// 首次访问时自动初始化（延迟加载）

std::string get_platform_thread_name() {
    char buff [100];
    pthread_getname_np(pthread_self(), buff, 100);
    return {buff};
}

thread_local std::string pthread_name = []() {
    // 先尝试获取系统设置的线程名
    std::string platform_name = get_platform_thread_name();

    // 如果未设置则生成唯一标识
    if (platform_name.empty() || platform_name == "unknown") {
        return std::string("Failed");
    }
    return platform_name;
}();

std::string procName;

std::string get_process_name() {
    if (procName.empty()) {
        const char* progName = getprogname();
        procName = progName ? std::string(progName) : "unknown";
    }
    return procName;
}



