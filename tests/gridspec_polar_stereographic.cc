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


// gridSpec of gridType=polar_stereographic (PROJ stere)


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


// The grid on the projection plane (x, y [m]), the false easting/northing placing the first grid point at (0, 0) to the
// millimetre (the first grid point is encoded in degrees, to the angular precision). The figure is part of the geometry:
// the default figure is shapeOfTheEarth=6 (here of messages of that figure, as they keep their own); GRIB1 encodes two
// figures only (earthIsOblate).
// The latitude of true scale (lat_ts) is GRIB2 only (GRIB1 is 60 degrees, of the projection centre's hemisphere).


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        // negative longitudes (GRIB2 does not encode them)
        { "regular_ll_sfc_grib2", R"({"false_xy":[1901416.78,7613145.491],"order":"i+j+","projection":{"lat_0":90,"lat_ts":60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})", "polar_stereographic", { { "Nx", 1121 }, { "Ny", 881 }, { "latitudeOfFirstGridPoint", 23148096 }, { "longitudeOfFirstGridPoint", 240977000 }, { "LaD", 60000000 }, { "orientationOfTheGrid", 255000000 }, { "projectionCentreFlag", 0 }, { "scanningMode", 64 }, { "shapeOfTheEarth", 6 } }, R"({"false_xy":[1901416.763,7613145.425],"order":"i+j+","projection":{"lat_0":90,"lat_ts":60,"lon_0":255,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})" },

        // south pole
        { "regular_ll_sfc_grib2", R"({"false_xy":[1901416.78,7613145.491],"order":"i+j+","projection":{"lat_0":-90,"lat_ts":-60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})", "polar_stereographic", { { "latitudeOfFirstGridPoint", -23148096 }, { "longitudeOfFirstGridPoint", 89023000 }, { "LaD", -60000000 }, { "projectionCentreFlag", 128 } }, R"({"false_xy":[1901416.763,7613145.425],"order":"i+j+","projection":{"lat_0":-90,"lat_ts":-60,"lon_0":255,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})" },

        // the latitude of true scale at the pole (the default)
        { "regular_ll_sfc_grib2", R"({"false_xy":[3757840.372,4230049.1],"order":"i+j+","projection":{"lat_0":90,"lon_0":330,"reverse":true,"type":"stere"},"x":[0,7170000,2500],"y":[0,7170000,2500]})", "polar_stereographic", { { "latitudeOfFirstGridPoint", 42113800 }, { "longitudeOfFirstGridPoint", 288383136 }, { "LaD", 90000000 }, { "orientationOfTheGrid", 330000000 } } },

        // GRIB1 (millidegrees: the false easting/northing to the metre)
        { "GRIB1", R"({"false_xy":[1901416.78,7613145.491],"order":"i+j+","projection":{"figure":"iau1965","lat_0":90,"lat_ts":60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})", "polar_stereographic", { { "projectionCentreFlag", 0 }, { "earthIsOblate", 1 } }, R"({"false_xy":[1901410.956,7613122.171],"order":"i+j+","projection":{"figure":"iau1965","lat_0":90,"lat_ts":60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})" },
        { "GRIB1", R"({"false_xy":[1901416.78,7613145.491],"order":"i+j+","projection":{"figure":"iau1965","lat_0":-90,"lat_ts":-60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})", "polar_stereographic", { { "projectionCentreFlag", 128 } }, R"({"false_xy":[1901410.956,7613122.171],"order":"i+j+","projection":{"figure":"iau1965","lat_0":-90,"lat_ts":-60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})" },
    }));
}


CASE("projection centre")
{
    // the pole of the projection (as the message can have another's)
    auto h = gridspec::sample("regular_ll_sfc_grib2");
    EXPECT(gridspec::set_gridspec(h.get(), R"({"false_xy":[1901416.763,7613145.425],"order":"i+j+","projection":{"lat_0":-90,"lat_ts":-60,"lon_0":255,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})") == CODES_SUCCESS);
    EXPECT_EQUAL(gridspec::get_long(h.get(), "projectionCentreFlag"), 128);
    EXPECT(gridspec::set_gridspec(h.get(), R"({"false_xy":[3757840.372,4230049.1],"order":"i+j+","projection":{"lat_0":90,"lon_0":330,"reverse":true,"type":"stere"},"x":[0,7170000,2500],"y":[0,7170000,2500]})") == CODES_SUCCESS);
    EXPECT_EQUAL(gridspec::get_long(h.get(), "projectionCentreFlag"), 0);
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=polar_stereographic.grib",
                                   {
                                       R"({"false_xy":[1901416.78,7613145.491],"order":"i+j+","projection":{"lat_0":90,"lat_ts":60,"lon_0":-105,"reverse":true,"type":"stere"},"x":[0,5334560,4763],"y":[0,4191440,4763]})",  // #0, GRIB1
                                       R"({"false_xy":[3757840.372,4230049.1],"order":"i+j+","projection":{"lat_0":90,"lon_0":-30,"reverse":true,"type":"stere"},"x":[0,7170000,2500],"y":[0,7170000,2500]})",                // #1, GRIB2
                                   },
                                   // GRIB1 of the default figure (not encodable in GRIB1); and a negative longitude (GRIB2 encodes 330)
                                   { 0, 1 }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
