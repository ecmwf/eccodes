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


// gridSpec of gridType=lambert_azimuthal_equal_area (PROJ laea), GRIB2 only


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
        { "GRIB2", R"({"false_xy":[5387500.044,-5387500.044],"projection":{"figure":"wgs84","lat_0":90,"reverse":true,"type":"laea"},"x":[0,10775000,25000],"y":[0,-10775000,-25000]})", "lambert_azimuthal_equal_area", { { "Nx", 432 }, { "Ny", 432 }, { "latitudeOfFirstGridPoint", 16623926 }, { "longitudeOfFirstGridPoint", 225000000 }, { "standardParallel", 90000000 }, { "centralLongitude", 0 }, { "Dx", 25000000 }, { "Dy", 25000000 }, { "scanningMode", 0 }, { "shapeOfTheEarth", 5 } } },
        { "GRIB2", R"({"false_xy":[1818500.021,-2287499.955],"projection":{"figure":"grs80","lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,4995000,5000],"y":[0,-4745000,-5000]})", "lambert_azimuthal_equal_area", { { "latitudeOfFirstGridPoint", 66982143 }, { "longitudeOfFirstGridPoint", 324965976 }, { "standardParallel", 52000000 }, { "centralLongitude", 10000000 }, { "shapeOfTheEarth", 4 } } },

        // the default figure
        { "regular_ll_sfc_grib2", R"({"false_xy":[1809553.577,-2282061.633],"projection":{"lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,4995000,5000],"y":[0,-4745000,-5000]})", "lambert_azimuthal_equal_area", { { "latitudeOfFirstGridPoint", 66978845 }, { "longitudeOfFirstGridPoint", 325005706 }, { "shapeOfTheEarth", 6 } }, R"({"false_xy":[1809553.613,-2282061.622],"projection":{"lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,4995000,5000],"y":[0,-4745000,-5000]})" },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=lambert_azimuthal_equal_area.grib",
                                   {
                                       R"({"false_xy":[5387500.044,-5387500.044],"projection":{"figure":"wgs84","lat_0":90,"reverse":true,"type":"laea"},"x":[0,10775000,25000],"y":[0,-10775000,-25000]})",         // #0, GRIB2
                                       R"({"false_xy":[1818500.021,-2287499.955],"projection":{"figure":"grs80","lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,4995000,5000],"y":[0,-4745000,-5000]})",  // #1, GRIB2
                                       R"({"false_xy":[1809553.577,-2282061.633],"projection":{"lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,4995000,5000],"y":[0,-4745000,-5000]})",                   // #2, GRIB2
                                       R"({"false_xy":[1949999.969,-2100000.007],"projection":{"figure":"grs80","lat_0":55,"lon_0":10,"reverse":true,"type":"laea"},"x":[0,3798000,2000],"y":[0,-4398000,-2000]})",  // #3, GRIB2
                                   },
                                   // the first grid point to the angular precision (the false easting/northing to the centimetre)
                                   { 2 }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
