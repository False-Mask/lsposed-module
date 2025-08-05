//
// Created by rose on 2025/8/4.
//

#ifndef LSPOSED_TOOLS_TRACER_H
#define LSPOSED_TOOLS_TRACER_H
#include <thread>
#include "faster/lock_free_queue.h"
#include "faster/logger_entry.h"

void InitTrace();
static std::thread threadInstance;

#endif //LSPOSED_TOOLS_TRACER_H
