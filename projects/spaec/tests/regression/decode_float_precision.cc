// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Decoding into float must compute like legacy ecCodes: the scaling is done in double
// (with the powers of 2 and 10 computed in float) and the result is rounded to float once.
// Computing in float loses the low bits of samples with more than 24 bits, and values near zero
// that result from a large negative reference value came out completely wrong (0 instead of
// 0.00042 for 30 bits per value, reference value -1040, binary scale factor -19).
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include "spaec/detail/CodeOption.h"
#include "spaec/detail/Span.h"
#include "spaec/detail/SpAec.h"
#include "spaec/detail/encoder/EncoderSeq.h"

using namespace spaec::detail;

int main()
{
    try {
        struct Case
        {
            uint8_t bpv;
            float reference;
            int binaryScale;
            int decimalScale;
        };
        // From ecCodes messages whose float values were wrong, plus wider ranges
        const Case cases[] = { { 30, -1040.0f, -19, -1 }, { 28, -35589.6f, -12, 0 }, { 32, -9.99999f, -27, -2 }, { 32, -3.0e6f, -8, 3 }, { 25, 12.5f, -20, 1 }, { 16, -100.0f, -4, 2 } };
        const unsigned seed = std::random_device{}();
        std::mt19937 rng(seed);
        size_t failures = 0;
        for (const Case& c : cases) {
            const size_t n = 4099;
            std::vector<uint32_t> samples(n);
            std::uniform_int_distribution<uint64_t> any(0, (uint64_t{ 1 } << c.bpv) - 1);
            for (size_t i = 0; i < n; ++i) {
                samples[i] = static_cast<uint32_t>(i % 7 == 0 ? 0 : any(rng));
            }
            std::vector<uint8_t> stream(encoder::SpAec<double>::maxEncodedSize(n, c.bpv, 32));
            encoder::EncoderSeq<uint32_t, uint32_t> enc{ c.bpv, 32, 128, CodeOptionSet::Basic };
            const size_t size = enc.encode(Span<uint32_t>(samples), Span<uint8_t>(stream));

            // Legacy ecCodes: double arithmetic, powers computed in float, one rounding to float
            const double bscale = grib_power<float>(c.binaryScale, 2);
            const double dscale = grib_power<float>(-c.decimalScale, 10);
            for (uint8_t threads : { 1, 4 }) {
                std::vector<float> decoded(n);
                encoder::SpAec<float> spaec{ c.bpv, 32, 128, CodeOptionSet::Basic, c.reference, c.binaryScale, c.decimalScale, threads };
                spaec.decode(Span<uint8_t>(stream.data(), size), Span<float>(decoded));
                for (size_t i = 0; i < n; ++i) {
                    const double exact = (static_cast<double>(samples[i]) * bscale + static_cast<double>(c.reference)) * dscale;
                    const float expected = static_cast<float>(exact);
                    if (decoded[i] != expected) {
                        if (++failures <= 5) {
                            std::cerr << std::setprecision(9) << "FAIL bpv=" << int(c.bpv) << " threads=" << int(threads) << " index " << i << ": decoded "
                                      << decoded[i] << ", expected " << expected << " (exact " << exact << ")\n";
                        }
                    }
                }
            }
        }
        if (failures != 0) {
            std::cerr << failures << " wrong float values (seed " << seed << ")\n";
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
