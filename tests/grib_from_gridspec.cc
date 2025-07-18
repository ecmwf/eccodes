/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"

#include "eccodes/eccodes.h"
#include "eccodes/geo/GribFromSpec.h"
#include "eccodes/grib_api_internal.h"


bool get_string(const grib_handle* h, const char* key, std::string& value)
{
    char buffer[10240];
    size_t size = sizeof(buffer);
    ASSERT(h);
    int err = codes_get_string(h, key, buffer, &size);

    if (err == CODES_NOT_FOUND) {
        return false;
    }

    ASSERT(err == 0);

    value = buffer;
    return true;
}


CASE("grid: O2")
{
    for (auto* h : {
             static_cast<codes_handle*>(nullptr),
             codes_grib_handle_new_from_samples(nullptr, "GRIB1"),
             codes_grib_handle_new_from_samples(nullptr, "GRIB2"),
         }) {
        ::eckit::geo::spec::Custom spec{ { "grid", "o2" } };

        CODES_CHECK(eccodes::geo::GribFromSpec::set(h, spec), nullptr);
        EXPECT(h != nullptr);

        long valid = 0;
        CODES_CHECK(codes_get_long(h, "isMessageValid", &valid), nullptr);
        EXPECT(valid == 1);

        std::string type;
        get_string(h, "gridType", type);
        EXPECT(type == "reduced_gg");

        std::string name;
        get_string(h, "gridName", name);
        EXPECT(name == "O2");

        long N = 0;
        CODES_CHECK(codes_get_long(h, "N", &N), nullptr);
        EXPECT(N == 2);

        long numberOfDataPoints = 0;
        CODES_CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints), nullptr);
        EXPECT(numberOfDataPoints == 88);

        codes_handle_delete(h);
    }
}


CASE("grid: 1/1")
{
    for (auto* h : {
             static_cast<codes_handle*>(nullptr),
             codes_grib_handle_new_from_samples(nullptr, "GRIB1"),
             codes_grib_handle_new_from_samples(nullptr, "GRIB2"),
         }) {
        ::eckit::geo::spec::Custom spec{ { "grid", "1/1" } };

        CODES_CHECK(eccodes::geo::GribFromSpec::set(h, spec), nullptr);
        EXPECT(h != nullptr);

        long valid = 0;
        CODES_CHECK(codes_get_long(h, "isMessageValid", &valid), nullptr);
        EXPECT(valid == 1);

        std::string type;
        get_string(h, "gridType", type);
        EXPECT(type == "regular_ll");

        long Ni                 = 0;
        long Nj                 = 0;
        long numberOfDataPoints = 0;
        CODES_CHECK(codes_get_long(h, "Ni", &Ni), nullptr);
        CODES_CHECK(codes_get_long(h, "Nj", &Nj), nullptr);
        CODES_CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints), nullptr);

        EXPECT(Ni * Nj == numberOfDataPoints);

        codes_handle_delete(h);
    }
}


int main(int argc, char* argv[])
{
    return eckit::testing::run_tests(argc, argv);
}
