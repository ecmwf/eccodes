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


// gridSpec of gridType=healpix, GRIB2 only


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;


CASE("gridSpec to GRIB and back")
{
    // the first longitude is 45 degrees (ECC-2162); the order is ring (the default) or nested
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB2", R"({"grid":"H2"})", "healpix", { { "Nside", 2 }, { "orderingConvention", "ring" }, { "longitudeOfFirstGridPoint", 45000000 }, { "numberOfDataPoints", 48 } } },
        { "GRIB2", R"({"grid":"H2","order":"nested"})", "healpix", { { "Nside", 2 }, { "orderingConvention", "nested" }, { "numberOfDataPoints", 48 } } },
        { "GRIB2", R"({"grid":"H1"})", "healpix", { { "Nside", 1 }, { "numberOfDataPoints", 12 } } },
        { "GRIB2", R"({"grid":"H1024","order":"ring"})", "healpix", { { "Nside", 1024 }, { "orderingConvention", "ring" }, { "numberOfDataPoints", 12582912 } }, R"({"grid":"H1024"})" },

        // set on a message of another grid, keeping its figure
        { "regular_ll_sfc_grib2", R"({"grid":"H2"})", "healpix", { { "Nside", 2 }, { "shapeOfTheEarth", 6 } } },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=healpix.grib",
                                   {
                                       R"({"grid":"H2"})",
                                       R"({"grid":"H2","order":"nested"})",
                                       R"({"grid":"H32"})",
                                       R"({"grid":"H32","order":"nested"})",
                                       R"({"grid":"H1024","order":"nested"})",
                                   }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
