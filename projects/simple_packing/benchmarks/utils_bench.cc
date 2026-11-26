// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// usage: utils_bench [--runs N] [--only CASE]
//
// The speed of the utilities of src/Utils.cc, in nanoseconds per call: nearest_smaller_value
// (IEEE and IBM), number_of_bits, get_binary_scale_factor, check_data_values_range, and of
// sp::scaling with the four methods (which call them). Fixed inputs (the same in every run): the
// fastest of N runs (default 7). --only CASE measures one case (e.g. under callgrind:
// valgrind --tool=callgrind utils_bench --runs 1 --only ieee; "none" measures only the setup).
#include <algorithm>
#include <chrono>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "simple_packing/detail/Utils.h"
#include "simple_packing/simple_packing.h"

namespace
{

volatile double sinkDouble = 0;
volatile int64_t sinkInt = 0;

struct Case
{
    const char* name;
    const char* description;
    size_t calls;
    std::function<void()> run;
};

}  // namespace

int main(int argc, char** argv)
{
    int32_t runs = 7;
    std::string only;
    for (int a = 1; a < argc; ++a) {
        const std::string arg = argv[a];
        if (arg == "--runs" && a + 1 < argc) {
            runs = std::max(1, std::stoi(argv[++a]));
        }
        else if (arg == "--only" && a + 1 < argc) {
            only = argv[++a];
        }
    }

    // Inputs: reference values of fields (any sign, magnitudes 1e-6 to 1e6), coded maxima, ranges
    std::mt19937_64 rng(42);
    const size_t n = 100000;
    std::vector<double> values(n), ranges(n), tinyRanges(n);
    std::vector<uint64_t> maxima(n);
    std::vector<int64_t> bits(n);
    for (size_t i = 0; i < n; ++i) {
        const double sign = (rng() & 1) ? 1.0 : -1.0;
        values[i] = sign * std::pow(10.0, -6.0 + 12.0 * static_cast<double>(rng() >> 11) * 0x1p-53);
        ranges[i] = std::pow(10.0, -3.0 + 8.0 * static_cast<double>(rng() >> 11) * 0x1p-53);
        tinyRanges[i] = std::ldexp(1.0 + static_cast<double>(rng() >> 11) * 0x1p-53, -1000 - static_cast<int32_t>(rng() % 70));
        maxima[i] = rng() >> (33 + rng() % 31);
        bits[i] = 8 + static_cast<int64_t>(rng() % 17);  // 8 to 24 bits per value
    }

    using sp::detail::FloatType;
    const std::vector<Case> cases = {
        { "ieee", "nearest_smaller_value, IEEE", n,
          [&] {
              double s = 0;
              for (double v : values)
                  s += *sp::detail::nearest_smaller_value(v, FloatType::IEEE32);
              sinkDouble = s;
          } },
        { "ibm", "nearest_smaller_value, IBM", n,
          [&] {
              double s = 0;
              for (double v : values)
                  s += *sp::detail::nearest_smaller_value(v, FloatType::IBM32);
              sinkDouble = s;
          } },
        { "bits", "number_of_bits", n,
          [&] {
              int64_t s = 0;
              for (uint64_t m : maxima)
                  s += sp::detail::number_of_bits(m).value_or(-1);
              sinkInt = s;
          } },
        { "scale", "get_binary_scale_factor, typical ranges", n,
          [&] {
              int64_t s = 0;
              for (size_t i = 0; i < n; ++i)
                  s += sp::detail::get_binary_scale_factor(ranges[i], 0.0, bits[i]).value_or(0);
              sinkInt = s;
          } },
        { "scale_tiny", "get_binary_scale_factor, tiny ranges", n / 10,
          [&] {
              int64_t s = 0;
              for (size_t i = 0; i < n / 10; ++i)
                  s += sp::detail::get_binary_scale_factor(tinyRanges[i], 0.0, bits[i]).value_or(1000);
              sinkInt = s;
          } },
        { "range", "check_data_values_range", n,
          [&] {
              for (size_t i = 0; i < n; ++i)
                  sp::detail::check_data_values_range(-ranges[i], ranges[i]);
          } },
        { "scaling_bits", "sp::scaling, Method::bits", n,
          [&] {
              double s = 0;
              for (size_t i = 0; i < n; ++i)
                  s += sp::scaling(values[i], values[i] + ranges[i], sp::Method::bits(static_cast<int32_t>(bits[i]))).referenceValue;
              sinkDouble = s;
          } },
        { "scaling_best", "sp::scaling, Method::bestPrecision", n,
          [&] {
              double s = 0;
              for (size_t i = 0; i < n; ++i)
                  s += sp::scaling(values[i], values[i] + ranges[i], sp::Method::bestPrecision(static_cast<int32_t>(bits[i]))).referenceValue;
              sinkDouble = s;
          } },
        { "scaling_decimals", "sp::scaling, Method::decimals", n,
          [&] {
              double s = 0;
              for (size_t i = 0; i < n; ++i)
                  s += sp::scaling(values[i], values[i] + ranges[i], sp::Method::decimals(2)).referenceValue;
              sinkDouble = s;
          } },
        { "scaling_bits_decimals", "sp::scaling, Method::bitsAndDecimals", n,
          [&] {
              double s = 0;
              for (size_t i = 0; i < n; ++i)
                  s += sp::scaling(values[i], values[i] + ranges[i], sp::Method::bitsAndDecimals(static_cast<int32_t>(bits[i]), 2)).referenceValue;
              sinkDouble = s;
          } },
    };

    if (only.empty()) {
        std::printf("simple_packing %s: nanoseconds per call, fastest of %d runs\n\n| Case | ns per call |\n|---|---|\n", sp::version(), runs);
    }
    for (const Case& c : cases) {
        if (!only.empty() && only != c.name) {
            continue;
        }
        double best = 1e30;
        for (int32_t r = 0; r < runs; ++r) {
            const auto t0 = std::chrono::steady_clock::now();
            c.run();
            best = std::min(best, std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count());
        }
        std::printf("| %s (%s) | %.1f |\n", c.description, c.name, best * 1e9 / static_cast<double>(c.calls));
    }
    return 0;
}
