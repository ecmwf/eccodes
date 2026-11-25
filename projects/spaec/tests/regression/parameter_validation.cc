// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unsupported coding parameters must be rejected with std::invalid_argument
// (an odd block size used to overflow the encoder's buffers).

#include <cstdint>
#include <exception>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "spaec/CodeOption.h"
#include "spaec/decoder/DecoderSeq.h"
#include "spaec/encoder/EncoderSeq.h"
#include "spaec/SpAec.h"

using namespace spaec;

static int failures = 0;

template <typename F>
static void expectInvalid(const std::string& what, F&& f)
{
    try {
        f();
        std::cerr << "FAIL: " << what << " was accepted\n";
        failures++;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "OK: " << what << ": " << e.what() << '\n';
    }
}

template <typename F>
static void expectValid(const std::string& what, F&& f)
{
    try {
        f();
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << what << " was rejected: " << e.what() << '\n';
        failures++;
    }
}

using SpAec = encoder::SpAec<double>;

int main()
{
    const auto basic = CodeOptionSet::Basic;

    for (int bpv : { 0, 33, 64, 255 }) {
        expectInvalid("bpv " + std::to_string(bpv), [&] { SpAec(bpv, 32, 64, basic, 0.0); });
    }
    for (int blockSize : { 0, 1, 2, 7, 9, 12, 63, 65, 128, 255 }) {
        expectInvalid("block size " + std::to_string(blockSize), [&] { SpAec(16, blockSize, 64, basic, 0.0); });
    }
    for (int rsi : { 0, 4097, 65535 }) {
        expectInvalid("rsi " + std::to_string(rsi), [&] { SpAec(16, 32, rsi, basic, 0.0); });
    }
    expectInvalid("0 threads", [&] { SpAec(16, 32, 64, basic, 0.0, 0, 0, 0); });
    expectInvalid("binary scale factor 40000", [&] { SpAec(16, 32, 64, basic, 0.0, 40000, 0); });
    expectInvalid("decimal scale factor -40000", [&] { SpAec(16, 32, 64, basic, 0.0, 0, -40000); });
    expectInvalid("NaN reference value", [&] { SpAec(16, 32, 64, basic, std::nan("")); });
    expectInvalid("infinite reference value", [&] { SpAec(16, 32, 64, basic, std::numeric_limits<double>::infinity()); });
    expectInvalid("restricted code options with 6 bits per value", [&] { SpAec(6, 32, 64, CodeOptionSet::Restricted, 0.0); });

    // Low-level encoder and decoder
    expectInvalid("encoder with block size 9", [&] { encoder::EncoderSeq<uint8_t, uint8_t>(8, 9, 1, basic); });
    expectInvalid("encoder with 16 bits in a 8-bit type", [&] { encoder::EncoderSeq<uint8_t, uint8_t>(16, 8, 1, basic); });
    expectInvalid("decoder with block size 9", [&] { DecoderSeq<uint8_t, uint8_t>(8, 9, 1, basic); });
    expectInvalid("decoder with 16 bits in a 8-bit type", [&] { DecoderSeq<uint8_t, uint8_t>(16, 8, 1, basic); });
    expectInvalid("decoder with rsi 0", [&] { DecoderSeq<uint16_t, uint16_t>(16, 8, 0, basic); });

    // Supported parameters
    for (int bpv : { 1, 8, 16, 24, 32 }) {
        for (int blockSize : { 8, 16, 32, 64 }) {
            for (int rsi : { 1, 128, 4096 }) {
                expectValid("bpv " + std::to_string(bpv) + " block size " + std::to_string(blockSize) + " rsi " + std::to_string(rsi),
                            [&] { SpAec(bpv, blockSize, rsi, basic, 0.0, -32767, 32767, 4); });
            }
        }
    }

    return failures == 0 ? 0 : 1;
}
