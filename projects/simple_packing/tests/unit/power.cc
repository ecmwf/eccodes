// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// sp_power(s, n) = n^s by repeated multiplication or division (like the legacy ecCodes code).
// Powers of 2 are exact over the whole range (including denormals) and must equal std::ldexp,
// powers of 10 are exact as far as they are representable (10^22 in double, 10^10 in float).
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

#include "simple_packing/detail/Utils.h"

#include "../fp_diagnostics.h"

using namespace sp::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

template <typename T>
static void powersOfTwo(int64_t minExponent, int64_t maxExponent)
{
    const std::string type = sizeof(T) == 4 ? "float" : "double";
    for (int64_t e = minExponent; e <= maxExponent; ++e) {
        const T expected = std::ldexp(T(1), static_cast<int>(e));
        check(sp_power<T>(e, 2) == expected, type + " 2^" + std::to_string(e));
    }
    // Beyond the range: infinity and 0
    check(std::isinf(sp_power<T>(maxExponent + 1, 2)), type + " 2^" + std::to_string(maxExponent + 1) + " must be infinite");
    check(sp_power<T>(minExponent - 1, 2) == T(0), type + " 2^" + std::to_string(minExponent - 1) + " must be 0");
}

template <typename T>
static void powersOfTen(int64_t maxExact)
{
    const std::string type = sizeof(T) == 4 ? "float" : "double";
    T expected = 1;
    for (int64_t e = 0; e <= maxExact; ++e) {
        check(sp_power<T>(e, 10) == expected, type + " 10^" + std::to_string(e));
        expected *= 10;
    }
    check(sp_power<T>(-1, 10) == T(1) / T(10), type + " 10^-1");
}

int main()
{
    // The smallest exponents give denormals: 2^-1074 (double), 2^-149 (float)
    powersOfTwo<double>(-1074, 1023);
    powersOfTwo<float>(-149, 127);
    powersOfTen<double>(22);
    powersOfTen<float>(10);
    // Special cases of the loop: s = 0 and s = 1
    check(sp_power<double>(0, 7) == 1 && sp_power<double>(1, 7) == 7 && sp_power<double>(2, 7) == 49 && sp_power<double>(-2, 2) == 0.25, "small exponents");
    if (failures != 0) {
        fp_diagnostics::explain();
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
