// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: the value mapping of the encoder computes in double for every value type, like the
// unmapper of the decoder. For every scaling, spaec::compress<float>(v) and
// spaec::compress<double>((double)v) must give the same compressed bytes: the value is converted
// to double and the factors (10^D, 2^-E) and the reference value are used as doubles. Computing
// the mapping in float rounds the intermediates (the difference of two floats near a large
// reference value needs more than 24 bits), which gave other samples than a double compression
// of the same values.
// The reference value of the Scaling is used as a double for every value type, also when it is not
// exactly representable as a float (compress and decompress of float values must not round it).
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/detail/SpAec.h"
#include "spaec/spaec.h"

static int32_t failures = 0;
static int64_t cases = 0;

static void check(bool ok, const std::string& what)
{
    ++cases;
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

// The SimpleMapper of the float coder must map like the SimpleMapper of the double coder applied
// to the converted values (the value mapping, independent of the CCSDS encoder)
static void mapperCase(float referenceValue, int32_t binaryScaleFactor, int32_t decimalScaleFactor, const std::vector<float>& values,
                       const std::string& what)
{
    const spaec::detail::encoder::SpAec<float>::SimpleMapper<uint32_t> mapperF(referenceValue, binaryScaleFactor, decimalScaleFactor);
    const spaec::detail::encoder::SpAec<double>::SimpleMapper<uint32_t> mapperD(referenceValue, binaryScaleFactor, decimalScaleFactor);
    for (size_t i = 0; i < values.size(); ++i) {
        // The range checks use mapped(): both must agree on what is out of range
        const double mappedF = mapperF.mapped(values[i]);
        const double mappedD = mapperD.mapped(static_cast<double>(values[i]));
        check((mappedF >= 0.0 && mappedF < 4294967296.0) == (mappedD >= 0.0 && mappedD < 4294967296.0),
              what + ": mapped(" + std::to_string(values[i]) + ") range " + std::to_string(mappedF) + " vs " + std::to_string(mappedD));
        if (mappedF >= 0.0 && mappedF < 4294967296.0) {
            check(mapperF(values[i]) == mapperD(static_cast<double>(values[i])),
                  what + ": value " + std::to_string(values[i]) + " maps to different samples");
        }
    }
    cases++;
}

// Compresses the values as floats and as the converted doubles; the compressed bytes must be the
// same. A scaling whose samples do not decode to finite floats is rejected by compress<float>
// (see checkDecodable): both compressions must then throw.
static void sameCodes(const std::vector<float>& values, const spaec::Scaling& s, const spaec::Options& o, const std::string& what)
{
    std::vector<double> doubles(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        doubles[i] = values[i];  // exact conversion
    }
    bool floatThrew = false, doubleThrew = false;
    std::vector<uint8_t> packedF, packedD;
    try {
        packedF = spaec::compress(values, s, o);
    }
    catch (const std::exception&) {
        floatThrew = true;
    }
    try {
        packedD = spaec::compress(doubles, s, o);
    }
    catch (const std::exception&) {
        doubleThrew = true;
    }
    check(floatThrew == doubleThrew, what + ": the float and the double compression must both throw or both succeed");
    if (!floatThrew) {
        check(packedF == packedD, what + ": " + std::to_string(values.size()) + " values compressed differently (" +
                                                      std::to_string(packedF.size()) + " bytes)");
    }
}

// A scaling with all the fields set
struct Conf
{
    int32_t decimalScaleFactor;
    int32_t binaryScaleFactor;
    uint32_t bitsPerSample;
    float referenceValue;
};

static spaec::Scaling scalingOf(const Conf& c)
{
    spaec::Scaling s;
    s.referenceValue = c.referenceValue;
    s.binaryScaleFactor = c.binaryScaleFactor;
    s.decimalScaleFactor = c.decimalScaleFactor;
    return s;
}

// Values whose difference to the reference value is not exactly representable in float: the
// case where a float arithmetic gave other samples (the difference of two floats of magnitude
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
        const double k = static_cast<double>(rng() % (uint64_t{ 1 } << std::min<uint32_t>(c.bitsPerSample, 62)));
        // (v * 10^D - R) * 2^-E = k + 0.5 in double
        values[i] = static_cast<float>((static_cast<double>(c.referenceValue) + (k + 0.5) * divisor) / decimal);
    }
    return values;
}

// Values spread over the whole range of the samples (random bit patterns of the coded values)
static std::vector<float> fullRangeValues(const Conf& c, size_t n, std::mt19937_64& rng)
{
    const double decimal = std::pow(10.0, c.decimalScaleFactor);
    const double divisor = std::pow(2.0, c.binaryScaleFactor);
    std::vector<float> values(n);
    for (size_t i = 0; i < n; ++i) {
        const double k = static_cast<double>(rng() % (uint64_t{ 1 } << std::min<uint32_t>(c.bitsPerSample, 62)));
        values[i] = static_cast<float>((static_cast<double>(c.referenceValue) + k * divisor) / decimal);
    }
    return values;
}

static void confCase(const Conf& c, const std::vector<float>& values, const spaec::Options& o, const std::string& what)
{
    sameCodes(values, scalingOf(c), o, what + " (D=" + std::to_string(c.decimalScaleFactor) +
                                            ", E=" + std::to_string(c.binaryScaleFactor) +
                                            ", bpv=" + std::to_string(c.bitsPerSample) + ")");
    if (values.size() <= 8) {
        mapperCase(c.referenceValue, c.binaryScaleFactor, c.decimalScaleFactor, values, what + ", mapper");
    }
}

static void confCases(const std::vector<Conf>& confs, size_t n, std::mt19937_64& rng)
{
    spaec::Options options;
    for (const Conf& c : confs) {
        const std::string what = std::to_string(n) + " values";
        confCase(c, valuesNearReference(c, n, rng), options, what + " near the reference");
        confCase(c, halfwayValues(c, n, rng), options, what + " at half-way points");
        confCase(c, fullRangeValues(c, n, rng), options, what + " over the full range");
    }
}

int main()
{
    std::mt19937_64 rng(20260227);

    // The scalings of the brief: decimal scale factors != 0 (D = -2, 1, 3), binary scale factors
    // below and above zero, and 1 to 32 bits per value
    std::vector<Conf> confs;
    for (int32_t D : { -2, 0, 1, 3 }) {
        for (int32_t E : { -17, -8, 0, 5, 12 }) {
            for (uint32_t bpv : { 1u, 4u, 8u, 16u, 24u, 32u }) {
                confs.push_back(Conf{ D, E, bpv, 100000.0f });
            }
        }
    }
    // Negative reference values
    confs.push_back(Conf{ 0, -8, 24, -273.15f });

    // Field sizes over the RSI boundaries (blocks of 32 samples, 128 blocks per RSI)
    for (size_t n : { size_t{ 1 }, size_t{ 7 }, size_t{ 32 }, size_t{ 4095 }, size_t{ 4096 }, size_t{ 4097 }, size_t{ 9000 } }) {
        confCases(confs, n, rng);
    }

    // Options: other block sizes and reference sample intervals, no preprocessor, and the
    // parallel encoder (the compressed data must not depend on the number of threads)
    {
        const Conf c{ 1, -8, 16, 100000.0f };
        const std::vector<float> values = valuesNearReference(c, 5000, rng);
        for (uint32_t blockSize : { 8u, 16u, 64u }) {
            spaec::Options o;
            o.blockSize = blockSize;
            confCase(c, values, o, "block size " + std::to_string(blockSize));
        }
        for (uint32_t rsi : { 1u, 512u }) {
            spaec::Options o;
            o.referenceInterval = rsi;
            confCase(c, values, o, "rsi " + std::to_string(rsi));
        }
        spaec::Options none;
        none.preprocessor = spaec::Preprocessor::None;
        confCase(c, values, none, "without preprocessor");
        spaec::Options par;
        par.threads = 4;
        confCase(c, values, par, "4 threads");
    }

    // The values that cannot be coded must be rejected in both paths (a value outside the range
    // of the samples after the scaling, NaN and infinity)
    {
        const spaec::Scaling s{ 100000.0f, -8, 0 };
        const std::vector<float> bad = { 99999.0f, 100000.0f + 512.0f, std::numeric_limits<float>::quiet_NaN(),
                                         std::numeric_limits<float>::infinity() };
        sameCodes(bad, s, spaec::Options(), "values that cannot be coded");
    }

    // A reference value that is not a float: float values are coded and decoded with the reference
    // value as a double, as double values are (rounding it to a float shifts every sample)
    {
        const double reference = 100000.0 + 1.0 / 3.0;
        const spaec::Scaling s{ reference, -8, 0 };
        std::uniform_real_distribution<double> offset(1.0, 200.0);
        std::vector<float> values(5000);
        for (float& v : values) {
            v = static_cast<float>(reference + offset(rng));
        }
        spaec::Options o;
        o.bitsPerSample = 16;
        sameCodes(values, s, o, "reference value that is not a float");
        const std::vector<uint8_t> data = spaec::compress(std::vector<double>(values.begin(), values.end()), s, o);
        const std::vector<float> backF = spaec::decompress<float>(data, values.size(), s, o);
        const std::vector<double> backD = spaec::decompress<double>(data, values.size(), s, o);
        bool same = true;
        for (size_t i = 0; i < values.size(); ++i) {
            same &= backF[i] == static_cast<float>(backD[i]);
        }
        ++cases;
        check(same, "reference value that is not a float: decompress<float> must give the decoded doubles rounded to float");
    }

    if (failures != 0) {
        std::cerr << failures << " failures of " << cases << " cases\n";
        return 1;
    }
    std::cout << "OK " << cases << " cases\n";
    return 0;
}
