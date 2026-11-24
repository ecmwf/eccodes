// Unit tests of the value mappers of SpAec derived from regression tests:
// - decode_float_precision: SimpleUnmapper for float scales in double (powers computed in float)
//   and rounds to float once; computing in float lost the low bits of samples with more than 24
//   bits and gave 0 instead of 0.00042 near a large negative reference value
// - encode_fp_contraction: SimpleMapper rounds every operation of ((v * 10^D) - R) * 2^-E + 0.5
//   separately (with a fused multiply-add, values at a rounding boundary were mapped off by one)
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>

#include "ccsds/SpAec.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

static void unmapperFloat()
{
    struct Case
    {
        float reference;
        int binaryScale;
        int decimalScale;
        uint32_t mapped;
    };
    const Case cases[] = {
        { -1040.0f, -19, -1, (1040u << 19) + 22 },  // (22 * 2^-19) * 10 = 0.00042: 0 in float
        { -1040.0f, -19, -1, (1u << 30) - 1 },
        { -35589.6f, -12, 0, 123456789 },
        { -9.99999f, -27, -2, 0xFFFFFFFFu },
        { -3.0e6f, -8, 3, 0x89ABCDEFu },
        { 12.5f, -20, 1, 33554431 },
    };
    for (const auto& c : cases) {
        encoder::SpAec<float>::SimpleUnmapper<uint32_t> unmapper(c.reference, c.binaryScale, c.decimalScale);
        // ecCodes with libaec: powers in float, scaling in double, one rounding to float
        const double bscale = grib_power<float>(c.binaryScale, 2);
        const double dscale = grib_power<float>(-c.decimalScale, 10);
        const float expected = static_cast<float>((static_cast<double>(c.mapped) * bscale + static_cast<double>(c.reference)) * dscale);
        const float actual = unmapper(c.mapped);
        check(actual == expected, "unmap " + std::to_string(c.mapped) + " (R=" + std::to_string(c.reference) + "): " + std::to_string(actual) +
                                      " instead of " + std::to_string(expected));
    }
}

// The mapped value with every operation rounded separately (volatile prevents contraction)
static uint32_t separate(double value, int decimalScale, int binaryScale, double reference)
{
    volatile double product = value * grib_power<double>(decimalScale, 10);
    volatile double difference = product - reference;
    volatile double scaled = difference * grib_power<double>(-binaryScale, 2);
    volatile double rounded = scaled + 0.5;
    return static_cast<uint32_t>(rounded);
}

static void mapperRounding()
{
    struct Case
    {
        double value;
        int decimalScale;
        int binaryScale;
        double reference;
        uint32_t mapped;
    };
    // Values at a rounding boundary (a fused multiply-add maps them one lower)
    const Case cases[] = { { 13136.093945312499, 1, -8, 99646, 8119025 },
                           { 1263.81806640625, 2, -8, 69140, 14653903 },
                           { 6890.2935546874996, 1, -8, 65279, 927728 },
                           { 1025.2067187499999, 2, -5, 54860, 1525142 },
                           { 1211.4638085937499, 2, -8, 87227, 8683362 } };
    for (const auto& c : cases) {
        encoder::SpAec<double>::SimpleMapper<uint32_t> mapper(c.reference, c.binaryScale, c.decimalScale);
        const uint32_t mapped = mapper(c.value);
        check(mapped == c.mapped, "map " + std::to_string(c.value) + ": " + std::to_string(mapped) + " instead of " + std::to_string(c.mapped));
    }
    // Values half-way between two mapped values (before rounding the division)
    std::mt19937_64 rng(42);
    int wrong = 0;
    for (int i = 0; i < 2000; ++i) {
        const int decimalScale = 1 + static_cast<int>(rng() % 3);
        const int binaryScale = -static_cast<int>(rng() % 12);
        const double reference = static_cast<double>(rng() % 100000);
        const double k = static_cast<double>(rng() % (1u << 23));
        const double value = (reference + std::ldexp(k + 0.5, binaryScale)) / grib_power<double>(decimalScale, 10);
        encoder::SpAec<double>::SimpleMapper<uint32_t> mapper(reference, binaryScale, decimalScale);
        wrong += mapper(value) != separate(value, decimalScale, binaryScale, reference);
    }
    check(wrong == 0, std::to_string(wrong) + " of 2000 half-way values mapped with a different rounding");
}

int main()
{
    unmapperFloat();
    mapperRounding();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
