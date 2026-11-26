// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Optimiser parameters outside their range must be rejected with std::invalid_argument when the
// optimiser is created: negative or huge bits per value and scale factors made the parameter
// search loop forever or for a very long time.

#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"

#include "../time_limit.h"

using namespace sp::detail;

static int failures = 0;

// Runs f with a time limit of 5 seconds: it must throw std::invalid_argument
template <typename F>
static void expectInvalid(const std::string& what, F&& f)
{
    auto result = runWithTimeLimit(what, std::forward<F>(f));
    const char* problem = "accepted";
    try {
        result.get();
    }
    catch (const std::invalid_argument&) {
        problem = nullptr;
    }
    catch (...) {
        problem = "other exception";
    }
    if (problem) {
        std::cerr << "FAIL: " << what << ": " << problem << '\n';
        failures++;
    }
}

int main()
{
    for (int64_t bpv : { -1LL, 0LL, 64LL, 65LL, 100000000000LL }) {  // long long: 32-bit long on some platforms
        const std::string b = std::to_string(bpv);
        expectInvalid("DecOptimizer with " + b + " bits per value", [&] { DecOptimizer(bpv, 127).getParams(-1, 1); });
        expectInvalid("BinDecOptimizer with " + b + " bits per value", [&] { BinDecOptimizer(0, 1, static_cast<int>(bpv)).getParams(-1, 1); });
    }
    for (int64_t last : { -INT64_C(1), INT64_C(0), INT64_C(100000) }) {
        expectInvalid("DecOptimizer with last " + std::to_string(last), [&] { DecOptimizer(16, last).getParams(-1, 1); });
    }
    for (int dsf : { -1000000, 1000000 }) {
        expectInvalid("BPVOptimizer with decimal scale factor " + std::to_string(dsf), [&] { BPVOptimizer(dsf).getParams(-1, 1); });
    }

    // Supported parameters
    try {
        DecOptimizer(63, 127).getParams(-1, 1);
        DecOptimizer(1, 99).getParams(-1, 1);
        BinDecOptimizer(1, 1, 63).getParams(0, 1);
        BinDecOptimizer(0, 1, 32).getParams(320.63377474930917, 581.49648981621397);
        BPVOptimizer(-10).getParams(-1e10, 1e10);
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: supported parameters: " << e.what() << '\n';
        failures++;
    }

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
