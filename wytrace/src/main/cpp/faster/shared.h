//
// Created by rose on 2025/8/5.
//

#ifndef LSPOSED_TOOLS_SHARED_H
#define LSPOSED_TOOLS_SHARED_H

#include <string>

extern thread_local std::string pthread_name;

std::string get_platform_thread_name();


#endif //LSPOSED_TOOLS_SHARED_H
