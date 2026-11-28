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


// gridSpec of gridType=albers (Albers equal-area, PROJ aea), GRIB2 only (template 3.31, set directly)


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB2", R"({"order":"i+j+","projection":{"figure":"grs80","lat_0":23,"lat_1":29.5,"lat_2":45.5,"lon_0":264,"reverse":true,"type":"aea"},"x":[0,1500000,50000],"y":[0,1400000,50000]})", "albers", { { "gridDefinitionTemplateNumber", 31 }, { "Nx", 31 }, { "Ny", 29 }, { "LaD", 23000000 }, { "LoV", 264000000 }, { "Latin1", 29500000 }, { "Latin2", 45500000 }, { "scanningMode", 64 }, { "shapeOfTheEarth", 4 } } },
    }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
