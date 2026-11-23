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


// gridSpec of gridType=transverse_mercator (PROJ tmerc), GRIB2 only (template 3.12, set directly)


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


// The grid by the (x, y) of its first and last points [m], encoded in centimetres (exact).


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB2", R"({"false_xy":[400000,-100000],"order":"i+j+","projection":{"figure":{"a":6377563.4,"b":6356256.91},"k_0":0.999601244926453,"lat_0":49,"lon_0":-2,"reverse":true,"type":"tmerc"},"x":[500000,506000,2000],"y":[200000,206000,3000]})", "transverse_mercator", { { "gridDefinitionTemplateNumber", 12 }, { "Ni", 4 }, { "Nj", 3 }, { "X1", 50000000 }, { "X2", 50600000 }, { "Di", 200000 }, { "Y1", 20000000 }, { "Y2", 20600000 }, { "Dj", 300000 }, { "XR", 40000000 }, { "YR", -10000000 }, { "scanningMode", 64 }, { "shapeOfTheEarth", 7 } } },
        { "GRIB2", R"({"false_xy":[400000,-100000],"order":"i-j-","projection":{"figure":{"a":6377563.4,"b":6356256.91},"k_0":0.999601244926453,"lat_0":49,"lon_0":-2,"reverse":true,"type":"tmerc"},"x":[506000,500000,-2000],"y":[206000,200000,-3000]})", "transverse_mercator", { { "X1", 50600000 }, { "X2", 50000000 }, { "scanningMode", 128 } } },
    }));
}


CASE("GRIB to gridSpec")
{
    // the scanning mode (j positive) is inconsistent with the grid's first/last points (Y1 > Y2)
    EXPECT(gridspec::check_decoded("gridspec/gridType=transverse_mercator.grib",
                                   {
                                       R"(error: Problem with calculation of geographic attributes)",  // #0, GRIB2
                                   }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
