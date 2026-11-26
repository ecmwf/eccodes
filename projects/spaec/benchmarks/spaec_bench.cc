// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// usage: spaec_bench [--runs N] [--threads 1,2,4,...] [field.u32 ...]
//
// An overview of the speed of SpAec: compression and decompression of integer samples (blocks of 32,
// reference interval 128, unit-delay preprocessor) with both split-sample selectors, and with
// several threads. Built with libaec (SPAEC_BENCH_LIBAEC), libaec is measured as well, and the data
// of Search must be the data of libaec. Without files: four synthetic fields of 1440 x 721 values
// with 9, 12, 16 and 24 bits. A file: the bits per sample (uint32), then one uint32 per sample.
// Times: the fastest of N runs (default 5), warm caches. Exit status 1 if a check fails.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <random>
#include <string>
#include <vector>

#ifdef SPAEC_BENCH_LIBAEC
    #include <libaec.h>
#endif

#include "spaec/spaec.h"

namespace
{

struct Field
{
    std::string name;
    uint32_t bits;
    std::vector<uint32_t> samples;
};

int32_t errors = 0;

void fail(const std::string& what)
{
    std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    ++errors;
}

// A smooth field on a 1440 x 721 grid with some noise, quantised to the given bits
Field syntheticField(uint32_t bits)
{
    const size_t nx = 1440, ny = 721;
    std::mt19937_64 rng(bits);
    std::normal_distribution<double> noise(0.0, 0.002);
    const double pi = 3.14159265358979323846;
    std::vector<double> v(nx * ny);
    for (size_t j = 0; j < ny; ++j) {
        for (size_t i = 0; i < nx; ++i) {
            const double x = 2.0 * pi * static_cast<double>(i) / nx, y = pi * static_cast<double>(j) / ny;
            v[j * nx + i] = std::sin(3 * x) * std::sin(2 * y) + 0.5 * std::cos(7 * x + 5 * y) + 0.2 * std::sin(19 * x) * std::cos(13 * y) + noise(rng);
        }
    }
    const auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    const double scale = (std::ldexp(1.0, static_cast<int>(bits)) - 1.0) / (*mx - *mn);
    Field f{ "synthetic" + std::to_string(bits), bits, std::vector<uint32_t>(v.size()) };
    for (size_t i = 0; i < v.size(); ++i) {
        f.samples[i] = static_cast<uint32_t>((v[i] - *mn) * scale + 0.5);
    }
    return f;
}

bool readField(const char* path, Field& f)
{
    FILE* file = std::fopen(path, "rb");
    if (!file) {
        return false;
    }
    uint32_t w = 0;
    std::vector<uint32_t> words;
    while (std::fread(&w, 4, 1, file) == 1) {
        words.push_back(w);
    }
    std::fclose(file);
    if (words.size() < 2 || words[0] < 1 || words[0] > 32) {
        return false;
    }
    std::string name(path);
    name = name.substr(name.find_last_of("/\\") + 1);
    f = Field{ name.substr(0, name.find('.')), words[0], std::vector<uint32_t>(words.begin() + 1, words.end()) };
    return true;
}

double fastest(int32_t runs, const std::function<void()>& job)
{
    double best = 1e30;
    for (int32_t r = 0; r < runs; ++r) {
        const auto t0 = std::chrono::steady_clock::now();
        job();
        best = std::min(best, std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count());
    }
    return best;
}

struct Totals
{
    double mib = 0, encode = 0, decode = 0;
    size_t bytes = 0;
};

// One field with samples of type T: the coders of the table, then the thread counts with Search
template <typename T>
void benchField(const Field& f, int32_t runs, const std::vector<uint32_t>& threads, std::vector<Totals>& totals, std::vector<Totals>& threadTotals)
{
    const std::vector<T> x(f.samples.begin(), f.samples.end());
    const size_t n = x.size();
    const double mib = static_cast<double>(n * sizeof(T)) / 1048576.0;
    spaec::Options o;
    o.bitsPerSample = f.bits;
    std::vector<uint8_t> out(spaec::maxCompressedSize(n, o));
    std::vector<T> back(n);
    std::vector<uint8_t> search;
    std::printf("%-14s %2u bits %8zu", f.name.c_str(), f.bits, n);

    size_t column = 0;
    auto report = [&](const char* name, double enc, double dec, size_t size) {
        std::printf(" | %s %6.0f %6.0f %9zu", name, mib / enc, mib / dec, size);
        totals[column].mib += mib;
        totals[column].encode += enc;
        totals[column].decode += dec;
        totals[column].bytes += size;
        ++column;
    };
#ifdef SPAEC_BENCH_LIBAEC
    {
        size_t size = 0;
        auto stream = [&](aec_stream& s) {
            s = aec_stream{};
            s.bits_per_sample = f.bits;
            s.block_size = 32;
            s.rsi = 128;
            s.flags = AEC_DATA_PREPROCESS;
        };
        const double enc = fastest(runs, [&] {
            aec_stream s;
            stream(s);
            s.next_in = reinterpret_cast<const uint8_t*>(x.data());
            s.avail_in = n * sizeof(T);
            s.next_out = out.data();
            s.avail_out = out.size();
            if (aec_buffer_encode(&s) != AEC_OK) {
                fail(f.name + ": libaec encode");
            }
            size = s.total_out;
        });
        const std::vector<uint8_t> data(out.begin(), out.begin() + static_cast<std::ptrdiff_t>(size));
        const double dec = fastest(runs, [&] {
            aec_stream s;
            stream(s);
            s.next_in = data.data();
            s.avail_in = data.size();
            s.next_out = reinterpret_cast<uint8_t*>(back.data());
            s.avail_out = n * sizeof(T);
            if (aec_buffer_decode(&s) != AEC_OK) {
                fail(f.name + ": libaec decode");
            }
        });
        if (back != x) {
            fail(f.name + ": libaec round trip");
        }
        report("libaec", enc, dec, size);
        search = data;
    }
#endif
    for (spaec::SplitSelector selector : { spaec::SplitSelector::Search, spaec::SplitSelector::Estimate }) {
        const bool isSearch = selector == spaec::SplitSelector::Search;
        o.splitSelector = selector;
        size_t size = 0;
        const double enc = fastest(runs, [&] { size = spaec::compress(x.data(), n, o, out.data(), out.size()); });
        const std::vector<uint8_t> data(out.begin(), out.begin() + static_cast<std::ptrdiff_t>(size));
        const double dec = fastest(runs, [&] { spaec::decompress(data.data(), data.size(), o, back.data(), n); });
        if (back != x) {
            fail(f.name + ": round trip");
        }
        if (isSearch && !search.empty() && data != search) {
            fail(f.name + ": Search differs from libaec");
        }
        if (isSearch) {
            search = data;
        }
        report(isSearch ? "Search" : "Estimate", enc, dec, size);
    }
    std::printf("\n");

    o.splitSelector = spaec::SplitSelector::Search;
    for (size_t i = 0; i < threads.size(); ++i) {
        o.threads = threads[i];
        size_t size = 0;
        const double enc = fastest(runs, [&] { size = spaec::compress(x.data(), n, o, out.data(), out.size()); });
        const double dec = fastest(runs, [&] { spaec::decompress(out.data(), size, o, back.data(), n); });
        if (size != search.size() || !std::equal(search.begin(), search.end(), out.begin()) || back != x) {
            fail(f.name + ": " + std::to_string(threads[i]) + " threads");
        }
        threadTotals[i].mib += mib;
        threadTotals[i].encode += enc;
        threadTotals[i].decode += dec;
    }
}

std::vector<uint32_t> parseList(const std::string& s)
{
    std::vector<uint32_t> list;
    size_t pos = 0;
    while (pos < s.size()) {
        const size_t end = s.find(',', pos);
        list.push_back(static_cast<uint32_t>(std::stoul(s.substr(pos, end - pos))));
        pos = end == std::string::npos ? s.size() : end + 1;
    }
    return list;
}

}  // namespace

int main(int argc, char** argv)
{
    int32_t runs = 5;
    std::vector<uint32_t> threads{ 1, 2, 4, 8, 16 };
    std::vector<Field> fields;
    for (int32_t a = 1; a < argc; ++a) {
        const std::string arg = argv[a];
        if (arg == "--runs" && a + 1 < argc) {
            runs = std::max(1, std::stoi(argv[++a]));
        }
        else if (arg == "--threads" && a + 1 < argc) {
            threads = parseList(argv[++a]);
        }
        else {
            Field f;
            if (!readField(argv[a], f)) {
                std::fprintf(stderr, "cannot read %s\n", argv[a]);
                return 2;
            }
            fields.push_back(std::move(f));
        }
    }
    if (fields.empty()) {
        for (uint32_t bits : { 9u, 12u, 16u, 24u }) {
            fields.push_back(syntheticField(bits));
        }
    }

    std::printf("spaec %s: MiB/s of the samples (encode, decode) and compressed bytes; fastest of %d runs\n", spaec::version(), runs);
    std::vector<const char*> names;
#ifdef SPAEC_BENCH_LIBAEC
    names.push_back("libaec");
#endif
    names.push_back("Search");
    names.push_back("Estimate");
    std::vector<Totals> totals(names.size()), threadTotals(threads.size());
    for (const Field& f : fields) {
        if (f.bits <= 8) {
            benchField<uint8_t>(f, runs, threads, totals, threadTotals);
        }
        else if (f.bits <= 16) {
            benchField<uint16_t>(f, runs, threads, totals, threadTotals);
        }
        else {
            benchField<uint32_t>(f, runs, threads, totals, threadTotals);
        }
    }

    std::printf("\n| Coder | Encode MiB/s | Decode MiB/s | Bytes |\n|---|---|---|---|\n");
    for (size_t c = 0; c < names.size(); ++c) {
        std::printf("| %s | %.0f | %.0f | %zu |\n", names[c], totals[c].mib / totals[c].encode, totals[c].mib / totals[c].decode, totals[c].bytes);
    }
    std::printf("\n| Threads (Search) | Encode MiB/s | Decode MiB/s |\n|---|---|---|\n");
    for (size_t i = 0; i < threads.size(); ++i) {
        std::printf("| %u | %.0f | %.0f |\n", threads[i], threadTotals[i].mib / threadTotals[i].encode, threadTotals[i].mib / threadTotals[i].decode);
    }
    if (errors != 0) {
        std::printf("%d checks failed\n", errors);
        return 1;
    }
    return 0;
}
