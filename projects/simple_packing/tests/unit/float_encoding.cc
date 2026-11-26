// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: the encoding of float values computes in double, like the decoding. For every
// scaling, sp::pack<float>(v) and sp::pack<double>((double)v) must give the same coded bytes:
// the value is converted to double and the factors (10^D, 2^-E) and the reference value are used
// as doubles. Computing the scaling in float rounds the intermediates (the difference of two
// floats near a large reference value needs more than 24 bits), which gave other codes than a
// double set of the same values.
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "simple_packing/simple_packing.h"

static int failures = 0;
static int64_t cases = 0;

static void check(bool ok, const std::string& what)
{
    ++cases;
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

// Packs the values as floats and as the converted doubles; the coded bytes must be the same.
// A scaling whose codes do not decode to finite floats is rejected by pack<float> (the codes
// would decode to infinity, see checkDecodable): both packs must then throw.
static void sameCodes(const std::vector<float>& values, const sp::Scaling& s, const std::string& what)
{
    std::vector<double> doubles(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        doubles[i] = values[i];  // exact conversion
    }
    bool floatThrew = false, doubleThrew = false;
    std::vector<uint8_t> packedF, packedD;
    try {
        packedF = sp::pack(values, s);
    }
    catch (const std::exception&) {
        floatThrew = true;
    }
    try {
        packedD = sp::pack(doubles, s);
    }
    catch (const std::exception&) {
        doubleThrew = true;
    }
    check(floatThrew == doubleThrew, what + ": the float and the double pack must both throw or both succeed");
    if (!floatThrew) {
        check(packedF == packedD, what + ": " + std::to_string(values.size()) + " values packed differently (" +
                                                      std::to_string(packedF.size()) + " bytes)");
    }
}

// A scaling with all the fields set
struct Conf
{
    int decimalScaleFactor;
    int binaryScaleFactor;
    int bitsPerValue;
    double referenceValue;
};

static sp::Scaling scalingOf(const Conf& c)
{
    sp::Scaling s;
    s.referenceValue = c.referenceValue;
    s.binaryScaleFactor = c.binaryScaleFactor;
    s.decimalScaleFactor = c.decimalScaleFactor;
    s.bitsPerValue = c.bitsPerValue;
    return s;
}

// Values whose difference to the reference value is not exactly representable in float: the
// case where a float arithmetic gave other codes (the difference of two floats of magnitude
// 10^5 has more significand bits than a float holds)
static std::vector<float> valuesNearReference(const Conf& c, size_t n, std::mt19937_64& rng)
{
    std::vector<float> values(n);
    for (size_t i = 0; i < n; ++i) {
        const double offset = static_cast<double>(static_cast<int64_t>(rng() % 2000001) - 1000000) / 1024.0;
        values[i] = static_cast<float>(static_cast<double>(c.referenceValue) + offset);
    }
    return values;
}

// Values that map to half-way points x.5 after the scaling (the cases most sensitive to the
// rounding of the intermediates)
static std::vector<float> halfwayValues(const Conf& c, size_t n, std::mt19937_64& rng)
{
    const double decimal = std::pow(10.0, c.decimalScaleFactor);
    const double divisor = std::pow(2.0, c.binaryScaleFactor);
    std::vector<float> values(n);
    for (size_t i = 0; i < n; ++i) {
        const double k = static_cast<double>(rng() % (uint64_t{ 1 } << std::min(c.bitsPerValue, 62)));
        // (v * 10^D - R) * 2^-E = k + 0.5 in double
        values[i] = static_cast<float>((static_cast<double>(c.referenceValue) + (k + 0.5) * divisor) / decimal);
    }
    return values;
}

// Values spread over the whole range of the codes (random bit patterns of the coded values)
static std::vector<float> fullRangeValues(const Conf& c, size_t n, std::mt19937_64& rng)
{
    const double decimal = std::pow(10.0, c.decimalScaleFactor);
    const double divisor = std::pow(2.0, c.binaryScaleFactor);
    std::vector<float> values(n);
    for (size_t i = 0; i < n; ++i) {
        const double k = static_cast<double>(rng() % (uint64_t{ 1 } << std::min(c.bitsPerValue, 62)));
        values[i] = static_cast<float>((static_cast<double>(c.referenceValue) + k * divisor) / decimal);
    }
    return values;
}

static void confCase(const Conf& c, const std::vector<float>& values, const std::string& what)
{
    sameCodes(values, scalingOf(c), what + " (D=" + std::to_string(c.decimalScaleFactor) +
                                        ", E=" + std::to_string(c.binaryScaleFactor) +
                                        ", bpv=" + std::to_string(c.bitsPerValue) + ")");
}

static void confCases(const std::vector<Conf>& confs, size_t n, std::mt19937_64& rng)
{
    for (const Conf& c : confs) {
        const std::string what = std::to_string(n) + " values";
        confCase(c, valuesNearReference(c, n, rng), what + " near the reference");
        confCase(c, halfwayValues(c, n, rng), what + " at half-way points");
        confCase(c, fullRangeValues(c, n, rng), what + " over the full range");
    }
}

int main()
{
    std::mt19937_64 rng(20260227);

    // The scalings of the brief: decimal scale factors != 0 (D = -2, 1, 3), binary scale factors
    // below and above zero, and 1 to 32 (and 63) bits per value
    std::vector<Conf> confs;
    for (int D : { -2, 0, 1, 3 }) {
        for (int E : { -17, -8, 0, 5, 12 }) {
            for (int bpv : { 1, 4, 8, 16, 24, 31, 32, 48, 63 }) {
                confs.push_back(Conf{ D, E, bpv, 100000.0 });
            }
        }
    }
    // A reference value that is not exactly representable as a float: the reference value is
    // used as a double in the encoding (as the double path does)
    confs.push_back(Conf{ 0, -8, 24, 0.1 });
    confs.push_back(Conf{ 2, 3, 16, 0.1 });
    // Negative reference values
    confs.push_back(Conf{ 0, -8, 24, -273.15 });

    // Field sizes over the chunk boundaries of the codecs (chunks of 512 values, blocks of 8)
    for (size_t n : { size_t{ 1 }, size_t{ 7 }, size_t{ 8 }, size_t{ 9 }, size_t{ 511 }, size_t{ 512 }, size_t{ 513 }, size_t{ 1200 } }) {
        confCases(confs, n, rng);
    }

    // The scaling of a field (as ecCodes computes it: the reference value is a float that the
    // optimizer chose to be exactly representable)
    {
        std::vector<float> field(3000);
        for (size_t i = 0; i < field.size(); ++i) {
            field[i] = static_cast<float>(280.0 - static_cast<double>(rng() % 60000) / 100.0);
        }
        for (int bits : { 8, 16, 24 }) {
            const sp::Scaling s = sp::scaling(field, sp::Method::bits(bits));
            sameCodes(field, s, "sp::scaling, " + std::to_string(bits) + " bits");
            const sp::Scaling sd = sp::scaling(field, sp::Method::bestPrecision(bits));
            sameCodes(field, sd, "sp::scaling bestPrecision, " + std::to_string(bits) + " bits");
            const sp::Scaling sdd = sp::scaling(field, sp::Method::bitsAndDecimals(bits, 2));
            sameCodes(field, sdd, "sp::scaling bitsAndDecimals, " + std::to_string(bits) + " bits, D=2");
        }
        // A constant field: no coded data, the reference value is the value
        const std::vector<float> constant(1000, field[0]);
        sameCodes(constant, sp::scaling(constant, sp::Method::bits(16)), "constant field");
    }

    // The values that cannot be coded must be rejected in both paths (a value below the
    // reference value, 2^bpv or more after the scaling, NaN and infinity)
    {
        const sp::Scaling s = scalingOf(Conf{ 0, -8, 24, 100000.0 });
        const std::vector<float> bad = { 99999.0f, 100000.0f + 256.0f, std::numeric_limits<float>::quiet_NaN(),
                                         std::numeric_limits<float>::infinity() };
        sameCodes(bad, s, "values that cannot be coded");
    }

    if (failures != 0) {
        std::cerr << failures << " failures of " << cases << " cases\n";
        return 1;
    }
    std::cout << "OK " << cases << " cases\n";
    return 0;
}
