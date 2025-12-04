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
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include "eckit/testing/Test.h"

#include "eccodes/eccodes.h"


#define CHECK(a) CODES_CHECK(a, nullptr)
#define SHOULD_WORK_TESTS 0


using map_count_spec_t = std::map<size_t, std::string>;


bool grib_to_gridspec(const std::string& path, const map_count_spec_t& specs)
{
    auto* in = std::fopen(path.c_str(), "rb");
    ASSERT(in != nullptr);

    int count = 0;
    int err   = 0;
    for (grib_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err)); ++count) {
        ASSERT(CODES_SUCCESS == err);
        ASSERT(h != nullptr);

        auto it = specs.find(count);
        if (it != specs.end() && !it->second.empty()) {
            // long numberOfDataPoints = 0;
            // CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints));

            char gridSpec[10240];
            size_t size = sizeof(gridSpec);

            EXPECT(CODES_SUCCESS == codes_get_string(h, "gridSpec", gridSpec, &size));

            std::cout << "count=" << count << " '" << gridSpec << "' (calculated) == '" << it->second << "' (expected)" << std::endl;

            // EXPECT(it->second == "?" || gridSpec == it->second);
        }

        codes_handle_delete(h);
    }

    std::fclose(in);
    return true;
}


#if SHOULD_WORK_TESTS
CASE("healpix")
{
    const map_count_spec_t specs{
        { 0, R"({"grid":"H2"})" },
        { 1, R"({"grid":"H2n"})" },
        { 2, R"({"grid":"H32"})" },
        { 3, R"({"grid":"H32n"})" },
        { 4, R"({"grid":"H1024n"})" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/healpix.grib", specs));
}
#endif


#if SHOULD_WORK_TESTS
CASE("icon")
{
    const map_count_spec_t specs{
        { 0, R"({"grid":"icon-grid-0012-r02b04-g"})" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/icon.grib", specs));
}
#endif


#if 0
CASE("lambert")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/lambert.grib", specs));
}
#endif


#if 0
CASE("lambert_azimuthal_equal_area")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/lambert_azimuthal_equal_area.grib", specs));
}
#endif


#if 0
CASE("mercator")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/mercator.grib", specs));
}
#endif


#if SHOULD_WORK_TESTS
CASE("orca")
{
    const map_count_spec_t specs{
        { 0, R"({"grid":"eORCA1_T","uid":"16076978a048410747dd7c9876677b28"})" },
        { 1, R"({"grid":"eORCA1_T"})" },
        { 2, R"({"grid":"eORCA1_U"})" },
        { 3, R"({"grid":"eORCA1_U","uid":"7378487847e050559b82d0792374a705"})" },
        { 4, R"({"grid":"eORCA1_V","uid":"d9622b55f3120eafb3dbaf5c742bc56c"})" },
        { 5, R"({"grid":"eORCA1_V"})" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/orca.grib", specs));
}
#endif


#if 0
CASE("polar_stereographic")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/polar_stereographic.grib", specs));
}
#endif


#if 1
CASE("reduced_gg")
{
    #if 1
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
            { 13, R"({"grid":"O160"})" },
            { 14, R"({"grid":"O160"})" },
            { 15, R"({"grid":"N200"})" },
            { 16, R"({"grid":"N200"})" },
            { 17, R"({"grid":"N256"})" },
            { 18, R"({"grid":"N320"})" },
            { 19, R"({"grid":"N320"})" },
            { 20, R"({"grid":"N320"})" },
            { 21, R"({"grid":"N320"})" },
            { 22, R"({"grid":"N400"})" },
            { 23, R"({"grid":"O400"})" },
            { 24, R"({"grid":"O400"})" },
            { 25, R"({"grid":"O400"})" },
            { 26, R"({"grid":"N640"})" },
            { 27, R"({"grid":"N640"})" },
            { 28, R"({"grid":"N640"})" },
            { 29, R"({"grid":"N640"})" },
            { 30, R"({"grid":"N640"})" },
            { 31, R"({"grid":"N640"})" },
            { 32, R"({"grid":"O640"})" },
            { 33, R"({"grid":"O640"})" },
            { 34, R"({"grid":"O640"})" },
            { 35, R"({"grid":"O1280"})" },
            { 36, R"({"grid":"O1280"})" },
            { 37, R"({"grid":"O1280"})" },
            { 38, R"({"grid":"O1280"})" },
            { 39, R"({"grid":"O1280"})" },
            { 40, R"({"grid":"O1280"})" },
        };

        EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_gg_global.grib", specs));
    }
    #endif


    #if 1
    SECTION("regional")
    {
        const map_count_spec_t specs{
            { 0, "?" },   // R"({"grid":"N640","area":[]})" },
            { 1, "?" },   // R"({"grid":"O640","area":[]})" },
            { 2, "?" },   // R"({"grid":"O640","area":[]})" },
            { 3, "?" },   // R"({"grid":"O640","area":[]})" },
            { 4, "?" },   // R"({"grid":"O640","area":[]})" },
            { 5, "?" },   // R"({"grid":"O640","area":[]})" },
            { 6, "?" },   // R"({"grid":"O640","area":[]})" },
            { 7, "?" },   // R"({"grid":"O640","area":[]})" },
            { 8, "?" },   // R"({"grid":"O640","area":[]})" },
            { 9, "?" },   // R"({"grid":"O640","area":[]})" },
            { 10, "?" },  // R"({"grid":"O640","area":[]})" },
            { 11, "?" },  // R"({"grid":"N1280","area":[]})" },
            { 12, "?" },  // R"({"grid":"N1280","area":[]})" },
            { 13, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 14, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 15, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 16, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 17, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 18, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 19, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 20, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 21, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 22, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 23, "?" },  // R"({"grid":"O1280","area":[]})" },
            { 24, "?" },  // R"({"grid":"O1280","area":[]})" },
        };
        EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_gg_regional.grib", specs));
    }
    #endif
}
#endif


#if SHOULD_WORK_TESTS
CASE("reduced_ll")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_ll.grib", specs));
}
#endif


#if 0
CASE("reduced_rotated_gg")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_rotated_gg.grib", specs));
}
#endif


#if SHOULD_WORK_TESTS
CASE("regular_gg")
{
    const map_count_spec_t specs{
        { 0, "?" },  // R"({"grid":"F48","area":[]})" },
        { 1, "?" },  // R"({"grid":"F48","area":[]})" },
        { 2, "?" },  // R"({"grid":"F60","area":[]})" },
        { 3, "?" },  // R"({"grid":"F80","area":[]})" },
        { 4, "?" },  // R"({"grid":"F80","area":[]})" },
        { 5, "?" },  // R"({"grid":"F640","area":[]})" },
        { 6, "?" },  // R"({"grid":"F2250","area":[]})" },
        { 7, R"({"grid":"F8"})" },
        { 8, R"({"grid":"F32"})" },
        { 9, R"({"grid":"F48"})" },
        { 10, R"({"grid":"F80"})" },
        { 11, R"({"grid":"F95"})" },
        { 12, R"({"grid":"F640"})" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/regular_gg.grib", specs));
}
#endif


#if SHOULD_WORK_TESTS
CASE("regular_ll")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/regular_ll.grib", specs));
}
#endif


#if 0
CASE("rotated_gg")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/rotated_gg.grib", specs));
}
#endif


#if 0
CASE("rotated_ll")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/rotated_ll.grib", specs));
}
#endif


#if SHOULD_WORK_TESTS
CASE("sh")
{
    const map_count_spec_t specs{
        { 0, R"({"grid":"T19"})" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/sh.grib", specs));
}
#endif


#if 0
CASE("space_view")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/space_view.grib", specs));
}
#endif


#if 0
CASE("transverse_mercator")
{
    const map_count_spec_t specs{
        { 0, "?" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/transverse_mercator.grib", specs));
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
