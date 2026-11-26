// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The first calls may come from several threads at once: the lookup table for IEEE floats was
// initialised on the first call, with a global flag checked and set without synchronisation. The
// library has no such state any more (its tables are compile-time constants). This test checks the
// results of concurrent first calls; in a build with the thread sanitizer (-fsanitize=thread for
// the whole project) it also fails on a data race.

#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "simple_packing/detail/Utils.h"

using namespace sp::detail;

int main()
{
    std::atomic<bool> start{ false };
    std::atomic<int32_t> wrong{ 0 };
    std::vector<std::thread> threads;
    for (int32_t t = 0; t < 16; ++t) {
        threads.emplace_back([&, t] {
            while (!start) {
            }
            const double v = 1.5 + t;
            auto r = nearest_smaller_value(v, FloatType::IEEE32);
            if (!r || *r != v) {
                wrong++;
            }
        });
    }
    start = true;
    for (auto& thread : threads) {
        thread.join();
    }
    if (wrong > 0) {
        std::cerr << "FAIL: " << wrong << " wrong results\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
