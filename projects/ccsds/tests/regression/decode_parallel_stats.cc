// The statistics of a parallel decode must match the sequential decode of the same stream
// (they used to stay at their defaults because only the sequential path collected them).

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <exception>
#include "ccsds/CodeOption.h"
#include "ccsds/Span.h"
#include "ccsds/SpAec.h"

static int failures = 0;

static void expectEqualStats(const char* what, uint8_t bpv, uint8_t blockSize, uint16_t rsi,
                             const std::vector<double>& data)
{
    encoder::SpAec<double> seq{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0 };
    std::vector<uint8_t> encoded(seq.maxEncodedSize(data.size()));
    const size_t size = seq.encode(Span<double>(const_cast<double*>(data.data()), data.size()), Span<uint8_t>(encoded));

    std::vector<double> decoded(data.size());
    seq.decode(Span<uint8_t>(encoded.data(), size), Span<double>(decoded));
    const auto seqStats = seq.getStats();

    encoder::SpAec<double> par{ bpv, blockSize, rsi, CodeOptionSet::Basic, 0.0, 0, 0, 4 };
    std::vector<double> decoded2(data.size());
    par.decode(Span<uint8_t>(encoded.data(), size), Span<double>(decoded2));
    if (decoded2 != decoded) {
        std::cerr << "FAIL: " << what << ": parallel values differ\n";
        failures++;
        return;
    }
    const auto parStats = par.getStats();

    if (parStats.nValues != seqStats.nValues || parStats.nBlocks != seqStats.nBlocks) {
        std::cerr << "FAIL: " << what << ": nValues " << parStats.nValues << " vs " << seqStats.nValues
                  << ", nBlocks " << parStats.nBlocks << " vs " << seqStats.nBlocks << '\n';
        failures++;
    }
    if (parStats.compressedSize != seqStats.compressedSize) {
        std::cerr << "FAIL: " << what << ": compressedSize " << parStats.compressedSize << " vs "
                  << seqStats.compressedSize << '\n';
        failures++;
    }
    if (parStats.counter.ncRef != seqStats.counter.ncRef || parStats.counter.ncNoRef != seqStats.counter.ncNoRef) {
        std::cerr << "FAIL: " << what << ": nc counters differ (" << parStats.counter.ncRef << " vs "
                  << seqStats.counter.ncRef << ", " << parStats.counter.ncNoRef << " vs " << seqStats.counter.ncNoRef << ")\n";
        failures++;
    }
    for (size_t k = 0; k < SpAecStats::kMax; ++k) {
        if (parStats.counter.ssRef[k] != seqStats.counter.ssRef[k]
            || parStats.counter.ssNoRef[k] != seqStats.counter.ssNoRef[k]) {
            std::cerr << "FAIL: " << what << ": ss counters differ at k=" << k << '\n';
            failures++;
            break;
        }
    }
    for (size_t i = 0; i <= SpAecStats::segmentSize; ++i) {
        if (parStats.counter.zbRef[i] != seqStats.counter.zbRef[i]
            || parStats.counter.zbNoRef[i] != seqStats.counter.zbNoRef[i]
            || parStats.counter.zbRosRef[i] != seqStats.counter.zbRosRef[i]
            || parStats.counter.zbRosNoRef[i] != seqStats.counter.zbRosNoRef[i]) {
            std::cerr << "FAIL: " << what << ": zb counters differ at " << i << '\n';
            failures++;
            break;
        }
    }
    if (parStats.counter.seRef != seqStats.counter.seRef || parStats.counter.seNoRef != seqStats.counter.seNoRef) {
        std::cerr << "FAIL: " << what << ": se counters differ\n";
        failures++;
    }
}

int main()
{
    try {
        // Zeros: zero-block runs and ROSes; sin data: split-sample and second extension;
        // random data: no compression
        {
            std::vector<double> zeros(3000, 0.0);
            expectEqualStats("zeros", 8, 8, 2, zeros);
        }
        {
            std::vector<double> sinData(5000);
            for (size_t i = 0; i < sinData.size(); ++i) {
                sinData[i] = 100.0 + 50.0 * std::sin(i * 0.01);
            }
            expectEqualStats("sin", 16, 32, 64, sinData);
        }
        {
            std::vector<double> mixed(8000);
            for (size_t i = 0; i < mixed.size(); ++i) {
                const size_t seg = (i / 64) % 3;
                mixed[i] = seg == 0 ? 0.0 : (seg == 1 ? static_cast<double>(i % 4) : static_cast<double>(i * 7919 % 256));
            }
            expectEqualStats("mixed", 8, 16, 3, mixed);
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
