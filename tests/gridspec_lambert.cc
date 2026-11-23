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


// gridSpec of gridType=lambert (Lambert conformal, PROJ lcc)


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


// The grid on the projection plane (x, y [m]), the false easting/northing placing the first grid point at (0, 0) to the
// millimetre (the first grid point is encoded in degrees, to the angular precision). The figure is part of the geometry:
// the default figure is shapeOfTheEarth=6 (here of messages of that figure, as they keep their own); GRIB1 encodes two
// figures only (earthIsOblate).


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        // the first grid point to the angular precision (the false easting/northing in the gridSpec back)
        { "regular_ll_sfc_grib2", R"({"false_xy":[921962.126,884482.74],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})", "lambert", { { "Nx", 739 }, { "Ny", 709 }, { "latitudeOfFirstGridPoint", 37338234 }, { "longitudeOfFirstGridPoint", 351651286 }, { "LaD", 45800000 }, { "LoV", 2000000 }, { "Latin1", 45800000 }, { "Latin2", 45800000 }, { "Dx", 2500000 }, { "Dy", 2500000 }, { "scanningMode", 0 }, { "shapeOfTheEarth", 6 } }, R"({"false_xy":[921962.129,884482.683],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})" },

        // secant (two standard parallels), of a figure
        { "GRIB2", R"({"false_xy":[289299.535,207955.983],"order":"i+j+","projection":{"figure":"iau1965","lat_0":49.833,"lat_1":49.833,"lat_2":51.167,"lon_0":4.359,"reverse":true,"type":"lcc"},"x":[0,600000,1000],"y":[0,590000,1000]})", "lambert", { { "latitudeOfFirstGridPoint", 47896000 }, { "longitudeOfFirstGridPoint", 492000 }, { "Latin1", 49833000 }, { "Latin2", 51167000 }, { "scanningMode", 64 }, { "shapeOfTheEarth", 2 } } },
        { "GRIB1", R"({"false_xy":[289299.535,207955.983],"order":"i+j+","projection":{"figure":"iau1965","lat_0":49.833,"lat_1":49.833,"lat_2":51.167,"lon_0":4.359,"reverse":true,"type":"lcc"},"x":[0,600000,1000],"y":[0,590000,1000]})", "lambert", { { "latitudeOfFirstGridPoint", 47896 }, { "longitudeOfFirstGridPoint", 492 }, { "Latin1", 49833 }, { "Latin2", 51167 }, { "scanningMode", 64 }, { "earthIsOblate", 1 } } },

        // southern hemisphere, negative longitudes (GRIB2 does not encode them)
        { "regular_ll_sfc_grib2", R"({"false_xy":[1995214.954,2495030.718],"order":"i+j+","projection":{"lat_0":-35,"lat_1":-35,"lon_0":-65,"reverse":true,"type":"lcc"},"x":[0,3992000,4000],"y":[0,4992000,4000]})", "lambert", { { "latitudeOfFirstGridPoint", -54377984 }, { "longitudeOfFirstGridPoint", 265691498 }, { "LoV", 295000000 }, { "projectionCentreFlag", 128 }, { "scanningMode", 64 } }, R"({"false_xy":[1995214.92,2495030.751],"order":"i+j+","projection":{"lat_0":-35,"lat_1":-35,"lon_0":295,"reverse":true,"type":"lcc"},"x":[0,3992000,4000],"y":[0,4992000,4000]})" },

        // alternating rows
        { "GRIB2", R"({"false_xy":[3271151.606,263793.733],"order":"i+-j+","projection":{"figure":"wgs84_sphere","lat_0":25,"lat_1":25,"lon_0":265,"reverse":true,"type":"lcc"},"x":[0,5953063.832,2539.703],"y":[0,4053365.988,2539.703]})", "lambert", { { "Nx", 2345 }, { "Ny", 1597 }, { "scanningMode", 80 }, { "shapeOfTheEarth", 8 } } },
    }));
}


CASE("projection centre")
{
    // the hemisphere of the projection (as the message can have another's)
    auto h = gridspec::sample("regular_ll_sfc_grib2");
    EXPECT(gridspec::set_gridspec(h.get(), R"({"false_xy":[1995214.92,2495030.751],"order":"i+j+","projection":{"lat_0":-35,"lat_1":-35,"lon_0":295,"reverse":true,"type":"lcc"},"x":[0,3992000,4000],"y":[0,4992000,4000]})") == CODES_SUCCESS);
    EXPECT_EQUAL(gridspec::get_long(h.get(), "projectionCentreFlag"), 128);
    EXPECT(gridspec::set_gridspec(h.get(), R"({"false_xy":[921962.129,884482.683],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})") == CODES_SUCCESS);
    EXPECT_EQUAL(gridspec::get_long(h.get(), "projectionCentreFlag"), 0);
}


CASE("default figure, on a message of another figure")
{
    // the message keeps its figure (the GRIB1 sphere, shapeOfTheEarth=0), so the grid (of the default figure) is encoded
    // as of the message's: the false easting/northing got back are of the other figure, and setting it again shifts it
    // again (known limitation)
    auto h = gridspec::sample("GRIB2");
    EXPECT(gridspec::set_gridspec(h.get(), R"({"false_xy":[921962.126,884482.74],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})") == CODES_SUCCESS);
    EXPECT_EQUAL(gridspec::get_long(h.get(), "shapeOfTheEarth"), 0);
    EXPECT_EQUAL(gridspec::get_gridspec(h.get()), R"({"false_xy":[921418.175,883960.842],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})");
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=lambert.grib",
                                   {
                                       R"({"false_xy":[921962.126,-885517.26],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})",                                                       // #0, GRIB1, centre=lfpw (fixed: the first grid point the top-left)
                                       R"({"false_xy":[289299.535,207955.983],"order":"i+j+","projection":{"figure":"iau1965","lat_0":49.833,"lat_1":49.833,"lat_2":51.167,"lon_0":4.359,"reverse":true,"type":"lcc"},"x":[0,600000,1000],"y":[0,590000,1000]})",  // #1, GRIB1
                                       R"({"false_xy":[289299.535,207955.983],"order":"i+j+","projection":{"figure":"iau1965","lat_0":49.833,"lat_1":49.833,"lat_2":51.167,"lon_0":4.359,"reverse":true,"type":"lcc"},"x":[0,600000,1000],"y":[0,590000,1000]})",  // #2, GRIB2
                                       R"({"false_xy":[1481641.677,537326.064],"order":"i+j+","projection":{"lat_0":53.5,"lat_1":53.5,"lon_0":5,"reverse":true,"type":"lcc"},"x":[0,1320000,2500],"y":[0,1220000,2500]})",                                         // #3, GRIB1
                                       R"({"false_xy":[1481641.677,537326.064],"order":"i+j+","projection":{"lat_0":53.5,"lat_1":53.5,"lon_0":5,"reverse":true,"type":"lcc"},"x":[0,1320000,2500],"y":[0,1220000,2500]})",                                         // #4, GRIB1
                                       R"({"false_xy":[3271151.606,263793.733],"order":"i+-j+","projection":{"figure":"wgs84_sphere","lat_0":25,"lat_1":25,"lon_0":265,"reverse":true,"type":"lcc"},"x":[0,5953063.832,2539.703],"y":[0,4053365.988,2539.703]})",  // #5, GRIB2
                                       R"({"false_xy":[-551891.198,1031350.226],"order":"i+j+","projection":{"lat_0":80,"lat_1":80,"lon_0":-34,"reverse":true,"type":"lcc"},"x":[0,1970000,2500],"y":[0,2470000,2500]})",                                          // #6, GRIB1
                                       R"({"false_xy":[-552217.004,1031959.078],"order":"i+j+","projection":{"lat_0":80,"lat_1":80,"lon_0":326,"reverse":true,"type":"lcc"},"x":[0,1970000,2500],"y":[0,2470000,2500]})",                                          // #7, GRIB2
                                       R"({"false_xy":[924450.73,-883010.46],"projection":{"lat_0":45.8,"lat_1":45.8,"lon_0":2,"reverse":true,"type":"lcc"},"x":[0,1845000,2500],"y":[0,-1770000,-2500]})",                                                        // #8, GRIB1, centre=lfpw (fixed: the first grid point the top-left)
                                       R"({"false_xy":[1995214.954,2495030.718],"order":"i+j+","projection":{"lat_0":-35,"lat_1":-35,"lon_0":295,"reverse":true,"type":"lcc"},"x":[0,3992000,4000],"y":[0,4992000,4000]})",                                        // #9, GRIB1
                                       R"({"false_xy":[704556.223,584635.182],"order":"i+j+","projection":{"lat_0":40,"lat_1":40,"lon_0":357.5,"reverse":true,"type":"lcc"},"x":[0,1410000,2500],"y":[0,1170000,2500]})",                                          // #10, GRIB1
                                   },
                                   // GRIB1 of the default figure (6367470 m sphere): the gridSpec (no figure) is not encodable in GRIB1
                                   { 0, 3, 4, 6, 8, 9, 10 }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
