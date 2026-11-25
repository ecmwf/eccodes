// Reference values that cannot be stored (beyond the float range, NaN, infinity) must be rejected:
// negative values below -FLT_MAX used to be "rounded" to garbage and packed as a constant field.

#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "simple_packing/optimizer/BPVOptimizer.h"
#include "simple_packing/optimizer/DecOptimizer.h"
#include "simple_packing/simple/Binary.h"

using namespace sp;

static int failures = 0;

static void expectNone(double value, FloatType type, const char* typeName)
{
    if (nearest_smaller_value(value, type).has_value()) {
        std::cerr << "FAIL: nearest " << typeName << " value <= " << value << " was found\n";
        failures++;
    }
}

template <typename F>
static void expectThrow(const char* what, F&& f)
{
    try {
        f();
        std::cerr << "FAIL: " << what << " was accepted\n";
        failures++;
    }
    catch (const std::exception& e) {
        std::cout << "OK: " << what << ": " << e.what() << '\n';
    }
}

int main()
{
    const double inf = std::numeric_limits<double>::infinity();
    for (double v : { -1e39, 1e39, -inf, inf, std::nan("") }) {
        expectNone(v, FloatType::IEEE32, "IEEE");
    }
    for (double v : { -1e76, 1e76, -inf, inf, std::nan("") }) {
        expectNone(v, FloatType::IBM32, "IBM");
    }

    // Values that are representable
    for (double v : { -1e30, -1.0, 0.0, 1.5, 1e30 }) {
        auto ieee = nearest_smaller_value(v, FloatType::IEEE32);
        if (!ieee || *ieee > v || static_cast<double>(static_cast<float>(*ieee)) != *ieee) {
            std::cerr << "FAIL: nearest IEEE value <= " << v << " is wrong\n";
            failures++;
        }
        auto ibm = nearest_smaller_value(v, FloatType::IBM32);
        if (!ibm || *ibm > v) {
            std::cerr << "FAIL: nearest IBM value <= " << v << " is wrong\n";
            failures++;
        }
    }

    // A field whose scaled minimum is beyond the float range
    std::vector<double> values = { -1e30, 1e30, 5.0 };
    expectThrow("BPVOptimizer with scaled minimum -1e39", [&] {
        SimplePackingBinary<double> sp(std::make_unique<BPVOptimizer>(9));
        sp.pack(values);
    });
    expectThrow("DecOptimizer with values beyond the float range", [&] {
        std::vector<double> big = { -1e300, 1e300 };
        SimplePackingBinary<double> sp(std::make_unique<DecOptimizer>(16, 127));
        sp.pack(big);
    });

    return failures == 0 ? 0 : 1;
}
