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
    const std::map<size_t, std::string> SPECS{
        { 0, R"({"grid":"N32"})" },
        { 1, R"({"grid":"O32"})" },
        { 2, R"({"east":350,"grid":[10,10],"north":90,"south":-90,"west":0})" },
        { 3, R"({"grid":"N32"})" },
        { 4, R"({"grid":"O32"})" },
        { 5, R"({"east":350,"grid":[10,10],"north":90,"south":-90,"west":0})" },
        { 6, R"({"grid":"eORCA025_T"})" },
        { 7, R"({"grid":"H2"})" },
        { 8, R"({"grid":"H2"})" },
        { 9, R"({"grid":"F2250"})" },
        { 10, R"({"grid":"F32"})" },
        { 11, R"({"grid":"F48"})" },
        { 12, R"({"grid":"F48"})" },
        { 13, R"({"grid":"F48"})" },
        { 14, R"({"grid":"F48"})" },
        { 15, R"({"grid":"F48"})" },
        { 16, R"({"grid":"F60"})" },
        { 17, R"({"grid":"F640"})" },
        { 18, R"({"grid":"F640"})" },
        { 19, R"({"grid":"F8"})" },
        { 20, R"({"grid":"F80"})" },
        { 21, R"({"grid":"F80"})" },
        { 22, R"({"grid":"F80"})" },
        { 23, R"({"grid":"F80"})" },
        { 24, R"({"grid":"F80"})" },
        { 25, R"({"grid":"F95"})" },
        { 26, R"({"grid":"H32"})" },
        { 27, R"({"grid":"H32"})" },
        { 28, R"({"grid":"N128"})" },
        { 29, R"({"grid":"N256"})" },
        { 30, "" },
        { 31, R"({"grid":"N320"})" },
        { 32, R"({"grid":"N640"})" },
        { 33, R"({"grid":"N640"})" },
        { 34, R"({"grid":"N80"})" },
        { 35, R"({"grid":"N80"})" },
        { 36, R"({"grid":"N80"})" },
        { 37, R"({"grid":"O1280"})" },
        { 38, R"({"grid":"O1280"})" },
        { 39, "" },
        { 40, R"({"grid":"O16"})" },
        { 41, R"({"grid":"O160"})" },
        { 42, R"({"grid":"O160"})" },
        { 43, R"({"grid":"O20"})" },
        { 44, R"({"grid":"O32"})" },
        { 45, R"({"grid":"O32"})" },
        { 46, R"({"grid":"O400"})" },
        { 47, R"({"grid":"O400"})" },
        { 48, R"({"grid":"O400"})" },
        { 49, R"({"grid":"O640"})" },
        { 50, "" },
        { 51, R"({"grid":"O8"})" },
        { 52, R"({"grid":"O80"})" },
        { 53, R"({"grid":"O80"})" },
        { 54, R"({"grid":"O80"})" },
        { 55, R"({"first_lonlat":[0,0],"grid":[1,1],"projection":{"lat_0":80,"lat_1":80,"lat_2":80,"lon_0":326,"r":6371229,"type":"lcc"},"shape":[11,11]})" },
        { 56, "" },
        { 57, "" },
        { 58, "" },
        { 59, "" },
        { 60, "" },
        { 61, "" },
        { 62, "" },
        { 63, R"({"east":1,"grid":[1,1],"north":1,"south":0,"west":0})" },
        { 64, R"({"east":3,"grid":[1,1],"north":3,"south":0,"west":0})" },
        { 65, R"({"east":359.995833333333,"grid":[0.00833333333333333,0.00833333333333333],"north":89.9958333333333,"south":-89.9958333333333,"west":0.00416666666666667})" },
        { 66, R"({"east":45,"grid":[0.5,0.5],"north":73.5,"south":33,"west":-27})" },
        { 67, R"({"east":359.82373,"grid":[0.351562,0.234375],"north":80.039062,"south":-80.039062,"west":0.175781})" },
        { 68, R"({"east":357,"grid":[3,3],"north":90,"south":-90,"west":0})" },
        { 69, R"({"east":359.883,"grid":[0.234375244299674,0.156250217202433],"north":89.922,"south":-89.922,"west":0.117})" },
        { 70, R"({"east":358.5,"grid":[1.5,1.5],"north":90,"south":-90,"west":0})" },
        { 71, R"({"east":359,"grid":[1,1],"north":90,"south":-78,"west":0})" },
        { 72, R"({"east":359.375,"grid":[1.25,0.833333333333333],"north":89.5833333333333,"south":-89.5833333333333,"west":0.625})" },
        { 73, R"({"east":359.8,"grid":[0.35,0.35],"north":89.55,"south":-90,"west":0})" },
        { 74, R"({"east":45,"grid":[0.1,0.1],"north":73,"south":33,"west":-27})" },
        { 75, R"({"east":1,"grid":[1,1],"north":1,"south":0,"west":0})" },
        { 76, R"({"east":179.875,"grid":[0.25,0.25],"north":89.875,"south":-89.875,"west":-179.875})" },
        { 77, R"({"east":1,"grid":[0.5,0.5],"north":1,"south":0,"west":0})" },
        { 78, R"({"east":359.95,"grid":[0.1,0.1],"north":89.95,"south":-89.95,"west":0.05})" },
        { 79, R"({"east":358.594,"grid":[1.40625098039216,0.462724935732648],"north":90,"south":-90,"west":0})" },
        { 80, R"({"east":359.824219,"grid":[0.351562,0.234375],"north":80.039062,"south":-80.039062,"west":0.175781})" },
        { 81, R"({"east":1,"grid":[0.5,0.5],"north":1,"south":0,"west":0})" },
        { 82, R"({"east":358,"grid":[2,4],"north":88,"south":-88,"west":0})" },
        { 83, R"({"east":359.5,"grid":[1,1],"north":89.5,"south":-89.5,"west":0.5})" },
        { 84, R"({"east":65,"grid":[1,1],"north":75,"south":15,"west":-65})" },
        { 85, R"({"east":8.1,"grid":[0.15,0.15],"north":47.1,"south":33,"west":-12.9})" },
        { 86, R"({"east":358,"grid":[2,2],"north":90,"south":-90,"west":0})" },
        { 87, R"({"east":2,"grid":[2,2],"north":2,"south":0,"west":0})" },
        { 88, R"({"east":351,"grid":[9.1,9],"north":90,"south":-90,"west":0})" },
        { 89, R"({"east":361,"grid":[5,5],"north":19,"south":-11,"west":6})" },
        { 90, R"({"east":351,"grid":[9,9],"north":90,"south":-90,"west":0})" },
        { 91, R"({"east":359.775,"grid":[0.45,0.3],"north":89.85,"south":-89.85,"west":0.225})" },
        { 92, R"({"east":359.55,"grid":[0.45,0.3],"north":90,"south":-90,"west":0})" },
        { 93, R"({"east":6,"grid":[2,2],"north":90,"south":84,"west":0})" },
        { 94, R"({"east":60,"grid":[3,3],"north":69,"south":21,"west":-60})" },
        { 95, R"({"east":358,"grid":[2,2],"north":90,"south":-90,"west":0})" },
        { 96, R"({"east":359,"grid":[1,1],"north":90,"south":-90,"west":0})" },
        { 97, R"({"east":357,"grid":[3,3],"north":90,"south":-78,"west":0})" },
        { 98, R"({"east":358.75,"grid":[1.25,0.833333333333333],"north":90,"south":-90,"west":0})" },
        { 99, R"({"east":359.93,"grid":[0.14062524423603,0.0937498697238145],"north":89.953,"south":-89.953,"west":0.07})" },
        { 100, R"({"east":351,"grid":[9,9],"north":90,"south":-90,"west":0})" },
        { 101, R"({"east":45,"grid":[0.2,0.2],"north":73,"south":33,"west":-27})" },
        { 102, R"({"east":357,"grid":[3,3],"north":90,"south":-78,"west":0})" },
        { 103, R"({"east":60,"grid":[3,3],"north":69,"south":21,"west":-60})" },
        { 104, R"({"east":357,"grid":[3,3],"north":90,"south":-78,"west":0})" },
        { 105, R"({"east":60,"grid":[3,3],"north":69,"south":21,"west":-60})" },
        { 106, R"({"east":357,"grid":[3,3],"north":90,"south":-90,"west":0})" },
        { 107, R"({"east":357,"grid":[3,3],"north":90,"south":-90,"west":0})" },
        { 108, "" },
        { 109, R"({"grid":"eORCA1_T"})" },
        { 110, R"({"grid":"eORCA1_T"})" },
        { 111, R"({"grid":"eORCA1_U"})" },
        { 112, R"({"grid":"eORCA1_U"})" },
        { 113, R"({"grid":"eORCA1_V"})" },
        { 114, R"({"grid":"eORCA1_V"})" },
        { 115, R"({"grid":"ICON_12_R02B04"})" },
    };

    const std::string path{ "grib_to_gridspec.grib" };

    auto* in = std::fopen(path.c_str(), "rb");
    ASSERT(in != nullptr);

    int count = 0;
    int err   = 0;
    for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err)); ++count) {
        ASSERT(CODES_SUCCESS == err);

        auto it = SPECS.find(count);
        if (it == SPECS.end() || it->second.empty()) {
            codes_handle_delete(h);
            continue;
        }

        // long numberOfDataPoints = 0;
        // CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints));

        std::string gridSpec;
        ASSERT(get_string(h, "gridSpec", gridSpec));

        const auto& gridSpecExpected = it->second;
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
