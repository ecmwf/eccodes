/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

// spaec: lossless compression of integer samples with CCSDS 121.0-B-3 (Adaptive Entropy Coding).
//
//     #include "spaec/spaec.h"
//
//     spaec::Options options;                 // 16 bits per sample, blocks of 32, basic code options
//     options.bitsPerSample = 12;
//     std::vector<uint8_t> data = spaec::compress(samples, options);
//     std::vector<uint16_t> back = spaec::decompress<uint16_t>(data, samples.size(), options);
//
// The compressed data hold no parameters: decompress needs the options of compress and the number
// of samples. Floating-point values are compressed with a Scaling to integers (GRIB CCSDS packing):
//
//     spaec::Scaling scaling{ 250.0, -4, 0 };  // value = (250 + sample * 2^-4) * 10^0
//     std::vector<uint8_t> data = spaec::compress(values, scaling, options);
//     std::vector<double> back = spaec::decompress<double>(data, values.size(), scaling, options);
//
// Errors are reported by exceptions: std::invalid_argument for options, samples and values that
// cannot be coded, std::length_error for an output buffer that is too small, spaec::DataError for
// compressed data that cannot be decoded (corrupt or too short; the data have no checksum, so
// corruption is not always detected). Everything in spaec/detail is the implementation.

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "spaec/detail/SpAec.h"

namespace spaec
{

// The set of code options (CCSDS 121.0-B-3, 5.2.1). Restricted: shorter option IDs for 1 to 4 bits
// per sample; it does not exist for 5 to 8 bits (an error) and equals Basic above 8 bits.
enum class CodeOptions
{
    Basic,
    Restricted
};

// The preprocessor (CCSDS 121.0-B-3, 4)
enum class Preprocessor
{
    UnitDelay,  // predicts each sample by its predecessor: for smooth data (the usual choice)
    Bypass,     // no prediction, but signed samples are mapped to non-negative values
    None        // the samples are coded as they are (unsigned samples only)
};

struct Options
{
    unsigned bitsPerSample = 16;       // n: 1 to 32, at most the bits of the sample type
    unsigned blockSize = 32;           // J: 8, 16, 32 or 64 samples
    unsigned referenceInterval = 128;  // r: blocks between two reference samples, 1 to 4096
    CodeOptions codeOptions = CodeOptions::Basic;
    Preprocessor preprocessor = Preprocessor::UnitDelay;
    bool signedSamples = false;  // samples are two's complement numbers of bitsPerSample bits (needs a preprocessor)
    unsigned threads = 1;        // 1 to 255; the compressed data do not depend on it
};

// The scaling of floating-point values to samples, as in GRIB:
// value * 10^decimalScaleFactor = referenceValue + sample * 2^binaryScaleFactor
// The scale factors are between -32767 and 32767, the reference value is finite.
struct Scaling
{
    double referenceValue = 0;
    int binaryScaleFactor = 0;
    int decimalScaleFactor = 0;
};

// Compressed data that are corrupt or too short
class DataError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

namespace detail
{

struct Config
{
    uint8_t bitsPerSample;
    uint8_t blockSize;
    uint16_t referenceInterval;
    CodeOptionSet codeOptionSet;
    Preprocessing preprocessing;
    uint8_t threads;
};

inline Config configOf(const Options& o)
{
    validateCcsdsParameters(o.bitsPerSample, o.blockSize, o.referenceInterval);
    if (o.threads < 1 || o.threads > 255) {
        throw std::invalid_argument("CCSDS: the number of threads must be between 1 and 255 (it is " + std::to_string(o.threads) + ")");
    }
    Config c;
    c.bitsPerSample = static_cast<uint8_t>(o.bitsPerSample);
    c.blockSize = static_cast<uint8_t>(o.blockSize);
    c.referenceInterval = static_cast<uint16_t>(o.referenceInterval);
    c.codeOptionSet = encoder::SpAec<double>::effectiveCodeOptionSet(
        o.codeOptions == CodeOptions::Restricted ? CodeOptionSet::Restricted : CodeOptionSet::Basic, c.bitsPerSample);
    // spaec::Preprocessor: detail has a class template of that name
    c.preprocessing.prediction = o.preprocessor == spaec::Preprocessor::UnitDelay ? Prediction::UnitDelay
                               : o.preprocessor == spaec::Preprocessor::Bypass    ? Prediction::Bypass
                                                                                  : Prediction::None;
    c.preprocessing.signedSamples = o.signedSamples;
    validatePreprocessing(c.preprocessing);
    c.threads = static_cast<uint8_t>(o.threads);
    return c;
}

template <typename T>
constexpr bool isSample = std::is_integral<T>::value && !std::is_same<T, bool>::value && sizeof(T) <= 4;

template <typename Sample>
[[noreturn]] void throwSampleDoesNotFit(const Sample* samples, size_t count, unsigned bits, bool isSigned, bool (*fits)(Sample, unsigned))
{
    size_t i = 0;
    while (i < count && fits(samples[i], bits)) {
        ++i;
    }
    throw std::invalid_argument("CCSDS: sample " + std::to_string(+samples[i]) + " at index " + std::to_string(i) + " does not fit in " +
                                std::to_string(bits) + (isSigned ? " bits (signed samples)" : " bits (unsigned samples)"));
}

// Throws std::invalid_argument if a sample does not fit bits: unsigned samples must be below
// 2^bits, signed samples (the bit pattern of Sample read as two's complement) between -2^(bits-1)
// and 2^(bits-1) - 1. One pass that the compiler vectorises; the index is searched only on error.
template <typename Sample>
void checkSamples(const Sample* samples, size_t count, unsigned bits, bool isSigned)
{
    using U = std::make_unsigned_t<Sample>;
    using S = std::make_signed_t<Sample>;
    if (bits >= 8 * sizeof(Sample)) {
        return;  // every value of Sample fits
    }
    if (!isSigned) {
        U any = 0;
        for (size_t i = 0; i < count; ++i) {
            any |= static_cast<U>(samples[i]);
        }
        if ((any >> bits) != 0) {
            throwSampleDoesNotFit<Sample>(samples, count, bits, false, [](Sample v, unsigned b) { return (static_cast<U>(v) >> b) == 0; });
        }
        return;
    }
    const S high = static_cast<S>((S(1) << (bits - 1)) - 1);
    const S low = static_cast<S>(-high - 1);
    S mn = 0, mx = 0;
    for (size_t i = 0; i < count; ++i) {
        const S v = static_cast<S>(samples[i]);
        mn = v < mn ? v : mn;
        mx = v > mx ? v : mx;
    }
    if (mn < low || mx > high) {
        throwSampleDoesNotFit<Sample>(samples, count, bits, true, [](Sample v, unsigned b) {
            const S h = static_cast<S>((S(1) << (b - 1)) - 1);
            const S s = static_cast<S>(v);
            return s >= static_cast<S>(-h - 1) && s <= h;
        });
    }
}

// Runs encode(target) with a target of the worst-case size: the output itself if it is large
// enough, else a temporary buffer whose content must fit the output
// (zeroed if the encoder needs that: zero)
template <typename Encode>
size_t encodeInto(uint8_t* output, size_t capacity, size_t worstCase, bool zero, Encode&& encode)
{
    if (capacity >= worstCase) {
        if (zero) {
            std::memset(output, 0, worstCase);
        }
        return encode(Span<uint8_t>(output, worstCase));
    }
    std::vector<uint8_t> scratch(worstCase, 0);
    const size_t size = encode(Span<uint8_t>(scratch));
    if (size > capacity) {
        throw std::length_error("CCSDS: the output buffer is too small (" + std::to_string(capacity) + " bytes, " + std::to_string(size) +
                                " bytes are needed; " + std::to_string(worstCase) + " bytes are always enough)");
    }
    std::memcpy(output, scratch.data(), size);
    return size;
}

template <typename Decode>
void decodeChecked(Decode&& decode)
{
    try {
        decode();
    }
    catch (const std::runtime_error& e) {
        throw DataError(e.what());
    }
}

}  // namespace detail

// Throws std::invalid_argument, with a message that names the valid values, if the options are invalid
inline void validate(const Options& options)
{
    detail::configOf(options);
}

// An upper bound of the size in bytes of count compressed samples (with a few bytes of slack): an
// output buffer of this size is always large enough
inline size_t maxCompressedSize(size_t count, const Options& options)
{
    const detail::Config c = detail::configOf(options);
    return detail::encoder::SpAec<double>::maxEncodedSize(count, c.bitsPerSample, c.blockSize);
}

// ---------------------------------------------------------------------------------------------
// Integer samples: uint8_t, uint16_t or uint32_t (int8_t, int16_t, int32_t for signed samples), of
// at least bitsPerSample bits. The type with the fewest bits is the fastest. Every sample must fit
// bitsPerSample bits (unsigned: 0 to 2^n - 1, signed: -2^(n-1) to 2^(n-1) - 1), else compress
// throws std::invalid_argument.

// Compresses count samples into output and returns the number of bytes written
template <typename Sample, std::enable_if_t<detail::isSample<Sample>, int> = 0>
size_t compress(const Sample* samples, size_t count, const Options& options, uint8_t* output, size_t capacity)
{
    using U = std::make_unsigned_t<Sample>;
    const detail::Config c = detail::configOf(options);
    detail::checkSamples(samples, count, c.bitsPerSample, c.preprocessing.signedSamples);
    const detail::Span<U> input(const_cast<U*>(reinterpret_cast<const U*>(samples)), count);
    const size_t worstCase = detail::encoder::SpAec<double>::maxEncodedSize(count, c.bitsPerSample, c.blockSize);
    return detail::encodeInto(output, capacity, worstCase, true, [&](detail::Span<uint8_t> target) {
        if (c.threads > 1) {
            detail::encoder::EncoderPar<U, U> encoder(c.bitsPerSample, c.blockSize, c.referenceInterval, c.codeOptionSet, c.threads, c.preprocessing);
            return encoder.encode(input, target);
        }
        detail::encoder::EncoderSeq<U, U> encoder(c.bitsPerSample, c.blockSize, c.referenceInterval, c.codeOptionSet, c.preprocessing);
        return encoder.encode(input, target);
    });
}

template <typename Sample, std::enable_if_t<detail::isSample<Sample>, int> = 0>
std::vector<uint8_t> compress(const std::vector<Sample>& samples, const Options& options)
{
    std::vector<uint8_t> output(maxCompressedSize(samples.size(), options));
    output.resize(compress(samples.data(), samples.size(), options, output.data(), output.size()));
    return output;
}

// Decompresses count samples; signed samples are returned sign-extended to Sample
template <typename Sample, std::enable_if_t<detail::isSample<Sample>, int> = 0>
void decompress(const uint8_t* data, size_t size, const Options& options, Sample* samples, size_t count)
{
    using U = std::make_unsigned_t<Sample>;
    const detail::Config c = detail::configOf(options);
    const detail::Span<uint8_t> source(const_cast<uint8_t*>(data), size);
    const detail::Span<U> target(reinterpret_cast<U*>(samples), count);
    detail::decodeChecked([&] {
        if (c.threads > 1) {
            detail::DecoderPar<U, U> decoder(c.bitsPerSample, c.blockSize, c.referenceInterval, c.codeOptionSet, c.threads, c.preprocessing);
            decoder.decodeParallel(source, target, [](U sample) { return sample; });
        }
        else {
            detail::DecoderSeq<U, U> decoder(c.bitsPerSample, c.blockSize, c.referenceInterval, c.codeOptionSet, c.preprocessing);
            decoder.decode(source, target);
        }
    });
}

template <typename Sample, std::enable_if_t<detail::isSample<Sample>, int> = 0>
std::vector<Sample> decompress(const std::vector<uint8_t>& data, size_t count, const Options& options)
{
    std::vector<Sample> samples(count);
    decompress(data.data(), data.size(), options, samples.data(), count);
    return samples;
}

// ---------------------------------------------------------------------------------------------
// Floating-point values (float, double): scaled to unsigned samples of bitsPerSample bits (rounded:
// a value is restored within half a step 2^E * 10^-D) and compressed in one pass. A value outside the range of the samples is an error (std::invalid_argument),
// and so is a scaling whose smallest or largest sample does not decode to a finite Value.

template <typename Value, std::enable_if_t<std::is_floating_point<Value>::value, int> = 0>
size_t compress(const Value* values, size_t count, const Scaling& scaling, const Options& options, uint8_t* output, size_t capacity)
{
    const detail::Config c = detail::configOf(options);
    if (c.preprocessing.signedSamples) {
        throw std::invalid_argument("CCSDS: scaled floating-point values are unsigned samples (signedSamples must be false)");
    }
    detail::encoder::SpAec<Value> coder(c.bitsPerSample, c.blockSize, c.referenceInterval, c.codeOptionSet, static_cast<Value>(scaling.referenceValue),
                                        scaling.binaryScaleFactor, scaling.decimalScaleFactor, c.threads, c.preprocessing.prediction);
    const detail::Span<Value> input(const_cast<Value*>(values), count);
    // SpAec::encode zeroes its target itself
    return detail::encodeInto(output, capacity, coder.maxEncodedSize(count), false, [&](detail::Span<uint8_t> target) { return coder.encode(input, target); });
}

template <typename Value, std::enable_if_t<std::is_floating_point<Value>::value, int> = 0>
std::vector<uint8_t> compress(const std::vector<Value>& values, const Scaling& scaling, const Options& options)
{
    std::vector<uint8_t> output(maxCompressedSize(values.size(), options));
    output.resize(compress(values.data(), values.size(), scaling, options, output.data(), output.size()));
    return output;
}

template <typename Value, std::enable_if_t<std::is_floating_point<Value>::value, int> = 0>
void decompress(const uint8_t* data, size_t size, const Scaling& scaling, const Options& options, Value* values, size_t count)
{
    const detail::Config c = detail::configOf(options);
    if (c.preprocessing.signedSamples) {
        throw std::invalid_argument("CCSDS: scaled floating-point values are unsigned samples (signedSamples must be false)");
    }
    detail::encoder::SpAec<Value> coder(c.bitsPerSample, c.blockSize, c.referenceInterval, c.codeOptionSet, static_cast<Value>(scaling.referenceValue),
                                        scaling.binaryScaleFactor, scaling.decimalScaleFactor, c.threads, c.preprocessing.prediction);
    detail::decodeChecked([&] { coder.decode(detail::Span<uint8_t>(const_cast<uint8_t*>(data), size), detail::Span<Value>(values, count)); });
}

template <typename Value, std::enable_if_t<std::is_floating_point<Value>::value, int> = 0>
std::vector<Value> decompress(const std::vector<uint8_t>& data, size_t count, const Scaling& scaling, const Options& options)
{
    std::vector<Value> values(count);
    decompress(data.data(), data.size(), scaling, options, values.data(), count);
    return values;
}

}  // namespace spaec
