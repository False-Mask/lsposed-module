//
// Created by rose on 2025/8/4.
//

#ifndef LSPOSED_TOOLS_LOCK_FREE_QUEUE_H
#define LSPOSED_TOOLS_LOCK_FREE_QUEUE_H

#include <atomic>
#include <vector>
#include <thread>
#include "utils.h"
#include <pmmintrin.h>



template<typename T, size_t BufferSize = 1024 * 1024>
class LockFreeRingBuffer {
public:
    struct alignas(64) Slot {
        std::atomic<bool> ready{false};
        T data;
    };

    LockFreeRingBuffer() : slots(BufferSize) {}

    bool try_push(const T& item) {
        size_t current = write_pos.load(std::memory_order_relaxed);
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
        size_t current = read_pos.load(std::memory_order_relaxed);

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

static bool isRunning = true;

template<typename T,size_t BufferSize = 1024 * 1024> void consumerLoop(LockFreeRingBuffer<T,BufferSize>&& ringBuffer) {
    T t;
    while (isRunning) {
        if (!ringBuffer.try_pop(t)) {
            ANDROID_LOG_DEBUG("%s %d %d",t.pnae,t.timestamp,t.type);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

template<typename T,size_t BufferSize = 1024 * 1024> void producerLoop(LockFreeRingBuffer<T,BufferSize>& ringBuffer,T&t) {
    while (!ringBuffer.try_push(t)) {
        LOGE("push Failed");
        sched_yield();
    }
}


#endif //LSPOSED_TOOLS_LOCK_FREE_QUEUE_H
