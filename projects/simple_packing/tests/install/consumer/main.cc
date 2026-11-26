// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// A user program of the install test: packs and unpacks values with the installed simple_packing
#include <cmath>
#include <iostream>
#include <vector>

#include "simple_packing/simple_packing.h"

int main()
{
    std::vector<double> values(1000);
    for (size_t i = 0; i < values.size(); ++i) {
        values[i] = 250.0 + 40.0 * std::sin(0.01 * static_cast<double>(i));
    }
    const sp::Scaling scaling = sp::scaling(values, sp::Method::bits(16));
    const std::vector<double> back = sp::unpack<double>(sp::pack(values, scaling), values.size(), scaling);
    const double step = std::ldexp(1.0, scaling.binaryScaleFactor) * std::pow(10.0, -scaling.decimalScaleFactor);
    for (size_t i = 0; i < values.size(); ++i) {
        if (!(std::fabs(back[i] - values[i]) <= step)) {
            std::cerr << "FAIL: value " << i << '\n';
            return 1;
        }
    }
    std::cout << "OK simple_packing " << sp::version() << '\n';
    return 0;
}
