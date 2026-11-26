// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test of the interface of simple_packing (simple_packing/simple_packing.h):
// - every Method gives a scaling with the given parameters, and pack/unpack restores the values
//   within one quantisation step, for double and float, with vectors and with buffers
// - the results are those of the implementation classes (optimisers and SimplePackingBinary)
// - constant fields: no bits, no packed data
// - range: minimum and maximum, NaN for fields with NaN or infinite values
// - errors: values that cannot be packed, buffers that are too small, packed data that are too short

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "simple_packing/simple_packing.h"

#include "../fp_diagnostics.h"

static int failures = 0;

static void check(bool ok, const std::string& what)
{
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        ++failures;
    }
}

template <typename E, typename F>
static void expect(const std::string& what, F&& f)
{
    try {
        f();
        check(false, what + ": no exception");
    }
    catch (const E&) {
    }
    catch (const std::exception& e) {
        check(false, what + ": other exception: " + e.what());
    }
}

// pack accepts the scaling for these values
static bool packs(const std::vector<double>& values, const sp::Scaling& s)
{
    try {
        sp::pack(values, s);
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

template <typename Value>
static std::vector<Value> field(size_t count, double base, double spread, unsigned seed)
{
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> u(0, 1);
    std::vector<Value> values(count);
    for (auto& v : values)
        v = static_cast<Value>(base + spread * u(rng));
    return values;
}

// The largest error of unpack(pack(values)), and the quantisation step of the scaling
template <typename Value>
static void roundTrip(const std::vector<Value>& values, const sp::Scaling& s, const std::string& what)
{
    const std::vector<uint8_t> packed = sp::pack(values, s);
    check(packed.size() == sp::packedSize(values.size(), s), what + ": size of the packed data");
    const std::vector<Value> back = sp::unpack<Value>(packed, values.size(), s);
    const double step = std::ldexp(1.0, s.binaryScaleFactor) * std::pow(10.0, -s.decimalScaleFactor);
    double worst = 0;
    for (size_t i = 0; i < values.size(); ++i)
        worst = std::fmax(worst, std::fabs(static_cast<double>(back[i]) - static_cast<double>(values[i])));
    const double tolerance = step * 1.0001 + (sizeof(Value) == 4 ? 1e-6 * std::fabs(static_cast<double>(values[0])) : 0);
    check(worst <= tolerance, what + ": error " + std::to_string(worst) + " above one step " + std::to_string(step));

    // With buffers
    std::vector<uint8_t> buffer(packed.size() + 1, 0xAB);
    sp::pack(values.data(), values.size(), s, buffer.data(), packed.size());
    check(std::equal(packed.begin(), packed.end(), buffer.begin()) && buffer[packed.size()] == 0xAB, what + ": pack into a buffer");
    std::vector<Value> out(values.size());
    sp::unpack(packed.data(), packed.size(), s, out.data(), out.size());
    check(out == back, what + ": unpack into a buffer");
}

int main()
{
    const std::vector<double> values = field<double>(5000, 250.15, 70.0, 1);
    const std::vector<float> floats(values.begin(), values.end());

    // The methods
    {
        const sp::Scaling s = sp::scaling(values, sp::Method::bits(16));
        check(s.bitsPerValue == 16, "bits(16): bits per value");
        roundTrip(values, s, "bits(16) double");
        roundTrip(floats, sp::scaling(floats, sp::Method::bits(16)), "bits(16) float");

        // the same scaling and data as the implementation classes (and so as ecCodes)
        sp::detail::SimplePackingBinary<double> reference(std::make_unique<sp::detail::DecOptimizer>(16, 127));
        const auto [params, packed] = reference.pack(values);
        check(params.referenceValue() == s.referenceValue && params.binaryScaleFactor() == s.binaryScaleFactor &&
                  params.decimalScaleFactor() == s.decimalScaleFactor && params.bitsPerValue() == s.bitsPerValue,
              "bits(16): the scaling of DecOptimizer");
        check(packed == sp::pack(values, s), "bits(16): the packed data of SimplePackingBinary");
    }
    for (int bits : { 1, 8, 12, 24, 32, 48 }) {
        const sp::Scaling s = sp::scaling(values, sp::Method::bits(bits));
        check(s.bitsPerValue == bits, "bits(" + std::to_string(bits) + "): bits per value");
        roundTrip(values, s, "bits(" + std::to_string(bits) + ")");
        const sp::Scaling best = sp::scaling(values, sp::Method::bestPrecision(bits));
        check(best.bitsPerValue == bits, "bestPrecision(" + std::to_string(bits) + "): bits per value");
        roundTrip(values, best, "bestPrecision(" + std::to_string(bits) + ")");
    }
    {
        const sp::Scaling s = sp::scaling(values, sp::Method::decimals(2));
        check(s.decimalScaleFactor == 2 && s.binaryScaleFactor == 0 && s.bitsPerValue == 13, "decimals(2): 13 bits for a range of 70.00");
        roundTrip(values, s, "decimals(2)");
        const sp::Scaling both = sp::scaling(values, sp::Method::bitsAndDecimals(12, 1));
        check(both.decimalScaleFactor == 1 && both.bitsPerValue == 12, "bitsAndDecimals(12, 1)");
        roundTrip(values, both, "bitsAndDecimals(12, 1)");
    }

    // The reference value is a 32-bit float of the chosen format, not above the minimum
    {
        const sp::Range<double> r = sp::range(values);
        const sp::Scaling ieee = sp::scaling(values, sp::Method::bits(16));
        check(ieee.referenceValue == static_cast<double>(static_cast<float>(ieee.referenceValue)) && ieee.referenceValue <= r.min, "IEEE reference value");
        const sp::Scaling ibm = sp::scaling(values, sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32).gribex());
        check(ibm.referenceValue <= r.min * std::pow(10.0, ibm.decimalScaleFactor) && ibm.binaryScaleFactor >= -126, "IBM reference value, GRIBEX limits");
        roundTrip(values, ibm, "IBM reference, GRIBEX");
        const sp::Scaling fromRange = sp::scaling(r.min, r.max, sp::Method::bits(16));
        check(fromRange.referenceValue == ieee.referenceValue && fromRange.binaryScaleFactor == ieee.binaryScaleFactor, "scaling from the range");
    }

    // Constant field
    {
        const std::vector<double> constant(100, 273.15);
        const sp::Scaling s = sp::scaling(constant, sp::Method::bits(16));
        check(s.bitsPerValue == 0 && sp::packedSize(constant.size(), s) == 0 && sp::pack(constant, s).empty(), "constant field: no bits, no data");
        const std::vector<double> back = sp::unpack<double>({}, constant.size(), s);
        check(back.size() == 100 && back[0] == s.referenceValue && std::fabs(back[99] - 273.15) < 1e-4, "constant field: the reference value");
    }

    // range
    {
        const sp::Range<double> r = sp::range(values);
        double mn = values[0], mx = values[0];
        for (double v : values) {
            mn = std::fmin(mn, v);
            mx = std::fmax(mx, v);
        }
        check(r.min == mn && r.max == mx, "range: minimum and maximum");
        for (double bad : { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity() }) {
            std::vector<double> w = values;
            w[1234] = bad;
            const sp::Range<double> rb = sp::range(w);
            check(std::isnan(rb.min) && std::isnan(rb.max), "range with a NaN or infinite value");
            expect<std::runtime_error>("scaling of a field with a NaN or infinite value", [&] { sp::scaling(w, sp::Method::bits(16)); });
        }
    }

    // Every scaling that sp::scaling returns can be packed (attack 3: decimals chose its bits from
    // max - min, but the reference value is rounded down to a float afterwards; bestPrecision
    // computed 2^bits - 1 in double, which is 2^bits from 54 bits on)
    {
        const std::vector<double> e9{ 1e9 + 1, 1e9 + 2 };
        const sp::Scaling d0 = sp::scaling(e9, sp::Method::decimals(0));
        check(packs(e9, d0) && d0.bitsPerValue == 2, "decimals(0) of {1e9+1, 1e9+2}: 2 bits (the reference value is 1e9)");
        const std::vector<double> e8{ 1e8 + 1, 1e8 + 2 };
        check(packs(e8, sp::scaling(e8, sp::Method::decimals(0))), "decimals(0) of {1e8+1, 1e8+2}");
        const std::vector<double> e7{ 1e7 + 1, 1e7 + 2 };
        const sp::Scaling d7 = sp::scaling(e7, sp::Method::decimals(0));
        check(packs(e7, d7) && d7.bitsPerValue == 1, "decimals(0) of {1e7+1, 1e7+2}: still 1 bit (exact reference value)");
        std::vector<double> quarter(5);
        for (size_t i = 0; i < quarter.size(); ++i)
            quarter[i] = 0.25 + std::ldexp(static_cast<double>(i), -53);
        for (int bits : { 54, 55, 60, 63 }) {
            check(packs(quarter, sp::scaling(quarter, sp::Method::bestPrecision(bits))), "bestPrecision(" + std::to_string(bits) + ") of 0.25 + i * 2^-53");
        }
        long unusable = 0, cases = 0;
        std::vector<double> v(16);
        for (int expo = -60; expo <= 60; expo += 3) {
            for (int spread = -60; spread <= 20; spread += 7) {
                for (size_t i = 0; i < v.size(); ++i)
                    v[i] = std::ldexp(1.0, expo) + std::ldexp(static_cast<double>(i), spread);
                for (int bits = 1; bits <= 63; bits += 2) {
                    for (const sp::Method& m : { sp::Method::bits(bits), sp::Method::bestPrecision(bits), sp::Method::bitsAndDecimals(bits, bits % 9 - 4),
                                                 sp::Method::decimals(bits % 9 - 4) }) {
                        sp::Scaling s;
                        try {
                            s = sp::scaling(v, m);
                        }
                        catch (const std::exception&) {
                            continue;  // refusing a field is allowed
                        }
                        ++cases;
                        unusable += !packs(v, s);
                    }
                }
            }
        }
        check(unusable == 0, std::to_string(unusable) + " of " + std::to_string(cases) + " scalings cannot be packed");
    }

    // The Method setters return a Method, not a reference into a temporary
    {
        check(!std::is_reference<decltype(sp::Method::bits(16).gribex())>::value, "Method::gribex returns a Method");
        check(!std::is_reference<decltype(sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32))>::value,
              "Method::referenceFormat returns a Method");
        const sp::Method chained = sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32).gribex();
        const sp::Scaling viaChain = sp::scaling(values, chained);
        const sp::Scaling direct = sp::scaling(values, sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32).gribex());
        check(viaChain.referenceValue == direct.referenceValue && viaChain.binaryScaleFactor == direct.binaryScaleFactor, "a stored chained Method");
    }

    // A Scaling that is not filled in is an error, not a constant field that discards the values
    {
        const sp::Scaling unset;
        expect<std::invalid_argument>("pack with a default Scaling", [&] { sp::pack(values, unset); });
        expect<std::invalid_argument>("unpack with a default Scaling", [&] { sp::unpack<double>(std::vector<uint8_t>(10), 10, unset); });
        expect<std::invalid_argument>("packedSize of a default Scaling", [&] { sp::packedSize(10, unset); });
    }

    // Errors
    {
        const sp::Scaling s = sp::scaling(values, sp::Method::bits(16));
        const std::vector<uint8_t> packed = sp::pack(values, s);
        std::vector<uint8_t> small(packed.size(), 0xCD);
        expect<std::length_error>("output buffer too small", [&] { sp::pack(values.data(), values.size(), s, small.data(), packed.size() - 1); });
        check(small[packed.size() - 1] == 0xCD && small[0] == 0xCD, "a too small buffer was written");
        std::vector<double> out(values.size());
        expect<std::invalid_argument>("packed data too short", [&] { sp::unpack(packed.data(), packed.size() - 1, s, out.data(), out.size()); });
        expect<std::invalid_argument>("bits(0)", [&] { sp::scaling(values, sp::Method::bits(0)); });
        expect<std::invalid_argument>("bits(64)", [&] { sp::scaling(values, sp::Method::bits(64)); });
        sp::Scaling invalid = s;
        invalid.bitsPerValue = 65;
        expect<std::invalid_argument>("65 bits per value", [&] { sp::packedSize(10, invalid); });
        expect<std::runtime_error>("minimum above maximum", [&] { sp::scaling(2.0, 1.0, sp::Method::bits(16)); });
    }

    if (failures) {
        fp_diagnostics::explain();
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "interface ok\n";
    return 0;
}
