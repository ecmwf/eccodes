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


// gridSpec of gridType=space_view, GRIB2 only (template 3.90, set directly)


#include <string>
#include <utility>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


// The satellite at nr Earth (equatorial) radii from the centre, dx, dy the apparent Earth diameters in grid lengths,
// xp, yp the sub-satellite point and xo, yo the first grid point, in grid lengths.


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB2", R"({"dx":3622,"dy":3622,"nr":6.6107,"nx":16,"ny":16,"type":"space_view","xo":1500,"xp":1856,"yo":1700,"yp":1856})", "space_view", { { "gridDefinitionTemplateNumber", 90 }, { "Nx", 16 }, { "Ny", 16 }, { "dx", 3622 }, { "dy", 3622 }, { "Xp", 1856000 }, { "Yp", 1856000 }, { "Xo", 1500 }, { "Yo", 1700 }, { "Nr", 6610700 }, { "scanningMode", 0 }, { "shapeOfTheEarth", 6 } } },
        { "GRIB2", R"({"dx":3622,"dy":3610,"figure":"wgs84","lon_0":-75,"nr":6.6107,"nx":16,"ny":12,"type":"space_view","xo":2400,"xp":1856,"yo":1200,"yp":1856})", "space_view", { { "longitudeOfSubSatellitePoint", -75000000 }, { "shapeOfTheEarth", 5 }, { "numberOfDataPoints", 192 } } },
        { "GRIB2", R"({"dx":3622,"dy":3610,"figure":{"a":6378140,"b":6356755},"nr":6.6107,"nx":16,"ny":16,"order":"i-j+","type":"space_view","xo":1500,"xp":1856,"yo":1700,"yp":1856})", "space_view", { { "scanningMode", 192 }, { "shapeOfTheEarth", 7 } } },
        { "GRIB2", R"({"dx":5434,"dy":5434,"figure":"iau1965","lon_0":140.7,"nr":6.61071,"nx":16,"ny":16,"type":"space_view","xo":2600,"xp":2750,"yo":2700,"yp":2750})", "space_view", { { "longitudeOfSubSatellitePoint", 140700000 }, { "shapeOfTheEarth", 2 } } },
    }));
}


CASE("GRIB keys to gridSpec: not representable")
{
    // a space view of GRIB keys eckit::geo cannot describe (the message is set from a gridSpec, then a key)
    for (const auto& [key, value] : std::vector<std::pair<const char*, long>>{
             { "dx", 0 },                                 // apparent Earth diameter
             { "latitudeOfSubSatellitePoint", 1000000 },  // satellite off the equator plane
             { "Nr", 900000 },                            // satellite inside the Earth
         }) {
        auto h = gridspec::sample("GRIB2");
        EXPECT(gridspec::set_gridspec(h.get(), R"({"dx":3622,"dy":3622,"nr":6.6107,"nx":16,"ny":16,"type":"space_view","xo":1500,"xp":1856,"yo":1700,"yp":1856})") == CODES_SUCCESS);
        EXPECT(codes_set_long(h.get(), key, value) == CODES_SUCCESS);
        EXPECT_EQUAL(gridspec::get_gridspec(h.get()), std::string("error: ") + codes_get_error_message(CODES_GEOCALCULUS_PROBLEM));
    }
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=space_view.grib",
                                   {
                                       R"({"dx":3622,"dy":3610,"figure":{"a":6378140,"b":6356755},"nr":6.6107,"nx":1900,"ny":900,"type":"space_view","xp":764,"yp":1774})",                   // #0, GRIB2
                                       R"({"dx":2416,"dy":2416,"figure":"iau1965","lon_0":57,"nr":6.61071,"nx":2500,"ny":2500,"type":"space_view","xo":1,"xp":1250,"yo":1,"yp":1250})",       // #1, GRIB1
                                       R"({"dx":2712,"dy":2712,"figure":"iau1965","lon_0":-135,"nr":6.61071,"nx":2816,"ny":2816,"type":"space_view","xo":1,"xp":1408,"yo":1,"yp":1408})",     // #2, GRIB1
                                       R"({"dx":2712,"dy":2712,"figure":"iau1965","lon_0":-75,"nr":6.61071,"nx":2816,"ny":2816,"type":"space_view","xo":1,"xp":1408,"yo":1,"yp":1408})",      // #3, GRIB1
                                       R"({"dx":3622,"dy":3622,"figure":"iau1965","nr":6.61071,"nx":3712,"ny":3712,"type":"space_view","xo":1,"xp":1856,"yo":1,"yp":1856})",                  // #4, GRIB1
                                       R"({"dx":3622,"dy":3610,"figure":{"a":6378140,"b":6356755},"nr":6.6107,"nx":3712,"ny":3712,"order":"i-j+","type":"space_view","xp":1856,"yp":1856})",  // #5, GRIB2
                                       R"({"dx":4833,"dy":4833,"figure":"iau1965","lon_0":57,"nr":6.61071,"nx":5000,"ny":5000,"type":"space_view","xo":1,"xp":2500,"yo":1,"yp":2500})",       // #6, GRIB1
                                       R"({"dx":5434,"dy":5434,"figure":"iau1965","lon_0":140.7,"nr":6.61071,"nx":5500,"ny":5500,"type":"space_view","xo":1,"xp":2750,"yo":1,"yp":2750})",    // #7, GRIB1
                                   },
                                   // GRIB1: encoded by the GRIB2 template only
                                   { 1, 2, 3, 4, 6, 7 }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
