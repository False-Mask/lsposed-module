//
// Created by rose on 2025/8/4.
//
#include "faster/tracer.h"
#include "faster/lock_free_queue.h"
#include <barrier>
#include <list>
#include "trace_thread.h"
#include "utils.h"

void InitTrace() {
    int threads = 4;
    auto b = new std::barrier(threads);
    std::list<std::thread> ts;
    for (int i = 0;i < threads; i++) {
        LOGE("start Thread %d", i);
        TraceThread tt;
        auto threadInstance = std::thread(&TraceThread::traceLoop,tt, threads, i, b);
        threadInstance.detach();
    }
}

void StartTrace() {
    isRunning.store(true, std::memory_order_relaxed);
    std::unique_lock<std::mutex> lock(mtx);
    cv.notify_all();
}

void StopTrace() {
    isRunning.store(false, std::memory_order_relaxed);
}

volatile LockFreeRingBuffer<LogEntry>* processingUnit;

LockFreeRingBuffer<LockFreeRingBuffer<LogEntry>*> fullBufferQueue;

LockFreeRingBuffer<LockFreeRingBuffer<LogEntry>*> pendingBufferQueue;

//std::vector<std::thread> threadLists;
