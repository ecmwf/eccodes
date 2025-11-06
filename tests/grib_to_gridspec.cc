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

            std::cout << "count=" << count << " '" << gridSpec << "' == '" << it->second << "' (expected)" << std::endl;

            EXPECT(gridSpec == it->second);

            codes_handle_delete(h);
        }
    }

    std::fclose(in);
    return true;
}


CASE("grib_to_gridspec/albers.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/albers.grib", specs));
}


CASE("grib_to_gridspec/healpix.grib")
{
    const map_count_spec_t specs{
        { 0, "H2" },
        { 1, "H2n" },
        { 2, "H32n" },
        { 3, "H32" },
        { 4, "H36" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/healpix.grib", specs));
}


CASE("grib_to_gridspec/icon.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/icon.grib", specs));
}


CASE("grib_to_gridspec/lambert.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/lambert.grib", specs));
}


CASE("grib_to_gridspec/lambert_azimuthal_equal_area.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/lambert_azimuthal_equal_area.grib", specs));
}


CASE("grib_to_gridspec/mercator.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/mercator.grib", specs));
}


CASE("grib_to_gridspec/orca.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/orca.grib", specs));
}


CASE("grib_to_gridspec/polar_stereographic.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/polar_stereographic.grib", specs));
}


CASE("grib_to_gridspec/reduced_gg.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_gg.grib", specs));
}


CASE("grib_to_gridspec/reduced_ll.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_ll.grib", specs));
}


CASE("grib_to_gridspec/reduced_rotated_gg.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/reduced_rotated_gg.grib", specs));
}


CASE("grib_to_gridspec/regular_gg.grib")
{
    const map_count_spec_t specs{
        // { 0, R"({"grid:"F2250","area":[]})" },
        { 1, "F32" },
        // { 2, R"({"grid:"F48,"area":[]})" },
        // { 3, R"({"grid:"F48,"area":[]})" },
        { 4, "F48" },
        // { 5, R"({"grid:"F60,"area":[]})" },
        // { 6, R"({"grid:"F640,"area":[]})" },
        { 7, "F640" },
        { 8, "F8" },
        { 9, "F80" },
        // { 10, R"({"grid:"F80,"area":[]})" },
        // { 11, R"({"grid:"F80,"area":[]})" },
        // { 12, R"({"grid:"F95,"area":[]})" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/regular_gg.grib", specs));
}


CASE("grib_to_gridspec/regular_ll.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/regular_ll.grib", specs));
}


CASE("grib_to_gridspec/rotated_gg.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/rotated_gg.grib", specs));
}


CASE("grib_to_gridspec/rotated_ll.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/rotated_ll.grib", specs));
}


CASE("grib_to_gridspec/sh.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/sh.grib", specs));
}


CASE("grib_to_gridspec/space_view.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/space_view.grib", specs));
}


CASE("grib_to_gridspec/transverse_mercator.grib")
{
    const map_count_spec_t specs{
        { 0, R"()" },
    };

    EXPECT(grib_to_gridspec("grib_to_gridspec/transverse_mercator.grib", specs));
}


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
