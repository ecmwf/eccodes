// Decoding must check the size of the coded buffer: it read ceil(bitsPerValue * nVals / 8) bytes
// whatever the size of the buffer was.

#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "simple_packing/simple/Binary.h"

using namespace sp;

static int failures = 0;

static void expectInvalid(const char* what, long bpv, size_t bufferSize, size_t nVals)
{
    Parameters params;
    params.bitsPerValue(bpv);
    SimplePackingBinary<double> sp(nullptr);
    std::vector<unsigned char> buffer(bufferSize, 0x5a);
    std::vector<double> out(std::min<size_t>(nVals, 100000));
    try {
        sp.unpackTo(params, buffer.data(), buffer.size(), nVals, out.data());
        std::cerr << "FAIL: " << what << " was accepted\n";
        failures++;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "OK: " << what << ": " << e.what() << '\n';
    }
}

int main()
{
    expectInvalid("empty buffer", 16, 0, 496);
    expectInvalid("buffer one byte too short", 12, 91, 61);  // needs 92 bytes
    expectInvalid("buffer one byte too short (64 bits)", 64, 7, 1);
    expectInvalid("number of values overflowing the size", 64, 64, std::numeric_limits<size_t>::max() / 8);

    // Exactly sized buffers work, also through unpack()
    for (long bpv : { 1L, 12L, 16L, 33L, 64L }) {
        const size_t n = 61;
        Parameters params;
        params.bitsPerValue(bpv);
        SimplePackingBinary<double> sp(nullptr);
        std::vector<unsigned char> buffer((bpv * n + 7) / 8, 0);
        std::vector<double> out(n);
        try {
            sp.unpackTo(params, buffer.data(), buffer.size(), n, out.data());
            sp.unpack(params, buffer, n);
        }
        catch (const std::exception& e) {
            std::cerr << "FAIL: exactly sized buffer with " << bpv << " bits per value: " << e.what() << '\n';
            failures++;
        }
    }
    return failures == 0 ? 0 : 1;
}
