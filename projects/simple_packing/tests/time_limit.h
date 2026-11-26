// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// runWithTimeLimit(what, f): runs f() in a thread and returns its future (get() gives the result
// or rethrows the exception of f). If f does not return within the time limit, the test program
// reports "FAIL: <what>: hangs" and ends: a hang is a failure. Portable (also on Windows): no
// child processes. A crash ends the whole test program, which then fails as well.
#pragma once

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

template <typename F>
auto runWithTimeLimit(const std::string& what, F&& f, int32_t seconds = 5)
{
    using Result = decltype(f());
    std::packaged_task<Result()> task(std::forward<F>(f));
    std::future<Result> future = task.get_future();
    std::thread thread(std::move(task));
    if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout) {
        std::cerr << "FAIL: " << what << ": hangs" << std::endl;
        std::_Exit(1);  // the thread cannot be stopped
    }
    thread.join();
    return future;
}
