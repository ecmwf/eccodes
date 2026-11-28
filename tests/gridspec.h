/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


// Helpers of the gridSpec key tests (gridspec_*.cc). The tests are tables, each row a behaviour of the key:
//
// - encode_t: a gridSpec set on a sample, the GRIB keys it encodes, and the gridSpec got back;
// - decode_t: a gridSpec got from GRIB keys set on a sample (the decoder's view of these keys);
// - check_decoded: the gridSpec of every message of a test data file.
//
// Every row is checked and every mismatch reported (check_all), so one run shows a whole behaviour.
// The edition is never converted by a row: a gridSpec is set on a message of the edition encoding it.


#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "eccodes/eccodes.h"


namespace gridspec
{


using handle_type = std::unique_ptr<codes_handle, decltype(&codes_handle_delete)>;


// Directory holding the "gridspec/*.grib" test data, set from the first command-line argument
inline std::string DATA_DIR = ".";


// A GRIB key value, compared exactly (integers, strings) or to 1e-9 (doubles)
struct value_t
{
    enum class type_t
    {
        LONG,
        DOUBLE,
        STRING
    } type;
    long l = 0;
    double d = 0;
    std::string s;

    value_t(int v) : type(type_t::LONG), l(v) {}
    value_t(long v) : type(type_t::LONG), l(v) {}
    value_t(double v) : type(type_t::DOUBLE), d(v) {}
    value_t(const char* v) : type(type_t::STRING), s(v) {}
    value_t(const std::string& v) : type(type_t::STRING), s(v) {}
};


using keys_t = std::vector<std::pair<std::string, value_t>>;


// Keys of a reduced Gaussian grid: N, Nj (rows), the first and last grid points (in the edition's units) and the number of
// points, and more
struct reduced_t : keys_t
{
    reduced_t(long N, long Nj, long lat1, long lon1, long lat2, long lon2, long numberOfDataPoints, const keys_t& more = {}) :
        keys_t{ { "N", N },
                { "Nj", Nj },
                { "latitudeOfFirstGridPoint", lat1 },
                { "longitudeOfFirstGridPoint", lon1 },
                { "latitudeOfLastGridPoint", lat2 },
                { "longitudeOfLastGridPoint", lon2 },
                { "numberOfDataPoints", numberOfDataPoints } }
    {
        insert(end(), more.begin(), more.end());
    }
};


// Keys of a regular grid: Ni, Nj and the first and last grid points (in the edition's units: GRIB1 millidegrees,
// GRIB2 microdegrees), and more
struct regular_t : keys_t
{
    regular_t(long Ni, long Nj, long lat1, long lon1, long lat2, long lon2, const keys_t& more = {}) :
        keys_t{ { "Ni", Ni },
                { "Nj", Nj },
                { "latitudeOfFirstGridPoint", lat1 },
                { "longitudeOfFirstGridPoint", lon1 },
                { "latitudeOfLastGridPoint", lat2 },
                { "longitudeOfLastGridPoint", lon2 } }
    {
        insert(end(), more.begin(), more.end());
    }
};


inline handle_type sample(const std::string& name)
{
    handle_type h(codes_grib_handle_new_from_samples(nullptr, name.c_str()), &codes_handle_delete);
    ASSERT_MSG(h, "sample '" + name + "'");
    return h;
}


inline handle_type clone(const codes_handle* h)
{
    handle_type c(codes_handle_clone(h), &codes_handle_delete);
    ASSERT(c);
    return c;
}


inline bool has_data(const std::string& path)
{
    std::unique_ptr<FILE, int (*)(FILE*)> file(std::fopen((DATA_DIR + "/" + path).c_str(), "rb"), &std::fclose);
    if (!file) {
        std::cout << "test data '" << path << "' not available, skipped" << std::endl;
    }
    return file != nullptr;
}


// The messages of a test data file (path relative to DATA_DIR)
inline std::vector<handle_type> messages(const std::string& path)
{
    std::unique_ptr<FILE, int (*)(FILE*)> file(std::fopen((DATA_DIR + "/" + path).c_str(), "rb"), &std::fclose);
    ASSERT_MSG(file, "test data '" + path + "'");

    std::vector<handle_type> handles;
    for (int err = 0;;) {
        handle_type h(codes_grib_handle_new_from_file(nullptr, file.get(), &err), &codes_handle_delete);
        ASSERT(err == CODES_SUCCESS);
        if (!h) {
            return handles;
        }
        handles.emplace_back(std::move(h));
    }
}


inline int set_gridspec(codes_handle* h, const std::string& spec)
{
    size_t length = spec.length();
    return codes_set_string(h, "gridSpec", spec.c_str(), &length);
}


// The gridSpec, or the error message if it cannot be got
inline std::string get_gridspec(const codes_handle* h)
{
    std::string buffer(10240, '\0');
    size_t length = buffer.size();
    if (auto err = codes_get_string(h, "gridSpec", buffer.data(), &length); err != CODES_SUCCESS) {
        return std::string("error: ") + codes_get_error_message(err);
    }
    return buffer.substr(0, length);
}


inline std::string to_string(const codes_handle* h, const std::string& key, const value_t& expected)
{
    std::ostringstream os;
    os.precision(15);

    int err = CODES_SUCCESS;
    switch (expected.type) {
        case value_t::type_t::LONG: {
            long v = 0;
            err = codes_get_long(h, key.c_str(), &v);
            os << v;
            break;
        }
        case value_t::type_t::DOUBLE: {
            double v = 0;
            err = codes_get_double(h, key.c_str(), &v);
            os << v;
            break;
        }
        case value_t::type_t::STRING: {
            char buffer[1024];
            size_t length = sizeof(buffer);
            err = codes_get_string(h, key.c_str(), buffer, &length);
            os << buffer;
            break;
        }
    }

    return err == CODES_SUCCESS ? os.str() : std::string("error: ") + codes_get_error_message(err);
}


// The keys not as expected, as "key=value (expected)"
inline std::vector<std::string> mismatches(const codes_handle* h, const keys_t& keys)
{
    std::vector<std::string> result;
    for (const auto& [key, expected] : keys) {
        const auto got = to_string(h, key, expected);

        bool ok = false;
        std::ostringstream want;
        want.precision(15);
        switch (expected.type) {
            case value_t::type_t::LONG:
                ok = got == std::to_string(expected.l);
                want << expected.l;
                break;
            case value_t::type_t::DOUBLE:
                ok = got.rfind("error", 0) != 0 && eckit::types::is_approximately_equal(std::stod(got), expected.d, 1e-9);
                want << expected.d;
                break;
            case value_t::type_t::STRING:
                ok = got == expected.s;
                want << expected.s;
                break;
        }

        if (!ok) {
            result.emplace_back(key + "=" + got + " (expected " + want.str() + ")");
        }
    }
    return result;
}


inline long get_long(const codes_handle* h, const char* key)
{
    long value = 0;
    CODES_CHECK(codes_get_long(h, key, &value), key);
    return value;
}


// Collects a row's failures, reported at the end
struct report_t
{
    explicit report_t(std::string context) : context_(std::move(context)) {}

    void expect(bool ok, const std::string& what)
    {
        if (!ok) {
            failures_.emplace_back(what);
        }
    }

    void expect_equal(const std::string& got, const std::string& expected, const std::string& what)
    {
        expect(got == expected, what + ": '" + got + "' (expected '" + expected + "')");
    }

    void add(const std::vector<std::string>& failures, const std::string& what)
    {
        for (const auto& f : failures) {
            failures_.emplace_back(what + ": " + f);
        }
    }

    bool ok() const { return failures_.empty(); }

    ~report_t()
    {
        if (!failures_.empty()) {
            std::cerr << "FAILED " << context_ << std::endl;
            for (const auto& f : failures_) {
                std::cerr << "  " << f << std::endl;
            }
        }
    }

private:
    const std::string context_;
    std::vector<std::string> failures_;
};


// Message after setting a geometry: valid, and of values as many as the points (n) once set (the values, and their
// packing, are the message's until set; so is the GRIB1 numberOfDataPoints, of the values)
inline void check_message(report_t& report, const codes_handle* h, size_t n, const std::string& what)
{
    auto c = clone(h);

    const std::vector<double> values(n, 0.);
    report.expect(codes_set_double_array(c.get(), "values", values.data(), values.size()) == CODES_SUCCESS, what + ": set values");

    size_t length = 4;
    report.expect(codes_set_string(c.get(), "messageValidityChecks", "grid", &length) == CODES_SUCCESS, what + ": messageValidityChecks");

    long valid = 0;
    report.expect(codes_get_long(c.get(), "isMessageValid", &valid) == CODES_SUCCESS && valid == 1, what + ": isMessageValid");

    long numberOfDataPoints = 0;
    size_t size = 0;
    report.expect(codes_get_long(c.get(), "numberOfDataPoints", &numberOfDataPoints) == CODES_SUCCESS &&
                      codes_get_size(c.get(), "values", &size) == CODES_SUCCESS &&
                      static_cast<size_t>(numberOfDataPoints) == n && size == n,
                  what + ": numberOfDataPoints=" + std::to_string(numberOfDataPoints) + " values=" + std::to_string(size) +
                      " (expected " + std::to_string(n) + ")");
}


// gridSpec set on a sample: the GRIB keys encoded, and the gridSpec got back (empty: the gridSpec set)
struct encode_t
{
    std::string sample;
    std::string spec;
    std::string gridType;
    keys_t keys;
    std::string back;
};


inline bool check(const encode_t& test)
{
    const auto back = test.back.empty() ? test.spec : test.back;
    report_t report("encode " + test.spec + " on " + test.sample);

    auto h = sample(test.sample);
    const auto edition = get_long(h.get(), "edition");

    if (auto err = set_gridspec(h.get(), test.spec); err != CODES_SUCCESS) {
        report.expect(false, std::string("set: ") + codes_get_error_message(err));
        return false;
    }

    report.expect(get_long(h.get(), "edition") == edition, "edition converted");
    report.add(mismatches(h.get(), { { "gridType", test.gridType } }), "key");
    report.add(mismatches(h.get(), test.keys), "key");
    // the number of points (values) of the grid (of spherical harmonics, the real coefficients)
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(back));
    check_message(report, h.get(), grid->size(), "message");
    report.expect_equal(get_gridspec(h.get()), back, "gridSpec");

    // the gridSpec got back is a fixed point, set on this message or on a new one
    for (auto* target : { "this message", "new message" }) {
        auto h2 = target == std::string("this message") ? clone(h.get()) : sample(test.sample);
        if (auto err = set_gridspec(h2.get(), back); err != CODES_SUCCESS) {
            report.expect(false, std::string("set back on ") + target + ": " + codes_get_error_message(err));
            continue;
        }
        report.add(mismatches(h2.get(), test.keys), std::string("key set back on ") + target);
        report.expect_equal(get_gridspec(h2.get()), back, std::string("gridSpec set back on ") + target);
    }

    return report.ok();
}


// gridSpec set on a sample that cannot encode it (error code)
struct encode_error_t
{
    std::string sample;
    std::string spec;
    int error;
};


inline bool check(const encode_error_t& test)
{
    report_t report("encode (error) " + test.spec + " on " + test.sample);

    auto h = sample(test.sample);
    const auto err = set_gridspec(h.get(), test.spec);
    report.expect(err == test.error, std::string("set: '") + codes_get_error_message(err) + "' (expected '" +
                                         codes_get_error_message(test.error) + "')");
    return report.ok();
}


// GRIB keys set on a sample (in order), and the gridSpec got (a gridSpec, or "error: ..." with the error message)
struct decode_t
{
    std::string sample;
    keys_t keys;
    std::string spec;
};


inline bool check(const decode_t& test)
{
    std::ostringstream what;
    for (const auto& [key, value] : test.keys) {
        what << key << "=" << (value.type == value_t::type_t::LONG ? std::to_string(value.l) : value.type == value_t::type_t::DOUBLE ? std::to_string(value.d)
                                                                                                                                     : value.s)
             << " ";
    }
    report_t report("decode " + what.str() + "on " + test.sample);

    auto h = sample(test.sample);
    for (const auto& [key, value] : test.keys) {
        int err = CODES_SUCCESS;
        switch (value.type) {
            case value_t::type_t::LONG:
                err = codes_set_long(h.get(), key.c_str(), value.l);
                break;
            case value_t::type_t::DOUBLE:
                err = codes_set_double(h.get(), key.c_str(), value.d);
                break;
            case value_t::type_t::STRING: {
                size_t length = value.s.length();
                err = codes_set_string(h.get(), key.c_str(), value.s.c_str(), &length);
                break;
            }
        }
        report.expect(err == CODES_SUCCESS, "set " + key + ": " + codes_get_error_message(err));
    }

    report.expect_equal(get_gridspec(h.get()), test.spec, "gridSpec");
    return report.ok();
}


// The gridSpec of every message of a test data file ("error: ..." with the error message if it cannot be got, ending in
// "..." for the beginning of a long gridSpec); and each gridSpec set back on its message gives it again, but for the
// messages not_fixed
inline bool check_decoded(const std::string& path, const std::vector<std::string>& specs,
                          const std::set<size_t>& not_fixed = {})
{
    if (!has_data(path)) {
        return true;
    }

    const auto handles = messages(path);
    report_t report("decode '" + path + "'");
    report.expect(handles.size() == specs.size(), "messages: " + std::to_string(handles.size()) + " (expected " +
                                                      std::to_string(specs.size()) + ")");

    for (size_t i = 0; i < handles.size() && i < specs.size(); ++i) {
        const auto what = "#" + std::to_string(i);
        const auto spec = get_gridspec(handles[i].get());
        if (const auto& e = specs[i]; e.size() > 3 && e.compare(e.size() - 3, 3, "...") == 0) {
            report.expect_equal(spec.substr(0, e.size() - 3) + "...", e, what);
        }
        else {
            report.expect_equal(spec, e, what);
        }

        if (not_fixed.count(i) == 0 && spec.rfind("error", 0) != 0) {
            auto h = clone(handles[i].get());
            const auto err = set_gridspec(h.get(), spec);
            report.expect(err == CODES_SUCCESS, what + " set back: " + codes_get_error_message(err));
            if (err == CODES_SUCCESS) {
                report.expect_equal(get_gridspec(h.get()), spec, what + " set back");
            }
        }
    }

    return report.ok();
}


// All rows checked (each reporting its mismatches), true if all as expected
template <typename T>
bool check_all(const std::vector<T>& tests)
{
    size_t failed = 0;
    for (const auto& test : tests) {
        failed += check(test) ? 0 : 1;
    }
    if (failed > 0) {
        std::cerr << failed << " of " << tests.size() << " rows failed" << std::endl;
    }
    return failed == 0;
}


// Test driver: "<test> [<test-data-directory>]", enabled by ECCODES_ECKIT_GEO
inline int main(int argc, char* argv[])
{
    const auto* ev_name = "ECCODES_ECKIT_GEO";
    if (const auto* ev_val = std::getenv(ev_name); ev_val == nullptr || std::atol(ev_val) == 0) {
        std::printf("%s: This test is disabled (env. variable %s is not set)", argv[0], ev_name);
        return 0;
    }

    if (argc > 1) {
        DATA_DIR = argv[1];
        argc = 1;  // consume it, the rest is for eckit's test runner
    }

    return eckit::testing::run_tests(argc, argv);
}


}  // namespace gridspec
