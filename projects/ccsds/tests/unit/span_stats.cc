// Unit tests of Span, SplitBlock and the statistics of a decoded stream:
// - SplitBlock splits n values into full blocks and a part for every n up to 300 and every block size
// - Span: begin/end/data/size/drop
// - the code option counters of the statistics add up to the number of blocks, the statistics
//   print without NaN or infinity
#include <cstdint>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "ccsds/Span.h"
#include "ccsds/Stats.h"
#include "ccsds/decoder/DecoderSeq.h"
#include "ccsds/encoder/EncoderSeq.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static void splitBlock()
{
    for (size_t block : { 1, 8, 32, 64 }) {
        for (size_t n = 0; n <= 300; ++n) {
            std::vector<int> v(n);
            for (size_t i = 0; i < n; ++i) v[i] = static_cast<int>(i);
            SplitBlock<int> s(Span<int>(v), block);
            const std::string name = "SplitBlock(" + std::to_string(n) + ", " + std::to_string(block) + ")";
            check(s.nFull() == n / block, name + ": nFull");
            check(s.hasPart() == (n % block != 0) && s.nPart() == (n % block != 0 ? 1u : 0u), name + ": part");
            bool ok = true;
            for (size_t b = 0; b < s.nFull(); ++b) {
                ok = ok && s.full(b).size() == block && s.full(b)[0] == static_cast<int>(b * block);
            }
            if (s.hasPart()) {
                ok = ok && s.part().size() == n % block && s.part()[0] == static_cast<int>(s.nFull() * block);
            }
            check(ok, name + ": contents");
        }
    }
}

static void span()
{
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    Span<int> s(v);
    int sum = 0;
    for (int x : s) sum += x;
    check(s.size() == 5 && s.data() == v.data() && sum == 15 && s.end() - s.begin() == 5, "Span over 5 values");
    Span<int> d = s.drop(2);
    check(d.size() == 3 && d[0] == 3 && d.data() == v.data() + 2, "Span::drop(2)");
    check(s.drop(5).size() == 0, "Span::drop of all values");
}

static void statistics()
{
    std::mt19937_64 rng(7);
    const size_t n = 20000;
    std::vector<uint16_t> samples(n);
    for (size_t i = 0; i < n; ++i) {  // zero runs, small values, noise of different widths
        const size_t part = (i / 500) % 4;
        samples[i] = static_cast<uint16_t>(part == 0 ? 7 : part == 1 ? 7 + rng() % 3 : part == 2 ? rng() & 0xFF : rng() & 0xFFFF);
    }
    encoder::EncoderSeq<uint16_t, uint16_t> encoder(16, 32, 64, CodeOptionSet::Basic);
    std::vector<uint8_t> stream(n * 3 + 64, 0);
    stream.resize(encoder.encode(Span<uint16_t>(samples), Span<uint8_t>(stream)));
    DecoderSeq<uint16_t, uint16_t> decoder(16, 32, 64, CodeOptionSet::Basic);
    std::vector<uint16_t> decoded(n);
    decoder.decode(Span<uint8_t>(stream), Span<uint16_t>(decoded));
    const SpAecStats stats = decoder.getStats();
    check(stats.nValues == n && stats.nBlocks == (n + 31) / 32 && stats.compressedSize == stream.size(), "statistics: sizes");

    // Every block is counted once: zero-block runs count their blocks
    const auto& c = stats.counter;
    size_t blocks = c.seNoRef + c.seRef + c.ncNoRef + c.ncRef;
    for (size_t k = 0; k < c.ssNoRef.size(); ++k) blocks += c.ssNoRef[k] + c.ssRef[k];
    for (size_t len = 0; len < c.zbNoRef.size(); ++len) blocks += len * (c.zbNoRef[len] + c.zbRef[len] + c.zbRosNoRef[len] + c.zbRosRef[len]);
    check(blocks == stats.nBlocks, "statistics: the counters add up to " + std::to_string(blocks) + " blocks instead of " + std::to_string(stats.nBlocks));

    SpAecStats copy = stats;
    std::ostringstream out;
    std::streambuf* old = std::cout.rdbuf(out.rdbuf());
    copy.print();
    copy.printMetrics();
    std::cout.rdbuf(old);
    check(!out.str().empty() && out.str().find("nan") == std::string::npos && out.str().find("inf") == std::string::npos, "statistics print without nan or inf");
}

int main()
{
    splitBlock();
    span();
    statistics();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
