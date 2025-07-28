//
// Created by rose on 2025/8/4.
//

#ifndef LSPOSED_TOOLS_LOGGER_ENTRY_H
#define LSPOSED_TOOLS_LOGGER_ENTRY_H

#include "lock_free_queue.h"

enum Type {
    TRACE_BEGIN,
    TRACE_END
};

struct LogEntry {
    const char* pname;
    long timestamp;
    Type type;
};

static LockFreeRingBuffer<LogEntry> lock_free_ringbuffer;

#endif //LSPOSED_TOOLS_LOGGER_ENTRY_H
