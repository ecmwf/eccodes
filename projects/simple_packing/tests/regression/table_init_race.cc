// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The lookup table for IEEE floats must be initialised safely when the first calls come from
// several threads at once (a global flag was checked and set without synchronisation). This test
// is compiled with -fsanitize=thread together with Utils.cc, and fails on a data race.

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "simple_packing/Utils.h"

using namespace sp;

int main()
{
    std::atomic<bool> start{ false };
    std::atomic<int> wrong{ 0 };
    std::vector<std::thread> threads;
    for (int t = 0; t < 16; ++t) {
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
