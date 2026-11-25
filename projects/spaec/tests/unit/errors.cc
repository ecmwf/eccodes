// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: every error SpAec can report is triggered once, with the expected exception type and
// message. Errors that cannot be triggered: ThreadPool::enqueue on a stopped pool (the pool is only
// stopped by its destructor)
#include <cmath>
#include <cstdint>
#include <future>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/BitBuffer.h"
#include "spaec/CodeOption.h"
#include "spaec/SpAec.h"
#include "spaec/ThreadPool.h"
#include "spaec/Utils.h"
#include "spaec/decoder/DecoderSeq.h"
#include "spaec/encoder/EncoderPar.h"
#include "spaec/encoder/EncoderSeq.h"

using namespace spaec;

static int failures = 0;
static int cases = 0;

// f() must throw an E whose message contains `message`
template <typename E, typename F>
static void expect(const std::string& what, const std::string& message, F&& f)
{
    ++cases;
    try {
        f();
        std::cerr << "FAIL: " << what << ": no exception\n";
    }
    catch (const E& e) {
        if (std::string(e.what()).find(message) != std::string::npos) {
            return;
        }
        std::cerr << "FAIL: " << what << ": message \"" << e.what() << "\" does not contain \"" << message << "\"\n";
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << what << ": unexpected exception type: " << e.what() << '\n';
    }
    ++failures;
}

using IA = std::invalid_argument;
using RE = std::runtime_error;
using LE = std::length_error;

static void threadPool()
{
    expect<IA>("ThreadPool(0)", "at least 1", [] { ThreadPool pool(0); });
}

static void codeOptions()
{
    // Bits per value outside 1..32 (no check before: the classes are the first to see them)
    for (unsigned bpv : { 0u, 33u }) {
        const auto b = static_cast<uint8_t>(bpv);
        const std::string n = " bpv " + std::to_string(bpv);
        expect<IA>("CodeOptionBase restricted" + n, "Unsupported", [&] { CodeOptionBase(b, CodeOptionSet::Restricted); });
        expect<IA>("CodeOptionBase basic" + n, "Unsupported", [&] { CodeOptionBase(b, CodeOptionSet::Basic); });
        expect<IA>("ZeroBlockCodeOption" + n, "Unsupported", [&] { ZeroBlockCodeOption(b, CodeOptionSet::Basic); });
        expect<IA>("SecondExtensionCodeOption" + n, "Unsupported", [&] { SecondExtensionCodeOption(b, CodeOptionSet::Basic); });
        expect<IA>("SplitSampleLimits" + n, "Unsupported", [&] { SplitSampleLimits(b, CodeOptionSet::Basic); });
        expect<IA>("SplitSampleIdLength" + n, "Unsupported", [&] { SplitSampleIdLength(b, CodeOptionSet::Basic); });
        expect<IA>("NoCompressionCodeOption" + n, "Unsupported", [&] { NoCompressionCodeOption(b, CodeOptionSet::Basic); });
    }
    // The restricted set has no split sample option for 1 and 2 bits
    for (uint8_t bpv : { 1, 2 }) {
        expect<IA>("SplitSampleCodeOption restricted bpv " + std::to_string(bpv), "not supported for bpv 1 or 2", [&] { SplitSampleCodeOption(bpv, 0, CodeOptionSet::Restricted); });
    }
}

static void parameters()
{
    expect<IA>("validateCcsdsParameters bpv 0", "bits per value", [] { validateCcsdsParameters(0, 32, 1); });
    expect<IA>("validateCcsdsParameters bpv 33", "bits per value", [] { validateCcsdsParameters(33, 32, 1); });
    expect<IA>("validateCcsdsParameters block size 12", "block size", [] { validateCcsdsParameters(16, 12, 1); });
    expect<IA>("validateCcsdsParameters RSI 0", "reference sample interval", [] { validateCcsdsParameters(16, 32, 0); });
    expect<IA>("validateCcsdsParameters RSI too large", "reference sample interval", [] { validateCcsdsParameters(16, 32, maxRsiSize + 1); });
    // More bits per value than the sample type has
    expect<IA>("EncoderSeq<uint8_t> bpv 9", "bits per value", [] { encoder::EncoderSeq<uint8_t, uint8_t>(9, 8, 1, CodeOptionSet::Basic); });
    expect<IA>("DecoderSeq<uint16_t> bpv 17", "bits per value", [] { DecoderSeq<uint16_t, uint16_t>(17, 8, 1, CodeOptionSet::Basic); });
}

static void spaecErrors()
{
    using S = encoder::SpAec<double>;
    expect<IA>("SpAec restricted with 6 bits", "restricted set of code options", [] { S(6, 32, 128, CodeOptionSet::Restricted, 0); });
    expect<IA>("SpAec 0 threads", "number of threads", [] { S(16, 32, 128, CodeOptionSet::Basic, 0, 0, 0, 0); });
    expect<IA>("SpAec binary scale factor", "binary scale factor 40000", [] { S(16, 32, 128, CodeOptionSet::Basic, 0, 40000, 0); });
    expect<IA>("SpAec decimal scale factor", "decimal scale factor -40000", [] { S(16, 32, 128, CodeOptionSet::Basic, 0, 0, -40000); });
    expect<IA>("SpAec NaN reference", "reference value", [] { S(16, 32, 128, CodeOptionSet::Basic, std::nan("")); });
    expect<IA>("SpAec bpv 0", "bits per value", [] { S(0, 32, 128, CodeOptionSet::Basic, 0); });
    expect<LE>("SpAec::maxEncodedSize too large", "does not fit in size_t", [] { S::maxEncodedSize(std::numeric_limits<size_t>::max(), 32, 8); });

    S spaec(8, 16, 4, CodeOptionSet::Basic, 10.0);
    std::vector<double> values(100, 100.0);
    std::vector<uint8_t> small(10);
    expect<IA>("SpAec::encode small target", "output buffer is too small", [&] { spaec.encode(Span<double>(values), Span<uint8_t>(small)); });
    std::vector<uint8_t> target(spaec.maxEncodedSize(values.size()));
    values[7] = 300.0;
    expect<IA>("SpAec::encode value out of range", "at index 7 cannot be packed", [&] { spaec.encode(Span<double>(values), Span<uint8_t>(target)); });
}

static void encoderPar()
{
    std::vector<uint16_t> samples(20000);
    for (size_t i = 0; i < samples.size(); ++i)
        samples[i] = static_cast<uint16_t>((i * 2654435761u) >> 16);
    for (size_t threads : { 1, 4 }) {
        encoder::EncoderPar<uint16_t, uint16_t> par(16, 32, 64, CodeOptionSet::Basic, threads);
        std::vector<uint8_t> small(1000, 0);
        expect<IA>("EncoderPar small target, " + std::to_string(threads) + " threads", "output buffer is too small",
                   [&] { par.encode(Span<uint16_t>(samples), Span<uint8_t>(small)); });
    }
}

static void bitBuffer()
{
    std::vector<uint8_t> data(2 + BitBuffer::guardBytes, 0);
    BitBuffer reader{ Span<uint8_t>(data), 0, 16 };
    expect<RE>("BitBuffer read beyond the data", "truncated or corrupt", [&] { reader.readCodeword(); });

    std::vector<uint8_t> out(16, 0);
    BitBuffer writer{ Span<uint8_t>(out) };
    const uint64_t lengths[] = { 500 };
    expect<LE>("BitBuffer write beyond the buffer", "output buffer is too small", [&] { writer.writeCodewords(lengths, 1); });
}

static void decoder()
{
    DecoderSeq<uint16_t, uint16_t> d(16, 8, 64, CodeOptionSet::Basic);
    std::vector<uint8_t> data(8 + BitBuffer::guardBytes, 0);
    BitBuffer w{ Span<uint8_t>(data) };
    w.writeCodeword(2);  // a run of 3 zero blocks
    std::vector<uint16_t> segment(4 * 8);
    expect<RE>("zbDecode at a block beyond the segment", "zero block at block 4", [&] {
        const BitBuffer b{ Span<uint8_t>(data), 0, 64 };
        d.zbDecode<ReferenceType::NoRef>(b, Span<uint16_t>(segment), 4, 8);
    });
    expect<RE>("zbDecode of a run beyond the blocks that remain", "run of 3 zero blocks", [&] {
        const BitBuffer b{ Span<uint8_t>(data), 0, 64 };
        d.zbDecode<ReferenceType::NoRef>(b, Span<uint16_t>(segment), 2, 4);
    });
    std::vector<uint8_t> empty;
    std::vector<uint16_t> out(100);
    expect<RE>("decode of an empty stream", "truncated or corrupt", [&] { d.decode(Span<uint8_t>(empty), Span<uint16_t>(out)); });
}

int main()
{
    threadPool();
    codeOptions();
    parameters();
    spaecErrors();
    encoderPar();
    bitBuffer();
    decoder();
    if (failures != 0) {
        std::cerr << failures << " of " << cases << " error cases failed\n";
        return 1;
    }
    std::cout << "OK: " << cases << " error cases\n";
    return 0;
}
