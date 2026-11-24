// Unit test of the SpAec class (the interface used by ecCodes) and of the decode variants of
// DecoderSeq it uses:
// - the constructor rejects invalid parameters (bits per value, block size, RSI, thread count,
//   code option set, scale factors beyond +-32767, non-finite reference value)
// - encode/decode round trip of exactly representable values for 1 to 32 bits per value, all
//   block sizes, several RSIs and lengths (0, partial blocks and RSIs), double and float, with one
//   and several threads (identical streams)
// - encode rejects values that cannot be coded (first, middle and last position) and too small
//   targets; decode rejects truncated data
// - DecoderSeq::decode, decodeFused and decodeSpans give the same values
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/SpAec.h"
#include "spaec/encoder/EncoderSeq.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

template <typename F>
static bool throwsInvalid(F&& f)
{
    try {
        f();
    }
    catch (const std::invalid_argument&) {
        return true;
    }
    catch (...) {
        return false;
    }
    return false;
}

static void constructor()
{
    using S = encoder::SpAec<double>;
    check(!throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, 1.5, -3, 2, 4); }), "valid parameters must be accepted");
    check(throwsInvalid([] { S(0, 32, 128, CodeOptionSet::Basic, 0); }), "0 bits per value");
    check(throwsInvalid([] { S(33, 32, 128, CodeOptionSet::Basic, 0); }), "33 bits per value");
    check(throwsInvalid([] { S(16, 12, 128, CodeOptionSet::Basic, 0); }), "block size 12");
    check(throwsInvalid([] { S(16, 32, 0, CodeOptionSet::Basic, 0); }), "RSI 0");
    check(throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, 0, 0, 0, 0); }), "0 threads");
    check(throwsInvalid([] { S(8, 32, 128, CodeOptionSet::Restricted, 0); }), "restricted code option set");
    check(throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, 0, 32768, 0); }), "binary scale factor 32768");
    check(throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, 0, -32768, 0); }), "binary scale factor -32768");
    check(throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, 0, 0, 32768); }), "decimal scale factor 32768");
    check(!throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, 0, 32767, -32767); }), "scale factors +-32767 must be accepted");
    check(throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, std::nan("")); }), "NaN reference value");
    check(throwsInvalid([] { S(16, 32, 128, CodeOptionSet::Basic, -std::numeric_limits<double>::infinity()); }), "infinite reference value");
}

// Values (s * 2^E + R) * 10^-D with E = -2, D = 0, R = -100: exact in float and double up to 22 bits
template <typename T>
static std::vector<T> values(size_t n, unsigned bpv, std::mt19937_64& rng)
{
    const uint64_t maxSample = (uint64_t{ 1 } << bpv) - 1;
    std::vector<T> v(n);
    for (size_t i = 0; i < n; ++i) {
        const uint64_t s = (i % 50 < 10) ? 0 : (i % 7 == 0 ? maxSample : rng() & maxSample);  // zero runs, limits, noise
        v[i] = static_cast<T>(std::ldexp(static_cast<double>(s), -2) - 100.0);
    }
    return v;
}

template <typename T>
static void roundTrip(unsigned bpv, unsigned blockSize, unsigned rsi, size_t n, std::mt19937_64& rng)
{
    const std::vector<T> input = values<T>(n, bpv, rng);
    const std::string name = std::string(sizeof(T) == 4 ? "float" : "double") + " bpv=" + std::to_string(bpv) + " bs=" + std::to_string(blockSize) +
                             " rsi=" + std::to_string(rsi) + " n=" + std::to_string(n);
    std::vector<uint8_t> first;
    for (uint8_t threads : { 1, 4 }) {
        encoder::SpAec<T> spaec(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), static_cast<uint16_t>(rsi), CodeOptionSet::Basic, T(-100), -2, 0, threads);
        std::vector<T> data = input;
        std::vector<uint8_t> stream(spaec.maxEncodedSize(n));
        const size_t length = spaec.encode(Span<T>(data), Span<uint8_t>(stream));
        stream.resize(length);
        if (threads == 1) {
            first = stream;
        }
        else {
            check(stream == first, name + ": streams of 1 and 4 threads differ");
        }
        std::vector<T> decoded(n);
        spaec.decode(Span<uint8_t>(stream), Span<T>(decoded));
        check(decoded == input, name + ", " + std::to_string(threads) + " threads: decoded values differ");
    }
}

static void roundTrips()
{
    std::mt19937_64 rng(1);
    for (unsigned bpv : { 1, 2, 7, 8, 9, 15, 16, 17, 22 }) {
        for (unsigned blockSize : { 8, 16, 32, 64 }) {
            for (unsigned rsi : { 1, 3, 64 }) {
                for (size_t n : { size_t{ 0 }, size_t{ 1 }, size_t{ 7 }, size_t{ 8 }, size_t{ 9 }, size_t{ 1000 } }) {
                    roundTrip<double>(bpv, blockSize, rsi, n, rng);
                    roundTrip<float>(bpv, blockSize, rsi, n, rng);
                }
            }
        }
    }
    for (unsigned bpv : { 24, 25, 31, 32 }) {  // beyond the float mantissa: double only
        roundTrip<double>(bpv, 32, 128, 5000, rng);
    }
}

static void rejections()
{
    using S = encoder::SpAec<double>;
    S spaec(8, 16, 4, CodeOptionSet::Basic, 10.0);  // values 10 .. 265
    for (size_t pos : { size_t{ 0 }, size_t{ 33 }, size_t{ 99 } }) {
        for (double bad : { 9.0, 266.0, std::nan(""), std::numeric_limits<double>::infinity() }) {
            std::vector<double> v(100, 100.0);
            v[pos] = bad;
            std::vector<uint8_t> stream(spaec.maxEncodedSize(v.size()));
            check(throwsInvalid([&] { spaec.encode(Span<double>(v), Span<uint8_t>(stream)); }), "value " + std::to_string(bad) + " at " + std::to_string(pos) + " must be rejected");
        }
    }
    std::vector<double> v(100, 265.0);
    std::vector<uint8_t> exact(spaec.maxEncodedSize(v.size())), small(exact.size() - 1);
    check(!throwsInvalid([&] { spaec.encode(Span<double>(v), Span<uint8_t>(exact)); }), "a target of maxEncodedSize bytes must be accepted");
    check(throwsInvalid([&] { spaec.encode(Span<double>(v), Span<uint8_t>(small)); }), "a smaller target must be rejected");

    // Truncated data: every prefix of a stream of noise must fail to decode 100 values
    std::mt19937_64 rng(2);
    std::vector<double> noise(100);
    for (auto& x : noise)
        x = 10.0 + static_cast<double>(rng() % 256);
    std::vector<uint8_t> stream(spaec.maxEncodedSize(noise.size()));
    stream.resize(spaec.encode(Span<double>(noise), Span<uint8_t>(stream)));
    for (size_t length = 0; length + 1 < stream.size(); ++length) {
        std::vector<uint8_t> prefix(stream.begin(), stream.begin() + static_cast<std::ptrdiff_t>(length));
        std::vector<double> out(noise.size());
        bool thrown = false;
        try {
            spaec.decode(Span<uint8_t>(prefix), Span<double>(out));
        }
        catch (const std::runtime_error&) {
            thrown = true;
        }
        check(thrown, "decoding " + std::to_string(length) + " of " + std::to_string(stream.size()) + " bytes must fail");
    }
}

static void decodeVariants()
{
    std::mt19937_64 rng(3);
    for (size_t n : { size_t{ 1 }, size_t{ 100 }, size_t{ 4096 }, size_t{ 5000 } }) {
        std::vector<uint16_t> samples(n);
        for (size_t i = 0; i < n; ++i)
            samples[i] = static_cast<uint16_t>((i % 300 < 40) ? 0 : rng() & 0xFFF);
        encoder::EncoderSeq<uint16_t, uint16_t> encoder(12, 32, 128, CodeOptionSet::Basic);
        std::vector<uint8_t> stream(n * 3 + 64, 0);
        stream.resize(encoder.encode(Span<uint16_t>(samples), Span<uint8_t>(stream)));
        DecoderSeq<uint16_t, uint16_t> decoder(12, 32, 128, CodeOptionSet::Basic);
        std::vector<uint16_t> a(n), b(n), c(n);
        decoder.decode(Span<uint8_t>(stream), Span<uint16_t>(a));
        decoder.decodeFused(Span<uint8_t>(stream), n, [&](uint16_t v, size_t i) { b[i] = v; });
        decoder.decodeSpans(Span<uint8_t>(stream), n, [&](const uint16_t* v, size_t count, size_t offset) {
            for (size_t i = 0; i < count; ++i)
                c[offset + i] = v[i];
        });
        check(a == samples && b == samples && c == samples, "decode, decodeFused and decodeSpans of " + std::to_string(n) + " values");
    }

    // Fewer values than encoded, ending inside a run of 3 zero blocks (not "remainder of segment"):
    // the run of the last RSI reaches beyond the requested values (allowed: decoding stops when the
    // output is full)
    {
        const size_t n = 4096 + 1000, runStart = 4096 + 256;
        std::vector<uint16_t> samples(n);
        for (size_t i = 0; i < n; ++i)
            samples[i] = static_cast<uint16_t>(1 + rng() % 4000);
        for (size_t i = runStart; i < runStart + 96; ++i)
            samples[i] = samples[runStart - 1];  // preprocessed: 0
        encoder::EncoderSeq<uint16_t, uint16_t> encoder(12, 32, 128, CodeOptionSet::Basic);
        std::vector<uint8_t> stream(n * 3 + 64, 0);
        stream.resize(encoder.encode(Span<uint16_t>(samples), Span<uint8_t>(stream)));
        for (size_t inRun : { size_t{ 1 }, size_t{ 40 }, size_t{ 64 }, size_t{ 95 } }) {
            const size_t m = runStart + inRun;
            DecoderSeq<uint16_t, uint16_t> decoder(12, 32, 128, CodeOptionSet::Basic);
            std::vector<uint16_t> a(m), b(m, 7), c(m, 7);
            const std::string name = std::to_string(m) + " values, ending inside a zero-block run";
            try {
                decoder.decode(Span<uint8_t>(stream), Span<uint16_t>(a));
                decoder.decodeFused(Span<uint8_t>(stream), m, [&](uint16_t v, size_t i) { b[i] = v; });
                decoder.decodeSpans(Span<uint8_t>(stream), m, [&](const uint16_t* v, size_t count, size_t offset) {
                    for (size_t i = 0; i < count; ++i)
                        c[offset + i] = v[i];
                });
                const std::vector<uint16_t> expected(samples.begin(), samples.begin() + static_cast<std::ptrdiff_t>(m));
                check(a == expected && b == expected && c == expected, name);
            }
            catch (const std::exception& e) {
                check(false, name + ": " + e.what());
            }
        }
    }

    // Fewer values than encoded, the field ending in zeros ("remainder of segment")
    for (size_t n : { size_t{ 300 }, size_t{ 4096 + 900 }, size_t{ 3 * 4096 } }) {
        std::vector<uint16_t> samples(n, 0);
        for (size_t i = 0; i + 300 < n; ++i)
            samples[i] = static_cast<uint16_t>(rng() & 0xFFF);
        encoder::EncoderSeq<uint16_t, uint16_t> encoder(12, 32, 128, CodeOptionSet::Basic);
        std::vector<uint8_t> stream(n * 3 + 64, 0);
        stream.resize(encoder.encode(Span<uint16_t>(samples), Span<uint8_t>(stream)));
        for (size_t fewer : { size_t{ 1 }, size_t{ 31 }, size_t{ 32 }, size_t{ 33 }, size_t{ 200 }, size_t{ 299 } }) {
            const size_t m = n - fewer;
            DecoderSeq<uint16_t, uint16_t> decoder(12, 32, 128, CodeOptionSet::Basic);
            std::vector<uint16_t> a(m), b(m, 7), c(m, 7);
            const std::string name = std::to_string(m) + " of " + std::to_string(n) + " values ending in zeros";
            try {
                decoder.decode(Span<uint8_t>(stream), Span<uint16_t>(a));
                decoder.decodeFused(Span<uint8_t>(stream), m, [&](uint16_t v, size_t i) { b[i] = v; });
                decoder.decodeSpans(Span<uint8_t>(stream), m, [&](const uint16_t* v, size_t count, size_t offset) {
                    for (size_t i = 0; i < count; ++i)
                        c[offset + i] = v[i];
                });
                const std::vector<uint16_t> expected(samples.begin(), samples.begin() + static_cast<std::ptrdiff_t>(m));
                check(a == expected && b == expected && c == expected, name);
            }
            catch (const std::exception& e) {
                check(false, name + ": " + e.what());
            }
        }
    }
}

// The restricted set of code options: 1 to 4 bits per value (the stream of EncoderSeq
// with the restricted set), rejected for 5 to 8 bits, the basic set for more bits (its IDs are the
// same)
static void restrictedSet()
{
    using S = encoder::SpAec<double>;
    std::mt19937_64 rng(5);
    for (unsigned bpv : { 1u, 2u, 3u, 4u, 12u }) {
        for (unsigned blockSize : { 8u, 64u }) {
            const unsigned rsi = 3;
            const size_t n = 5000;
            std::vector<uint16_t> samples(n);
            uint16_t v = 0;
            for (size_t i = 0; i < n; ++i) {
                if (rng() % 3 == 0)
                    v = static_cast<uint16_t>(rng() & ((1u << bpv) - 1));
                samples[i] = v;
            }
            const auto cos = bpv <= 4 ? CodeOptionSet::Restricted : CodeOptionSet::Basic;
            encoder::EncoderSeq<uint16_t, uint16_t> reference(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), rsi, cos);
            std::vector<uint8_t> expected(n * 3 + 64, 0);
            expected.resize(reference.encode(Span<uint16_t>(samples), Span<uint8_t>(expected)));

            const std::vector<double> input(samples.begin(), samples.end());
            for (uint8_t threads : { 1, 3 }) {
                const std::string what = "restricted set, " + std::to_string(bpv) + " bits, block size " + std::to_string(blockSize) + ", " + std::to_string(threads) + " threads";
                try {
                    S spaec(static_cast<uint8_t>(bpv), static_cast<uint8_t>(blockSize), rsi, CodeOptionSet::Restricted, 0.0, 0, 0, threads);
                    std::vector<uint8_t> stream(spaec.maxEncodedSize(n), 0);
                    stream.resize(spaec.encode(Span<double>(const_cast<double*>(input.data()), n), Span<uint8_t>(stream)));
                    check(stream == expected, what + ": stream differs from EncoderSeq");
                    std::vector<double> decoded(n);
                    spaec.decode(Span<uint8_t>(stream), Span<double>(decoded));
                    check(decoded == input, what + ": decoded values differ");
                }
                catch (const std::exception& e) {
                    check(false, what + ": " + e.what());
                }
            }
        }
    }
    for (uint8_t bpv = 5; bpv <= 8; ++bpv) {
        check(throwsInvalid([bpv] { S(bpv, 32, 128, CodeOptionSet::Restricted, 0); }), "restricted set with " + std::to_string(bpv) + " bits must be rejected");
    }
}

int main()
{
    constructor();
    roundTrips();
    rejections();
    decodeVariants();
    restrictedSet();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
