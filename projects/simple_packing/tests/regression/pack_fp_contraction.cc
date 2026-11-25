// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Packing must round every operation of ((value * 10^D) - R) * 2^-E + 0.5 separately, like the
// legacy ecCodes code built without -march=native. With -march=native (the default of
// SIMPLE_PACKING_NATIVE_ARCH), GCC contracted value * 10^D - R into a fused multiply-add, which
// rounds once: values close to a rounding boundary were packed differently (off by one), so the
// GRIB data depended on the machine the library was built on.
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "simple_packing/simple/Binary.h"
#include "simple_packing/optimizer/BinDecOptimizer.h"

using namespace sp;

// The coded value with every operation rounded separately (volatile prevents contraction)
static uint64_t expectedCoded(double value, int decimalScale, int binaryScale, double reference)
{
    volatile double product = value * sp_power<double>(decimalScale, 10);
    volatile double difference = product - reference;
    volatile double scaled = difference * sp_power<double>(-binaryScale, 2);
    volatile double rounded = scaled + 0.5;
    return static_cast<uint64_t>(rounded);
}

static uint64_t packedCoded(double value, int decimalScale, int binaryScale, double reference)
{
    SimplePackingBinary<double> sp(std::make_unique<BinDecOptimizer>(0, 1, 16));
    const std::vector<double> values(8, value);
    std::vector<unsigned char> out(3 * 8 + 8, 0);
    sp.packTo(Parameters(24, decimalScale, binaryScale, reference, false), values.data(), values.size(), out.data());
    return (static_cast<uint64_t>(out[0]) << 16) | (static_cast<uint64_t>(out[1]) << 8) | out[2];
}

int main()
{
    int failures = 0;

    // Values found to be packed differently with a fused multiply-add, and their correct coded values
    struct Case
    {
        double value;
        int decimalScale;
        int binaryScale;
        double reference;
        uint64_t coded;
    };
    const Case cases[] = { { 13136.093945312499, 1, -8, 99646, 8119025 },
                           { 1263.81806640625, 2, -8, 69140, 14653903 },
                           { 6890.2935546874996, 1, -8, 65279, 927728 },
                           { 1025.2067187499999, 2, -5, 54860, 1525142 },
                           { 1211.4638085937499, 2, -8, 87227, 8683362 } };
    for (const auto& c : cases) {
        const uint64_t coded = packedCoded(c.value, c.decimalScale, c.binaryScale, c.reference);
        if (coded != c.coded) {
            std::cerr << "value " << c.value << ": packed " << coded << ", expected " << c.coded << '\n';
            ++failures;
        }
    }

    // Values half-way between two coded values (before rounding the division): the cases most
    // sensitive to the rounding of the intermediate results
    std::mt19937_64 rng(42);
    for (int i = 0; i < 20000; ++i) {
        const int decimalScale = 1 + static_cast<int>(rng() % 3);
        const int binaryScale = -static_cast<int>(rng() % 12);
        const double reference = static_cast<double>(rng() % 100000);
        const double k = static_cast<double>(rng() % (1u << 23));
        const double value = (reference + std::ldexp(k + 0.5, binaryScale)) / sp_power<double>(decimalScale, 10);
        const uint64_t coded = packedCoded(value, decimalScale, binaryScale, reference);
        const uint64_t expected = expectedCoded(value, decimalScale, binaryScale, reference);
        if (coded != expected && failures < 20) {
            std::cerr << "value " << value << " (D=" << decimalScale << ", E=" << binaryScale << ", R=" << reference << "): packed " << coded
                      << ", expected " << expected << '\n';
        }
        failures += coded != expected;
    }

    if (failures != 0) {
        std::cerr << failures << " values packed with a different rounding\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
