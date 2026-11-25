#pragma once

#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

namespace spaec
{

class ThreadPool
{
public:
    ThreadPool(size_t threads) : stop_(false)
    {
        if (threads == 0) {
            throw std::invalid_argument("ThreadPool: the number of threads must be at least 1");
        }
        try {
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
        catch (...) {
            // Thread creation failed (e.g. resource limit): join the threads already started,
            // a joinable std::thread in the destroyed vector would call std::terminate
            stopAndJoin();
            throw;
        }
    }

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

    ~ThreadPool() { stopAndJoin(); }

private:
    void stopAndJoin()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
    using returnType = std::invoke_result_t<F, Args...>;

    // The arguments are stored in a tuple and passed as lvalues, like the captured copies of a lambda
    auto task = std::make_shared<std::packaged_task<returnType()>>(
        [f = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            return std::apply(f, args);
        });

    std::future<returnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (stop_) throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_one();

    return res;
}

}  // namespace spaec
