// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Fuzzer: encoder::SpAec<double>::decode — the sequential decoder (nThreads=1) and the
// parallel decoder (nThreads>1) must agree on every input: equal values, or both must
// report an error. The first bytes choose the coding parameters, the rest is the
// (arbitrary, possibly corrupt) compressed stream.

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "spaec/detail/CodeOption.h"
#include "spaec/detail/Span.h"
#include "spaec/detail/SpAec.h"

using namespace spaec::detail;

namespace
{

uint16_t le16(const uint8_t* p)
{
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
}

struct Params
{
    uint8_t bpv;
    uint8_t blockSize;
    uint16_t rsi;
    uint16_t nValues;
    uint8_t nThreads;
};

// Header layout (shared with generate_seeds.cc):
// [0] bpv 1..32, [1] block size 8/16/32/64, [2..3] rsi-1, [4..5] nValues, [6] threads 1..8
bool parseParams(const uint8_t* data, size_t size, Params& params, size_t& headerSize)
{
    if (size < 7) {
        return false;
    }
    static const uint8_t blockSizes[4] = { 8, 16, 32, 64 };
    params.bpv = static_cast<uint8_t>((data[0] % 32) + 1);
    params.blockSize = blockSizes[data[1] % 4];
    params.rsi = static_cast<uint16_t>((le16(data + 2) % 4096) + 1);  // 1..4096 blocks (maxRsiSize)
    params.nValues = le16(data + 4);
    params.nThreads = static_cast<uint8_t>((data[6] % 8) + 1);
    headerSize = 7;
    return true;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    Params params;
    size_t headerSize = 0;
    if (!parseParams(data, size, params, headerSize)) {
        return 0;
    }
    const uint8_t* streamData = data + headerSize;
    const size_t streamSize = size - headerSize;
    std::vector<uint8_t> stream(streamData, streamData + streamSize);

    std::vector<double> seq(params.nValues), par(params.nValues);
    bool seqOk = true, parOk = true;
    try {
        encoder::SpAec<double> decoder{ params.bpv, params.blockSize, params.rsi,
                                        CodeOptionSet::Basic, 0.0, 0, 0, 1 };
        decoder.decode(Span<uint8_t>(stream), Span<double>(seq));
    }
    catch (...) {
        seqOk = false;
    }
    try {
        encoder::SpAec<double> decoder{ params.bpv, params.blockSize, params.rsi,
                                        CodeOptionSet::Basic, 0.0, 0, 0,
                                        static_cast<uint8_t>(params.nThreads < 2 ? 2 : params.nThreads) };
        decoder.decode(Span<uint8_t>(stream), Span<double>(par));
    }
    catch (...) {
        parOk = false;
    }

    if (seqOk != parOk) {
        __builtin_trap();  // one decoder accepted what the other rejected
    }
    if (seqOk && seq != par) {
        __builtin_trap();  // silent disagreement
    }
    return 0;
}
