// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the interface of spaec (spaec/spaec.h):
// - compress and decompress of integer samples of every type, signed samples as signed types, all
//   preprocessors, both code option sets, with vectors and with buffers; threads give the same data
// - output buffers: any buffer that holds the compressed data is accepted, a smaller one is a
//   std::length_error and is not written beyond its end
// - floating-point values with a scaling
// - errors: invalid options (std::invalid_argument, naming the option), corrupt and truncated
//   data (spaec::DataError)

#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/spaec.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

template <typename E, typename F>
static void expect(const std::string& what, const std::string& message, F&& f)
{
    try {
        f();
        check(false, what + ": no exception");
    }
    catch (const E& e) {
        check(std::string(e.what()).find(message) != std::string::npos, what + ": message '" + e.what() + "' does not contain '" + message + "'");
    }
    catch (const std::exception& e) {
        check(false, what + ": other exception: " + e.what());
    }
}

// A smooth field with noise in the range of the samples
template <typename Sample>
static std::vector<Sample> field(size_t count, unsigned bits, bool isSigned, unsigned seed)
{
    std::mt19937 rng(seed);
    const int64_t lo = isSigned ? -(int64_t{ 1 } << (bits - 1)) : 0;
    const int64_t hi = isSigned ? (int64_t{ 1 } << (bits - 1)) - 1 : (int64_t{ 1 } << bits) - 1;
    std::vector<Sample> samples(count);
    int64_t v = (lo + hi) / 2;
    for (auto& s : samples) {
        v += static_cast<int64_t>(rng() % 9) - 4;
        if (rng() % 500 == 0) v = lo + static_cast<int64_t>(rng() % static_cast<uint64_t>(hi - lo + 1));
        v = std::min(hi, std::max(lo, v));
        s = static_cast<Sample>(v);
    }
    return samples;
}

template <typename Sample>
static void roundTrip(unsigned bits, const spaec::Options& base, const std::string& what)
{
    spaec::Options options = base;
    options.bitsPerSample = bits;
    for (size_t count : { size_t{ 0 }, size_t{ 1 }, size_t{ 100 }, size_t{ 5000 } }) {
        const auto samples = field<Sample>(count, bits, options.signedSamples, bits + static_cast<unsigned>(count));
        const std::vector<uint8_t> data = spaec::compress(samples, options);
        check(data.size() <= spaec::maxCompressedSize(count, options), what + ": larger than maxCompressedSize");
        check(spaec::decompress<Sample>(data, count, options) == samples, what + " count=" + std::to_string(count));

        // The same data with 4 threads and with buffers
        spaec::Options threaded = options;
        threaded.threads = 4;
        check(spaec::compress(samples, threaded) == data, what + ": 4 threads give other data");
        check(spaec::decompress<Sample>(data, count, threaded) == samples, what + ": decompress with 4 threads");
        std::vector<uint8_t> exact(data.size() + 1, 0xAB);  // a buffer of exactly the compressed size
        check(spaec::compress(samples.data(), count, options, exact.data(), data.size()) == data.size() &&
                  std::equal(data.begin(), data.end(), exact.begin()) && exact[data.size()] == 0xAB,
              what + ": buffer of the compressed size");
        std::vector<Sample> back(count);
        spaec::decompress(data.data(), data.size(), options, back.data(), count);
        check(back == samples, what + ": decompress into a buffer");
    }
}

int main()
{
    // Defaults: 16 bits per sample, blocks of 32, reference interval 128, basic options, unit delay
    {
        const spaec::Options options;
        check(options.bitsPerSample == 16 && options.blockSize == 32 && options.referenceInterval == 128 && options.threads == 1, "defaults");
        roundTrip<uint16_t>(16, options, "defaults");
    }

    for (spaec::Preprocessor preprocessor : { spaec::Preprocessor::UnitDelay, spaec::Preprocessor::Bypass, spaec::Preprocessor::None }) {
        spaec::Options options;
        options.preprocessor = preprocessor;
        options.blockSize = 16;
        options.referenceInterval = 4;
        const std::string name = preprocessor == spaec::Preprocessor::UnitDelay ? "unit delay" : preprocessor == spaec::Preprocessor::Bypass ? "bypass"
                                                                                                                                             : "none";
        roundTrip<uint8_t>(7, options, name + " uint8_t 7 bits");
        roundTrip<uint16_t>(12, options, name + " uint16_t 12 bits");
        roundTrip<uint32_t>(12, options, name + " uint32_t 12 bits");
        roundTrip<uint32_t>(32, options, name + " uint32_t 32 bits");
        if (preprocessor != spaec::Preprocessor::None) {
            options.signedSamples = true;
            roundTrip<int8_t>(8, options, name + " int8_t 8 bits");
            roundTrip<int16_t>(11, options, name + " int16_t 11 bits");
            roundTrip<int32_t>(24, options, name + " int32_t 24 bits");
        }
    }
    {
        spaec::Options options;
        options.codeOptions = spaec::CodeOptions::Restricted;
        roundTrip<uint8_t>(3, options, "restricted 3 bits");
        roundTrip<uint16_t>(12, options, "restricted 12 bits (the basic set)");
    }

    // Output buffers that are too small: an error, and nothing is written beyond their end
    {
        spaec::Options options;
        const auto samples = field<uint16_t>(3000, 16, false, 7);
        const std::vector<uint8_t> data = spaec::compress(samples, options);
        std::vector<uint8_t> small(data.size(), 0xCD);
        expect<std::length_error>("buffer one byte too small", "too small",
                                  [&] { spaec::compress(samples.data(), samples.size(), options, small.data(), data.size() - 1); });
        check(small[data.size() - 1] == 0xCD, "the byte after a too small buffer was written");
    }

    // Floating-point values with a scaling: value = (reference + sample * 2^E) * 10^-D
    {
        spaec::Options options;
        options.bitsPerSample = 12;
        const spaec::Scaling scaling{ 250.0, -4, 0 };
        const auto samples = field<uint16_t>(4000, 12, false, 3);
        std::vector<double> values(samples.size());
        std::vector<float> floats(samples.size());
        for (size_t i = 0; i < samples.size(); ++i) {
            values[i] = 250.0 + samples[i] / 16.0;
            floats[i] = static_cast<float>(values[i]);
        }
        const std::vector<uint8_t> data = spaec::compress(values, scaling, options);
        check(data == spaec::compress(samples, options), "scaled values give the data of their samples");
        check(spaec::decompress<double>(data, values.size(), scaling, options) == values, "double values");
        check(spaec::compress(floats, scaling, options) == data, "float values give the same data");
        check(spaec::decompress<float>(data, floats.size(), scaling, options) == floats, "float values");
        std::vector<double> outOfRange = values;
        outOfRange[17] = 1e6;
        expect<std::invalid_argument>("value out of range", "cannot be packed", [&] { spaec::compress(outOfRange, scaling, options); });
        options.signedSamples = true;
        expect<std::invalid_argument>("scaling with signed samples", "signedSamples", [&] { spaec::compress(values, scaling, options); });
    }

    // Invalid options
    {
        const std::vector<uint16_t> samples(64, 1);
        auto with = [](auto set) {
            spaec::Options o;
            set(o);
            return o;
        };
        using IA = std::invalid_argument;
        expect<IA>("0 bits", "bits per value", [&] { spaec::validate(with([](spaec::Options& o) { o.bitsPerSample = 0; })); });
        expect<IA>("33 bits", "bits per value", [&] { spaec::validate(with([](spaec::Options& o) { o.bitsPerSample = 33; })); });
        expect<IA>("block size 12", "block size", [&] { spaec::validate(with([](spaec::Options& o) { o.blockSize = 12; })); });
        expect<IA>("reference interval 0", "reference sample interval", [&] { spaec::validate(with([](spaec::Options& o) { o.referenceInterval = 0; })); });
        expect<IA>("reference interval 4097", "reference sample interval", [&] { spaec::validate(with([](spaec::Options& o) { o.referenceInterval = 4097; })); });
        expect<IA>("0 threads", "threads", [&] { spaec::validate(with([](spaec::Options& o) { o.threads = 0; })); });
        expect<IA>("restricted with 6 bits", "restricted", [&] {
            spaec::validate(with([](spaec::Options& o) { o.bitsPerSample = 6; o.codeOptions = spaec::CodeOptions::Restricted; }));
        });
        expect<IA>("signed samples without preprocessor", "need a preprocessor", [&] {
            spaec::validate(with([](spaec::Options& o) { o.signedSamples = true; o.preprocessor = spaec::Preprocessor::None; }));
        });
        expect<IA>("20 bits in uint16_t", "bits per value", [&] { spaec::compress(samples, with([](spaec::Options& o) { o.bitsPerSample = 20; })); });
        spaec::validate(spaec::Options{});  // valid: no exception
    }

    // Corrupt and truncated data
    {
        const spaec::Options options;
        const auto samples = field<uint16_t>(2000, 16, false, 11);
        std::vector<uint8_t> data = spaec::compress(samples, options);
        data.resize(data.size() / 2);
        expect<spaec::DataError>("truncated data", "truncated or corrupt", [&] { spaec::decompress<uint16_t>(data, samples.size(), options); });
        expect<spaec::DataError>("no data", "truncated or corrupt", [&] { spaec::decompress<uint16_t>(std::vector<uint8_t>{}, 100, options); });
        try {
            spaec::decompress<uint16_t>(data, samples.size(), options);
        }
        catch (const std::runtime_error&) {  // DataError is a std::runtime_error
        }
    }

    if (failures) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "interface ok\n";
    return 0;
}
