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
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/testing/Test.h"

#include "eccodes/geo/iterator/Iterator.h"
#include "eccodes/geo/GribToSpec.h"
#include "eccodes/eccodes.h"
#include "eccodes/geo/eckit.h"


namespace eccodes::tests
{


bool difference(const std::string& label, const std::vector<double>& l1, const std::vector<double>& l2)
{
    bool result = false;
    ASSERT(l1.size() == l2.size());

    for (size_t i = 0; i < l1.size(); ++i) {
        if (!eckit::types::is_approximately_equal(l1[i], l2[i], eckit::geo::PointLonLat::EPS)) {
            std::printf("%s: index %zu:\t%.16g\t- %.16g\t= %.16g\n", label.c_str(), i, l1[i], l2[i], l2[i] - l1[i]);
            result = true;
        }
    }

    return result;
}


CASE("reduced_gg")
{
    SECTION("coordinates(eccodes) == coordinates(eckit::geo::Grid)")
    {
        for (int i = 1; i < eckit::Main::instance().argc(); ++i) {
            const auto& path = eckit::Main::instance().argv(i);

            auto* in = std::fopen(path.c_str(), "rb");
            ASSERT(in != nullptr);

            int err   = 0;
            int count = 0;
            for (grib_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err)); ++count) {
                ASSERT(err == CODES_SUCCESS);
                ASSERT(h != nullptr);

                std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(geo::GribToSpec(h)));
                ASSERT(grid);

                auto [ekg_lats, ekg_lons] = grid->to_latlons();
                ASSERT(grid->size() == ekg_lats.size());
                ASSERT(grid->size() == ekg_lons.size());

                long N = 0;
                CODES_CHECK(codes_get_long(h, "numberOfDataPoints", &N), 0);
                EXPECT(grid->size() == N);

                struct iterator_t : std::unique_ptr<geo_iterator::Iterator, decltype(&geo_iterator::gribIteratorDelete)>
                {
                    explicit iterator_t(element_type* ptr) : unique_ptr(ptr, &geo_iterator::gribIteratorDelete) {}
                } it(geo_iterator::gribIteratorNew(h, 0, &err));
                ASSERT(err == CODES_SUCCESS);
                ASSERT(it);

                long n = 0;
                std::vector<double> ecc_lats(N);
                std::vector<double> ecc_lons(N);
                for (auto *lat = ecc_lats.data(), *lon = ecc_lons.data(); n < N && it->next(lat++, lon++, nullptr) != 0; n++) {}
                ASSERT(n == N);

                EXPECT_NOT(difference(path + " latitudes (eccodes - eckit::geo::Grid)", ecc_lats, ekg_lats));
                EXPECT_NOT(difference(path + " longitudes (eccodes - eckit::geo::Grid)", ecc_lons, ekg_lons));
            }

            std::fclose(in);
        }
    }
}


}  // namespace eccodes::tests


int main(int argc, char* argv[])
{
    const auto* ev_name = "ECCODES_ECKIT_GEO";
    const auto* ev_val  = std::getenv(ev_name);
    if (ev_val == nullptr || std::atol(ev_val) == 0) {
        std::printf("%s: This test is disabled (env. variable %s is not set)", argv[0], ev_name);
        return 0;
    }

    return eckit::testing::run_tests(argc, argv);
}
