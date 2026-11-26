// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// usage: sp_bench [--runs N] [--threads 1,2,4,...] [field.u32 ...]
//
// An overview of the speed of simple_packing: packing (sp::range, sp::scaling with Method::bits and
// sp::pack) and unpacking of double and float values, and of doubles with several threads (each
// thread packs a part of a multiple of 8 values, so that the parts start at a byte). Without files:
// four synthetic fields of 1440 x 721 values with 9, 12, 16 and 24 bits. A file: the bits per value
// (uint32), then one uint32 per value (the values are x * 0.01 + 250). Times: the fastest of N runs
// (default 5), warm caches. Exit status 1 if a check fails.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "simple_packing/simple_packing.h"

namespace
{

struct Field
{
    std::string name;
    int32_t bits;
    std::vector<double> values;
};

int32_t errors = 0;

void fail(const std::string& what)
{
    std::fprintf(stderr, "FAIL: %s\n", what.c_str());
    ++errors;
}

// A smooth field on a 1440 x 721 grid with some noise (temperatures around 250 to 300 K)
Field syntheticField(int32_t bits)
{
    const size_t nx = 1440, ny = 721;
    const double pi = 3.14159265358979323846;
    std::mt19937_64 rng(static_cast<uint64_t>(bits));
    std::normal_distribution<double> noise(0.0, 0.05);
    Field f{ "synthetic" + std::to_string(bits), bits, std::vector<double>(nx * ny) };
    for (size_t j = 0; j < ny; ++j) {
        for (size_t i = 0; i < nx; ++i) {
            const double x = 2.0 * pi * static_cast<double>(i) / nx, y = pi * static_cast<double>(j) / ny;
            f.values[j * nx + i] = 275.0 + 20.0 * std::sin(3 * x) * std::sin(2 * y) + 5.0 * std::cos(7 * x + 5 * y) + noise(rng);
        }
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
    f = Field{ name.substr(0, name.find('.')), static_cast<int32_t>(words[0]), std::vector<double>(words.size() - 1) };
    for (size_t i = 1; i < words.size(); ++i) {
        f.values[i - 1] = words[i] * 0.01 + 250.0;
    }
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

// Runs job(part, start, count) for the parts of n values: a multiple of 8 values each
template <typename F>
void inParts(uint32_t threads, size_t n, F&& job)
{
    const size_t part = ((n + threads - 1) / threads + 7) / 8 * 8;
    std::vector<std::thread> pool;
    for (uint32_t t = 0; t < threads && t * part < n; ++t) {
        const size_t start = t * part, count = std::min(part, n - start);
        if (threads == 1) {
            job(t, start, count);
        }
        else {
            pool.emplace_back([&job, t, start, count] { job(t, start, count); });
        }
    }
    for (std::thread& th : pool) {
        th.join();
    }
}

struct Totals
{
    double mib = 0, pack = 0, unpack = 0;
};

// Packs and unpacks the values as T with the given number of threads; checks the result
template <typename T>
void bench(const Field& f, uint32_t threads, int32_t runs, std::vector<uint8_t>& reference, Totals& totals)
{
    const std::vector<T> values(f.values.begin(), f.values.end());
    const size_t n = values.size();
    std::vector<T> back(n);
    std::vector<uint8_t> packed;
    sp::Scaling scaling;
    const double pack = fastest(runs, [&] {
        std::vector<sp::Range<T>> ranges(threads, sp::Range<T>{ values[0], values[0] });
        inParts(threads, n, [&](uint32_t t, size_t start, size_t count) { ranges[t] = sp::range(values.data() + start, count); });
        T mn = ranges[0].min, mx = ranges[0].max;
        for (const sp::Range<T>& r : ranges) {
            mn = std::min(mn, r.min);
            mx = std::max(mx, r.max);
        }
        scaling = sp::scaling(mn, mx, sp::Method::bits(f.bits));
        packed.resize(sp::packedSize(n, scaling));
        inParts(threads, n, [&](uint32_t, size_t start, size_t count) {
            const size_t offset = start * static_cast<size_t>(scaling.bitsPerValue) / 8;
            sp::pack(values.data() + start, count, scaling, packed.data() + offset, packed.size() - offset);
        });
    });
    const double unpack = fastest(runs, [&] {
        inParts(threads, n, [&](uint32_t, size_t start, size_t count) {
            const size_t offset = start * static_cast<size_t>(scaling.bitsPerValue) / 8;
            sp::unpack(packed.data() + offset, packed.size() - offset, scaling, back.data() + start, count);
        });
    });
    const double step = std::ldexp(1.0, scaling.binaryScaleFactor) * std::pow(10.0, -scaling.decimalScaleFactor);
    for (size_t i = 0; i < n; ++i) {
        if (!(std::fabs(static_cast<double>(back[i]) - static_cast<double>(values[i])) <= step)) {
            fail(f.name + ": value " + std::to_string(i) + " not restored");
            break;
        }
    }
    if (reference.empty()) {
        reference = packed;
    }
    else if (packed != reference) {
        fail(f.name + ": " + std::to_string(threads) + " threads give other packed data");
    }
    const double mib = static_cast<double>(n * sizeof(T)) / 1048576.0;
    totals.mib += mib;
    totals.pack += pack;
    totals.unpack += unpack;
    std::printf(" | %6.0f %6.0f", mib / pack, mib / unpack);
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
        for (int32_t bits : { 9, 12, 16, 24 }) {
            fields.push_back(syntheticField(bits));
        }
    }

    std::printf("simple_packing %s: MiB/s of the values (pack, unpack); fastest of %d runs\n", sp::version(), runs);
    std::printf("%-14s %-7s %8s | %13s | %13s | doubles with %s threads\n", "field", "bits", "values", "double", "float", "1..n");
    Totals doubles, floats;
    std::vector<Totals> threadTotals(threads.size());
    for (const Field& f : fields) {
        std::printf("%-14s %2d bits %8zu", f.name.c_str(), f.bits, f.values.size());
        std::vector<uint8_t> refDouble, refFloat;
        bench<double>(f, 1, runs, refDouble, doubles);
        bench<float>(f, 1, runs, refFloat, floats);
        for (size_t i = 0; i < threads.size(); ++i) {
            bench<double>(f, threads[i], runs, refDouble, threadTotals[i]);
        }
        std::printf("\n");
    }

    std::printf("\n| Values | Pack MiB/s | Unpack MiB/s |\n|---|---|---|\n");
    std::printf("| double | %.0f | %.0f |\n", doubles.mib / doubles.pack, doubles.mib / doubles.unpack);
    std::printf("| float | %.0f | %.0f |\n", floats.mib / floats.pack, floats.mib / floats.unpack);
    std::printf("\n| Threads (double) | Pack MiB/s | Unpack MiB/s |\n|---|---|---|\n");
    for (size_t i = 0; i < threads.size(); ++i) {
        std::printf("| %u | %.0f | %.0f |\n", threads[i], threadTotals[i].mib / threadTotals[i].pack, threadTotals[i].mib / threadTotals[i].unpack);
    }
    if (errors != 0) {
        std::printf("%d checks failed\n", errors);
        return 1;
    }
    return 0;
}
