// Fuzzer for GRIB simple packing: unpack with fuzzed parameters (bits per value, scale
// factors, reference value, number of values) and a fuzzed buffer must not crash (errors
// are exceptions), and pack -> unpack of fuzzed doubles must give the values back within
// the quantisation step of the chosen parameters.

#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#include "simple_packing/Parameters.h"
#include "simple_packing/optimizer/BinDecOptimizer.h"
#include "simple_packing/simple/Binary.h"

namespace
{

uint16_t le16(const uint8_t* p)
{
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
}

uint32_t le32(const uint8_t* p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

double bitCastDouble(const uint8_t* p)
{
    uint64_t bits = 0;
    for (int i = 0; i < 8; ++i) {
        bits |= static_cast<uint64_t>(p[i]) << (8 * i);
    }
    double d;
    std::memcpy(&d, &bits, sizeof(d));
    return d;
}

// The distance to the next smaller double (the spacing of the format at that magnitude,
// the subnormal ulp 5e-324 near zero)
long double ulpOf(double value)
{
    const double next = std::nextafter(value, 0.0);
    return std::fabs(static_cast<long double>(value)) - std::fabs(static_cast<long double>(next));
}

long signedFactor(const uint8_t* p)
{
    const uint16_t v = le16(p);
    const long magnitude = v % 32768;
    return (v / 32768) ? -magnitude : magnitude;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < 19) {
        return 0;
    }
    SimplePackingBinary<double> unpacker(nullptr);

    // --- unpack with fuzzed parameters and buffer ---
    const bool exactSize = data[0] & 1;
    Parameters params;
    params.bitsPerValue(static_cast<long>(data[1] % 65));  // 0..64 (0: constant field)
    params.decimalScaleFactor(signedFactor(data + 2));
    params.binaryScaleFactor(signedFactor(data + 4));
    params.referenceValue(bitCastDouble(data + 6));
    params.isConstantField((data[0] & 2) != 0);
    const size_t n = le32(data + 14) % 100001;  // up to 100000 values

    const uint8_t* buffer = data + 19;
    const size_t available = size - 19;
    const size_t needed = params.bitsPerValue() > 0 ? (static_cast<size_t>(params.bitsPerValue()) * n + 7) / 8 : 0;
    const size_t bufferSize = exactSize ? (available < needed ? available : needed) : available;

    try {
        std::vector<double> out(n);
        unpacker.unpackTo(params, buffer, bufferSize, n, out.data());
    }
    catch (const std::exception&) {
        // rejected input (e.g. the buffer is too short): fine
    }

    // --- pack -> unpack round trip with fuzzed doubles ---
    size_t n2 = (data[18] % 256) + 1;
    if (available / 8 < n2) {
        n2 = available / 8;
    }
    if (n2 == 0) {
        return 0;
    }
    const uint8_t* doubleBytes = buffer + available - n2 * 8;  // take the values from the end
    std::vector<double> values(n2);
    for (size_t i = 0; i < n2; ++i) {
        values[i] = bitCastDouble(doubleBytes + 8 * i);
    }

    try {
        SimplePackingBinary<double> packer(std::make_unique<BinDecOptimizer>(
            (data[0] & 4) != 0, (data[0] & 8) != 0, static_cast<int>(data[1] % 64)));
        auto [packedParams, packed] = packer.pack(values);
        const std::vector<double> back = packer.unpack(packedParams, packed, n2);
        const long bpv = packedParams.bitsPerValue();
        const long e = packedParams.binaryScaleFactor();
        const long d = packedParams.decimalScaleFactor();
        if (packedParams.isConstantField() || bpv == 0) {
            for (size_t i = 0; i < n2; ++i) {
                if (back[i] != packedParams.referenceValue()) {
                    __builtin_trap();  // a constant field must decode to the reference value
                }
            }
            return 0;
        }
        // Rounding error of the double pipeline in grid units: the cancellation between
        // value*10^D and the reference value R is amplified by 2^-E, so it scales with
        // |R|*2^-E (plus 2^bpv for the grid index itself), roughly a few units of 1e-16 of it.
        const long double gridSpan = std::ldexpl(1.0L, static_cast<int>(bpv)) +
                                     std::fabsl(static_cast<long double>(packedParams.referenceValue())) *
                                         std::ldexpl(1.0L, static_cast<int>(-e));
        // Strict bit-exact re-pack when the double round trip cannot lose precision.
        // The target must start zeroed: the codec ORs its bits into the buffer (like pack()).
        if (bpv <= 47 && e >= -60 && e <= 60 && d >= -22 && d <= 22 && gridSpan <= 2.5e14L) {
            std::vector<unsigned char> repacked(packed.size(), 0x00);
            packer.packTo(packedParams, back.data(), n2, repacked.data());
            if (repacked != packed) {
                __builtin_trap();  // unpack(pack(v)) does not pack to the same bits
            }
        }
        // Loose check: back[i] is within a step (plus pipeline rounding) of values[i].
        // The encode and decode each round a few times: the total error is a handful of
        // units in 1e-15 of the grid span, plus a few ulps of the results themselves
        // (the step can be far below one ulp, e.g. for subnormal doubles)
        const long double step = std::ldexpl(1.0L, static_cast<int>(e)) / std::powl(10.0L, static_cast<int>(d));
        const long double tol = (0.501L + 2e-14L * gridSpan) * step;
        if (std::isfinite(step) && step > 0 && std::isfinite(tol)) {
            for (size_t i = 0; i < n2; ++i) {
                const long double diff = std::fabs(static_cast<long double>(back[i]) - static_cast<long double>(values[i]));
                const long double ulpSlack = 8.0L * (ulpOf(back[i]) + ulpOf(values[i]));
                if (!(diff <= tol + ulpSlack)) {
                    __builtin_trap();  // decoded value outside the quantisation step
                }
            }
        }
    }
    catch (const std::exception&) {
        // values or ranges the packer rejects (NaN, overflow, empty range): fine
    }
    return 0;
}
