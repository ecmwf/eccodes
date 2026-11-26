// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit tests of the range checks of simple_packing derived from regression tests:
// - pack_value_range: sp_minmax gives NaN, NaN if any value is NaN or infinite, at any position
//   (every lane, the first value, the tail); it ignored NaN
// - field_range: check_data_values_range rejects ranges the optimisers cannot handle (minimum >
//   maximum, NaN, infinity, +-DBL_MAX)
// - reference_value_range: nearest_smaller_value gives no value beyond the float range or for NaN,
//   and exact values at the boundaries
// - scale_factor_range: Parameters is fully initialised (the constructor with values ignored
//   isConstantField)
#include <algorithm>
#include <cstdint>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

#include "simple_packing/detail/Parameters.h"
#include "simple_packing/detail/Utils.h"

#include "../fp_diagnostics.h"

using namespace sp::detail;

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

template <typename T>
static void minmax()
{
    const T nan = std::numeric_limits<T>::quiet_NaN();
    const T inf = std::numeric_limits<T>::infinity();
    // Up to 80 values: several steps of a vectorised loop (16 doubles or 32 floats) and every tail
    for (size_t n = 1; n <= 80; ++n) {
        std::vector<T> values(n);
        for (size_t i = 0; i < n; ++i) {
            values[i] = static_cast<T>((i * 37) % 11) - T(5);
        }
        const auto [mn, mx] = sp_minmax(values.data(), n);
        T emn = values[0], emx = values[0];
        for (T v : values) {
            emn = std::min(emn, v);
            emx = std::max(emx, v);
        }
        check(mn == emn && mx == emx, "sp_minmax of " + std::to_string(n) + " values");
        // The minimum and the maximum at every position (in every lane of a vectorised loop)
        for (size_t pos = 0; pos < n; ++pos) {
            for (T extreme : { T(-1000), T(1000) }) {
                std::vector<T> w = values;
                w[pos] = extreme;
                const auto [a, b] = sp_minmax(w.data(), n);
                const T wmn = *std::min_element(w.begin(), w.end()), wmx = *std::max_element(w.begin(), w.end());
                check(a == wmn && b == wmx && (extreme < 0 ? a : b) == extreme,
                      "sp_minmax with " + std::to_string(extreme) + " at " + std::to_string(pos) + " of " + std::to_string(n));
            }
        }
        for (size_t pos = 0; pos < n; ++pos) {
            for (T bad : { nan, inf, -inf }) {
                std::vector<T> w = values;
                w[pos] = bad;
                const auto [a, b] = sp_minmax(w.data(), n);
                check(std::isnan(a) && std::isnan(b), "sp_minmax with " + std::to_string(bad) + " at " + std::to_string(pos) + " of " + std::to_string(n) + ": " +
                                                          std::to_string(a) + ", " + std::to_string(b));
            }
        }
    }
}

static bool rangeRejected(double mn, double mx)
{
    try {
        check_data_values_range(mn, mx);
    }
    catch (const std::exception&) {
        return true;
    }
    return false;
}

static void dataRange()
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    check(!rangeRejected(-1, 1) && !rangeRejected(0, 0) && !rangeRejected(-1e300, 1e300), "valid ranges must be accepted");
    check(rangeRejected(1, -1), "minimum > maximum must be rejected");
    check(rangeRejected(nan, 1) && rangeRejected(1, nan) && rangeRejected(nan, nan), "NaN must be rejected");
    check(rangeRejected(-inf, 1) && rangeRejected(1, inf), "infinity must be rejected");
    check(rangeRejected(-DBL_MAX, 0) && rangeRejected(0, DBL_MAX), "+-DBL_MAX must be rejected");
}

static void referenceValue()
{
    const double inf = std::numeric_limits<double>::infinity();
    for (double v : { 1e39, -1e39, inf, -inf, std::nan(""), static_cast<double>(FLT_MAX) * 1.0000001 }) {
        check(!nearest_smaller_value(v).has_value(), "nearest float <= " + std::to_string(v) + " must not exist");
    }
    struct Case
    {
        double value, expected;
    };
    // Like the legacy ecCodes table: no denormals (the smallest positive value is FLT_MIN)
    const Case exact[] = { { FLT_MAX, FLT_MAX }, { -FLT_MAX, -FLT_MAX }, { 0.0, 0.0 }, { 1.5, 1.5 }, { -2.0, -2.0 }, { FLT_MIN, FLT_MIN }, { 0x1p-149, 0.0 } };
    for (const auto& c : exact) {
        const auto r = nearest_smaller_value(c.value);
        check(r.has_value() && *r == c.expected, "nearest float <= " + std::to_string(c.value));
    }
    // 0.1 is not a float: the largest float below it
    const auto r = nearest_smaller_value(0.1);
    const float below = std::nextafter(0.1f, 0.0f);  // (float)0.1 is above 0.1
    check(r.has_value() && *r == static_cast<double>(below), "nearest float <= 0.1");
}

// sp_minmax of no values, and get_binary_scale_factor for a zero range and for ranges too small
// for any binary scale factor (also so small that scaling them up would overflow to infinity)
static void emptyAndTinyRanges()
{
    const auto [mn, mx] = sp_minmax(static_cast<const double*>(nullptr), 0);
    check(mn == 0 && mx == 0, "sp_minmax of 0 values");
    for (int64_t bpv : { INT64_C(1), INT64_C(16), INT64_C(63) }) {
        const auto zero = get_binary_scale_factor(5.0, 5.0, bpv);
        check(zero && *zero == 0, "get_binary_scale_factor of a zero range");
        check(!get_binary_scale_factor(1e-300, 0.0, bpv), "get_binary_scale_factor of a range of 1e-300 (scale below -127)");
        check(!get_binary_scale_factor(1e-320, 0.0, bpv), "get_binary_scale_factor of a denormal range");
    }
    // The smallest range of scale -127 is 2^-127 * (2^bpv - 1) (exact in double up to 52 bits)
    for (int64_t bpv : { INT64_C(1), INT64_C(16), INT64_C(52) }) {
        const double smallest = std::ldexp(1.0, -127) * static_cast<double>((uint64_t{ 1 } << bpv) - 1);
        const auto at = get_binary_scale_factor(smallest, 0.0, bpv);
        check(at && *at == -127, "get_binary_scale_factor of the smallest range of scale -127, " + std::to_string(bpv) + " bits");
        check(!get_binary_scale_factor(smallest / 2, 0.0, bpv), "get_binary_scale_factor of half that range, " + std::to_string(bpv) + " bits");
    }
}

static void parameters()
{
    // Constructed in memory that is not zero: members without an initialiser keep the 0xAB bytes
    alignas(Parameters) unsigned char memory[sizeof(Parameters)];
    std::memset(memory, 0xAB, sizeof(memory));
    const Parameters& p = *new (memory) Parameters;
    check(p.bitsPerValue() == 0 && p.decimalScaleFactor() == 0 && p.binaryScaleFactor() == 0 && p.referenceValue() == 0.0 && !p.isConstantField(),
          "a default-constructed Parameters must be initialised");
    // The constructor with values must take isConstantField (it ignored it and left it uninitialised)
    for (bool constant : { false, true }) {
        std::memset(memory, 0xAB, sizeof(memory));
        const Parameters& q = *new (memory) Parameters(16, 2, -3, 1.5, constant);
        check(q.bitsPerValue() == 16 && q.decimalScaleFactor() == 2 && q.binaryScaleFactor() == -3 && q.referenceValue() == 1.5 && q.isConstantField() == constant,
              std::string("Parameters(16, 2, -3, 1.5, ") + (constant ? "true" : "false") + ")");
    }
}

int main()
{
    minmax<double>();
    minmax<float>();
    dataRange();
    referenceValue();
    parameters();
    emptyAndTinyRanges();
    if (failures != 0) {
        fp_diagnostics::explain();
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
