// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// spaec in one program from translation units compiled for different instruction sets (this one
// without AVX2, isa_odr_avx2.cc with -mavx2): the code has AVX2 variants (#if defined(__AVX2__)),
// so each unit must use its own instance of the functions (spaec/detail/Isa.h), not one chosen by
// the linker for both (One Definition Rule: e.g. AVX2 code on a CPU without AVX2). Both instances
// must write the same data.

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "spaec/spaec.h"

using Compress = std::vector<uint8_t> (*)(const std::vector<uint16_t>&, const spaec::Options&);
Compress compressAvx2();

int main()
{
    int failures = 0;
    const Compress own = &spaec::compress<uint16_t>;
    const Compress avx2 = compressAvx2();
    if (own == avx2) {
        std::cerr << "FAIL: the translation units with and without AVX2 share one instance of spaec::compress\n";
        ++failures;
    }
    if (__builtin_cpu_supports("avx2")) {
        std::mt19937 rng(1);
        std::vector<uint16_t> samples(10000);
        int v = 2000;
        for (auto& s : samples) {
            v = std::max(0, std::min(4095, v + static_cast<int>(rng() % 31) - 15));
            s = static_cast<uint16_t>(v);
        }
        for (auto selector : { spaec::SplitSelector::Search, spaec::SplitSelector::Estimate }) {
            spaec::Options options;
            options.bitsPerSample = 12;
            options.splitSelector = selector;
            if (own(samples, options) != avx2(samples, options)) {
                std::cerr << "FAIL: the instances with and without AVX2 write different data\n";
                ++failures;
            }
        }
    }
    if (failures == 0) {
        std::cout << "OK\n";
    }
    return failures == 0 ? 0 : 1;
}
