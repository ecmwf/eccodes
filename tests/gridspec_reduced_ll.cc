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


// gridSpec of gridType=reduced_ll


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::encode_t;
using gridspec::keys_t;


// Nj (rows), the first and last grid points (in the edition's units) and the number of points
keys_t grid(long Nj, long lat1, long lon1, long lat2, long lon2, long numberOfDataPoints)
{
    return { { "Nj", Nj },
             { "latitudeOfFirstGridPoint", lat1 },
             { "longitudeOfFirstGridPoint", lon1 },
             { "latitudeOfLastGridPoint", lat2 },
             { "longitudeOfLastGridPoint", lon2 },
             { "numberOfDataPoints", numberOfDataPoints } };
}


CASE("gridSpec to GRIB and back")
{
    EXPECT(gridspec::check_all<encode_t>({
        // global: the last longitude is of the row of most points, rows can have no points
        { "reduced_ll_sfc_grib1", R"({"pl":[0,10,0],"type":"reduced_ll"})", "reduced_ll", grid(3, 90000, 0, -90000, 324000, 10) },
        { "reduced_ll_sfc_grib2", R"({"pl":[0,10,0],"type":"reduced_ll"})", "reduced_ll", grid(3, 90000000, 0, -90000000, 324000000, 10) },
        { "reduced_ll_sfc_grib1", R"({"pl":[2,10,2],"type":"reduced_ll"})", "reduced_ll", grid(3, 90000, 0, -90000, 324000, 14) },
        { "reduced_ll_sfc_grib2", R"({"area":[90,0,-90,360],"pl":[2,10,2],"type":"reduced_ll"})", "reduced_ll", grid(3, 90000000, 0, -90000000, 324000000, 14), R"({"pl":[2,10,2],"type":"reduced_ll"})" },

        // an area: the rows' points from its west to its east
        { "reduced_ll_sfc_grib1", R"({"area":[10,-10,-10,10],"pl":[4,8,4],"type":"reduced_ll"})", "reduced_ll", grid(3, 10000, -10000, -10000, 10000, 16) },
        { "reduced_ll_sfc_grib2", R"({"area":[10,-10,-10,10],"pl":[4,8,4],"type":"reduced_ll"})", "reduced_ll", grid(3, 10000000, 350000000, -10000000, 10000000, 16), R"({"area":[10,350,-10,370],"pl":[4,8,4],"type":"reduced_ll"})" },

        // set on a message of another grid
        { "GRIB1", R"({"pl":[0,10,0],"type":"reduced_ll"})", "reduced_ll", grid(3, 90000, 0, -90000, 324000, 10) },
        { "GRIB2", R"({"pl":[0,10,0],"type":"reduced_ll"})", "reduced_ll", grid(3, 90000000, 0, -90000000, 324000000, 10) },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=reduced_ll.grib",
                                   {
                                       R"({"pl":[2,6,14,20,26,32,38,44,50,58,64,70,76,82,88,94,102,108...)",  // #0
                                       R"({"area":[90,-98,5,54],"pl":[2,4,6,8,12,14,16,20,22,24,28,30,...)",
                                       R"({"area":[90,0,-78,360],"pl":[2,6,14,20,26,32,38,44,50,58,64,...)",
                                       R"({"pl":[2,6,14,20,26,32,38,44,50,58,64,70,76,82,88,94,100,108...)",
                                   }));
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
