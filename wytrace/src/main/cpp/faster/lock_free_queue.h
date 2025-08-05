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

template<typename T, size_t BufferSize = 1024 * 128>
class LockFreeRingBuffer {
public:
    LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;

    std::atomic<bool> writeLock = false;
    std::atomic<bool> readLock = false;


    struct alignas(64) Slot {
        std::atomic<bool> ready{false};
        T data;
    };

    LockFreeRingBuffer() : slots(BufferSize) {}

    void push(const T& item) {
        while (!try_push(item)) {}
    }

    void pop(const T& item) {
        while (!try_pop(item)) {}
    }

    bool try_push(const T& item) {
        // try get cas lock
        bool expect = false;
        while (!writeLock.compare_exchange_weak(expect, true,std::memory_order_acquire)) {}

        // check if the buffer is full
        size_t current = write_pos.load(std::memory_order_acquire);
        size_t next = (current + 1) % BufferSize;

        if (next == read_pos.load(std::memory_order_acquire)) {
            return false;
        }

        // save data
        slots[current].data = item;
        slots[current].ready.store(true, std::memory_order_release);

        // update write index
        write_pos.store(next, std::memory_order_release);
        // release the cas lock
        writeLock.store(false,std::memory_order_release);
        return true;
    }

    bool try_pop(T& item) {
        bool expect = false;
        while (!readLock.compare_exchange_weak(expect, true, std::memory_order_acquire)) {
        }

        // check if the buffer is full
        size_t current = read_pos.load(std::memory_order_acquire);

        if (current == write_pos.load(std::memory_order_acquire)) {
            return false;
        }

        // 等待数据就绪
        while (!slots[current].ready.load(std::memory_order_acquire)) {}
        // get the data
        item = std::move(slots[current].data);
        slots[current].ready.store(false, std::memory_order_release);
        // update index
        read_pos.store((current + 1) % BufferSize, std::memory_order_release);
        // release the lock
        readLock.store(false, std::memory_order_release);
        return true;
    }

    // usafe operator
    bool readSlot(T& t,int idx) {
        auto& sl = slots[idx];
        auto expect = true;
        if (!sl.ready.compare_exchange_weak(expect, false, std::memory_order_acquire)) {
            return false;
        }
        t = std::move(sl.data);
        return true;
    }

    int getSlotLen() {
        return slots.size();
    }

    bool isFull() {
        bool expect = false;
//        while (!readLock.compare_exchange_weak(expect, true, std::memory_order_acquire)) {
//            LOGE("READ!");
//        }
//        while (!writeLock.compare_exchange_weak(expect, true, std::memory_order_acquire)) {
//            LOGE("WRITE!");
//        }
        auto read = read_pos.load(std::memory_order_relaxed);
        auto write = write_pos.load(std::memory_order_relaxed);
        auto next = (write + 1) % BufferSize;
//        readLock.store(false, std::memory_order_release);
//        writeLock.store(false, std::memory_order_release);
        return read == next;
    }

private:
    alignas(64) std::atomic<size_t> write_pos{0};
    alignas(64) std::atomic<size_t> read_pos{0};
    std::vector<Slot> slots;
};

extern std::atomic<bool> destroy;
extern std::atomic<bool> isRunning;
extern std::atomic<int> lock;
extern std::mutex mtx;
extern std::condition_variable cv;
const int IDLE = -1;

template<typename T,size_t BufferSize = 1024 * 1024> void producerLoop(LockFreeRingBuffer<T,BufferSize>& ringBuffer,T&t) {
    while (!ringBuffer.try_push(t)) {
//        LOGE("push Failed");
        sched_yield();
    }
}

extern LockFreeRingBuffer<LogEntry> lock_free_ringbuffer;

