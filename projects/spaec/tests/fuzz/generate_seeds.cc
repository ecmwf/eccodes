// Seed corpus generator for the spaec fuzzers: writes valid CCSDS streams (encoded with
// EncoderSeq) prefixed with the parameter header parsed by the fuzz harnesses:
//   [0] bits per value 1..32, [1] block size 8/16/32/64, [2..3] rsi 1..4096 (LE16 + 1),
//   [4..5] nValues (LE16), [6] decoder threads 1..8, rest: the compressed stream.
// Seeds: random data, zero blocks, zero-block runs, remainder-of-segment (ROS), no
// compression (NC) and second extension (SE) blocks.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "spaec/CodeOption.h"
#include "spaec/Span.h"
#include "spaec/SpAec.h"
#include "spaec/encoder/EncoderSeq.h"

using namespace spaec;

namespace
{

uint8_t blockSizeIndex(uint8_t blockSize)
{
    return blockSize == 8 ? 0 : blockSize == 16 ? 1
                          : blockSize == 32     ? 2
                                                : 3;
}

void writeSeed(const std::string& dir, const std::string& name, uint8_t bpv, uint8_t blockSize,
               uint16_t rsi, uint16_t nValues, uint8_t nThreads, std::vector<uint32_t> values)
{
    encoder::EncoderSeq<uint32_t, uint32_t> streamEncoder{ bpv, blockSize, rsi, CodeOptionSet::Basic };
    std::vector<uint8_t> target(encoder::SpAec<double>::maxEncodedSize(values.size(), bpv, blockSize), 0);
    const size_t size = streamEncoder.encode(Span<uint32_t>(values), Span<uint8_t>(target));

    std::vector<uint8_t> file(7 + size, 0);
    file[0] = static_cast<uint8_t>(bpv - 1);
    file[1] = blockSizeIndex(blockSize);
    file[2] = static_cast<uint8_t>((rsi - 1) & 0xff);
    file[3] = static_cast<uint8_t>(((rsi - 1) >> 8) & 0xff);
    file[4] = static_cast<uint8_t>(nValues & 0xff);
    file[5] = static_cast<uint8_t>((nValues >> 8) & 0xff);
    file[6] = static_cast<uint8_t>(nThreads - 1);
    std::memcpy(file.data() + 7, target.data(), size);

    const std::string path = dir + "/" + name;
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) {
        std::fprintf(stderr, "cannot open %s\n", path.c_str());
        std::exit(1);
    }
    const size_t written = std::fwrite(file.data(), 1, file.size(), f);
    if (written != file.size() || std::fclose(f) != 0) {
        std::fprintf(stderr, "cannot write %s\n", path.c_str());
        std::exit(1);
    }
}

// Deterministic splitmix64: a stable corpus matters more than unpredictability here
struct SeedRng
{
    uint64_t state;
    uint64_t operator()()
    {
        state += 0x9e3779b97f4a7c15ull;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
        return z ^ (z >> 31);
    }
};

std::vector<uint32_t> randomValues(size_t n, uint64_t range, SeedRng& rng)
{
    std::vector<uint32_t> values(n);
    for (auto& v : values) {
        v = static_cast<uint32_t>(rng() % range);
    }
    return values;
}

}  // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <output-dir>\n", argv[0]);
        return 1;
    }
    const std::string dir = argv[1];
    SeedRng rng{ 20251223 };

    // Random data (mixed code options, partial blocks and partial RSIs included)
    writeSeed(dir, "random_8_32_64", 8, 32, 64, 517, 4, randomValues(517, 256, rng));
    writeSeed(dir, "random_16_16_40", 16, 16, 40, 300, 2, randomValues(300, 1ull << 16, rng));
    writeSeed(dir, "random_32_8_1", 32, 8, 1, 65, 8, randomValues(65, 1ull << 32, rng));
    writeSeed(dir, "random_1_64_4096", 1, 64, 4096, 65535, 6, randomValues(65535, 2, rng));
    writeSeed(dir, "random_12_8_3", 12, 8, 3, 77, 3, randomValues(77, 1ull << 12, rng));

    // Zero blocks only: constant data, runs below the remainder-of-segment threshold
    writeSeed(dir, "zero_blocks", 8, 8, 3, 48, 2, std::vector<uint32_t>(48, 7));
    writeSeed(dir, "zero_blocks_16", 16, 16, 2, 128, 2, std::vector<uint32_t>(128, 0));

    // Zero-block runs of 5..10 blocks inside a segment (explicit run codewords)
    {
        std::vector<uint32_t> values;
        for (int rep = 0; rep < 8; ++rep) {
            for (int i = 0; i < 7 * 8; ++i)
                values.push_back(0);
            for (int i = 0; i < 8; ++i)
                values.push_back(200 + static_cast<uint32_t>(rep));
        }
        writeSeed(dir, "zero_runs", 8, 8, 4096, static_cast<uint16_t>(values.size()), 4, values);
    }

    // Remainder-of-segment (ROS): a whole segment of zero blocks
    writeSeed(dir, "ros_only", 8, 8, 64, 512, 2, std::vector<uint32_t>(512, 123));
    writeSeed(dir, "ros_many", 16, 32, 4096, 65535, 3, std::vector<uint32_t>(65535, 1));

    // No compression only: alternating extreme values defeat FS and SE
    {
        std::vector<uint32_t> values(64);
        for (size_t i = 0; i < values.size(); ++i)
            values[i] = (i % 2) ? 255u : 0u;
        writeSeed(dir, "nc_only", 8, 8, 2, 64, 2, values);
    }

    // Second extension only: pairs of equal values decreasing by one per pair
    {
        std::vector<uint32_t> values(64);
        uint32_t v = 200;
        for (size_t i = 0; i < values.size(); i += 2) {
            values[i] = v;
            values[i + 1] = v;
            --v;
        }
        writeSeed(dir, "se_only", 8, 8, 2, 64, 2, values);
        std::vector<uint32_t> v2(128);
        uint32_t w = 40000;
        for (size_t i = 0; i < v2.size(); i += 2) {
            v2[i] = w;
            v2[i + 1] = w;
            --w;
        }
        writeSeed(dir, "se_only_16", 16, 16, 2, 128, 3, v2);
    }

    std::printf("seeds written to %s\n", dir.c_str());
    return 0;
}
