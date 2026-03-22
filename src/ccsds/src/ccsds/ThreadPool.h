#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>

class ThreadPool {
   public:
    ThreadPool(size_t threads) : stop_(false) {
        for (size_t i = 0; i < threads; ++i) {
            workers_.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex_);
                        this->condition_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
                        if (this->stop_ && this->tasks_.empty()) return;
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread& worker : workers_) {
            worker.join();
        }
    }

   private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using returnType = typename std::result_of<F(Args...)>::type;

    // auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto task = std::make_shared<std::packaged_task<returnType()>>(
            [f = std::forward<F>(f), ... args = std::forward<Args>(args)]() mutable {
                return f(args...);
            }
            );

    std::future<returnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_one();

    return res;
}
