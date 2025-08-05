//
// Created by rose on 2025/8/4.
//

#pragma once
#include <thread>
#include <vector>
#include "faster/lock_free_queue.h"
#include "faster/logger_entry.h"

void InitTrace();

void StartTrace();

void StopTrace();

extern LockFreeRingBuffer<LockFreeRingBuffer<LogEntry>*> fullBufferQueue;

extern LockFreeRingBuffer<LockFreeRingBuffer<LogEntry>*> pendingBufferQueue;

extern volatile LockFreeRingBuffer<LogEntry>* processingUnit;
