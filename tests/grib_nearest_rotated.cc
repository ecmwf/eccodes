/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// ECC-600: nearest neighbour for rotated lat/lon grids (see grib_ecc-600.sh), with or without eckit::geo


#include <cmath>
#include <cstdio>
#include <memory>
#include <string>

#include "eckit/testing/Test.h"

#include "eccodes/eccodes.h"


// Directory holding the "tigge/*.grib" test data, set from the first command-line argument
static std::string DATA_DIR = ".";


struct FileCloser
{
    void operator()(FILE* f) const noexcept
    {
        if (f != nullptr) {
            std::fclose(f);
        }
    }
};


CASE("ECC-600")
{
    struct test_t
    {
        double lat;
        double lon;
        int index;
        double latitude;
        double longitude;
        double distance;
    };

    const test_t tests[] = {
        { 40., 0., 54294, 39.98, 0., 2.03 },
        { 50., -10., 145684, 49.99, -9.97, 2.57 },
    };

    std::unique_ptr<FILE, FileCloser> file(std::fopen((DATA_DIR + "/tigge/tiggelam_cnmc_sfc.grib").c_str(), "rb"));
    EXPECT(file);

    size_t count = 0;
    for (int err = 0;; ++count) {
        std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> h(
            codes_handle_new_from_file(nullptr, file.get(), PRODUCT_GRIB, &err), &codes_handle_delete);
        EXPECT(err == CODES_SUCCESS);
        if (!h) {
            break;
        }

        for (const auto& test : tests) {
            double lat      = 0;
            double lon      = 0;
            double value    = 0;
            double distance = 0;
            int index       = 0;
            EXPECT(codes_grib_nearest_find_multiple(h.get(), 0, &test.lat, &test.lon, 1, &lat, &lon, &value, &distance,
                                                    &index) == CODES_SUCCESS);

            EXPECT_EQUAL(index, test.index);
            EXPECT(std::abs(lat - test.latitude) < 0.01);
            EXPECT(std::abs(lon - test.longitude) < 0.01);
            EXPECT(std::abs(distance - test.distance) < 0.01);
        }
    }

    EXPECT(count > 0);
}


int main(int argc, char* argv[])
{
    // Usage: grib_nearest_rotated [<test-data-directory>]
    if (argc > 1) {
        DATA_DIR = argv[1];
        argc     = 1;  // consume it, the rest is for eckit's test runner
    }

    return eckit::testing::run_tests(argc, argv);
}
