// Unit test of the optimisers (DecOptimizer, Dec64Optimizer, BinDecOptimizer, BPVOptimizer) and
// of SimplePackingBinary::pack/unpack: for fields of many ranges, the chosen parameters must be
// valid and the field must be restored within one quantisation step:
// - bits per value 1..64 (0 or a constant field only if min == max)
// - binary scale factor within +-127 (except BPVOptimizer, which uses 0)
// - the reference value is not above the scaled minimum and is a float (stored as IEEE float)
// - |decoded - value| <= 2^E * 10^-D (one quantisation step)
// and number_of_bits (the bits of a value) exhaustively for small values and at every power of 2
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "simple_packing/optimizer/BPVOptimizer.h"
#include "simple_packing/optimizer/BinDecOptimizer.h"
#include "simple_packing/optimizer/Dec64Optimizer.h"
#include "simple_packing/optimizer/DecOptimizer.h"
#include "simple_packing/simple/Binary.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static std::vector<double> field(double mn, double mx)
{
    std::vector<double> v = { mn, mx };
    for (int i = 1; i < 99; ++i) {
        v.push_back(mn + (mx - mn) * (i * 0.37 - std::floor(i * 0.37)));  // spread over the range
    }
    return v;
}

static void checkOptimizer(const std::string& name, std::unique_ptr<Optimizer> optimizer, double mn, double mx, bool binaryScaleZero = false)
{
    SimplePackingBinary<double> sp(std::move(optimizer));
    const std::vector<double> values = field(mn, mx);
    const std::string what = name + " [" + std::to_string(mn) + ", " + std::to_string(mx) + "]";
    try {
        auto [params, packed] = sp.pack(values);
        const double decimal = sp_power<double>(params.decimalScaleFactor(), 10);
        if (mn == mx) {
            check(params.isConstantField() || params.bitsPerValue() == 0, what + ": a constant field");
        }
        else {
            check(params.bitsPerValue() >= 1 && params.bitsPerValue() <= 64, what + ": bits per value " + std::to_string(params.bitsPerValue()));
            check(binaryScaleZero ? params.binaryScaleFactor() == 0 : std::labs(params.binaryScaleFactor()) <= 127,
                  what + ": binary scale factor " + std::to_string(params.binaryScaleFactor()));
        }
        check(params.referenceValue() <= mn * decimal || mn == mx, what + ": reference value above the scaled minimum");
        check(static_cast<double>(static_cast<float>(params.referenceValue())) == params.referenceValue(), what + ": the reference value is not a float");

        const std::vector<double> decoded = sp.unpack(params, packed, values.size());
        const double step = sp_power<double>(params.binaryScaleFactor(), 2) / decimal;
        double worst = 0;
        for (size_t i = 0; i < values.size(); ++i) {
            worst = std::max(worst, std::fabs(decoded[i] - values[i]) - step - 1e-12 * std::fabs(values[i]));
        }
        check(worst <= 0, what + ": decoded values differ by more than one step " + std::to_string(step));
    }
    catch (const std::exception& e) {
        check(false, what + ": " + e.what());
    }
}

static void optimizers()
{
    const double ranges[][2] = { { 0, 1 }, { -1, 1 }, { 250.15, 320.7 }, { 0, 1e-4 }, { -1e10, 1e10 }, { 1e-30, 3e-30 }, { -5e3, 7e7 },
                                 { 101325, 101325 }, { 0, 0 }, { -273.15, -273.15 }, { 0.001, 1000 }, { 1e20, 1.0001e20 } };
    for (const auto& r : ranges) {
        for (long bpv : { 1L, 8L, 12L, 16L, 24L, 32L }) {
            checkOptimizer("DecOptimizer(" + std::to_string(bpv) + ")", std::make_unique<DecOptimizer>(bpv, 127), r[0], r[1]);
            checkOptimizer("BinDecOptimizer(" + std::to_string(bpv) + ")", std::make_unique<BinDecOptimizer>(0, 1, static_cast<int>(bpv)), r[0], r[1]);
        }
        for (long bpv : { 8L, 16L, 24L }) {
            checkOptimizer("Dec64Optimizer(" + std::to_string(bpv) + ")", std::make_unique<Dec64Optimizer>(bpv, 127), r[0], r[1]);
        }
        // Decimal precision: the scaled range must be below 2^31 (like ecCodes, larger ones are rejected)
        for (int d : { 0, 2 }) {
            if ((r[1] - r[0]) * sp_power<double>(d, 10) < 2147483647.0 && r[1] - r[0] >= 1e-3) {
                checkOptimizer("BPVOptimizer(" + std::to_string(d) + ")", std::make_unique<BPVOptimizer>(d), r[0], r[1], true);
            }
        }
    }
}

static void numberOfBits()
{
    // The bits of x: 0 for 0, k for 2^(k-1) <= x < 2^k
    for (unsigned long x = 0; x < (1ul << 16); ++x) {
        const long expected = x == 0 ? 0 : static_cast<long>(std::floor(std::log2(static_cast<double>(x)))) + 1;
        const auto bits = number_of_bits(x);
        check(bits && *bits == expected, "number_of_bits(" + std::to_string(x) + ")");
    }
    for (int k = 1; k <= 31; ++k) {
        const unsigned long p = 1ul << k;
        const auto below = number_of_bits(p - 1), at = number_of_bits(p);
        check(below && *below == k, "number_of_bits(2^" + std::to_string(k) + " - 1)");
        check(!at || *at == k + 1, "number_of_bits(2^" + std::to_string(k) + ")");
    }
}

int main()
{
    optimizers();
    numberOfBits();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
