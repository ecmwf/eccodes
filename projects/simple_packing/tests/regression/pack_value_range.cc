// Values that cannot be packed with the given parameters (below the reference value, above the
// range of bits per value, NaN, infinity) must be rejected: their conversion to integers wrapped
// around silently. A NaN in a field must also be detected when the parameters are computed (the
// minimum and maximum ignored it).

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "simple_packing/optimizer/DecOptimizer.h"
#include "simple_packing/simple/Binary.h"

using namespace sp;

static int failures = 0;

int main()
{
    const double nan = std::nan("");
    const double inf = std::numeric_limits<double>::infinity();

    // 8 bits, reference 10, no scaling: values must be in [10, 265]
    Parameters params;
    params.bitsPerValue(8);
    params.referenceValue(10.0);
    SimplePackingBinary<double> sp(nullptr);
    std::vector<unsigned char> buffer(16);
    for (double bad : { -1.0, 9.0, 266.0, 1e300, nan, inf, -inf }) {
        std::vector<double> values(10, 100.0);
        values[3] = bad;
        try {
            sp.packTo(params, values.data(), values.size(), buffer.data());
            std::cerr << "FAIL: value " << bad << " was packed\n";
            failures++;
        }
        catch (const std::runtime_error& e) {
            std::cout << "OK: " << e.what() << '\n';
        }
    }

    // The limits can be packed
    std::vector<double> limits = { 10.0, 265.0, 265.4, 100.0 };
    sp.packTo(params, limits.data(), limits.size(), buffer.data());

    // A NaN somewhere in a field
    for (size_t pos : { 0, 1, 5 }) {
        std::vector<double> values = { 1, 2, 3, 4, 5, 6, 7, 8 };
        values[pos] = nan;
        SimplePackingBinary<double> opt(std::make_unique<DecOptimizer>(16, 127));
        try {
            opt.pack(values);
            std::cerr << "FAIL: field with NaN at " << pos << " was packed\n";
            failures++;
        }
        catch (const std::runtime_error&) {
        }
    }

    return failures == 0 ? 0 : 1;
}
