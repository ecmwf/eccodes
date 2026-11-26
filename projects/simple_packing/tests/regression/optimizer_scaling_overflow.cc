// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Fields the decimal scale factor search cannot scale without overflowing the double range
// (values near +-DBL_MAX scaled by 10^D, or a decimal scale factor so large that value * 10^D
// overflows) must be rejected with a clear error: the search overflowed to inf/NaN and then
// converted the NaN to integers (undefined behaviour) or failed much later with a misleading
// message. Each case runs with a time limit; the error must mention the
// range that cannot be scaled.

#include <exception>
#include <iostream>
#include <string>

#include "simple_packing/detail/optimizer/BPVOptimizer.h"

#include "../fp_diagnostics.h"
#include "../time_limit.h"

using namespace sp::detail;

static int failures = 0;

// Runs f with a time limit of 5 seconds: it must throw an error about the unscaled range
template <typename F>
static void expectScaleError(const std::string& what, F&& f)
{
    auto result = runWithTimeLimit(what, std::forward<F>(f));
    const char* problem = "no error";
    try {
        result.get();
    }
    catch (const std::exception& e) {
        problem = std::string(e.what()).find("cannot scale the range") != std::string::npos ? nullptr : "wrong error message";
    }
    if (problem) {
        std::cerr << "FAIL: " << what << ": " << problem << '\n';
        failures++;
    }
}

int main()
{
    expectScaleError("BPVOptimizer: value * 10^decimalScaleFactor overflows", [] {
        BPVOptimizer(400).getParams(1.0, 2.0);
    });
    expectScaleError("BPVOptimizer: value * 10^decimalScaleFactor overflows (320)", [] {
        BPVOptimizer(320).getParams(1.0, 2.0);
    });
    expectScaleError("BPVOptimizer: scaling overflows to inf", [] {
        BPVOptimizer(2).getParams(-1.7e308, -1.7e308 + 1e-300);
    });

    if (failures == 0) {
        std::cout << "OK\n";
    }
    if (failures != 0) {
        fp_diagnostics::explain();
    }
    return failures == 0 ? 0 : 1;
}
