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


#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/testing/Test.h"

#include "eccodes/geo/iterator/Iterator.h"
#include "eccodes/geo/GribToSpec.h"
#include "eccodes/eccodes.h"
#include "eccodes/geo/eckit.h"


namespace eccodes::tests
{


std::string get_string(codes_handle* h, const char* key)
{
    char value[1024] = { 0 };
    size_t length = sizeof(value);
    return codes_get_string(h, key, value, &length) == CODES_SUCCESS ? value : "";
}


long get_long(codes_handle* h, const char* key)
{
    long value = 0;
    return codes_get_long(h, key, &value) == CODES_SUCCESS ? value : 0;
}


CASE("coordinates(eccodes) == coordinates(eckit::geo::Grid)")
{
    for (int i = 1; i < eckit::Main::instance().argc(); ++i) {
        const auto& path = eckit::Main::instance().argv(i);

        auto* in = std::fopen(path.c_str(), "rb");
        ASSERT(in != nullptr);

        int err = 0;
        for (grib_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            ASSERT(err == CODES_SUCCESS);
            std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> handle(h, &codes_handle_delete);

            const auto gridType = get_string(h, "gridType");
            const auto label = path + " (gridType=" + gridType + ")";

            std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(geo::GribToSpec(h)));
            ASSERT(grid);

            auto [ekg_lats, ekg_lons] = grid->to_latlons();
            ASSERT(grid->size() == ekg_lats.size());
            ASSERT(grid->size() == ekg_lons.size());

            const auto N = static_cast<size_t>(get_long(h, "numberOfDataPoints"));
            EXPECT(grid->size() == N);

            // eccodes iterators listing the points west to east and south to north (i+j+, reordering the values), or west
            // to east and north to south (space_view, i+j-)
            const auto ipos_jpos = gridType == "lambert" || gridType == "lambert_lam" || gridType == "mercator" ||
                                   gridType == "polar_stereographic";
            std::string order = ipos_jpos ? "i+j+" : gridType == "space_view" ? "i+j-"
                                                                              : "";

            // GRIB-405: the eccodes iterator latitudes always increase from the first grid point (some projections)
            if (ipos_jpos && get_long(h, "jScansPositively") == 0) {
                std::printf("%s: eccodes iterator latitudes increase from the first grid point (GRIB-405), not compared\n",
                            label.c_str());
                continue;
            }

            // the eccodes iterator does not unrotate regular Gaussian grids
            if (gridType == "rotated_gg") {
                std::printf("%s: eccodes iterator points not unrotated, not compared\n", label.c_str());
                continue;
            }

            // alternativeRowScanning, ignored by other eccodes iterators: compared to the eccodes iterator of the message
            // without alternativeRowScanning (eg. eckit::geo grid order i+-j- as i+j-)
            std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> plain(nullptr, &codes_handle_delete);
            if (!ipos_jpos && get_long(h, "alternativeRowScanning") != 0) {
                plain.reset(codes_handle_clone(h));
                ASSERT(plain && codes_set_long(plain.get(), "alternativeRowScanning", 0) == CODES_SUCCESS);

                if (order.empty()) {
                    auto is_sign = [](char c) { return c == '+' || c == '-'; };
                    order = grid->order();
                    order.erase(std::unique(order.begin(), order.end(),
                                            [&](char a, char b) { return is_sign(a) && is_sign(b); }),
                                order.end());
                }
            }

            std::unique_ptr<geo_iterator::Iterator, decltype(&geo_iterator::gribIteratorDelete)> it(
                geo_iterator::gribIteratorNew(plain ? plain.get() : h, 0, &err), &geo_iterator::gribIteratorDelete);

            if (err != CODES_SUCCESS || !it) {
                std::printf("%s: eccodes iterator not supported, not compared\n", label.c_str());
                continue;
            }

            size_t n = 0;
            std::vector<double> ecc_lats(N);
            std::vector<double> ecc_lons(N);
            for (auto *lat = ecc_lats.data(), *lon = ecc_lons.data(); n < N && it->next(lat++, lon++, nullptr) != 0; n++) {}
            ASSERT(n == N);

            // eckit::geo grid point k is the eccodes iterator point ren[k]
            auto ren = order.empty() ? eckit::geo::Grid::renumber_type(N) : grid->reorder(order);
            if (order.empty()) {
                std::iota(ren.begin(), ren.end(), 0);
            }

            // tolerance: GRIB precision (GRIB1, millidegrees), eccodes iterator rounding (rotations, to microdegrees), eccodes
            // iterator regular Gaussian longitudes (as the last longitude encoded, eg. 357.188 for N64, to millidegrees)
            const auto eps = get_long(h, "edition") == 1 || gridType == "regular_gg" ? 1e-3 : 1e-5;

            // space_view: points on the limb (tangent) can be visible or not (Earth as a sphere, to 0.01 degree)
            const auto space_view = gridType == "space_view";
            auto on_the_limb = [h](double lat, double lon) {
                double Nr = 0;
                double lon0 = 0;
                ASSERT(codes_get_double(h, "NrInRadiusOfEarthScaled", &Nr) == CODES_SUCCESS);
                ASSERT(codes_get_double(h, "longitudeOfSubSatellitePointInDegrees", &lon0) == CODES_SUCCESS);

                constexpr double d2r = M_PI / 180.;
                const auto angle = std::acos(std::cos(lat * d2r) * std::cos((lon - lon0) * d2r));
                return std::abs(angle - std::acos(1. / Nr)) < 0.01 * d2r;
            };

            size_t differences = 0;
            for (size_t k = 0; k < N; ++k) {
                const auto lat1 = ecc_lats.at(ren.at(k));
                const auto lon1 = ecc_lons.at(ren.at(k));
                const auto lat2 = ekg_lats[k];
                const auto lon2 = ekg_lons[k];

                // points not visible (space_view) are (0, 0) by the eccodes iterator, (nan, nan) by eckit::geo
                const auto visible1 = !(space_view && lat1 == 0. && lon1 == 0.);
                const auto visible2 = !std::isnan(lat2) && !std::isnan(lon2);
                if (!visible1 || !visible2) {
                    if (visible1 == visible2 || (space_view && on_the_limb(visible1 ? lat1 : lat2, visible1 ? lon1 : lon2))) {
                        continue;
                    }
                }
                else {
                    // longitude differences as distances along the parallel (ill-conditioned near the poles)
                    const auto dlat = std::abs(lat2 - lat1);
                    const auto dlon = std::abs(eckit::geo::PointLonLat::normalise_angle_to_minimum(lon2 - lon1, -180.)) *
                                      std::cos(lat2 * M_PI / 180.);
                    if (dlat <= eps && dlon <= eps) {
                        continue;
                    }
                }

                if (differences++ < 10) {
                    std::printf("%s: index %zu: eccodes (%.9f, %.9f), eckit::geo (%.9f, %.9f)\n", label.c_str(), k, lat1,
                                lon1, lat2, lon2);
                }
            }

            EXPECT(differences == 0);
        }

        std::fclose(in);
    }
}


}  // namespace eccodes::tests


int main(int argc, char* argv[])
{
    return eckit::testing::run_tests(argc, argv);
}
