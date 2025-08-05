//
// Created by rose on 2025/8/4.
//
#include "faster/tracer.h"

void traceLoop() {
//    LOGE("traceLoop Addr %p", &lock_free_ringbuffer);
    consumerLoop<LogEntry>(lock_free_ringbuffer);

}

void InitTrace() {
    threadInstance = std::thread(traceLoop);
    threadInstance.detach();
}
