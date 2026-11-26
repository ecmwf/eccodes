// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the optimisers (DecOptimizer, BinDecOptimizer, BPVOptimizer) and
// of SimplePackingBinary::pack/unpack: for fields of many ranges, the chosen parameters must be
// valid and the field must be restored within one quantisation step:
// - bits per value 1..64 (0 or a constant field only if min == max)
// - binary scale factor within +-127 (except BPVOptimizer, which uses 0)
// - the reference value is not above the scaled minimum and is a float (stored as IEEE float)
// - |decoded - value| <= 2^E * 10^-D (one quantisation step)
// and number_of_bits (the bits of a value) exhaustively for small values and at every power of 2
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/BinOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"
#include "simple_packing/detail/simple/Array.h"
#include "simple_packing/detail/simple/Binary.h"

using namespace sp::detail;

static int32_t failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok && failures++ < 30) {
        std::cerr << "FAIL: " << what << '\n';
    }
}

static std::vector<double> field(double mn, double mx)
{
    std::vector<double> v = { mn, mx };
    for (int32_t i = 1; i < 99; ++i) {
        v.push_back(mn + (mx - mn) * (i * 0.37 - std::floor(i * 0.37)));  // spread over the range
    }
    return v;
}

// maxScale: the largest binary scale factor (127 in GRIB edition 1 and for the decimal optimisers,
// 32767 in GRIB edition 2); ibm: the reference value is an IBM float (not an IEEE float)
static void checkOptimizer(const std::string& name, std::unique_ptr<Optimizer> optimizer, double mn, double mx, bool binaryScaleZero = false, int64_t maxScale = 127, bool ibm = false)
{
    if (ibm) {
        optimizer->referenceFloatType(FloatType::IBM32);
    }
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
            check(binaryScaleZero ? params.binaryScaleFactor() == 0 : std::abs(params.binaryScaleFactor()) <= maxScale,
                  what + ": binary scale factor " + std::to_string(params.binaryScaleFactor()));
        }
        check(params.referenceValue() <= mn * decimal || mn == mx, what + ": reference value above the scaled minimum");
        const auto stored = nearest_smaller_value(params.referenceValue(), ibm ? FloatType::IBM32 : FloatType::IEEE32);
        check(stored && *stored == params.referenceValue(), what + ": the reference value is not an " + (ibm ? "IBM" : "IEEE") + " float");

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
    const double ranges[][2] = { { 0, 1 }, { -1, 1 }, { 250.15, 320.7 }, { 0, 1e-4 }, { -1e10, 1e10 }, { 1e-30, 3e-30 }, { -5e3, 7e7 }, { 101325, 101325 }, { 0, 0 }, { -273.15, -273.15 }, { 0.001, 1000 }, { 1e20, 1.0001e20 } };
    for (const auto& r : ranges) {
        for (int64_t bpv : { INT64_C(1), INT64_C(8), INT64_C(12), INT64_C(16), INT64_C(24), INT64_C(32) }) {
            checkOptimizer("DecOptimizer(" + std::to_string(bpv) + ")", std::make_unique<DecOptimizer>(bpv, 127), r[0], r[1]);
            checkOptimizer("BinDecOptimizer(" + std::to_string(bpv) + ")", std::make_unique<BinDecOptimizer>(0, 1, static_cast<int32_t>(bpv)), r[0], r[1]);
        }
        // BinOptimizer: decimal scale factor and bits per value given (the scaled range must fit
        // a binary scale factor within +-127 for these fields)
        for (int32_t d : { -2, 0, 3 }) {
            for (int64_t bpv : { INT64_C(1), INT64_C(12), INT64_C(24) }) {
                if (r[0] != r[1] && (r[1] - r[0]) * sp_power<double>(d, 10) < 1e-20) {
                    continue;  // too small for any binary scale factor
                }
                checkOptimizer("BinOptimizer(" + std::to_string(d) + ", " + std::to_string(bpv) + ")", std::make_unique<BinOptimizer>(d, bpv), r[0], r[1], false, 32767);
            }
        }
        // Decimal precision: the scaled range must be below 2^31 (like ecCodes, larger ones are rejected)
        for (int32_t d : { 0, 2 }) {
            if ((r[1] - r[0]) * sp_power<double>(d, 10) < 2147483647.0 && r[1] - r[0] >= 1e-3) {
                checkOptimizer("BPVOptimizer(" + std::to_string(d) + ")", std::make_unique<BPVOptimizer>(d), r[0], r[1], true);
            }
        }
    }
}

// The optimisers called directly with a constant field (SimplePacking handles constant fields
// before calling them), with IEEE and IBM reference values
static void constantFields()
{
    for (bool ibm : { false, true }) {
        const FloatType type = ibm ? FloatType::IBM32 : FloatType::IEEE32;
        std::vector<std::pair<std::string, std::unique_ptr<Optimizer>>> optimizers;
        optimizers.emplace_back("DecOptimizer", std::make_unique<DecOptimizer>(16, 127));
        optimizers.emplace_back("BinDecOptimizer", std::make_unique<BinDecOptimizer>(0, 0, 16));
        optimizers.emplace_back("BPVOptimizer", std::make_unique<BPVOptimizer>(2));
        optimizers.emplace_back("BinOptimizer", std::make_unique<BinOptimizer>(2, 16));
        for (auto& [name, o] : optimizers) {
            o->referenceFloatType(type);
            for (double v : { 0.0, -273.15, 101325.0, 1e-30 }) {
                const std::string what = name + (ibm ? " IBM" : " IEEE") + " constant field " + std::to_string(v);
                try {
                    const Parameters p = o->getParams(v, v);
                    // BPVOptimizer keeps its decimal scale factor: the reference value is scaled
                    const auto ref = nearest_smaller_value(v * sp_power<double>(p.decimalScaleFactor(), 10), type);
                    check(p.bitsPerValue() == 0 && (p.isConstantField() || name == "BPVOptimizer") && ref && p.referenceValue() == *ref,
                          what + ": bits per value " + std::to_string(p.bitsPerValue()) + ", reference value " + std::to_string(p.referenceValue()));
                }
                catch (const std::exception& e) {
                    check(false, what + ": " + e.what());
                }
            }
        }
    }
}

// SimplePackingBinary::packTo and unpackTo of a constant field: no bytes are written, all values
// are the reference value
static void constantFieldPackTo()
{
    SimplePackingBinary<double> sp(nullptr);
    const Parameters params(0, 0, 0, 273.25, true);
    const std::vector<double> values(10, 273.25);
    std::vector<uint8_t> out(16, 0xA5);
    sp.packTo(params, values.data(), values.size(), out.data());
    check(std::all_of(out.begin(), out.end(), [](uint8_t c) { return c == 0xA5; }), "packTo of a constant field must not write");
    std::vector<double> decoded(values.size(), -1);
    sp.unpackTo(params, out.data(), 0, decoded.size(), decoded.data());
    check(decoded == values, "unpackTo of a constant field");
}

// BinDecOptimizer in every mode (GRIBEX compatibility: binary scale factor in one byte, decimal
// scale factors with too small scaled ranges skipped; 32-bit floats: reference and maximum
// decodable as float) and with IEEE and IBM reference values, also for ranges near the limits
// of the modes: near FLT_MAX, beyond 10^300 (decimal scale factors that would overflow skipped),
// below DBL_EPSILON, a minimum below DBL_EPSILON (the fallback without decimal search)
static void binDecModes()
{
    const double ranges[][2] = { { 0, 1 }, { -1, 1 }, { 250.15, 320.7 }, { -5e3, 7e7 }, { 1e38, 3e38 }, { 3e38, 3.3e38 }, { 0, 1e300 }, { 0, 1e303 },
                                 { 0, 1e-300 }, { 1, 1 + 1e-15 }, { 1e-20, 1 }, { -1e-20, 1 }, { 0, 1e-4 } };
    for (int32_t gribex : { 0, 1 }) {
        for (int32_t c32 : { 0, 1 }) {
            for (bool ibm : { false, true }) {
                for (int32_t bpv : { 1, 16, 32 }) {
                    for (const auto& r : ranges) {
                        if (ibm && r[1] - r[0] > 1e75) {
                            continue;  // beyond the IBM range: no reference value
                        }
                        const std::string name = "BinDecOptimizer(gribex " + std::to_string(gribex) + ", 32-bit " + std::to_string(c32) + ", " + std::to_string(bpv) + (ibm ? ", IBM)" : ")");
                        checkOptimizer(name, std::make_unique<BinDecOptimizer>(gribex, c32, bpv), r[0], r[1], false, gribex ? 127 : 32767, ibm);
                    }
                }
            }
        }
    }
}

// SimplePackingArray (one array element per value) with every number of bits per value 1..32 and
// a constant field
static void arrayPacking()
{
    for (int64_t bpv = 1; bpv <= 32; ++bpv) {
        SimplePackingArray<double> sp(std::make_unique<DecOptimizer>(bpv, 127));
        const std::vector<double> values = field(-3.5, 1234.25);
        const std::string what = "SimplePackingArray " + std::to_string(bpv) + " bits";
        try {
            auto [params, coded] = sp.pack(values);
            check(params.bitsPerValue() == bpv, what + ": bits per value " + std::to_string(params.bitsPerValue()));
            check(coded.index() == (bpv <= 8 ? 0u : bpv <= 16 ? 1u : 2u), what + ": array of another width");
            const std::vector<double> decoded = sp.unpack(params, coded, values.size());
            const double step = sp_power<double>(params.binaryScaleFactor(), 2) / sp_power<double>(params.decimalScaleFactor(), 10);
            for (size_t i = 0; i < values.size(); ++i) {
                check(std::fabs(decoded[i] - values[i]) <= step * 1.000001, what + ": value " + std::to_string(i));
            }
        }
        catch (const std::exception& e) {
            check(false, what + ": " + e.what());
        }
    }
}

// BinOptimizer keeps the given decimal scale factor and bits per value; the reference value is the
// nearest float not above the scaled minimum and the binary scale factor that of
// get_binary_scale_factor for the scaled maximum
static void binOptimizer()
{
    const double ranges[][2] = { { 0, 1 }, { -1, 1 }, { 250.15, 320.7 }, { -5e3, 7e7 }, { 101325, 101400 } };
    for (const auto& r : ranges) {
        for (int32_t d : { -3, 0, 2, 5 }) {
            for (int64_t bpv : { INT64_C(1), INT64_C(8), INT64_C(16), INT64_C(32), INT64_C(63) }) {
                const std::string what = "BinOptimizer(" + std::to_string(d) + ", " + std::to_string(bpv) + ") [" + std::to_string(r[0]) + ", " + std::to_string(r[1]) + "]";
                try {
                    const Parameters p = BinOptimizer(d, bpv).getParams(r[0], r[1]);
                    const double decimal = sp_power<double>(d, 10);
                    const auto ref = nearest_smaller_value(r[0] * decimal, FloatType::IEEE32);
                    const auto scale = get_binary_scale_factor(r[1] * decimal, *ref, bpv);
                    check(p.decimalScaleFactor() == d && p.bitsPerValue() == bpv && p.referenceValue() == *ref && scale && p.binaryScaleFactor() == *scale && !p.isConstantField(),
                          what + ": parameters");
                }
                catch (const std::exception& e) {
                    check(false, what + ": " + e.what());
                }
            }
        }
    }
}

static void numberOfBits()
{
    // The bits of x: 0 for 0, k for 2^(k-1) <= x < 2^k
    for (uint64_t x = 0; x < (UINT64_C(1) << 16); ++x) {
        const int64_t expected = x == 0 ? 0 : static_cast<int64_t>(std::floor(std::log2(static_cast<double>(x)))) + 1;
        const auto bits = number_of_bits(x);
        check(bits && *bits == expected, "number_of_bits(" + std::to_string(x) + ")");
    }
    for (int32_t k = 1; k <= 31; ++k) {
        const uint64_t p = UINT64_C(1) << k;
        const auto below = number_of_bits(p - 1), at = number_of_bits(p);
        check(below && *below == k, "number_of_bits(2^" + std::to_string(k) + " - 1)");
        check(!at || *at == k + 1, "number_of_bits(2^" + std::to_string(k) + ")");
    }
}

int main()
{
    optimizers();
    constantFields();
    constantFieldPackTo();
    binDecModes();
    binOptimizer();
    arrayPacking();
    numberOfBits();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
