//
// Created by rose on 2025/8/9.
//

#include "lock_free_queue.h"

LockFreeRingBuffer<LogEntry> lock_free_ringbuffer;

std::atomic<bool> isRunning(false);

std::atomic<bool> destroy(false);

std::atomic<int> lock = -1;

std::mutex mtx;
std::condition_variable cv;