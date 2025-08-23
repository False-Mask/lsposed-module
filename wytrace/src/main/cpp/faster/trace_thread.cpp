//
// Created by rose on 2025/8/20.
//

#include "trace_thread.h"
#include "faster/tracer.h"
#include "utils.h"
#include <barrier>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdexcept>

void TraceThread::traceLoop(int len, int threadKey, std::barrier<>* barrier) {
    consumerLoop(len, threadKey, barrier);
}

void mergeUsingMmap(const std::vector<std::string>& sources, const std::string& dest) {
    // 1. 计算总大小并创建目标文件
    size_t totalSize = 0;
    for (const auto& src : sources) {
        struct stat st;
        if (stat(src.c_str(), &st) == -1) {
            throw std::runtime_error("Failed to get file size: " + src);
        }
        totalSize += st.st_size;
    }

    // 打开目标文件并设置大小
    int destFd = open(dest.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (destFd == -1) {
        throw std::runtime_error("Failed to open destination file");
    }
    if (ftruncate(destFd, totalSize) == -1) {  // 预分配空间
        close(destFd);
        throw std::runtime_error("Failed to resize destination file");
    }

    // 2. 映射目标文件到内存
    void* destMap = mmap(nullptr, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, destFd, 0);
    if (destMap == MAP_FAILED) {
        close(destFd);
        throw std::runtime_error("Failed to mmap destination file");
    }

    // 3. 逐个映射源文件并拷贝
    char* destPtr = static_cast<char*>(destMap);
    for (const auto& src : sources) {
        int srcFd = open(src.c_str(), O_RDONLY);
        if (srcFd == -1) {
            munmap(destMap, totalSize);
            close(destFd);
            throw std::runtime_error("Failed to open source file: " + src);
        }

        struct stat st;
        if (stat(src.c_str(), &st) == -1) {
            close(srcFd);
            munmap(destMap, totalSize);
            close(destFd);
            throw std::runtime_error("Failed to get source file size: " + src);
        }

        size_t fileSize = st.st_size;
        void* srcMap = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, srcFd, 0);
        if (srcMap == MAP_FAILED) {
            close(srcFd);
            munmap(destMap, totalSize);
            close(destFd);
            throw std::runtime_error("Failed to mmap source file: " + src);
        }

        // 执行内存拷贝（需检查对齐）
        memcpy(destPtr, srcMap, fileSize);

        // 解除映射并关闭文件
        munmap(srcMap, fileSize);
        close(srcFd);
        destPtr += fileSize;  // 移动目标指针
        LOGE("successfully copy file %s", src.c_str());
    }

    // 解除目标文件映射并关闭
    munmap(destMap, totalSize);
    close(destFd);
}

void TraceThread::mergeAllFiles(int len, int key) {
    // lock
    int expect = IDLE;
    LOGE("merge list %d thread expect %d, set to %d", key, IDLE, key);
    auto v = lock.compare_exchange_strong(expect, key);
    if (!v) {
        LOGE("merge list %d thread failed", key);
        return;
    }
    // merge all files
    std::vector<std::string> sources;
    for (int i = 0;i < len; i++) {
        std::string str;
        makeFileName(i, str);
        sources.emplace_back(str);
        LOGE("log merge list: %s", str.c_str());
    }
    std::string finalStr = std::string("/data/user/0/") +  get_process_name() + "/." + get_process_name();
    LOGE("log target list: %s", finalStr.c_str());
    mergeUsingMmap(sources, finalStr);
    // unlock
    lock.store(IDLE, std::memory_order_release);

}

void TraceThread::makeFileName(int key,std::string& str) {
    str = std::string("/data/user/0/") +  get_process_name() + "/." + get_process_name() + "-" + std::to_string(key);
}

void TraceThread::consumerLoop(int len,int threadKey, std::barrier<>* barrier) {
    LogEntry t;
    LockFreeRingBuffer<LogEntry>* pending = nullptr;
    Writer localWriter;
    LogEntry logEntry;
    while (likely(!destroy.load(std::memory_order_relaxed))) {
        // notRunning Status
        if (unlikely(fullBufferQueue.isEmpty() &&
         pendingBufferQueue.isEmpty() &&
         (pending == nullptr ||  pending->isEmpty()) &&
         (processingUnit == nullptr) &&
         !isRunning.load(std::memory_order_relaxed))) {

            // before sleep
            if (localWriter.getFileLen() > 0) {
                localWriter.Destroy();
                barrier->arrive_and_wait();
                mergeAllFiles(len, threadKey);
            }
            // wait util next term
            {
                std::unique_lock<std::mutex> mtxLock(mtx);
                LOGE("thread %d is sleeping waiting for wake up", threadKey);
                cv.wait(mtxLock, [] {
                    return isRunning.load(std::memory_order_acquire);
                });
            }
            // next turn is begin
            if (isRunning.load(std::memory_order_relaxed)) {
                std::string fileName;
                makeFileName(threadKey, fileName);
                localWriter.Init(fileName, 1024 * 1024 * (1024 / len));
            }
            // before start
        }
        // running status
        // state 0 -> default
        // state 1 -> acquired the buffer
        // state 2 -> acquired the pendingBuffer
        int state = 0;
        if (processingUnit == nullptr && pending == nullptr) {
            // acquire the buffer
            LockFreeRingBuffer<LogEntry>* bq;
            LOGD("thread %d trying pop fullBufferQueue isEmpty=%d", threadKey, fullBufferQueue.isEmpty());
            if (fullBufferQueue.try_pop(bq)) {
                processingUnit = bq;
                state = 1;
            } else {
                LOGE("thread %d pending %p", threadKey , pending);
                if (pending != nullptr) {
                    LOGE("thread %d pending is empty=%d", threadKey, pending->isEmpty());
                }
                LOGD("thread %d trying pop pendingBufferQueue isEmpty=%d", threadKey, pendingBufferQueue.isEmpty());
                auto res = pendingBufferQueue.try_pop(pending);
                if (res) {
                    state = 2;
                }
            }
        } else if (processingUnit != nullptr) {
            state = 1;
        } else if (pending != nullptr) {
            state = 2;
        }


        // process
        if (state == 0) {
            LOGE("%d waiting", threadKey);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else if (state == 1 && processingUnit != nullptr) {
            if (pending == processingUnit || (pending != nullptr && pending->isEmpty())) {
                pending = nullptr;
            }
            // 1. wait all prepared
            barrier->arrive_and_wait();

            process(len, threadKey, localWriter, logEntry);

            LOGE("thread %d processed one buffer %p", threadKey, const_cast<LockFreeRingBuffer<LogEntry>*>(processingUnit));

            // wait all finished
            barrier->arrive_and_wait();

            // lock
            auto expect = IDLE;
            if (processingUnit != nullptr && lock.compare_exchange_strong(expect, threadKey)) {
                LOGD("thread %d enter reset branch, processUnit is %p ", threadKey, processingUnit);
                const_cast<LockFreeRingBuffer<LogEntry>*>(processingUnit)->clear();
                processingUnit = nullptr;
                lock.store(IDLE);
            }
            barrier->arrive_and_wait();
//            LOGE("process finished");
        } else if (state == 2 && pending != nullptr) {
            auto res = pending->try_pop(logEntry);
            LOGE("thread %d is steal task res=%d", threadKey, res);
            if (res) {
                LOGE("write logEntry");
                localWriter.Write(logEntry);
            }
        }
    }
}

void TraceThread::process(int len, int key, Writer& w, LogEntry& entry) {
    auto* unsafeProcessUnit = const_cast<LockFreeRingBuffer<LogEntry>*>(processingUnit);
    auto slotLen = unsafeProcessUnit->getSlotLen();
    int part = slotLen / len;
    int processCnt = part;
    if (key == len - 1) {
        processCnt += slotLen % len;
    }
    int begin = part * key;
    int end = begin + processCnt;
//    LOGE("slot len %d, process len %d, begin %d, end %d", slotLen, processCnt, begin, end);
    for (int i = begin;i < end; i++) {
        auto res = unsafeProcessUnit->readSlot(entry, i);
        // write value;
        if (res && entry.methodName.c_str() && entry.pname.c_str()) {
            w.Write(entry);
        }
    }
}
