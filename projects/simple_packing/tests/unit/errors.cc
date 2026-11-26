// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: every error simple_packing can report is triggered once, with the expected exception
// type and message. A case that does not return within 10 s fails the test (an infinite loop
// instead of an error). Inputs found by a random search over the optimiser parameters and fields.
// Errors that cannot be triggered: none (every throw statement is reached here).
#include <atomic>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "simple_packing/detail/Utils.h"
#include "simple_packing/detail/optimizer/BPVOptimizer.h"
#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/optimizer/BinOptimizer.h"
#include "simple_packing/detail/optimizer/DecOptimizer.h"
#include "simple_packing/detail/simple/Array.h"
#include "simple_packing/detail/simple/Binary.h"

#include "../fp_diagnostics.h"

using namespace sp::detail;

static int failures = 0;
static int cases = 0;
static std::atomic<long> finished{ 0 };
static std::string current;

// Ends the test if a case does not return
static void startWatchdog()
{
    std::thread([] {
        long last = -1;
        for (;;) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            const long now = finished.load();
            if (now == last) {
                std::fprintf(stderr, "FAIL: %s: no result after 10 s (infinite loop)\n", current.c_str());
                fp_diagnostics::explain();
                (void)std::fflush(stderr);
                std::_Exit(1);
            }
            last = now;
        }
    }).detach();
}

// f() must throw an E whose message contains `message`
template <typename E, typename F>
static void expect(const std::string& what, const std::string& message, F&& f)
{
    ++cases;
    current = what;
    try {
        f();
        std::cerr << "FAIL: " << what << ": no exception\n";
        ++failures;
    }
    catch (const E& e) {
        if (std::string(e.what()).find(message) == std::string::npos) {
            std::cerr << "FAIL: " << what << ": message \"" << e.what() << "\" does not contain \"" << message << "\"\n";
            ++failures;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << what << ": unexpected exception type: " << e.what() << '\n';
        ++failures;
    }
    ++finished;
}

using IA = std::invalid_argument;
using RE = std::runtime_error;
using RangeE = std::range_error;

static void utils()
{
    expect<IA>("get_binary_scale_factor 0 bits", "between 1 and 63", [] { get_binary_scale_factor(1, 0, 0); });
    expect<IA>("get_binary_scale_factor 64 bits", "between 1 and 63", [] { get_binary_scale_factor(1, 0, 64); });
    expect<RangeE>("get_binary_scale_factor infinite range", "not finite", [] { get_binary_scale_factor(DBL_MAX, -DBL_MAX, 16); });
    expect<RangeE>("get_binary_scale_factor maximum < minimum", "maximum is smaller", [] { get_binary_scale_factor(0, 1, 16); });
    expect<RangeE>("get_binary_scale_factor scale > 127", "too large (binary scale factor 996", [] { get_binary_scale_factor(1e300, 0, 1); });
    expect<RE>("check_data_values_range NaN", "Values out of range", [] { check_data_values_range(NAN, 0); });
    expect<RE>("check_data_values_range infinity", "Values out of range", [] { check_data_values_range(0, INFINITY); });
    expect<RE>("check_data_values_range DBL_MAX", "Values out of range", [] { check_data_values_range(0, DBL_MAX); });
    expect<RE>("check_data_values_range minimum > maximum", "Values out of range", [] { check_data_values_range(2, 1); });
}

static void constructors()
{
    expect<IA>("DecOptimizer 0 bits", "bits per value 0 is out of range", [] { DecOptimizer(0, 127); });
    expect<IA>("DecOptimizer 64 bits", "bits per value 64 is out of range", [] { DecOptimizer(64, 127); });
    expect<IA>("DecOptimizer exponent limit 0", "exponent limit 0", [] { DecOptimizer(16, 0); });
    expect<IA>("DecOptimizer exponent limit 1024", "exponent limit 1024", [] { DecOptimizer(16, 1024); });
    expect<IA>("BinDecOptimizer 0 bits", "bits per value 0 is out of range", [] { BinDecOptimizer(0, 0, 0); });
    expect<IA>("BinDecOptimizer 64 bits", "bits per value 64 is out of range", [] { BinDecOptimizer(0, 0, 64); });
    expect<IA>("BPVOptimizer decimal scale factor 32768", "decimal scale factor 32768", [] { BPVOptimizer(32768); });
    expect<IA>("BPVOptimizer decimal scale factor -32768", "decimal scale factor -32768", [] { BPVOptimizer(-32768); });
}

template <typename O>
static std::unique_ptr<O> ibm(std::unique_ptr<O> o)
{
    o->referenceFloatType(FloatType::IBM32);
    return o;
}

static void decOptimizer()
{
    expect<RE>("DecOptimizer NaN", "Values out of range", [] { DecOptimizer(16, 127).getParams(NAN, NAN); });
    expect<RE>("DecOptimizer constant field without reference value", "unable to find nearest_smaller_value", [] { DecOptimizer(16, 127).getParams(1e300, 1e300); });
    // Scaling up a denormal range gives inf - inf
    expect<RE>("DecOptimizer range scaled to NaN", "cannot scale the range",
               [] { ibm(std::make_unique<DecOptimizer>(41, 25))->getParams(-0x0.000039d0aaa7ep-1022, -0x0.000039d0aaa7dp-1022); });
    // ... or, with values of both signs, an infinite range
    expect<RE>("DecOptimizer range scaled to infinity", "cannot scale the range", [] { DecOptimizer(12, 13).getParams(-1e-320, 1e-320); });
    expect<RE>("DecOptimizer reference value out of range", "unable to find nearest_smaller_value of",
               [] { DecOptimizer(52, 52).getParams(-0x1.0c814d72799a2p+586, -0x1.0c814d72799a1p+586); });
    expect<RE>("DecOptimizer range too small", "unable to compute binary_scale_factor", [] { DecOptimizer(16, 1023).getParams(0, 1e-100); });
    expect<RangeE>("DecOptimizer binary scale factor > 127", "too large (binary scale factor",
                   [] { ibm(std::make_unique<DecOptimizer>(63, 110))->getParams(-0x1.00de3b8a19c9ep+551, -0x1.00de3b8a19c9dp+551); });
}

static void bpvOptimizer()
{
    expect<RE>("BPVOptimizer NaN", "Values out of range", [] { BPVOptimizer(2).getParams(NAN, NAN); });
    expect<RE>("BPVOptimizer range scaled to infinity", "cannot scale the range",
               [] { BPVOptimizer(15).getParams(-0x1.074f8c4d3cd7bp+128, 0x1.7e43c8800759cp+996); });
    expect<RE>("BPVOptimizer range beyond 2^63", "Range of values too large", [] { BPVOptimizer(0).getParams(0, 0x1p64); });
    expect<RE>("BPVOptimizer range of 2^63", "Range of values too large", [] { BPVOptimizer(0).getParams(0, 0x1p63); });
    expect<RE>("BPVOptimizer reference value out of range", "unable to find nearest_smaller_value",
               [] { BPVOptimizer(-1).getParams(0x1.660a2014727dcp+842, 0x1.660a2014727dcp+842); });
}

static void binDecOptimizer()
{
    expect<RE>("BinDecOptimizer NaN", "Values out of range", [] { BinDecOptimizer(0, 0, 16).getParams(NAN, NAN); });
    expect<RE>("BinDecOptimizer constant field without reference value", "unable to find nearest_smaller_value",
               [] { BinDecOptimizer(0, 0, 27).getParams(0x1.78fffbce4217dp+223, 0x1.78fffbce4217dp+223); });
    expect<RE>("BinDecOptimizer reference value out of range (optimal decimal scale factor)", "unable to find nearest_smaller_value",
               [] { BinDecOptimizer(0, 0, 26).getParams(-0x1.fffffep+127, -0x1.fffffdfffffffp+127); });
    expect<RE>("BinDecOptimizer reference value out of range (fallback)", "unable to find nearest_smaller_value",
               [] { BinDecOptimizer(0, 0, 4).getParams(-0x1.074f8c4d3cd7bp+128, -0x1.074f8c4d3cd7ap+128); });
    expect<RangeE>("BinDecOptimizer binary scale factor > 127", "too large (binary scale factor",
                   [] { ibm(std::make_unique<BinDecOptimizer>(0, 0, 36))->getParams(-0x1.191a005c4651fp+240, -0x1.191a005c4651ep+240); });
    // GRIBEX mode: the fallback scales a denormal range of both signs to infinity
    expect<RE>("BinDecOptimizer GRIBEX range scaled to infinity", "cannot scale the range", [] { BinDecOptimizer(1, 0, 61).getParams(-1e-320, 1e-320); });
}

static void binOptimizer()
{
    expect<IA>("BinOptimizer decimal scale factor 32768", "decimal scale factor 32768", [] { BinOptimizer(32768, 16); });
    expect<IA>("BinOptimizer 0 bits", "bits per value 0 is out of range", [] { BinOptimizer(0, 0); });
    expect<IA>("BinOptimizer 64 bits", "bits per value 64 is out of range", [] { BinOptimizer(0, 64); });
    expect<RE>("BinOptimizer NaN", "Values out of range", [] { BinOptimizer(0, 16).getParams(NAN, NAN); });
    expect<RE>("BinOptimizer constant field without reference value", "unable to find nearest_smaller_value", [] { BinOptimizer(0, 16).getParams(1e300, 1e300); });
    expect<RE>("BinOptimizer range scaled to infinity", "cannot scale the range", [] { BinOptimizer(10, 16).getParams(0, 1e300); });
    expect<RE>("BinOptimizer reference value out of range", "unable to find nearest_smaller_value of", [] { BinOptimizer(0, 16).getParams(-1e300, 0); });
    expect<RE>("BinOptimizer range too small", "unable to compute binary_scale_factor", [] { BinOptimizer(0, 16).getParams(0, 1e-100); });
    expect<RangeE>("BinOptimizer binary scale factor > 127", "too large (binary scale factor", [] { BinOptimizer(0, 1).getParams(0, 3e38); });
}

static void simplePacking()
{
    SimplePackingBinary<double> decodeOnly(nullptr);
    expect<std::logic_error>("SimplePacking without optimiser", "no optimiser", [&] { decodeOnly.getParameters(0.0, 1.0); });
    expect<RE>("SimplePacking constant field without reference value", "unable to find nearest_smaller_value", [&] { decodeOnly.getParameters(1e300, 1e300); });
    expect<RE>("SimplePacking NaN", "Values out of range", [&] { decodeOnly.getParameters(NAN, NAN); });
    std::vector<double> nan{ 1, NAN, 2 };
    expect<RE>("SimplePacking field with NaN", "Values out of range", [&] { SimplePackingBinary<double>(std::make_unique<DecOptimizer>(16, 127)).pack(nan); });
}

static Parameters params(long bpv, long bsf = 0, long dsf = 0, double ref = 0)
{
    Parameters p;
    p.bitsPerValue(bpv);
    p.binaryScaleFactor(bsf);
    p.decimalScaleFactor(dsf);
    p.referenceValue(ref);
    return p;
}

static void binary()
{
    using SP = SimplePackingBinary<double>;
    SP sp(nullptr);
    std::vector<double> values{ 1, 300, 2 };
    std::vector<unsigned char> out(64);
    expect<IA>("packTo 65 bits", "unsupported number of bits per value 65", [&] { sp.packTo(params(65), values.data(), 3, out.data()); });
    expect<IA>("packTo -1 bits", "unsupported number of bits per value -1", [&] { sp.packTo(params(-1), values.data(), 3, out.data()); });
    expect<IA>("packTo binary scale factor 32768", "binary scale factor 32768", [&] { sp.packTo(params(16, 32768), values.data(), 3, out.data()); });
    expect<IA>("packTo decimal scale factor -32768", "decimal scale factor -32768", [&] { sp.packTo(params(16, 0, -32768), values.data(), 3, out.data()); });
    expect<RE>("packTo value out of range", "value 300 at index 1 cannot be packed with 8 bits", [&] { sp.packTo(params(8), values.data(), 3, out.data()); });
    expect<RE>("packTo value below the reference value", "value 1 at index 0 cannot be packed", [&] { sp.packTo(params(16, 0, 0, 10), values.data(), 3, out.data()); });
    std::vector<double> nan{ 1, 2, NAN };
    expect<RE>("packTo NaN", "value nan at index 2", [&] { sp.packTo(params(16), nan.data(), 3, out.data()); });
    std::vector<double> decoded(100);
    expect<IA>("unpackTo short data", "too short (19 bytes for 10 values with 16 bits", [&] { sp.unpackTo(params(16), out.data(), 19, 10, decoded.data()); });
    expect<IA>("unpackTo overflowing size", "too short", [&] { sp.unpackTo(params(16), out.data(), 64, std::numeric_limits<size_t>::max() / 8, decoded.data()); });
    expect<IA>("unpackTo 65 bits", "unsupported number of bits per value 65", [&] { sp.unpackTo(params(65), out.data(), 64, 3, decoded.data()); });
    expect<IA>("unpack binary scale factor -32768", "binary scale factor -32768", [&] { sp.unpack(params(8, -32768), std::vector<unsigned char>(8), 3); });
}

static void array()
{
    using SPA = SimplePackingArray<double>;
    std::vector<double> values{ 0, 0.5, 1 };
    expect<RE>("SimplePackingArray::pack 40 bits", "Unsupported bitsPerValue for Array output format: 40",
               [&] { SPA(std::make_unique<DecOptimizer>(40, 127)).pack(values); });
    SPA spa(nullptr);
    expect<RE>("SimplePackingArray::unpack 40 bits", "Unsupported bitsPerValue for Array output format: 40",
               [&] { spa.unpack(params(40), std::vector<uint32_t>(3), 3); });
    expect<std::bad_variant_access>("SimplePackingArray::unpack of data of another width", "",
                                    [&] { spa.unpack(params(16), std::vector<uint8_t>(3), 3); });
}

// Fields that must not raise an error
template <typename F>
static void noError(const std::string& what, F&& f)
{
    ++cases;
    current = what;
    try {
        if (!f()) {
            std::cerr << "FAIL: " << what << ": wrong result\n";
            ++failures;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "FAIL: " << what << ": unexpected exception: " << e.what() << '\n';
        ++failures;
    }
    ++finished;
}

static void noErrors()
{
    using SPA = SimplePackingArray<double>;
    noError("SimplePackingArray with 1 bit per value", [] {
        SPA spa(std::make_unique<DecOptimizer>(1, 127));
        const std::vector<double> values{ 0, 1, 1, 0 };
        auto [p, coded] = spa.pack(values);
        return p.bitsPerValue() == 1 && spa.unpack(p, coded, values.size()) == values;
    });
    noError("SimplePackingArray with a constant field", [] {
        SPA spa(std::make_unique<DecOptimizer>(16, 127));
        const std::vector<double> values{ 5, 5, 5 };
        auto [p, coded] = spa.pack(values);
        return spa.unpack(p, coded, values.size()) == values;
    });
}

int main()
{
    startWatchdog();
    utils();
    constructors();
    decOptimizer();
    bpvOptimizer();
    binDecOptimizer();
    binOptimizer();
    simplePacking();
    binary();
    array();
    noErrors();
    if (failures != 0) {
        fp_diagnostics::explain();
        std::cerr << failures << " of " << cases << " error cases failed\n";
        return 1;
    }
    std::cout << "OK: " << cases << " error cases\n";
    return 0;
}
