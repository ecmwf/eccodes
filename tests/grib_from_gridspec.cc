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


#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "eccodes/eccodes.h"
#include "eccodes/geo/GribFromSpec.h"
#include "eccodes/grib_api_internal.h"

#define CHECK(a) CODES_CHECK(a, nullptr)


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

        CHECK(eccodes::geo::GribFromSpec::set(h, spec));
        EXPECT(h != nullptr);

        long valid = 0;
        CHECK(codes_get_long(h, "isMessageValid", &valid));
        EXPECT(valid == 1);

        std::string type;
        get_string(h, "gridType", type);
        EXPECT(type == "reduced_gg");

        std::string name;
        get_string(h, "gridName", name);
        EXPECT(name == "O2");

        long N = 0;
        CHECK(codes_get_long(h, "N", &N));
        EXPECT(N == 2);

        long numberOfDataPoints = 0;
        CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints));
        EXPECT(numberOfDataPoints == 88);

        size_t pl_size = 0;
        CHECK(codes_get_size(h, "pl", &pl_size));

        std::vector<long> pl(pl_size, 0);
        auto size = pl_size;
        CHECK(codes_get_long_array(h, "pl", pl.data(), &size));
        ASSERT(pl_size == size);

        std::vector<long> pl_expected{ 20L, 24, 24, 20 };
        EXPECT(pl == pl_expected);

        std::vector<double> area(4);
        CHECK(codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &area[0]));
        CHECK(codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &area[1]));
        CHECK(codes_get_double(h, "latitudeOfLastGridPointInDegrees", &area[2]));
        CHECK(codes_get_double(h, "longitudeOfLastGridPointInDegrees", &area[3]));

        EXPECT(eckit::types::is_strictly_greater(90., area[0]));
        EXPECT(eckit::types::is_approximately_equal(area[1], 0.));
        EXPECT(eckit::types::is_strictly_greater(area[2], -90.));
        EXPECT(eckit::types::is_strictly_greater(360., area[3]));

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

        CHECK(eccodes::geo::GribFromSpec::set(h, spec));
        EXPECT(h != nullptr);

        long valid = 0;
        CHECK(codes_get_long(h, "isMessageValid", &valid));
        EXPECT(valid == 1);

        std::string type;
        get_string(h, "gridType", type);
        EXPECT(type == "regular_ll");

        long Ni                 = 0;
        long Nj                 = 0;
        long numberOfDataPoints = 0;
        CHECK(codes_get_long(h, "Ni", &Ni));
        CHECK(codes_get_long(h, "Nj", &Nj));
        CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints));

        EXPECT(Ni * Nj == numberOfDataPoints);

        std::vector<double> area(4);
        CHECK(codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &area[0]));
        CHECK(codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &area[1]));
        CHECK(codes_get_double(h, "latitudeOfLastGridPointInDegrees", &area[2]));
        CHECK(codes_get_double(h, "longitudeOfLastGridPointInDegrees", &area[3]));

        EXPECT(eckit::types::is_approximately_equal(area[0], 90.));
        EXPECT(eckit::types::is_approximately_equal(area[1], 0.));
        EXPECT(eckit::types::is_approximately_equal(area[2], -90.));
        // EXPECT(eckit::types::is_approximately_equal(area[0], 360. - 1.));

        codes_handle_delete(h);
    }
}


int main(int argc, char* argv[])
{
    return eckit::testing::run_tests(argc, argv);
}
