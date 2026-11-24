// Unit tests of the parallel decoder derived from regression tests:
// - decode_parallel: DecoderPar must decode inputs smaller than one RSI and a trailing partial RSI
//   like DecoderSeq (they were not decoded), and ThreadPool must pass an exception of a task to
//   the caller
// - decode_parallel_stats: DecoderPar::getStats must equal the statistics of DecoderSeq (they stayed
//   at their defaults)
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ccsds/ThreadPool.h"
#include "ccsds/decoder/DecoderPar.h"
#include "ccsds/decoder/DecoderSeq.h"
#include "ccsds/encoder/EncoderSeq.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

static bool sameStats(const SpAecStats& a, const SpAecStats& b)
{
    const auto& x = a.counter;
    const auto& y = b.counter;
    return a.nBlocks == b.nBlocks && a.nValues == b.nValues && a.compressedSize == b.compressedSize && a.bpv == b.bpv &&
           a.blockSize == b.blockSize && a.rsiSize == b.rsiSize && x.ssNoRef == y.ssNoRef && x.ssRef == y.ssRef && x.seNoRef == y.seNoRef &&
           x.seRef == y.seRef && x.ncNoRef == y.ncNoRef && x.ncRef == y.ncRef && x.zbNoRef == y.zbNoRef && x.zbRef == y.zbRef &&
           x.zbRosNoRef == y.zbRosNoRef && x.zbRosRef == y.zbRosRef;
}

// bpv 16, block size 32, RSI 4 blocks (128 values): a random walk with zero runs
static void parallelLikeSequential(size_t n, size_t nThreads)
{
    std::vector<uint16_t> values(n);
    uint32_t x = 1000, seed = 12345;
    for (size_t i = 0; i < n; ++i) {
        seed = seed * 1103515245u + 12345u;
        x = (i / 200) % 3 == 0 ? 0 : (x + (seed >> 16) % 64) % 65536;
        values[i] = static_cast<uint16_t>(x);
    }
    encoder::EncoderSeq<uint16_t, uint16_t> encoder(16, 32, 4, CodeOptionSet::Basic);
    std::vector<uint8_t> stream(n * 3 + 64, 0);
    stream.resize(encoder.encode(Span<uint16_t>(values), Span<uint8_t>(stream)));

    DecoderSeq<uint16_t, uint16_t> sequential(16, 32, 4, CodeOptionSet::Basic);
    std::vector<uint16_t> expected(n);
    sequential.decode(Span<uint8_t>(stream), Span<uint16_t>(expected));

    DecoderPar<uint16_t, uint16_t> parallel(16, 32, 4, CodeOptionSet::Basic, static_cast<uint8_t>(nThreads));
    std::vector<uint16_t> decoded(n, 7);
    const std::string name = std::to_string(n) + " values, " + std::to_string(nThreads) + " threads";
    try {
        parallel.decodeParallel(Span<uint8_t>(stream), Span<uint16_t>(decoded), [](uint16_t v) { return v; });
        check(decoded == values && expected == values, name + ": values differ");
        check(sameStats(parallel.getStats(), sequential.getStats()), name + ": statistics differ from the sequential decoder");
    }
    catch (const std::exception& e) {
        check(false, name + ": " + e.what());
    }
}

static void threadPool()
{
    ThreadPool pool(2);
    auto ok = pool.enqueue([] { return 42; });
    auto bad = pool.enqueue([]() -> int { throw std::runtime_error("task failed"); });
    check(ok.get() == 42, "ThreadPool: result of a task");
    bool thrown = false;
    try {
        bad.get();
    }
    catch (const std::runtime_error&) {
        thrown = true;
    }
    check(thrown, "ThreadPool: the exception of a task must reach the caller");

    thrown = false;
    try {
        ThreadPool none(0);
    }
    catch (const std::invalid_argument&) {
        thrown = true;
    }
    check(thrown, "ThreadPool with 0 threads must throw");
}

// Truncated streams: DecoderPar must report the error like DecoderSeq, whether it is found while
// the calling thread looks for the start of the next thread's RSIs (half of the stream) or by a
// thread decoding its RSIs (only the end missing), and must wait for the started threads
static void truncated()
{
    std::vector<uint16_t> values(40000);
    for (size_t i = 0; i < values.size(); ++i) {
        values[i] = static_cast<uint16_t>((i * 2654435761u) >> 20);
    }
    encoder::EncoderSeq<uint16_t, uint16_t> encoder(16, 16, 8, CodeOptionSet::Basic);
    std::vector<uint8_t> stream(values.size() * 3 + 64, 0);
    stream.resize(encoder.encode(Span<uint16_t>(values), Span<uint8_t>(stream)));
    for (double keep : { 0.5, 0.999 }) {
        std::vector<uint8_t> part(stream.begin(), stream.begin() + static_cast<std::ptrdiff_t>(static_cast<double>(stream.size()) * keep));
        for (size_t nThreads : { 2, 4 }) {
            DecoderPar<uint16_t, uint16_t> decoder(16, 16, 8, CodeOptionSet::Basic, static_cast<uint8_t>(nThreads));
            std::vector<uint16_t> decoded(values.size());
            const std::string what = std::to_string(keep) + " of the stream, " + std::to_string(nThreads) + " threads";
            try {
                decoder.decodeParallel(Span<uint8_t>(part), Span<uint16_t>(decoded), [](uint16_t v) { return v; });
                check(false, what + ": no error");
            }
            catch (const std::runtime_error& e) {
                check(std::string(e.what()).find("truncated or corrupt") != std::string::npos, what + ": " + e.what());
            }
        }
    }
}

int main()
{
    for (size_t nThreads : { 1, 2, 3, 8 }) {
        for (size_t n : { size_t{ 1 }, size_t{ 50 }, size_t{ 127 }, size_t{ 128 }, size_t{ 129 }, size_t{ 128 * 5 + 77 }, size_t{ 128 * 8 } }) {
            parallelLikeSequential(n, nThreads);
        }
    }
    threadPool();
    truncated();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
