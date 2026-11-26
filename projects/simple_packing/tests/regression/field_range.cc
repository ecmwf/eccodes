// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Field ranges the optimisers cannot handle (minimum > maximum, non-finite or overflowing range,
// NaN, values beyond the float range) must be rejected with an exception: several of them made
// the parameter search loop forever. Each case runs with a time limit.

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"
#include "simple_packing/detail/simple/Binary.h"

#include "../fp_diagnostics.h"
#include "../time_limit.h"

using namespace sp::detail;

static int failures = 0;

// Runs f with a time limit of 5 seconds: it must throw
template <typename F>
static void expectError(const std::string& what, F&& f)
{
    auto result = runWithTimeLimit(what, std::forward<F>(f));
    try {
        result.get();
    }
    catch (const std::exception&) {
        return;
    }
    std::cerr << "FAIL: " << what << ": no error\n";
    failures++;
}

int main()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::nan("");

    struct Range
    {
        const char* name;
        double min, max;
    };
    for (const Range& r : { Range{ "minimum > maximum", 5.0, 1.0 }, Range{ "overflowing range", -1e308, 1e308 },
                            Range{ "negative beyond the float range", -1e50, -9.99505e49 }, Range{ "NaN", nan, 1.0 },
                            Range{ "infinity", 0.0, inf } }) {
        expectError(std::string(r.name) + " (DecOptimizer)", [&] {
            SimplePackingBinary<double> sp(std::make_unique<DecOptimizer>(16, 127));
            sp.getParameters(r.min, r.max);
        });
        expectError(std::string(r.name) + " (BinDecOptimizer)", [&] {
            SimplePackingBinary<double> sp(std::make_unique<BinDecOptimizer>(0, 1, 16));
            sp.getParameters(r.min, r.max);
        });
        expectError(std::string(r.name) + " (BPVOptimizer)", [&] {
            SimplePackingBinary<double> sp(std::make_unique<BPVOptimizer>(2));
            sp.getParameters(r.min, r.max);
        });
    }

    // The scale factor search directly
    expectError("get_binary_scale_factor with maximum < minimum", [&] {
        get_binary_scale_factor(1.0, 5.0, 24);
    });
    expectError("get_binary_scale_factor with an infinite range", [&] {
        get_binary_scale_factor(1e308, -1e308, 24);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    if (failures != 0) {
        fp_diagnostics::explain();
    }
    return failures == 0 ? 0 : 1;
}
