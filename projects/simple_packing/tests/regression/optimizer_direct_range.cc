// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Calling an optimiser's getParams() directly with an out-of-range field (minimum > maximum,
// non-finite values) must be rejected with an exception: the parameter search looped forever
// for minimum > maximum and converted NaN or infinity to integers (undefined behaviour)
// Otherwise. SimplePacking::getParameters() validates on its own, the optimisers did not.
// Each case runs with a five-second time limit.

#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <string>

#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"

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

// Runs f with a time limit of 5 seconds: it must return constant-field parameters that decode
// to 'value'
template <typename F>
static void expectConstant(const std::string& what, double value, F&& f)
{
    auto result = runWithTimeLimit(what, std::forward<F>(f));
    const char* problem = nullptr;
    try {
        const Parameters params = result.get();
        const double decoded = params.referenceValue() / std::pow(10.0, static_cast<double>(params.decimalScaleFactor()));
        const bool ok = params.bitsPerValue() == 0 && params.isConstantField() && std::fabs(decoded - value) <= 1e-6 * value;
        if (!ok) {
            problem = "wrong parameters";
        }
    }
    catch (const std::exception&) {
        problem = "threw";
    }
    if (problem) {
        std::cerr << "FAIL: " << what << ": " << problem << '\n';
        failures++;
    }
}

int main()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::nan("");

    expectError("DecOptimizer: minimum > maximum", [] {
        DecOptimizer(24, 127).getParams(1.0, -1.0);
    });
    expectError("BinDecOptimizer: minimum > maximum", [] {
        BinDecOptimizer(0, 1, 16).getParams(2.0, -1.0);
    });
    expectError("BinDecOptimizer: NaN", [=] {
        BinDecOptimizer(0, 1, 16).getParams(nan, 1.0);
    });
    expectError("BPVOptimizer: minimum > maximum", [] {
        BPVOptimizer(2).getParams(1.0, -1.0);
    });
    expectError("BPVOptimizer: infinity", [=] {
        BPVOptimizer(2).getParams(0.0, inf);
    });

    // A constant field: the reference value must represent the field, not a default
    expectConstant("BinDecOptimizer: constant field", 5.0, [] {
        return BinDecOptimizer(0, 1, 16).getParams(5.0, 5.0);
    });
    expectConstant("DecOptimizer: constant field", 5.0, [] {
        return DecOptimizer(24, 127).getParams(5.0, 5.0);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
