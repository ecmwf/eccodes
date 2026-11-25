// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Dec64Optimizer must choose the decimal scale factor so that the binary scale factor stays within
// +-last (it scaled small ranges down instead of up, e.g. binary scale factor -288 for last 127),
// the packed values must keep the precision of bitsPerValue, and ranges it cannot handle must be
// rejected.

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "simple_packing/optimizer/Dec64Optimizer.h"
#include "simple_packing/simple/Binary.h"

using namespace sp;

static int failures = 0;

static void check(double min, double max, long bpv, long last)
{
    std::vector<double> values(101);
    for (size_t i = 0; i < values.size(); ++i) {
        values[i] = min + (max - min) * i / (values.size() - 1);
    }
    try {
        SimplePackingBinary<double> sp(std::make_unique<Dec64Optimizer>(bpv, last));
        auto [params, buffer] = sp.pack(values);
        if (std::labs(params.binaryScaleFactor()) > last) {
            std::cerr << "FAIL: range [" << min << ", " << max << "]: binary scale factor " << params.binaryScaleFactor() << " beyond +-" << last << '\n';
            failures++;
        }
        auto decoded = sp.unpack(params, buffer, values.size());
        const double tolerance = (max - min) / std::ldexp(1.0, static_cast<int>(bpv) - 2);
        for (size_t i = 0; i < values.size(); ++i) {
            if (!(std::fabs(decoded[i] - values[i]) <= tolerance)) {
                std::cerr << "FAIL: range [" << min << ", " << max << "]: value " << values[i] << " decoded as " << decoded[i] << '\n';
                failures++;
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: range [" << min << ", " << max << "]: " << e.what() << '\n';
        failures++;
    }
}

int main()
{
    check(1e-40, 2e-40, 24, 127);
    check(0.0, 1e-30, 16, 127);
    check(1e30, 2e30, 24, 127);
    check(-5.0, 5.0, 16, 127);
    check(273.15, 310.0, 12, 63);

    try {
        Dec64Optimizer(16, 127).getParams(-1e308, 1e308);
        std::cerr << "FAIL: infinite range was accepted\n";
        failures++;
    }
    catch (const std::exception&) {
    }

    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
