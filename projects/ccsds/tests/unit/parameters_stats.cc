// Unit tests derived from regression tests:
// - parameter_validation: validateCcsdsParameters rejects unsupported bits per value, block sizes
//   and reference sample intervals with std::invalid_argument (an odd block size overflowed the
//   encoder's buffers)
// - stats_empty: SpAecStats of an empty field (no blocks) can be printed (it divided by the number
//   of blocks, which is 0)
#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ccsds/Stats.h"
#include "ccsds/Utils.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

static bool rejected(size_t bpv, size_t blockSize, size_t rsi, size_t maxBpv = 32)
{
    try {
        validateCcsdsParameters(bpv, blockSize, rsi, maxBpv);
    }
    catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

static void parameters()
{
    for (size_t bpv : { 1, 8, 16, 24, 32 }) {
        for (size_t blockSize : { 8, 16, 32, 64 }) {
            check(!rejected(bpv, blockSize, 1) && !rejected(bpv, blockSize, maxRsiSize),
                  "bpv " + std::to_string(bpv) + ", block size " + std::to_string(blockSize) + " must be accepted");
        }
    }
    for (size_t bpv : { 0, 33, 64, 1000 }) {
        check(rejected(bpv, 32, 128), "bpv " + std::to_string(bpv) + " must be rejected");
    }
    for (size_t blockSize : { 0, 1, 7, 9, 12, 24, 63, 128, 256 }) {
        check(rejected(16, blockSize, 128), "block size " + std::to_string(blockSize) + " must be rejected");
    }
    check(rejected(16, 32, 0), "reference sample interval 0 must be rejected");
    check(rejected(16, 32, maxRsiSize + 1), "reference sample interval beyond the maximum must be rejected");
    check(rejected(16, 32, 128, 8), "16 bits per value with samples of 8 bits must be rejected");
}

static void emptyStatistics()
{
    SpAecStats stats;  // no blocks, no values
    std::ostringstream out;
    std::streambuf* old = std::cout.rdbuf(out.rdbuf());
    try {
        stats.print();
        stats.printMetrics();
    }
    catch (const std::exception& e) {
        check(false, std::string("printing empty statistics: ") + e.what());
    }
    std::cout.rdbuf(old);
    const std::string text = out.str();
    check(text.find("nan") == std::string::npos && text.find("inf") == std::string::npos, "empty statistics must not contain nan or inf");
}

int main()
{
    parameters();
    emptyStatistics();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
