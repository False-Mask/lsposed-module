//
// Created by rose on 2025/8/4.
//

#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include "utils.h"
#include "process.h"
#include "logger_entry.h"
#include "file_writer.h"
#include "compiler.h"

template<typename T, size_t BufferSize = 1024 * 1024>
class LockFreeRingBuffer {
public:
    LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;


    struct alignas(64) Slot {
        std::atomic<bool> ready{false};
        T data;
    };

    LockFreeRingBuffer() : slots(BufferSize) {}

    bool try_push(const T& item) {
        size_t current = write_pos.load(std::memory_order_acquire);
        size_t next = (current + 1) % BufferSize;

        // 检查缓冲区是否已满
        if (next == read_pos.load(std::memory_order_acquire)) {
            return false;
        }

        slots[current].data = item;
        slots[current].ready.store(true, std::memory_order_release);
        write_pos.store(next, std::memory_order_release);
        return true;
    }

    bool try_pop(T& item) {
        size_t current = read_pos.load(std::memory_order_acquire);

        if (current == write_pos.load(std::memory_order_acquire)) {
            return false;
        }

        // 等待数据就绪
        while (!slots[current].ready.load(std::memory_order_acquire)) {
            sched_yield();
        }

        item = std::move(slots[current].data);
        slots[current].ready.store(false, std::memory_order_release);
        read_pos.store((current + 1) % BufferSize, std::memory_order_release);
        return true;
    }

private:
    alignas(64) std::atomic<size_t> write_pos{0};
    alignas(64) std::atomic<size_t> read_pos{0};
    std::vector<Slot> slots;
};

extern std::atomic<bool> destroy;
extern std::atomic<bool> isRunning;
extern std::mutex mtx;
extern std::condition_variable cv;


template<typename T,size_t BufferSize = 1024 * 1024> void consumerLoop(LockFreeRingBuffer<T,BufferSize>& ringBuffer) {
    T t;
    while (likely(!destroy.load(std::memory_order_relaxed))) {
        if (unlikely(!isRunning.load(std::memory_order_relaxed))) {
            std::unique_lock<std::mutex> lock(mtx);
            // before sleep
            if (!isRunning.load(std::memory_order_relaxed)) {
                writer.Destroy();
            }
//            writer.Destroy();
            cv.wait(lock, [] {
                return isRunning.load(std::memory_order_acquire);
            });
            if (isRunning.load(std::memory_order_relaxed)) {
                std::string fileName = std::string("/data/user/0/") +  get_process_name() + "/." + get_process_name();
                writer.Init(fileName, 1024 * 1024 * 1024);
            }
            // before start
        }
        if (ringBuffer.try_pop(t)) {
            writer.Write(t);
//            LOGE("consume items %s %s %ld %d",t.pname.c_str(),t.methodName.c_str(),t.timestamp,t.type);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

template<typename T,size_t BufferSize = 1024 * 1024> void producerLoop(LockFreeRingBuffer<T,BufferSize>& ringBuffer,T&t) {
    while (!ringBuffer.try_push(t)) {
//        LOGE("push Failed");
        sched_yield();
    }
}

extern LockFreeRingBuffer<LogEntry> lock_free_ringbuffer;
