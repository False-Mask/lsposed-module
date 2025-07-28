//
// Created by rose on 2025/8/4.
//

// AndroidThreadPool.h
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <android/log.h>

class AndroidThreadPool {
public:
    explicit AndroidThreadPool(size_t num_threads = std::thread::hardware_concurrency())
            : stop_all(false) {
        workers.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] { worker_loop(); });
        }
    }

    ~AndroidThreadPool() {
        {
            std::unique_lock lock(mutex_);
            stop_all = true;
        }
        cv_.notify_all();

        for (auto &worker: workers) {
            if (worker.joinable()) worker.join();
        }
    }

    template<class F, class... Args>
    void enqueue(F &&f, Args &&... args) {
        {
            std::unique_lock lock(mutex_);
            tasks.emplace([=]() {
                std::invoke(f, args...);
            });
        }
        cv_.notify_one();
    }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this]() {
                    return !tasks.empty() || stop_all;
                });

                if (stop_all && tasks.empty()) return;

                task = std::move(tasks.front());
                tasks.pop();
            }

            try {
                task();
            } catch (const std::exception &e) {
                __android_log_print(ANDROID_LOG_ERROR, "ThreadPool",
                                    "Task failed: %s", e.what());
            }
        }
    }

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    // Synchronization
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_all;

};