// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Bits per value outside 1..64 must be rejected with std::invalid_argument (they were used as
// an index into the table of codecs), and constant fields (0 bits per value) must decode to the
// reference value (decoding them crashed).

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "simple_packing/detail/optimizer/DecOptimizer.h"
#include "simple_packing/detail/simple/Binary.h"

using namespace sp::detail;

static int32_t failures = 0;

template <typename F>
static void expectInvalid(const char* what, int64_t bpv, F&& f)
{
    try {
        f();
        std::cerr << "FAIL: " << what << " with " << bpv << " bits per value was accepted\n";
        failures++;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "OK: " << e.what() << '\n';
    }
}

int main()
{
    std::vector<uint8_t> buffer(1024, 0x55);
    std::vector<double> values(16, 1.0);

    for (int64_t bpv : { -INT64_C(1), INT64_C(65), INT64_C(255) }) {
        Parameters params;
        params.bitsPerValue(bpv);
        SimplePackingBinary<double> sp(nullptr);
        expectInvalid("unpackTo", bpv, [&] { sp.unpackTo(params, buffer.data(), buffer.size(), values.size(), values.data()); });
        expectInvalid("packTo", bpv, [&] { sp.packTo(params, values.data(), values.size(), buffer.data()); });
    }

    // Constant field: no packed data, all values are the reference value
    {
        std::vector<double> constant(10, 42.5);
        SimplePackingBinary<double> sp(std::make_unique<DecOptimizer>(16, 127));
        auto [params, packed] = sp.pack(constant);
        try {
            auto decoded = sp.unpack(params, packed, constant.size());
            if (decoded != constant) {
                std::cerr << "FAIL: constant field decoded wrongly\n";
                failures++;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "FAIL: constant field: " << e.what() << '\n';
            failures++;
        }
    }

    return failures == 0 ? 0 : 1;
}
