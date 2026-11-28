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


// gridSpec of gridType=mercator (PROJ merc), GRIB2 only (template 3.10, set directly)


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
        { "GRIB2", R"({"false_xy":[7108383.475,-1800563.621],"order":"i+-j+","projection":{"figure":"wgs84_sphere","lat_ts":20,"reverse":true,"type":"merc"},"x":[0,422500,1250],"y":[0,280000,1250]})", "mercator", { { "gridDefinitionTemplateNumber", 10 }, { "Ni", 339 }, { "Nj", 225 }, { "latitudeOfFirstGridPoint", 16977500 }, { "longitudeOfFirstGridPoint", 291972200 }, { "latitudeOfLastGridPoint", 19522084 }, { "longitudeOfLastGridPoint", 296015559 }, { "LaD", 20000000 }, { "Di", 1250000 }, { "Dj", 1250000 }, { "scanningMode", 80 }, { "shapeOfTheEarth", 8 } } },

        // the default figure, north to south
        { "GRIB2", R"({"false_xy":[7108383.475,-1800563.621],"projection":{"lat_ts":20,"reverse":true,"type":"merc"},"x":[0,422500,1250],"y":[0,-280000,-1250]})", "mercator", { { "scanningMode", 0 }, { "shapeOfTheEarth", 6 }, { "numberOfDataPoints", 339 * 225 } }, R"({"false_xy":[7108383.438,-1800563.622],"projection":{"lat_ts":20,"reverse":true,"type":"merc"},"x":[0,422500,1250],"y":[0,-280000,-1250]})" },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=mercator.grib",
                                   {
                                       R"({"false_xy":[7108383.475,-1800563.621],"order":"i+-j+","projection":{"figure":"wgs84_sphere","lat_ts":20,"reverse":true,"type":"merc"},"x":[0,422500,1250],"y":[0,280000,1250]})",  // #0, GRIB2
                                   }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
