// Truncated or unterminated compressed data must be reported as an error: the decoder used to
// read beyond the end of its input.

#include <cstddef>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <exception>
#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/decoder/DecoderSeq.h"
#include "spaec/encoder/EncoderSeq.h"
#include "spaec/SpAec.h"

using namespace spaec;

static int failures = 0;

static void expectError(const char* what, Span<uint8_t> stream, size_t nValues, uint8_t bpv, uint8_t blockSize, uint16_t rsi)
{
    std::vector<double> values(nValues);
    encoder::SpAec<double> spaec{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0 };
    try {
        spaec.decode(stream, Span<double>(values));
        std::cerr << "FAIL: " << what << ": no error reported\n";
        failures++;
    }
    catch (const std::runtime_error&) {
    }
}

int main()
{
    try {
        // A valid stream
        const size_t n = 4000;
        std::vector<double> data(n);
        for (size_t i = 0; i < n; ++i) {
            data[i] = 100.0 + 50.0 * std::sin(i * 0.01) + (i % 7);
        }
        encoder::SpAec<double> spaec{ 16, 32, 64, CodeOptionSet::Basic, 0.0, -6, 0 };
        std::vector<uint8_t> encoded(n * 2 + 1024);
        const size_t size = spaec.encode(Span<double>(data), Span<uint8_t>(encoded));

        // Cut at every byte: every byte of the stream holds data that is needed
        for (size_t cut = 0; cut < size; ++cut) {
            std::vector<uint8_t> truncated(encoded.begin(), encoded.begin() + cut);
            expectError("truncated stream", Span<uint8_t>(truncated), n, 16, 32, 64);
        }

        // A codeword without terminating 1-bit (all zero data)
        std::vector<uint8_t> zeros(10, 0);
        expectError("unterminated codeword", Span<uint8_t>(zeros), 8, 8, 8, 1);

        // Many values from a tiny stream
        std::vector<uint8_t> tiny = { 0xff, 0xff };
        expectError("too short stream", Span<uint8_t>(tiny), 1000000, 16, 32, 64);

        // The complete stream decodes correctly, also with the low-level decoder on an exactly
        // sized buffer
        std::vector<double> decoded(n);
        spaec.decode(Span<uint8_t>(encoded.data(), size), Span<double>(decoded));
        for (size_t i = 0; i < n; ++i) {
            if (std::fabs(decoded[i] - data[i]) > 1.0 / 64) {
                std::cerr << "FAIL: value " << i << " decoded wrongly\n";
                failures++;
                break;
            }
        }

        std::vector<uint16_t> ints(n);
        for (size_t i = 0; i < n; ++i) {
            ints[i] = static_cast<uint16_t>(i * 37 % 65536);
        }
        encoder::EncoderSeq<uint16_t, uint16_t> enc{ 16, 32, 64, CodeOptionSet::Basic };
        std::vector<uint8_t> intEncoded(n * 2 + 1024);
        const size_t intSize = enc.encode(Span<uint16_t>(ints), Span<uint8_t>(intEncoded));
        std::vector<uint8_t> exact(intEncoded.begin(), intEncoded.begin() + intSize);
        std::vector<uint16_t> intDecoded(n);
        DecoderSeq<uint16_t, uint16_t> dec{ 16, 32, 64, CodeOptionSet::Basic };
        dec.decode(Span<uint8_t>(exact), Span<uint16_t>(intDecoded));
        if (intDecoded != ints) {
            std::cerr << "FAIL: exactly sized stream decoded wrongly\n";
            failures++;
        }

        if (failures == 0) {
            std::cout << "OK\n";
        }
        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
