// Conformance of SpAec to CCSDS 121.0-B-3 (Lossless Data Compression, Blue Book, August 2020),
// checked against a reference encoder and decoder written from the text of the standard only.
// Requirements and where they are checked:
//   3.1.6      J = 8, 16, 32, 64; n = 1..32 (unsigned samples; signed samples are not supported
//              by SpAec)                                                         -> all configurations
//   3.2, T3-1  FS codeword: m zeros and a one                                    -> fsCodewords, streams
//   3.3        split sample k: FS of the n-k MSBs, all FS codewords first, then the k LSBs -> streams
//   3.4        second extension: gamma = (a+b)(a+b+1)/2 + b, delta1 = 0 in a reference block,
//              FS codewords also for gamma >= 2^n                                -> streams, random policy
//   3.5, T3-2  zero block: runs within segments of 64 blocks of an RSI, one codeword per run,
//              ROS for >= 5 blocks to the end of a segment (also a smaller last segment); a block
//              with a reference sample is a zero block if its J-1 other samples are zero -> verify
//   3.5 NOTE   streams without ROS in segments smaller than 64 blocks can be decoded -> noRos policy
//   3.6        no compression: the J (J-1) preprocessed samples, n bits each      -> streams
//   3.7        selection: the shortest option; zero blocks always as zero block; ties: no
//              compression, then second extension, then the smallest k           -> verify
//   4.2.5/4.2.6 unit-delay predictor, the first sample of an RSI is the reference sample, coded
//              uncoded after the option ID                                       -> streams
//   4.3        r = 1..4096 blocks                                                -> configurations
//   4.4        prediction error mapper with theta = min(x^ - xmin, xmax - x^)    -> streams
//   5.2.1, T5-1 option IDs of the basic and restricted sets (restricted for n <= 4; for 4 < n <= 8
//              the IDs equal the basic set)                                      -> idTable, streams
//   5.2.2-5.2.6 CDS formats with and without reference sample                    -> streams
//   5.3.1      the stream ends on a byte boundary, the fill bits are zero        -> verify
//   any valid stream (other option choices, larger k, SE with large gamma) decodes -> random policy
// Not implemented by SpAec: packets and the compression identification packet
// (5.3, 6), the file header (7), the bypass predictor and coding without preprocessor (4.2.3),
// signed samples (3.1.6, 4.4).
// SpAec writes one zero byte for an empty input; the reference writes nothing.
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "spaec/CodeOption.h"
#include "spaec/decoder/DecoderPar.h"
#include "spaec/decoder/DecoderSeq.h"
#include "spaec/encoder/EncoderPar.h"
#include "spaec/encoder/EncoderSeq.h"

using namespace spaec;

static int failures = 0;
static long checks = 0;

static void fail(const std::string& what)
{
    if (++failures <= 40) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

// ---------------------------------------------------------------------------------------------
// Reference implementation (from the text of CCSDS 121.0-B-3 only)

struct Params
{
    unsigned n;       // bits per sample
    unsigned J;       // block size
    unsigned r;       // reference sample interval in blocks
    bool restricted;  // restricted set of code options

    std::string str() const
    {
        return "n=" + std::to_string(n) + " J=" + std::to_string(J) + " r=" + std::to_string(r) + (restricted ? " restricted" : " basic");
    }
    uint64_t xmax() const { return n == 64 ? ~uint64_t{ 0 } : (uint64_t{ 1 } << n) - 1; }
};

// Table 5-1: length of the ID of FS, split sample and no compression (the zero block and second
// extension IDs are that many zeros and one more bit), and the largest k (-1: no FS, no split)
struct IdTable
{
    unsigned len;
    int kmax;
};

static IdTable idTable(const Params& p)
{
    if (p.restricted && p.n <= 2)
        return { 1, -1 };
    if (p.restricted && p.n <= 4)
        return { 2, 1 };
    if (p.n <= 8)
        return { 3, 5 };
    if (p.n <= 16)
        return { 4, 13 };
    return { 5, 29 };
}

static std::string bitString(uint64_t v, unsigned len)
{
    std::string s;
    for (unsigned i = len; i-- > 0;)
        s += ((v >> i) & 1) ? '1' : '0';
    return s;
}

class BitWriter
{
public:
    void bit(unsigned b)
    {
        if (nBits_ % 8 == 0)
            bytes_.push_back(0);
        if (b)
            bytes_.back() |= static_cast<uint8_t>(0x80u >> (nBits_ % 8));
        ++nBits_;
    }
    void bits(uint64_t v, unsigned len)
    {
        for (unsigned i = len; i-- > 0;)
            bit((v >> i) & 1);
    }
    void fs(uint64_t m)  // Table 3-1: m zeros and a one
    {
        for (uint64_t i = 0; i < m; ++i)
            bit(0);
        bit(1);
    }
    std::vector<uint8_t> bytes() const { return bytes_; }  // fill bits are zero (5.3.1)

private:
    std::vector<uint8_t> bytes_;
    uint64_t nBits_ = 0;
};

class BitReader
{
public:
    explicit BitReader(const std::vector<uint8_t>& b) : b_(b) {}
    unsigned bit()
    {
        if (pos_ >= b_.size() * 8)
            throw std::runtime_error("reference decoder: end of the stream");
        const unsigned v = (b_[pos_ / 8] >> (7 - pos_ % 8)) & 1;
        ++pos_;
        return v;
    }
    uint64_t bits(unsigned len)
    {
        uint64_t v = 0;
        for (unsigned i = 0; i < len; ++i)
            v = (v << 1) | bit();
        return v;
    }
    uint64_t fs()
    {
        uint64_t m = 0;
        while (bit() == 0)
            ++m;
        return m;
    }
    uint64_t pos() const { return pos_; }
    size_t size() const { return b_.size(); }

private:
    const std::vector<uint8_t>& b_;
    uint64_t pos_ = 0;
};

// 4.4: prediction error mapper for unsigned n-bit samples (xmin = 0)
static uint64_t mapError(uint64_t x, uint64_t pred, uint64_t xmax)
{
    const uint64_t theta = std::min(pred, xmax - pred);
    if (x >= pred) {
        const uint64_t d = x - pred;
        return d <= theta ? 2 * d : theta + d;
    }
    const uint64_t d = pred - x;
    return d <= theta ? 2 * d - 1 : theta + d;
}

static uint64_t unmapError(uint64_t delta, uint64_t pred, uint64_t xmax)
{
    const uint64_t theta = std::min(pred, xmax - pred);
    if (delta <= 2 * theta)
        return delta % 2 == 0 ? pred + delta / 2 : pred - (delta + 1) / 2;
    return theta == pred ? pred + (delta - theta) : pred - (delta - theta);
}

// One coded data set (CDS), for the checks of the encoder's choices
struct Cds
{
    char kind;                     // 'Z' zero block, 'E' second extension, 'S' split sample (k), 'N' no compression
    int k = 0;                     // split sample: k (0 is FS)
    uint64_t blocks = 1;           // zero block: number of blocks
    bool ros = false;              // zero block: remainder-of-segment codeword
    bool ref = false;              // with reference sample
    size_t segBlock = 0;           // index of the first block in its segment
    size_t segLen = 0;             // blocks in its segment
    uint64_t bits = 0;             // length
    std::vector<uint64_t> deltas;  // preprocessed samples of the block (not for zero blocks)
};

enum class Policy
{
    Spec,             // shortest option, ties as in 3.7.4
    Random,           // any option (decodable, not optimal)
    NoRos,            // as Spec, but no ROS codeword in segments smaller than 64 blocks (3.5 NOTE)
    SecondExtension,  // the second extension for every block that is not a zero block
};

// 128-bit lengths (a second-extension symbol of two n = 32 bit samples has up to 65 bits);
// __extension__: no -Wpedantic warning
__extension__ using u128 = unsigned __int128;

// Lengths of the single-block options of a block (deltas[0] unused if ref)
struct OptionLengths
{
    uint64_t nc;
    u128 se;
    std::vector<uint64_t> ss;  // index k
};

static u128 gammaOf(uint64_t a, uint64_t b)
{
    const u128 s = static_cast<u128>(a) + b;
    return s * (s + 1) / 2 + b;
}

static OptionLengths optionLengths(const Params& p, const std::vector<uint64_t>& d, bool ref)
{
    const IdTable t = idTable(p);
    const size_t first = ref ? 1 : 0;
    const uint64_t refBits = ref ? p.n : 0;
    OptionLengths l;
    l.nc = t.len + refBits + (p.J - first) * uint64_t{ p.n };
    l.se = t.len + 1 + refBits;
    for (size_t i = 0; i < p.J; i += 2)
        l.se += gammaOf(i == 0 && ref ? 0 : d[i], d[i + 1]) + 1;  // 3.4.1: delta1 = 0 with reference
    for (int k = 0; k <= t.kmax; ++k) {
        uint64_t len = t.len + refBits;
        for (size_t i = first; i < p.J; ++i)
            len += (d[i] >> k) + 1 + k;
        l.ss.push_back(len);
    }
    return l;
}

static void writeBlock(BitWriter& w, const Params& p, const std::vector<uint64_t>& d, bool ref, uint64_t reference, char kind, int k)
{
    const IdTable t = idTable(p);
    const size_t first = ref ? 1 : 0;
    if (kind == 'N') {
        w.bits((uint64_t{ 1 } << t.len) - 1, t.len);
        if (ref)
            w.bits(reference, p.n);
        for (size_t i = first; i < p.J; ++i)
            w.bits(d[i], p.n);
    }
    else if (kind == 'E') {
        w.bits(0, t.len);
        w.bit(1);
        if (ref)
            w.bits(reference, p.n);
        for (size_t i = 0; i < p.J; i += 2)
            w.fs(static_cast<uint64_t>(gammaOf(i == 0 && ref ? 0 : d[i], d[i + 1])));
    }
    else {
        w.bits(static_cast<uint64_t>(k) + 1, t.len);
        if (ref)
            w.bits(reference, p.n);
        for (size_t i = first; i < p.J; ++i)
            w.fs(d[i] >> k);
        for (size_t i = first; i < p.J; ++i)
            w.bits(d[i] & ((uint64_t{ 1 } << k) - 1), static_cast<unsigned>(k));
    }
}

static std::vector<uint8_t> refEncode(const std::vector<uint64_t>& x, const Params& p, Policy policy, std::mt19937_64& rng)
{
    BitWriter w;
    const size_t N = x.size();
    const size_t nBlocks = (N + p.J - 1) / p.J;
    for (size_t b0 = 0; b0 < nBlocks; b0 += p.r) {
        const size_t nb = std::min<size_t>(p.r, nBlocks - b0);
        // Preprocessing of the RSI; a partial last block is completed with zero deltas
        std::vector<uint64_t> d(nb * p.J, 0);
        const uint64_t reference = x[b0 * p.J];
        for (size_t i = 1; i < nb * p.J && b0 * p.J + i < N; ++i)
            d[i] = mapError(x[b0 * p.J + i], x[b0 * p.J + i - 1], p.xmax());
        auto blockDeltas = [&](size_t b) { return std::vector<uint64_t>(d.begin() + b * p.J, d.begin() + (b + 1) * p.J); };
        auto isZero = [&](size_t b) {
            for (size_t i = (b == 0 ? 1 : 0); i < p.J; ++i)
                if (d[b * p.J + i] != 0)
                    return false;
            return true;
        };
        for (size_t s0 = 0; s0 < nb; s0 += 64) {
            const size_t segLen = std::min<size_t>(64, nb - s0);
            for (size_t j = 0; j < segLen;) {
                const size_t b = s0 + j;
                if (isZero(b)) {
                    size_t m = 0;
                    while (j + m < segLen && isZero(s0 + j + m))
                        ++m;
                    const bool toEnd = j + m == segLen;
                    const bool useRos = toEnd && m >= 5 && (policy != Policy::NoRos || segLen == 64);
                    w.bits(0, idTable(p).len);
                    w.bit(0);
                    if (b == 0)
                        w.bits(reference, p.n);
                    w.fs(useRos ? 4 : (m <= 4 ? m - 1 : m));  // Table 3-2
                    j += m;
                    continue;
                }
                const bool ref = b == 0;
                const std::vector<uint64_t> bd = blockDeltas(b);
                const OptionLengths l = optionLengths(p, bd, ref);
                char kind = 'N';
                int k = 0;
                if (policy == Policy::SecondExtension) {
                    kind = 'E';
                }
                else if (policy == Policy::Random) {
                    // Any option of reasonable length (FS codewords of up to a few times the
                    // no-compression length)
                    std::vector<std::pair<char, int>> options{ { 'N', 0 } };
                    if (l.se <= 4 * l.nc + 64)
                        options.push_back({ 'E', 0 });
                    for (size_t kk = 0; kk < l.ss.size(); ++kk)
                        if (l.ss[kk] <= 4 * l.nc + 64)
                            options.push_back({ 'S', static_cast<int>(kk) });
                    const auto o = options[rng() % options.size()];
                    kind = o.first;
                    k = o.second;
                }
                else {
                    u128 best = l.nc;
                    if (l.se < best) {
                        best = l.se;
                        kind = 'E';
                    }
                    for (size_t kk = 0; kk < l.ss.size(); ++kk)
                        if (l.ss[kk] < best) {
                            best = l.ss[kk];
                            kind = 'S';
                            k = static_cast<int>(kk);
                        }
                }
                writeBlock(w, p, bd, ref, reference, kind, k);
                ++j;
            }
        }
    }
    return w.bytes();
}

// Decodes N samples; throws on an invalid stream. Fills `trace` with the coded data sets.
static std::vector<uint64_t> refDecode(const std::vector<uint8_t>& stream, size_t N, const Params& p, std::vector<Cds>* trace = nullptr)
{
    const IdTable t = idTable(p);
    BitReader in(stream);
    std::vector<uint64_t> x(N);
    const size_t nBlocks = (N + p.J - 1) / p.J;
    for (size_t b0 = 0; b0 < nBlocks; b0 += p.r) {
        const size_t nb = std::min<size_t>(p.r, nBlocks - b0);
        std::vector<uint64_t> d(nb * p.J, 0);
        uint64_t reference = 0;
        for (size_t s0 = 0; s0 < nb; s0 += 64) {
            const size_t segLen = std::min<size_t>(64, nb - s0);
            for (size_t j = 0; j < segLen;) {
                const size_t b = s0 + j;
                const bool ref = b == 0;
                const uint64_t start = in.pos();
                Cds c;
                c.ref = ref;
                c.segBlock = j;
                c.segLen = segLen;
                const uint64_t id = in.bits(t.len);
                if (id == 0) {
                    const unsigned extra = in.bit();
                    if (ref)
                        reference = in.bits(p.n);
                    if (extra == 0) {
                        c.kind = 'Z';
                        const uint64_t m = in.fs();
                        if (m == 4) {
                            c.ros = true;
                            c.blocks = segLen - j;
                        }
                        else {
                            c.blocks = m < 4 ? m + 1 : m;
                        }
                        if (c.blocks == 0 || c.blocks > segLen - j)
                            throw std::runtime_error("reference decoder: zero-block run beyond the segment");
                    }
                    else {
                        c.kind = 'E';
                        for (size_t i = 0; i < p.J; i += 2) {
                            const uint64_t g = in.fs();
                            uint64_t s = 0;
                            while ((s + 1) * (s + 2) / 2 <= g)
                                ++s;
                            const uint64_t bb = g - s * (s + 1) / 2;
                            if (i == 0 && ref && s != bb)
                                throw std::runtime_error("reference decoder: delta1 of a reference block is not 0");
                            d[b * p.J + i] = s - bb;
                            d[b * p.J + i + 1] = bb;
                        }
                    }
                }
                else if (id == (uint64_t{ 1 } << t.len) - 1) {
                    c.kind = 'N';
                    if (ref)
                        reference = in.bits(p.n);
                    for (size_t i = ref ? 1 : 0; i < p.J; ++i)
                        d[b * p.J + i] = in.bits(p.n);
                }
                else {
                    c.kind = 'S';
                    c.k = static_cast<int>(id) - 1;
                    if (c.k > t.kmax)
                        throw std::runtime_error("reference decoder: invalid option ID");
                    if (ref)
                        reference = in.bits(p.n);
                    for (size_t i = ref ? 1 : 0; i < p.J; ++i)
                        d[b * p.J + i] = in.fs() << c.k;
                    for (size_t i = ref ? 1 : 0; i < p.J; ++i)
                        d[b * p.J + i] |= in.bits(static_cast<unsigned>(c.k));
                }
                if (c.kind != 'Z') {
                    c.deltas.assign(d.begin() + b * p.J, d.begin() + (b + 1) * p.J);
                    for (size_t i = ref ? 1 : 0; i < p.J; ++i)
                        if (d[b * p.J + i] > p.xmax())
                            throw std::runtime_error("reference decoder: preprocessed sample beyond 2^n - 1");
                }
                c.bits = in.pos() - start;
                if (trace)
                    trace->push_back(c);
                j += c.blocks;
            }
        }
        // Inverse preprocessing (4.2.5, 4.4)
        x[b0 * p.J] = reference;
        for (size_t i = 1; i < nb * p.J && b0 * p.J + i < N; ++i)
            x[b0 * p.J + i] = unmapError(d[i], x[b0 * p.J + i - 1], p.xmax());
    }
    // 5.3.1: the stream ends on the byte boundary after the last CDS, the fill bits are zero
    if ((in.pos() + 7) / 8 != in.size())
        throw std::runtime_error("reference decoder: " + std::to_string(in.size() * 8 - in.pos()) + " bits after the last CDS");
    while (in.pos() % 8 != 0)
        if (in.bit() != 0)
            throw std::runtime_error("reference decoder: fill bit is not zero");
    return x;
}

// ---------------------------------------------------------------------------------------------
// Checks of the tables

static void fsCodewords()
{
    // Table 3-1 / Table 3-2 through the reference writer and SpAec's BitBuffer
    for (uint64_t m = 0; m < 70; ++m) {
        std::vector<uint8_t> buf(32 + BitBuffer::guardBytes, 0);
        BitBuffer bb{ Span<uint8_t>(buf) };
        bb.writeCodeword(m);
        BitWriter w;
        w.fs(m);
        const auto expected = w.bytes();
        ++checks;
        if (!std::equal(expected.begin(), expected.end(), buf.begin()))
            fail("FS codeword of " + std::to_string(m) + " differs from Table 3-1");
    }
}

static void idTables()
{
    // Table 5-1, literally; columns: restricted n=1,2 | restricted n=3,4 | n<=8 | 8<n<=16 | 16<n<=32
    struct Row
    {
        const char* option;
        const char* ids[5];
    };
    const Row rows[] = {
        { "ZB", { "00", "000", "0000", "00000", "000000" } },
        { "SE", { "01", "001", "0001", "00001", "000001" } },
        { "k0", { "", "01", "001", "0001", "00001" } },
        { "k1", { "", "10", "010", "0010", "00010" } },
        { "k2", { "", "", "011", "0011", "00011" } },
        { "k5", { "", "", "110", "0110", "00110" } },
        { "k6", { "", "", "", "0111", "00111" } },
        { "k7", { "", "", "", "1000", "01000" } },
        { "k13", { "", "", "", "1110", "01110" } },
        { "k14", { "", "", "", "", "01111" } },
        { "k15", { "", "", "", "", "10000" } },
        { "k29", { "", "", "", "", "11110" } },
        { "NC", { "1", "11", "111", "1111", "11111" } },
    };
    auto column = [](const Params& p) { return p.restricted && p.n <= 2 ? 0 : p.restricted && p.n <= 4 ? 1
                                                                        : p.n <= 8                     ? 2
                                                                        : p.n <= 16                    ? 3
                                                                                                       : 4; };
    for (unsigned n = 1; n <= 32; ++n) {
        for (bool restricted : { false, true }) {
            if (restricted && n > 8)
                continue;
            const Params p{ n, 16, 1, restricted };
            const IdTable t = idTable(p);
            const int col = column(p);
            for (const Row& row : rows) {
                const std::string option = row.option;
                std::string ref;  // the ID of the reference implementation
                if (option == "ZB")
                    ref = bitString(0, t.len + 1);
                else if (option == "SE")
                    ref = bitString(1, t.len + 1);
                else if (option == "NC")
                    ref = bitString((uint64_t{ 1 } << t.len) - 1, t.len);
                else {
                    const int k = std::stoi(option.substr(1));
                    ref = k <= t.kmax ? bitString(static_cast<uint64_t>(k) + 1, t.len) : "";
                }
                ++checks;
                if (ref != row.ids[col])
                    fail("Table 5-1 " + option + " " + p.str() + ": reference ID " + ref + ", table " + row.ids[col]);
            }
            // SpAec's code options
            const auto cos = restricted ? CodeOptionSet::Restricted : CodeOptionSet::Basic;
            const auto n8 = static_cast<uint8_t>(n);
            auto id = [](uint64_t value, unsigned len) { return bitString(value, len); };
            try {
                ZeroBlockCodeOption zb(n8, cos);
                SecondExtensionCodeOption se(n8, cos);
                NoCompressionCodeOption nc(n8, cos);
                checks += 3;
                if (id(zb.value, zb.length) != bitString(0, t.len + 1))
                    fail("zero-block ID " + p.str());
                if (id(se.value, se.length) != bitString(1, t.len + 1))
                    fail("second-extension ID " + p.str());
                if (id(nc.value, nc.length) != bitString((uint64_t{ 1 } << t.len) - 1, t.len))
                    fail("no-compression ID " + p.str());
                for (int k = 0; k <= t.kmax; ++k) {
                    SplitSampleCodeOption ss(n8, static_cast<uint8_t>(k), cos);
                    ++checks;
                    if (id(ss.value, ss.length) != bitString(static_cast<uint64_t>(k) + 1, t.len))
                        fail("split-sample ID k=" + std::to_string(k) + " " + p.str());
                }
            }
            catch (const std::exception& e) {
                fail("code options " + p.str() + ": " + e.what());
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------
// Streams

// Checks a stream of SpAec's encoder against the standard; returns false if it cannot be decoded
static bool verify(const std::vector<uint8_t>& stream, const std::vector<uint64_t>& x, const Params& p, const std::string& what, long& kTies)
{
    if (x.empty()) {
        ++checks;
        if (!(stream.size() == 1 && stream[0] == 0))
            fail(what + ": empty input is not one zero byte");
        return true;
    }
    std::vector<Cds> trace;
    std::vector<uint64_t> decoded;
    try {
        decoded = refDecode(stream, x.size(), p, &trace);
    }
    catch (const std::exception& e) {
        fail(what + ": the reference decoder rejects the stream: " + e.what());
        return false;
    }
    ++checks;
    if (decoded != x) {
        fail(what + ": the reference decoder decodes other values");
        return false;
    }
    for (size_t i = 0; i < trace.size(); ++i) {
        const Cds& c = trace[i];
        ++checks;
        if (c.kind == 'Z') {
            // 3.5.3: one codeword per run (no zero block after a zero block in a segment);
            // ROS for >= 5 blocks to the end of the segment, also in a smaller last segment
            if (i > 0 && trace[i - 1].kind == 'Z' && c.segBlock != 0)
                fail(what + ": two zero-block codewords for one run");
            const bool toEnd = c.segBlock + c.blocks == c.segLen;
            if (c.ros && c.blocks < 5)
                fail(what + ": ROS for fewer than 5 blocks");
            if (!c.ros && toEnd && c.blocks >= 5)
                fail(what + ": no ROS for " + std::to_string(c.blocks) + " blocks to the end of a segment of " + std::to_string(c.segLen));
            continue;
        }
        // 3.7.2: a zero block (J-1 zero samples after a reference) must be coded as zero block
        bool zero = true;
        for (size_t j = c.ref ? 1 : 0; j < p.J; ++j)
            zero = zero && c.deltas[j] == 0;
        if (zero) {
            fail(what + ": a zero block coded with option " + c.kind);
            continue;
        }
        // 3.7.1, 3.7.3, 3.7.4: the shortest option; ties: no compression, second extension,
        // the smallest k
        const OptionLengths l = optionLengths(p, c.deltas, c.ref);
        u128 best = std::min<u128>(l.nc, l.se);
        int bestK = -1;
        for (size_t k = 0; k < l.ss.size(); ++k)
            if (l.ss[k] < best) {
                best = l.ss[k];
                bestK = static_cast<int>(k);
            }
        if (c.bits != best) {
            fail(what + ": CDS " + std::to_string(i) + " (" + c.kind + ") has " + std::to_string(c.bits) + " bits, the shortest option " +
                 std::to_string(static_cast<uint64_t>(best)));
            continue;
        }
        const char expectedKind = l.nc == best ? 'N' : l.se == best ? 'E'
                                                                    : 'S';
        if (c.kind != expectedKind)
            fail(what + ": CDS " + std::to_string(i) + " is " + c.kind + ", the tie rule of 3.7.4 selects " + expectedKind);
        else if (c.kind == 'S' && c.k != bestK)
            ++kTies;  // 3.7.4 c) (a recommendation): another k of the same length
    }
    return true;
}

template <typename T>
static void encodeAndDecode(const std::vector<uint64_t>& x, const Params& p, std::mt19937_64& rng, long& kTies, const std::string& name)
{
    const auto cos = p.restricted ? CodeOptionSet::Restricted : CodeOptionSet::Basic;
    const auto n8 = static_cast<uint8_t>(p.n);
    const auto J8 = static_cast<uint8_t>(p.J);
    const auto r16 = static_cast<uint16_t>(p.r);
    const std::string what = name + " " + p.str() + " N=" + std::to_string(x.size());
    std::vector<T> samples(x.begin(), x.end());

    // The encoder against the standard
    try {
        encoder::EncoderSeq<T, T> encoder(n8, J8, r16, cos);
        std::vector<uint8_t> stream(x.size() * 5 + 4096, 0);
        stream.resize(encoder.encode(Span<T>(samples), Span<uint8_t>(stream)));
        verify(stream, x, p, "EncoderSeq " + what, kTies);
        if (x.size() > 2000 && p.r <= 8) {
            encoder::EncoderPar<T, T> par(n8, J8, r16, cos, 3);
            std::vector<uint8_t> pstream(x.size() * 5 + 4096, 0);
            pstream.resize(par.encode(Span<T>(samples), Span<uint8_t>(pstream)));
            ++checks;
            if (pstream != stream)
                fail("EncoderPar " + what + ": differs from EncoderSeq");
        }
    }
    catch (const std::exception& e) {
        fail("EncoderSeq " + what + ": " + e.what());
    }

    // The decoders on streams of the reference encoder (with the choices of other encoders)
    for (Policy policy : { Policy::Spec, Policy::Random, Policy::NoRos }) {
        const char* pname = policy == Policy::Spec ? "spec" : policy == Policy::Random ? "random options"
                                                                                       : "no ROS in small segments";
        const std::vector<uint8_t> stream = refEncode(x, p, policy, rng);
        ++checks;
        try {
            if (refDecode(stream, x.size(), p) != x) {
                fail("reference round trip " + what + " (" + pname + ")");
                continue;
            }
        }
        catch (const std::exception& e) {
            fail("reference round trip " + what + " (" + pname + "): " + e.what());
            continue;
        }
        if (x.empty())
            continue;
        try {
            DecoderSeq<T, T> decoder(n8, J8, r16, cos);
            std::vector<T> decoded(x.size());
            decoder.decode(Span<uint8_t>(const_cast<uint8_t*>(stream.data()), stream.size()), Span<T>(decoded));
            ++checks;
            if (!std::equal(decoded.begin(), decoded.end(), samples.begin()))
                fail("DecoderSeq " + what + " (" + pname + "): other values");
        }
        catch (const std::exception& e) {
            fail("DecoderSeq " + what + " (" + pname + "): " + e.what());
        }
        try {
            DecoderPar<T, T> decoder(n8, J8, r16, cos, 3);
            std::vector<T> decoded(x.size());
            decoder.decodeParallel(Span<uint8_t>(const_cast<uint8_t*>(stream.data()), stream.size()), Span<T>(decoded), [](T v) { return v; });
            ++checks;
            if (!std::equal(decoded.begin(), decoded.end(), samples.begin()))
                fail("DecoderPar " + what + " (" + pname + "): other values");
        }
        catch (const std::exception& e) {
            fail("DecoderPar " + what + " (" + pname + "): " + e.what());
        }
    }
}

// 3.4.2: transformed symbols gamma >= 2^n (here up to about 2^24, beyond the float square root
// of a decoder)
// 4.4: the preprocessed samples are n-bit values. A stream with a larger one (an FS or split-sample
// codeword or a second-extension pair beyond 2^n - 1) is invalid and must be rejected, not decoded
// to wrapped values. A second-extension symbol of 2^n or more is valid if its pair is in range.
static void outOfRangeSamples()
{
    const unsigned n = 8, J = 8;
    const Params p{ n, J, 2, false };  // two blocks: a reference block (no compression), the test block
    struct Case
    {
        const char* what;
        char kind;
        int k;
        std::vector<uint64_t> deltas;
        bool valid;
    };
    const Case cases[] = {
        { "FS codeword 300", 'S', 0, { 300, 0, 0, 0, 0, 0, 0, 0 }, false },
        { "split sample k=2, sample 1023", 'S', 2, { 1023, 0, 0, 0, 0, 0, 0, 0 }, false },
        { "second extension, pair (300, 1)", 'E', 0, { 300, 1, 0, 0, 0, 0, 0, 0 }, false },
        { "second extension, pair (0, 256)", 'E', 0, { 0, 0, 0, 256, 0, 0, 0, 0 }, false },
        { "second extension, pair (255, 255)", 'E', 0, { 255, 255, 0, 0, 0, 0, 0, 0 }, true },
        { "FS codeword 255", 'S', 0, { 255, 0, 0, 0, 0, 0, 0, 0 }, true },
    };
    for (const auto& c : cases) {
        BitWriter w;
        writeBlock(w, p, { 0, 1, 2, 3, 4, 5, 6, 7 }, true, 100, 'N', 0);
        writeBlock(w, p, c.deltas, false, 0, c.kind, c.k);
        std::vector<uint8_t> stream = w.bytes();
        auto run = [&](const std::string& name, auto&& decode) {
            ++checks;
            try {
                decode();
                if (!c.valid)
                    fail(name + ", " + c.what + ": the invalid stream was decoded");
            }
            catch (const std::runtime_error& e) {
                if (c.valid)
                    fail(name + ", " + c.what + ": the valid stream was rejected: " + e.what());
            }
        };
        std::vector<uint8_t> out(2 * J);
        run("DecoderSeq", [&] { DecoderSeq<uint8_t, uint8_t>(n, J, 2, CodeOptionSet::Basic).decode(Span<uint8_t>(stream), Span<uint8_t>(out)); });
        run("DecoderPar", [&] {
            DecoderPar<uint8_t, uint8_t>(n, J, 2, CodeOptionSet::Basic, 2).decodeParallel(Span<uint8_t>(stream), Span<uint8_t>(out), [](uint8_t v) { return v; });
        });
        if (c.valid) {
            ++checks;
            const std::vector<uint64_t> expected = refDecode(stream, 2 * J, p);
            if (!std::equal(expected.begin(), expected.end(), out.begin()))
                fail(std::string("DecoderPar, ") + c.what + ": other values");
        }
    }
}

static void largeGamma()
{
    std::mt19937_64 rng(3);
    for (unsigned n : { 16u, 32u }) {
        for (uint64_t sum : { 2046u, 2047u, 2048u, 2049u, 4000u, 5791u }) {
            for (uint64_t b : { uint64_t{ 0 }, uint64_t{ 1 }, sum / 2, sum }) {
                // Samples whose deltas are (ref, 0 | a, b | 0, 0 | 0, 1) with a + b = sum
                const Params p{ n, 8, 1, false };
                const std::vector<uint64_t> deltas{ 0, 0, sum - b, b, 0, 0, 0, 1 };
                std::vector<uint64_t> x(deltas.size());
                x[0] = 12345;
                for (size_t i = 1; i < x.size(); ++i)
                    x[i] = unmapError(deltas[i], x[i - 1], p.xmax());
                ++checks;
                for (size_t i = 1; i < x.size(); ++i)
                    if (mapError(x[i], x[i - 1], p.xmax()) != deltas[i])
                        fail("largeGamma: field");
                const std::vector<uint8_t> stream = refEncode(x, p, Policy::SecondExtension, rng);
                const std::string what = "second extension, gamma " + std::to_string(static_cast<uint64_t>(gammaOf(sum - b, b))) + " " + p.str();
                auto run = [&](auto sample) {
                    using T = decltype(sample);
                    const std::vector<T> expected(x.begin(), x.end());
                    std::vector<T> decoded(x.size());
                    Span<uint8_t> in(const_cast<uint8_t*>(stream.data()), stream.size());
                    try {
                        DecoderSeq<T, T>(static_cast<uint8_t>(n), 8, 1, CodeOptionSet::Basic).decode(in, Span<T>(decoded));
                        ++checks;
                        if (decoded != expected)
                            fail("DecoderSeq " + what + ": other values");
                        std::fill(decoded.begin(), decoded.end(), T{ 0 });
                        DecoderPar<T, T>(static_cast<uint8_t>(n), 8, 1, CodeOptionSet::Basic, 2).decodeParallel(in, Span<T>(decoded), [](T v) { return v; });
                        ++checks;
                        if (decoded != expected)
                            fail("DecoderPar " + what + ": other values");
                    }
                    catch (const std::exception& e) {
                        fail("decoders " + what + ": " + e.what());
                    }
                };
                if (n == 16)
                    run(uint16_t{});
                else
                    run(uint32_t{});
            }
        }
    }
}

// Test fields: noise of a given number of bits, runs of constant values (zero blocks, also across
// segment and RSI boundaries), extreme values (the mapper's theta branch), small differences
// (ties between options)
static std::vector<uint64_t> field(const Params& p, size_t N, int kind, std::mt19937_64& rng)
{
    const uint64_t xmax = p.xmax();
    std::vector<uint64_t> x(N);
    uint64_t v = rng() & xmax;
    const unsigned noiseBits = static_cast<unsigned>(rng() % (p.n + 1));
    for (size_t i = 0; i < N; ++i) {
        switch (kind) {
            case 0:  // random walk with noise of noiseBits bits
            {
                const uint64_t step = noiseBits == 0 ? 0 : rng() & ((uint64_t{ 1 } << noiseBits) - 1);
                v = rng() % 2 ? (v > xmax - step ? xmax : v + step) : (v < step ? 0 : v - step);
                break;
            }
            case 1:  // uniform
                v = rng() & xmax;
                break;
            case 2:  // constant runs of random lengths (in blocks, sometimes unaligned) with spikes
                if (i % p.J == 0 && rng() % 4 == 0)
                    v = rng() & xmax;
                if (rng() % 97 == 0)
                    v = (v + 1) & xmax;
                break;
            case 3:  // extremes: near 0 and near xmax
            {
                const uint64_t e = std::min<uint64_t>(rng() & 3, xmax);
                v = rng() % 2 ? e : xmax - e;
                break;
            }
            default:  // small differences (0, +-1, +-2)
            {
                const uint64_t s = rng() % 3;
                v = rng() % 2 ? std::min(xmax, v + s) : (v < s ? 0 : v - s);
                break;
            }
        }
        x[i] = v;
    }
    return x;
}

template <typename T>
static void streams(const Params& p, std::mt19937_64& rng, long& kTies, int nFields)
{
    const size_t rsiSamples = size_t{ p.r } * p.J;
    for (int f = 0; f < nFields; ++f) {
        // Lengths: empty, one sample, partial blocks, partial RSIs, several segments and RSIs
        size_t N;
        switch (f % 6) {
            case 0:
                N = f == 0 ? 0 : 1;
                break;
            case 1:
                N = p.J - 1 + rng() % 2;
                break;
            case 2:
                N = rsiSamples * (1 + rng() % 2) + rng() % 3;
                break;
            default:
                N = 1 + rng() % std::min<size_t>(3 * rsiSamples + 2 * p.J, 6000);
                break;
        }
        const int kind = static_cast<int>(rng() % 5);
        encodeAndDecode<T>(field(p, N, kind, rng), p, rng, kTies, "field " + std::to_string(kind));
    }
}

int main()
{
    fsCodewords();
    idTables();
    largeGamma();
    outOfRangeSamples();

    std::mt19937_64 rng(121);
    long kTies = 0;
    const unsigned blockSizes[] = { 8, 16, 32, 64 };
    for (unsigned n = 1; n <= 32; ++n) {
        for (unsigned J : blockSizes) {
            for (unsigned r : { 1u, 2u, 3u, 7u, 64u, 65u, 100u, 128u }) {
                for (bool restricted : { false, true }) {
                    if (restricted && n > 8)
                        continue;
                    // Fewer fields for the long RSIs and the large block sizes
                    const int nFields = (r >= 64 ? 2 : 6) + (J <= 16 ? 2 : 0);
                    const Params p{ n, J, r, restricted };
                    if (n <= 8)
                        streams<uint8_t>(p, rng, kTies, nFields);
                    else if (n <= 16)
                        streams<uint16_t>(p, rng, kTies, nFields);
                    else
                        streams<uint32_t>(p, rng, kTies, nFields);
                }
            }
        }
    }
    // The largest reference sample interval (4.3)
    for (unsigned J : blockSizes) {
        const Params p{ 12, J, 4096, false };
        encodeAndDecode<uint16_t>(field(p, size_t{ 4096 } * J + 5, 2, rng), p, rng, kTies, "r=4096");
    }

    std::cout << checks << " checks, " << kTies << " split-sample blocks with another k of the same length than the smallest (3.7.4 c)\n";
    if (failures != 0) {
        std::cerr << failures << " failures\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
