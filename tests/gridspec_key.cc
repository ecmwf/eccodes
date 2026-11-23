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


// gridSpec key, of any grid type: errors, and what setting it changes in a message


#include <dirent.h>

#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "gridspec.h"


using gridspec::encode_error_t;


CASE("set: errors")
{
    // the message is unchanged
    EXPECT(gridspec::check_all<encode_error_t>({
        { "GRIB2", R"({"grid":)", CODES_GEOCALCULUS_PROBLEM },                           // not JSON/YAML
        { "GRIB2", R"({"grid":"X99"})", CODES_GEOCALCULUS_PROBLEM },                     // unknown name
        { "GRIB2", R"({"type":"none"})", CODES_GEOCALCULUS_PROBLEM },                    // unknown type
        { "GRIB2", R"({"area":[0,0,10,10],"grid":[1,1]})", CODES_GEOCALCULUS_PROBLEM },  // south of north
    }));

    auto h = gridspec::sample("GRIB2");
    const auto spec = gridspec::get_gridspec(h.get());
    EXPECT(gridspec::set_gridspec(h.get(), R"({"grid":"X99"})") != CODES_SUCCESS);
    EXPECT_EQUAL(gridspec::get_gridspec(h.get()), spec);
}


CASE("samples")
{
    // every GRIB sample's gridSpec, set back on it (of its edition), gives it again; but of samples not representable
    const std::set<std::string> not_representable{
        "lambert_bf_grib2",              // gridType=lambert_bf (no eckit::geo grid)
        "polar_stereographic_pl_grib1",  // Dx=Dy=0 (a single point, of more values)
        "polar_stereographic_sfc_grib1",
        "polar_stereographic_sfc_grib2",
    };

    // the samples directory (as set by the tests, not of the samples compiled in)
    const auto* path = std::getenv("ECCODES_SAMPLES_PATH");
    std::unique_ptr<DIR, int (*)(DIR*)> dir(path != nullptr ? opendir(path) : nullptr, [](DIR* d) { return d != nullptr ? closedir(d) : 0; });
    if (!dir) {
        std::cout << "samples directory not available (ECCODES_SAMPLES_PATH), skipped" << std::endl;
        return;
    }

    std::vector<std::string> names;
    for (const auto* entry = readdir(dir.get()); entry != nullptr; entry = readdir(dir.get())) {
        if (const std::string name = entry->d_name; name.size() > 5 && name.compare(name.size() - 5, 5, ".tmpl") == 0 &&
                                                    (name.rfind("GRIB", 0) == 0 || name.find("_grib") != std::string::npos)) {
            names.emplace_back(name.substr(0, name.size() - 5));
        }
    }
    EXPECT(names.size() > 100);

    size_t failed = 0;
    for (const auto& name : names) {
        auto h = gridspec::sample(name);
        const auto spec = gridspec::get_gridspec(h.get());

        if (not_representable.count(name) != 0) {
            failed += spec.rfind("error", 0) == 0 ? 0 : 1;
            continue;
        }

        auto c = gridspec::clone(h.get());
        if (gridspec::set_gridspec(c.get(), spec) != CODES_SUCCESS || gridspec::get_gridspec(c.get()) != spec ||
            gridspec::get_long(c.get(), "edition") != gridspec::get_long(h.get(), "edition")) {
            std::cerr << "FAILED sample " << name << ": " << spec << std::endl;
            ++failed;
        }
    }
    EXPECT_EQUAL(failed, 0);
}


CASE("get: buffer length")
{
    // the length of the gridSpec, and its terminating null character
    auto h = gridspec::sample("GRIB2");
    const std::string spec = R"({"area":[60,0,0,30],"grid":[2,2]})";

    std::string buffer(spec.length(), '\0');
    size_t length = buffer.size();
    EXPECT(codes_get_string(h.get(), "gridSpec", buffer.data(), &length) == CODES_BUFFER_TOO_SMALL);

    buffer.resize(spec.length() + 1);
    length = buffer.size();
    EXPECT(codes_get_string(h.get(), "gridSpec", buffer.data(), &length) == CODES_SUCCESS);
    EXPECT_EQUAL(length, spec.length());
    EXPECT_EQUAL(std::string(buffer.data()), spec);
}


CASE("set: other keys are kept")
{
    for (const auto* name : { "GRIB1", "GRIB2" }) {
        auto h = gridspec::sample(name);
        const gridspec::keys_t keys{ { "edition", gridspec::get_long(h.get(), "edition") },
                                     { "paramId", gridspec::get_long(h.get(), "paramId") },
                                     { "centre", gridspec::get_long(h.get(), "centre") },
                                     { "dataDate", gridspec::get_long(h.get(), "dataDate") },
                                     { "packingType", "grid_simple" },
                                     { "bitsPerValue", gridspec::get_long(h.get(), "bitsPerValue") } };

        for (const auto* spec : { R"({"grid":[1,1]})", R"({"grid":"O32"})", R"({"grid":"F32"})" }) {
            EXPECT(gridspec::set_gridspec(h.get(), spec) == CODES_SUCCESS);
            EXPECT(gridspec::mismatches(h.get(), keys).empty());
        }
    }
}


CASE("set: values")
{
    // the values are kept (and their packing), so as many as the points (e.g. the same grid) are as before
    for (const auto& [name, other] : std::vector<std::pair<const char*, const char*>>{
             { "GRIB1", R"({"grid":[1,1],"order":"i+j+"})" },        // 360x181 points
             { "GRIB2", R"({"area":[30,0,-30,60],"grid":[4,2]})" },  // 16x31 points
         }) {
        auto h = gridspec::sample(name);

        size_t n = 0;
        EXPECT(codes_get_size(h.get(), "values", &n) == CODES_SUCCESS && n == static_cast<size_t>(gridspec::get_long(h.get(), "numberOfDataPoints")));

        std::vector<double> values(n);
        std::iota(values.begin(), values.end(), 0.);
        EXPECT(codes_set_double_array(h.get(), "values", values.data(), n) == CODES_SUCCESS);

        auto get_values = [&h]() {
            size_t size = 0;
            EXPECT(codes_get_size(h.get(), "values", &size) == CODES_SUCCESS);
            std::vector<double> v(size);
            EXPECT(codes_get_double_array(h.get(), "values", v.data(), &size) == CODES_SUCCESS);
            return v;
        };

        // as packed
        values = get_values();

        // the same grid, and another of as many points
        EXPECT(gridspec::set_gridspec(h.get(), gridspec::get_gridspec(h.get())) == CODES_SUCCESS);
        EXPECT(get_values() == values);

        EXPECT(gridspec::set_gridspec(h.get(), other) == CODES_SUCCESS);
        EXPECT(get_values() == values);

        // of another number of points, the values are set (as many as the points)
        EXPECT(gridspec::set_gridspec(h.get(), R"({"area":[10,0,0,10],"grid":[1,1]})") == CODES_SUCCESS);
        EXPECT_EQUAL(gridspec::get_long(h.get(), "numberOfDataPoints"), 121);

        const std::vector<double> ones(121, 1.);
        EXPECT(codes_set_double_array(h.get(), "values", ones.data(), ones.size()) == CODES_SUCCESS);
        EXPECT(get_values() == ones);
    }
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
