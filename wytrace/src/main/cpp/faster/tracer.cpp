//
// Created by rose on 2025/8/4.
//
#include "faster/tracer.h"
#include "faster/lock_free_queue.h"

static void traceLoop() {
    consumerLoop<LogEntry>(lock_free_ringbuffer);

}

void InitTrace() {
    threadInstance = std::thread(traceLoop);
    threadInstance.detach();
}

void StartTrace() {
    isRunning.store(true, std::memory_order_relaxed);
    std::unique_lock<std::mutex> lock(mtx);
    cv.notify_one();
}

void StopTrace() {
    isRunning.store(false, std::memory_order_relaxed);
}
