// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The optimiser converted an out-of-range double to long (undefined behaviour, aborts under
// -fsanitize=float-cast-overflow): the encoded value for the maximum could exceed the long
// range when the reference value (a float32, rounded down) does not absorb pmin*10^D exactly
// and the binary scale factor is very negative. Found by the fuzz_simple_unpack fuzzer
// (minimised input 3a fc fc 40 40 bf b5 40..bf b5 40, two doubles near 32.5019608 with
// BinDecOptimizer(0, 1, knbit 60)); the values below reproduce it independently of the
// floating point path of the compiler that built the library.

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/simple/Binary.h"

using namespace sp::detail;

int main()
{
    // pmin is not a float32-exact multiple of any decimal scaling (residue up to the float32
    // spacing, 1024), pa = 1e-5 is tiny but far above the epsilon of the tiny-range branch:
    // the encoded maximum (residue + range) * 2^-E overflows the long range many times over
    const double pmin = 10000000102.4;
    const double pmax = pmin + 1e-5;
    const std::vector<double> values{pmin, pmax};

    try {
        SimplePackingBinary<double> packer(std::make_unique<BinDecOptimizer>(false, true, 62));
        auto [params, packed] = packer.pack(values);
        const std::vector<double> back = packer.unpack(params, packed, values.size());
        // Whatever parameters the optimiser selects, the round trip must stay consistent
        for (size_t i = 0; i < values.size(); ++i) {
            if (std::isnan(back[i])) {
                std::cerr << "FAIL: unpack produced NaN for value " << i << '\n';
                return 1;
            }
        }
    }
    catch (const std::exception&) {
        // rejected values or ranges are fine
    }
    return 0;
}
