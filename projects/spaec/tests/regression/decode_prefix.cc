// A zero-block run of a valid stream may reach beyond the values that were asked for
// (the last RSI of a shorter target): like libaec, which stops with success when its
// output buffer is full, decoding fewer values than encoded must succeed. The run used
// to be reported as corrupt data. Runs beyond the blocks of the RSI itself remain
// corrupt and are still rejected.

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <exception>
#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"

static int failures = 0;

// Decodes n values of the stream and expects success with the given values
static void expectValues(const char* what, Span<uint8_t> stream, size_t n, uint8_t bpv, uint8_t blockSize, uint16_t rsi,
                         double fill, uint8_t nThreads)
{
    std::vector<double> values(n, -1.0);
    encoder::SpAec<double> spaec{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, 0, 0, nThreads };
    try {
        spaec.decode(stream, Span<double>(values));
        for (size_t i = 0; i < n; ++i) {
            if (values[i] != fill) {
                std::cerr << "FAIL: " << what << ": value " << i << " is " << values[i] << ", expected " << fill << '\n';
                failures++;
                return;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << what << ": " << e.what() << '\n';
        failures++;
    }
}

// Decodes n values and expects an error (corrupt data)
static void expectError(const char* what, Span<uint8_t> stream, size_t n, uint8_t bpv, uint8_t blockSize, uint16_t rsi)
{
    std::vector<double> values(n);
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
        // bpv=8, block size 8, RSI 2: every RSI is two zero blocks, encoded as one zero-block
        // run of 2 with a reference sample. Asking for the values of the first block only
        // (16 of 32, 8 of 16, ...) cuts the run at the requested values, which is what libaec
        // does when its output buffer is full (used to throw "run of 2 zero blocks, but only
        // 1 blocks remain")
        {
            std::vector<double> data(320, 0.0);  // 20 RSIs
            encoder::SpAec<double> spaec{ 8, 8, 2, CodeOptionSet::Basic, 0.0 };
            std::vector<uint8_t> encoded(spaec.maxEncodedSize(data.size()));
            const size_t size = spaec.encode(Span<double>(data), Span<uint8_t>(encoded));
            Span<uint8_t> stream(encoded.data(), size);

            expectValues("all zeros, 320 values", stream, 320, 8, 8, 2, 0.0, 1);
            expectValues("all zeros, 319 values", stream, 319, 8, 8, 2, 0.0, 1);
            expectValues("all zeros, 318 values", stream, 318, 8, 8, 2, 0.0, 1);
            expectValues("all zeros, 305 values", stream, 305, 8, 8, 2, 0.0, 1);
            expectValues("all zeros, 9 values", stream, 9, 8, 8, 2, 0.0, 1);
            expectValues("all zeros, 9 values, 2 threads", stream, 9, 8, 8, 2, 0.0, 2);
            expectValues("all zeros, 17 values, 2 threads", stream, 17, 8, 8, 2, 0.0, 2);
        }

        // The same with non-zero data: the reference value is kept by the predictor
        {
            std::vector<double> data(64, 123.0);  // 2 RSIs of 2 blocks
            encoder::SpAec<double> spaec{ 8, 8, 2, CodeOptionSet::Basic, 0.0 };
            std::vector<uint8_t> encoded(spaec.maxEncodedSize(data.size()));
            const size_t size = spaec.encode(Span<double>(data), Span<uint8_t>(encoded));
            Span<uint8_t> stream(encoded.data(), size);

            expectValues("constant 123, 64 values", stream, 64, 8, 8, 2, 123.0, 1);
            expectValues("constant 123, 63 values", stream, 63, 8, 8, 2, 123.0, 1);
            expectValues("constant 123, 33 values", stream, 33, 8, 8, 2, 123.0, 1);
        }

        // A run that reaches beyond the blocks of its RSI is still corrupt
        {
            // RSI of 2 blocks: a zero block with the reference sample and a run of 3 blocks
            // (3 zero bits terminated by a one bit, i.e. run length 3)
            std::vector<uint8_t> stream = { 0x00, 0x01, 0x10 };
            expectError("run beyond the RSI", Span<uint8_t>(stream), 16, 8, 8, 2);
            expectError("run beyond the RSI, 32 values", Span<uint8_t>(stream), 32, 8, 8, 2);
        }

        // A run that fills the RSI exactly is fine
        {
            std::vector<uint8_t> stream = { 0x00, 0x04 };
            expectValues("run of 2 in an RSI of 2 blocks", Span<uint8_t>(stream), 16, 8, 8, 2, 0.0, 1);
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
}
