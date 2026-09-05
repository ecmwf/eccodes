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


#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "eccodes/eccodes.h"
#include "eccodes/geo/GribFromSpec.h"
#include "eccodes/geo/GribToSpec.h"


#define CHECK(a) CODES_CHECK(a, nullptr)


using map_count_spec_t = std::map<size_t, std::string>;


struct grib_file_t
{
    std::unique_ptr<FILE, decltype(&std::fclose)> file;
    std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> handle;

    grib_file_t(const std::string& path) : file(std::fopen(path.c_str(), "rb"), &std::fclose), handle(nullptr, &codes_handle_delete)
    {
        ASSERT(file);
    }

    decltype(handle)& next()
    {
        int err = 0;
        handle.reset(codes_grib_handle_new_from_file(nullptr, file.get(), &err));
        ASSERT(err == CODES_SUCCESS || not handle);
        return handle;
    }
};


bool get_string(const grib_handle* h, const char* key, std::string& value)
{
    ASSERT(h);

    char buffer[10240];
    size_t length = sizeof(buffer);
    int err       = codes_get_string(h, key, buffer, &length);

    if (err == CODES_NOT_FOUND) {
        return false;
    }

    ASSERT(err == 0);

    value = buffer;
    return true;
}


void set_string(grib_handle* h, const char* key, const std::string& value)
{
    size_t length = value.length();
    CHECK(codes_set_string(h, key, value.c_str(), &length));
}


bool grib_to_gridspec(const std::string& path, const map_count_spec_t& specs)
{
    int count = 0;
    for (grib_file_t file(path); file.next(); ++count) {
        if (auto it = specs.find(count);
            it != specs.end() && !it->second.empty()) {
            // long numberOfDataPoints = 0;
            // CHECK(codes_get_long(file.handle.get(), "numberOfDataPoints", &numberOfDataPoints));

            char buffer[10240];
            size_t length = sizeof(buffer);

            EXPECT(CODES_SUCCESS == codes_get_string(file.handle.get(), "gridSpec", buffer, &length));

            std::string gridSpec = buffer;

            // std::cout << "count=" << count << " '" << gridSpec << "'\t(calculated) == \n"
            //           << "count=" << count << " '" << it->second << "'\t(expected)\n"
            //           << std::endl;

            // cheap test
            if (gridSpec == it->second) {
                continue;
            }

            // expensive test
            std::unique_ptr<const eckit::geo::Grid> a(eckit::geo::GridFactory::make_from_string(it->second));
            std::unique_ptr<const eckit::geo::Grid> b(eckit::geo::GridFactory::make_from_string(gridSpec));
            ASSERT(a && b);

            EXPECT(*a == *b);
        }
    }

    return true;
}


CASE("gridType=healpix")
{
    const map_count_spec_t specs{
        { 0, R"({"grid":"H2"})" },
        { 1, R"({"grid":"H2","order":"nested"})" },
        { 2, R"({"grid":"H32"})" },
        { 3, R"({"grid":"H32","order":"nested"})" },
        { 4, R"({"grid":"H1024","order":"nested"})" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=healpix.grib", specs));
}


CASE("gridType=unstructured_grid")
{
    SECTION("icon")
    {
        const map_count_spec_t specs{
            { 0, R"({"grid":"icon-grid-0012-r02b04-g"})" },
        };

        EXPECT(grib_to_gridspec("gridspec/gridType=unstructured_grid,icon.grib", specs));
    }


    SECTION("orca")
    {
        const map_count_spec_t specs{
            { 0, R"({"grid":"eORCA1_T","uid":"16076978a048410747dd7c9876677b28"})" },
            { 1, R"({"grid":"eORCA1_T"})" },
            { 2, R"({"grid":"eORCA1_U"})" },
            { 3, R"({"grid":"eORCA1_U","uid":"7378487847e050559b82d0792374a705"})" },
            { 4, R"({"grid":"eORCA1_V","uid":"d9622b55f3120eafb3dbaf5c742bc56c"})" },
            { 5, R"({"grid":"eORCA1_V"})" },
        };

        EXPECT(grib_to_gridspec("gridspec/gridType=unstructured_grid,orca.grib", specs));
    }
}


#if 0
CASE("gridType=lambert")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=lambert.grib", specs));
}
#endif


#if 0
CASE("gridType=lambert_azimuthal_equal_area")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=lambert_azimuthal_equal_area.grib", specs));
}
#endif


#if 0
CASE("gridType=mercator")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=mercator.grib", specs));
}
#endif


#if 0
CASE("gridType=polar_stereographic")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=polar_stereographic.grib", specs));
}
#endif


CASE("gridType=reduced_gg")
{
    SECTION("global")
    {
        const map_count_spec_t specs{
            { 0, R"({"grid":"O8"})" },
            { 1, R"({"grid":"O16"})" },
            { 2, R"({"grid":"O20"})" },
            { 3, R"({"grid":"N32"})" },
            { 4, R"({"grid":"N32"})" },
            { 5, R"({"grid":"O32"})" },
            { 6, R"({"grid":"O32"})" },
            { 7, R"({"grid":"N80"})" },
            { 8, R"({"grid":"N80"})" },
            { 9, R"({"grid":"O80"})" },
            { 10, R"({"grid":"N128"})" },
            { 11, R"({"grid":"N160"})" },
            { 12, R"({"grid":"N160"})" },
            { 13, "" },
            { 14, "" },
            { 15, R"({"grid":"N200"})" },
            { 16, R"({"grid":"N200"})" },
            { 17, R"({"grid":"N256"})" },
            { 18, "" },
            { 19, R"({"grid":"N320"})" },
            { 20, R"({"grid":"N320"})" },
            { 21, R"({"grid":"N320"})" },
            { 22, R"({"grid":"N400"})" },
            { 23, "" },
            { 24, R"({"grid":"O400"})" },
            { 25, R"({"grid":"O400"})" },
            { 26, "" },
            { 27, R"({"grid":"N640"})" },
            { 28, "" },
            { 29, R"({"grid":"N640"})" },
            { 30, R"({"grid":"N640"})" },
            { 31, R"({"grid":"N640"})" },
            { 32, R"({"grid":"O640"})" },
            { 33, "" },
            { 34, R"({"grid":"O640"})" },
            { 35, R"({"grid":"O1280"})" },
            { 36, "" },
            { 37, R"({"grid":"O1280"})" },
            { 38, R"({"grid":"O1280"})" },
            { 39, R"({"grid":"O1280"})" },
            { 40, R"({"grid":"O1280"})" },
            { 41, "" },
            { 42, "" },
            { 43, "" },
            { 44, "" },
            { 45, "" },
            { 46, "" },
            { 47, "" },
            { 48, "" },
            { 49, "" },
            { 50, "" },
            { 51, "" },
            { 52, "" },
            { 53, "" },
            { 54, "" },
            { 55, "" },
            { 56, "" },
            { 57, "" },
            { 58, "" },
            { 59, "" },
            { 60, "" },
            { 61, "" },
            { 62, "" },
            { 63, "" },
            { 64, "" },
            { 65, "" },
        };
        EXPECT(grib_to_gridspec("gridspec/gridType=reduced_gg.grib", specs));
    }


    SECTION("o2")
    {
        const ::eckit::spec::Custom spec{ { "grid", "o2" } };

        for (const auto* name : {
                 "GRIB1",
                 "GRIB2",
             }) {
            auto* sample = codes_grib_handle_new_from_samples(nullptr, name);
            EXPECT(sample != nullptr);

            auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
            EXPECT(h != nullptr);

            long valid = 0;
            set_string(h, "messageValidityChecks", "grid");
            CHECK(codes_get_long(h, "isMessageValid", &valid));
            EXPECT(valid == 1);

            std::string type;
            get_string(h, "gridType", type);
            EXPECT(type == "reduced_gg");

            std::string gridName;
            get_string(h, "gridName", gridName);
            EXPECT(gridName == "O2");

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
}


CASE("gridType=reduced_ll")
{
    SECTION("global")
    {
        const map_count_spec_t specs{
            { 0, "" },
        };

        EXPECT(grib_to_gridspec("gridspec/gridType=reduced_ll.grib", specs));
    }


    SECTION("custom")
    {
        const ::eckit::spec::Custom spec{ { "type", "reduced_ll" }, { "pl", std::vector<long>{ 0, 10, 0 } }, { "area", std::vector<double>{ 90., 0., -90., 360. } } };

        for (const auto* name : {
                 "GRIB1",
                 "GRIB2",
             }) {
            auto* sample = codes_grib_handle_new_from_samples(nullptr, name);
            EXPECT(sample != nullptr);

            auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
            EXPECT(h != nullptr);

            long valid = 0;
            set_string(h, "messageValidityChecks", "grid");
            CHECK(codes_get_long(h, "isMessageValid", &valid));
            EXPECT(valid == 1);

            std::string type;
            get_string(h, "gridType", type);
            EXPECT(type == "reduced_ll");

            long Nj                 = 3;
            long numberOfDataPoints = 10;
            CHECK(codes_get_long(h, "Nj", &Nj));
            CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints));

            std::vector<double> area(4);
            CHECK(codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &area[0]));
            CHECK(codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &area[1]));
            CHECK(codes_get_double(h, "latitudeOfLastGridPointInDegrees", &area[2]));
            CHECK(codes_get_double(h, "longitudeOfLastGridPointInDegrees", &area[3]));

            EXPECT(eckit::types::is_approximately_equal(area[0], 90.));
            EXPECT(eckit::types::is_approximately_equal(area[1], 0.));
            EXPECT(eckit::types::is_approximately_equal(area[2], -90.));
            EXPECT(eckit::types::is_approximately_equal(area[3], 324.));

            codes_handle_delete(h);
        }
    }
}


#if 0
CASE("gridType=reduced_rotated_gg")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=reduced_rotated_gg.grib", specs));
}
#endif


CASE("gridType=regular_gg")
{
    const map_count_spec_t specs{
        { 0, "" },  // R"({"grid":"F48","area":[]})" },
        { 1, "" },  // R"({"grid":"F48","area":[]})" },
        { 2, "" },  // R"({"grid":"F60","area":[]})" },
        { 3, "" },  // R"({"grid":"F80","area":[]})" },
        { 4, "" },  // R"({"grid":"F80","area":[]})" },
        { 5, "" },  // R"({"grid":"F640","area":[]})" },
        { 6, "" },  // R"({"grid":"F2250","area":[]})" },
        { 7, R"({"grid":"F8"})" },
        { 8, R"({"grid":"F32"})" },
        { 9, R"({"grid":"F48"})" },
        { 10, R"({"grid":"F80"})" },
        { 11, R"({"grid":"F95"})" },
        { 12, R"({"grid":"F640"})" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=regular_gg.grib", specs));
}


CASE("gridType=regular_ll")
{
    SECTION("gridspec")
    {
        const map_count_spec_t specs{
            { 0, R"({"area":[1,0,0,1],"grid":[1,1]})" },
            { 1, R"({"area":[2,0,0,2],"grid":[2,2]})" },
            { 2, R"({"area":[1,0,0,1],"grid":[1,1]})" },
            { 3, R"({"area":[1,0,0,1],"grid":[0.5,0.5]})" },
            { 4, R"({"area":[1,0,0,1],"grid":[0.5,0.5]})" },
            { 5, R"({"area":[90,0,84,6],"grid":[2,2]})" },
            { 6, R"({"area":[3,0,0,3],"grid":[1,1]})" },
            { 7, R"({"grid":[10,10]})" },
            { 8, R"({"grid":[10,10]})" },
            { 9, R"({"grid":[9,9]})" },
            { 10, R"({"grid":[9,9]})" },
            { 11, R"({"grid":[9,9]})" },
            { 12, R"({"area":[69,-60,21,60],"grid":[3,3]})" },
            { 13, R"({"area":[19,6,-11,366],"grid":[5,5]})" },
            { 14, R"({"area":[72,-35,24,74],"grid":[1,1]})" },
            { 15, R"({"grid":[3,3]})" },
            { 16, R"({"area":[90,0,-78,360],"grid":[3,3]})" },
            { 17, R"({"area":[75,-65,15,65],"grid":[1,1]})" },
            { 18, R"({"area":[47.1,-12.9,33,8.1],"grid":[0.15,0.15]})" },
            { 19, R"({"area":[73.5,-27,33,45],"grid":[0.5,0.5]})" },
            { 20, R"({"grid":[2,4]})" },
            { 21, R"({"grid":[2,2]})" },
            { 22, R"({"grid":[2,2]})" },
            { 23, "" },  // jPointsAreConsecutive
            { 24, "" },  // jPointsAreConsecutive
            { 25, R"({"grid":[1.5,1.5]})" },
            { 26, R"({"grid":[1.40625098039216,0.462724935732648],"reference":[0,0.231362467866324]})" },
            { 27, R"({"grid":[1.25,0.833333333333333],"order":"i+j+"})" },
            { 28, R"({"area":[90,0,-78,360],"grid":[1,1]})" },
            { 29, R"({"grid":[1,1],"reference":[0.5,0.5]})" },
            { 30, R"({"grid":[1,1]})" },
            { 31, R"({"area":[73,-27,33,45],"grid":[0.2,0.2]})" },
            { 32, R"({"area":[73,-27,33,45],"grid":[0.1,0.1]})" },
            { 33, R"({"grid":[0.45,0.3],"order":"i+j+","reference":[0.225,0.15]})" },
            { 34, R"({"grid":[0.45,0.3],"order":"i+j+"})" },
            // { 35, R"({"area":[80.0390615702317,0.175781,-80.0390634297683,359.823707],"order":"i+j+","grid":[0.351562,0.234375]})" },
            // { 36, R"({"area":[80.0390615702317,0.175781,-80.0390634297683,359.823707],"order":"i+j+","grid":[0.351562,0.234375]})" },
            // { 37, R"({"area":[89.55,0,-90,360],"grid":[0.35,0.35]})" },
            // { 38, R"({"area":[90,-179.875,-90,180.125],"order":"i+j+","grid":[0.25,0.25]})" },
            { 39, R"({"grid":[0.1,0.1],"reference":[0.05,0.05]})" },
            { 40, R"({"grid":[0.00833333333333333,0.00833333333333333],"reference":[0.00416666666666667,0.00416666666666667]})" },
            { 41, R"({"area":[73,-27,33,45],"grid":[4,4]})" },
            { 42, R"({"area":[10,-0.5,-10,17.5],"grid":[0.5,0.5]})" },
            // { 43, R"({"area":[90,-177.5,-90,182.5],"grid":[5,5]})" },
            { 44, R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0.9375,0.625]})" },
            { 45, R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0,0.625]})" },
            { 46, R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0.9375,0]})" },
            { 47, R"({"grid":[1.25,0.833333333333333],"order":"i+j+","reference":[0.625,0.416666666666667]})" },
            { 48, R"({"grid":[0.5625,0.375],"order":"i+j+","reference":[0.28125,0.1875]})" },

            { 50, R"({"area":[20,20,17,24],"grid":[1,1]})" },
            { 51, R"({"area":[20,20,17,24],"grid":[1,1],"order":"i-j-"})" },
            { 52, R"({"area":[20,20,17,24],"grid":[1,1],"order":"i+j+"})" },
        };

        EXPECT(grib_to_gridspec("gridspec/gridType=regular_ll.grib", specs));
    }


    SECTION("1/1")
    {
        const ::eckit::spec::Custom spec{ { "grid", "1/1" } };

        for (const auto* name : {
                 "GRIB1",
                 "GRIB2",
             }) {
            auto* sample = codes_grib_handle_new_from_samples(nullptr, name);
            EXPECT(sample != nullptr);

            auto* h = eccodes::geo::GribFromSpec::set(sample, spec);
            EXPECT(h != nullptr);

            long valid = 0;
            set_string(h, "messageValidityChecks", "grid");
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
            EXPECT(eckit::types::is_approximately_equal(area[3], 360. - 1., 0.5 * 1e-6));

            codes_handle_delete(h);
        }
    }


    SECTION("special cases")
    {
        grib_file_t file("gridspec/gridType=regular_ll,isECMWFPostGRIB2MigrationMessage=1.grib");
        ASSERT(file.next());

        struct test_t
        {
            const std::string user;
            const std::string spec;
            const std::vector<size_t> shape;
        };

        for (const auto& test : std::vector<test_t>{
                 { "{grid:[0,0]}", R"({"area":[90,0,90,0],"grid":[0,0]})", { 1, 1 } },
                 { "{area:[0,0,0,0], grid:[0,0]}", R"({"area":[0,0,0,0],"grid":[0,0]})", { 1, 1 } },
                 { "{area:[0,0,0,0], grid:[1,1]}", R"({"area":[0,0,0,0],"grid":[0,0]})", { 1, 1 } },
                 { "{area:[60,0,60,30], grid:[2,2]}", R"({"area":[60,0,60,30],"grid":[2,0]})", { 1, 16 } },
                 { "{area:[60,0,0,0], grid:[2,2]}", R"({"area":[60,0,0,0],"grid":[0,2]})", { 31, 1 } },
                 { "{area:[60,0,0,30], grid:[2,2]}", R"({"area":[60,0,0,30],"grid":[2,2]})", { 31, 16 } },
             }) {
            std::unique_ptr<const ::eckit::geo::Grid> grid(::eckit::geo::GridFactory::make_from_string(test.user));

            EXPECT(test.shape == grid->shape());
            EXPECT(test.spec == grid->spec_str());

            size_t len_user   = test.user.length();
            size_t len_buffer = 500;
            std::string buffer(len_buffer, '\0');

            EXPECT(CODES_SUCCESS == codes_set_string(file.handle.get(), "gridSpec", test.user.data(), &len_user));
            EXPECT(CODES_SUCCESS == codes_get_string(file.handle.get(), "gridSpec", buffer.data(), &len_buffer));
            EXPECT(test.spec == std::string(buffer.data(), len_buffer));
        }
    }


    SECTION("scanningMode=96")
    {
        grib_file_t file("gridspec/gridType=regular_ll,scanningMode=96.grib");
        ASSERT(file.next());

        auto h = file.handle.get();

        long scanningMode = 0;
        EXPECT(CODES_SUCCESS == codes_get_long(h, "scanningMode", &scanningMode) && scanningMode == 96);

        size_t values_len = 0;
        EXPECT(CODES_SUCCESS == codes_get_size(h, "values", &values_len) && values_len == 6);

        std::vector<double> values(values_len);
        EXPECT(CODES_SUCCESS == codes_get_double_array(h, "values", values.data(), &values_len));

        eccodes::geo::GribToSpec spec(h);
        std::unique_ptr<const ::eckit::geo::Grid> grid(::eckit::geo::GridFactory::build(spec));
        ASSERT(grid);

        EXPECT(grid->order() == "j+i+");

        auto is_vector_approximately_equal = [](const std::vector<double>& a, const std::vector<double>& b, double eps) {
            if (a.size() != b.size()) { return false; }
            for (size_t i = 0; i < a.size(); ++i) {
                if (!eckit::types::is_approximately_equal(a[i], b[i], eps)) { return false; }
            }
            return true;
        };

        auto [lats, lons] = grid->to_latlons();

        EXPECT(is_vector_approximately_equal(values, { 0, 1, 2, 3, 4, 5 }, 1e-6));
        EXPECT(is_vector_approximately_equal(lats, { 0, 1, 0, 1, 0, 1 }, ::eckit::geo::PointLonLat::EPS));
        EXPECT(is_vector_approximately_equal(lons, { 0, 0, 1, 1, 2, 2 }, ::eckit::geo::PointLonLat::EPS));
    }
}


#if 0
CASE("gridType=rotated_gg")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=rotated_gg.grib", specs));
}
#endif


#if 0
CASE("gridType=rotated_ll")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=rotated_ll.grib", specs));
}
#endif


CASE("gridType=sh")
{
    const map_count_spec_t specs{
        { 0, R"({"grid":"T19"})" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=sh.grib", specs));
}


#if 0
CASE("gridType=space_view")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=space_view.grib", specs));
}
#endif


#if 0
CASE("gridType=transverse_mercator")
{
    const map_count_spec_t specs{
        { 0, "" },
    };

    EXPECT(grib_to_gridspec("gridspec/gridType=transverse_mercator.grib", specs));
}
#endif


int main(int argc, char* argv[])
{
    const auto* ev_name = "ECCODES_ECKIT_GEO";
    const auto* ev_val  = getenv(ev_name);
    if (ev_val != nullptr && atol(ev_val) != 0) {
        return eckit::testing::run_tests(argc, argv);
    }

    std::printf("%s: This test is disabled (env. variable %s is not set)", argv[0], ev_name);
    return 0;
}
