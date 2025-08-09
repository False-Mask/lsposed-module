//
// Created by rose on 2025/8/5.
//
#pragma once

#include <string>

extern thread_local std::string pthread_name;

std::string get_platform_thread_name();

std::string get_process_name();
