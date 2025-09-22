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
#include <fstream>
#include <string>
#include <vector>

#include "eckit/testing/Test.h"

#include "eccodes/eccodes.h"


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


CASE("grib_to_gridspec")
{
    // NOTE/FIXME: need corrections
    const std::vector<std::string> SPECS{
        R"({"grid":"N32"})",
        R"({"grid":"O32"})",
        R"({"east":350,"grid":[10,10],"north":90,"south":-90,"west":0})",
        R"({"grid":"N32"})",
        R"({"grid":"O32"})",
        R"({"east":350,"grid":[10,10],"north":90,"south":-90,"west":0})",
        R"({"grid":"eORCA025_T"})",
        R"({"grid":"H2"})",
        R"({"grid":"H2"})",
        R"({"grid":"F2250"})",
        R"({"grid":"F32"})",
        R"({"grid":"F48"})",
        R"({"grid":"F48"})",
        R"({"grid":"F48"})",
        R"({"grid":"F48"})",
        R"({"grid":"F48"})",
        R"({"grid":"F60"})",
        R"({"grid":"F640"})",
        R"({"grid":"F640"})",
        R"({"grid":"F8"})",
        R"({"grid":"F80"})",
        R"({"grid":"F80"})",
        R"({"grid":"F80"})",
        R"({"grid":"F80"})",
        R"({"grid":"F80"})",
        R"({"grid":"F95"})",
        R"({"grid":"H32"})",
        R"({"grid":"H32"})",
        R"({"grid":"N128"})",
        R"({"grid":"N256"})",
        "",
        R"({"grid":"N320"})",
        R"({"grid":"N640"})",
        R"({"grid":"N640"})",
        R"({"grid":"N80"})",
        R"({"grid":"N80"})",
        R"({"grid":"N80"})",
        R"({"grid":"O1280"})",
        R"({"grid":"O1280"})",
        "",
        R"({"grid":"O16"})",
        R"({"grid":"O160"})",
        R"({"grid":"O160"})",
        R"({"grid":"O20"})",
        R"({"grid":"O32"})",
        R"({"grid":"O32"})",
        R"({"grid":"O400"})",
        R"({"grid":"O400"})",
        R"({"grid":"O400"})",
        R"({"grid":"O640"})",
        "",
        R"({"grid":"O8"})",
        R"({"grid":"O80"})",
        R"({"grid":"O80"})",
        R"({"grid":"O80"})",
        R"({"first_lonlat":[0,0],"grid":[1,1],"projection":{"lat_0":80,"lat_1":80,"lat_2":80,"lon_0":326,"r":6371229,"type":"lcc"},"shape":[11,11]})",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        R"({"east":1,"grid":[1,1],"north":1,"south":0,"west":0})",
        R"({"east":3,"grid":[1,1],"north":3,"south":0,"west":0})",
        R"({"east":359.995833333333,"grid":[0.00833333333333333,0.00833333333333333],"north":89.9958333333333,"south":-89.9958333333333,"west":0.00416666666666667})",
        R"({"east":45,"grid":[0.5,0.5],"north":73.5,"south":33,"west":-27})",
        R"({"east":359.82373,"grid":[0.351562,0.234375],"north":80.039062,"south":-80.039062,"west":0.175781})",
        R"({"east":357,"grid":[3,3],"north":90,"south":-90,"west":0})",
        R"({"east":359.883,"grid":[0.234375244299674,0.156250217202433],"north":89.922,"south":-89.922,"west":0.117})",
        R"({"east":358.5,"grid":[1.5,1.5],"north":90,"south":-90,"west":0})",
        R"({"east":359,"grid":[1,1],"north":90,"south":-78,"west":0})",
        R"({"east":359.375,"grid":[1.25,0.833333333333333],"north":89.5833333333333,"south":-89.5833333333333,"west":0.625})",
        R"({"east":359.8,"grid":[0.35,0.35],"north":89.55,"south":-90,"west":0})",
        R"({"east":45,"grid":[0.1,0.1],"north":73,"south":33,"west":-27})",
        R"({"east":1,"grid":[1,1],"north":1,"south":0,"west":0})",
        R"({"east":179.875,"grid":[0.25,0.25],"north":89.875,"south":-89.875,"west":-179.875})",
        R"({"east":1,"grid":[0.5,0.5],"north":1,"south":0,"west":0})",
        R"({"east":359.95,"grid":[0.1,0.1],"north":89.95,"south":-89.95,"west":0.05})",
        R"({"east":358.594,"grid":[1.40625098039216,0.462724935732648],"north":90,"south":-90,"west":0})",
        R"({"east":359.824219,"grid":[0.351562,0.234375],"north":80.039062,"south":-80.039062,"west":0.175781})",
        R"({"east":1,"grid":[0.5,0.5],"north":1,"south":0,"west":0})",
        R"({"east":358,"grid":[2,4],"north":88,"south":-88,"west":0})",
        R"({"east":359.5,"grid":[1,1],"north":89.5,"south":-89.5,"west":0.5})",
        R"({"east":65,"grid":[1,1],"north":75,"south":15,"west":-65})",
        R"({"east":8.1,"grid":[0.15,0.15],"north":47.1,"south":33,"west":-12.9})",
        R"({"east":358,"grid":[2,2],"north":90,"south":-90,"west":0})",
        R"({"east":2,"grid":[2,2],"north":2,"south":0,"west":0})",
        R"({"east":351,"grid":[9.1,9],"north":90,"south":-90,"west":0})",
        R"({"east":361,"grid":[5,5],"north":19,"south":-11,"west":6})",
        R"({"east":351,"grid":[9,9],"north":90,"south":-90,"west":0})",
        R"({"east":359.775,"grid":[0.45,0.3],"north":89.85,"south":-89.85,"west":0.225})",
        R"({"east":359.55,"grid":[0.45,0.3],"north":90,"south":-90,"west":0})",
        R"({"east":6,"grid":[2,2],"north":90,"south":84,"west":0})",
        R"({"east":60,"grid":[3,3],"north":69,"south":21,"west":-60})",
        R"({"east":358,"grid":[2,2],"north":90,"south":-90,"west":0})",
        R"({"east":359,"grid":[1,1],"north":90,"south":-90,"west":0})",
        R"({"east":357,"grid":[3,3],"north":90,"south":-78,"west":0})",
        R"({"east":358.75,"grid":[1.25,0.833333333333333],"north":90,"south":-90,"west":0})",
        R"({"east":359.93,"grid":[0.14062524423603,0.0937498697238145],"north":89.953,"south":-89.953,"west":0.07})",
        R"({"east":351,"grid":[9,9],"north":90,"south":-90,"west":0})",
        R"({"east":45,"grid":[0.2,0.2],"north":73,"south":33,"west":-27})",
        R"({"east":357,"grid":[3,3],"north":90,"south":-78,"west":0})",
        R"({"east":60,"grid":[3,3],"north":69,"south":21,"west":-60})",
        R"({"east":357,"grid":[3,3],"north":90,"south":-78,"west":0})",
        R"({"east":60,"grid":[3,3],"north":69,"south":21,"west":-60})",
        R"({"east":357,"grid":[3,3],"north":90,"south":-90,"west":0})",
        R"({"east":357,"grid":[3,3],"north":90,"south":-90,"west":0})",
        "",
        R"({"grid":"eORCA1_T"})",
        R"({"grid":"eORCA1_T"})",
        R"({"grid":"eORCA1_U"})",
        R"({"grid":"eORCA1_U"})",
        R"({"grid":"eORCA1_V"})",
        R"({"grid":"eORCA1_V"})",
        R"({"grid":"ICON_12_R02B04"})",
    };

    const std::string path{ "grib_to_gridspec.grib" };

    auto* in = std::fopen(path.c_str(), "rb");
    ASSERT(in != nullptr);

    int count = 0;
    int err   = 0;
    for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err)); ++count) {
        ASSERT(CODES_SUCCESS == err);

        const auto& gridSpecExpected = SPECS.at(count);
        if (gridSpecExpected.empty()) {
            codes_handle_delete(h);
            continue;
        }

        // long numberOfDataPoints = 0;
        // CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints));

        std::string gridSpec;
        ASSERT(get_string(h, "gridSpec", gridSpec));
        EXPECT(gridSpec == gridSpecExpected);

        codes_handle_delete(h);
    }

    std::fclose(in);
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
