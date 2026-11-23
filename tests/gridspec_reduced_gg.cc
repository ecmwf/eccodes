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


// gridSpec of gridType=reduced_gg and reduced_rotated_gg


#include <string>
#include <vector>

#include "gridspec.h"


using gridspec::decode_t;
using gridspec::encode_t;

using grid = gridspec::reduced_t;


// Gaussian latitudes are encoded rounded outwards (GRIB1 millidegrees, GRIB2 microdegrees), as the last longitude of the
// row of most points (global)


CASE("global")
{
    EXPECT(gridspec::check_all<encode_t>({
        // octahedral (pl 20, 24, ... 4N+16) and classic (pl as the IFS)
        { "reduced_gg_pl_32_grib1", R"({"grid":"O2"})", "reduced_gg", grid(2, 4, 59445, 0, -59445, 345000, 88, { { "isOctahedral", 1 }, { "gridName", "O2" } }) },
        { "reduced_gg_pl_32_grib2", R"({"grid":"O2"})", "reduced_gg", grid(2, 4, 59444409, 0, -59444409, 345000000, 88, { { "isOctahedral", 1 }, { "gridName", "O2" } }) },
        { "reduced_gg_pl_32_grib1", R"({"grid":"N32"})", "reduced_gg", grid(32, 64, 87864, 0, -87864, 357188, 6114, { { "isOctahedral", 0 }, { "gridName", "N32" } }) },
        { "reduced_gg_pl_32_grib2", R"({"grid":"N32"})", "reduced_gg", grid(32, 64, 87863799, 0, -87863799, 357187500, 6114, { { "isOctahedral", 0 }, { "gridName", "N32" } }) },
        { "reduced_gg_pl_32_grib1", R"({"grid":"O1280"})", "reduced_gg", grid(1280, 2560, 89947, 0, -89947, 359930, 6599680) },
        { "reduced_gg_pl_32_grib2", R"({"grid":"O1280"})", "reduced_gg", grid(1280, 2560, 89946188, 0, -89946188, 359929907, 6599680) },
        { "reduced_gg_pl_32_grib2", R"({"grid":"N1024"})", "reduced_gg", grid(1024, 2048, 89932738, 0, -89932738, 359912110, 5447118) },

        // of a pl not known (an octahedral pl is octahedral)
        { "reduced_gg_pl_32_grib1", R"({"grid":"N2","pl":[16,16,16,16]})", "reduced_gg", grid(2, 4, 59445, 0, -59445, 337500, 64) },
        { "reduced_gg_pl_32_grib2", R"({"grid":"N2","pl":[16,16,16,16]})", "reduced_gg", grid(2, 4, 59444409, 0, -59444409, 337500000, 64) },
        { "reduced_gg_pl_32_grib2", R"({"N":2,"pl":[20,24,24,20],"type":"reduced_gg"})", "reduced_gg", grid(2, 4, 59444409, 0, -59444409, 345000000, 88), R"({"grid":"O2"})" },

        // set on a message of another grid
        { "GRIB1", R"({"grid":"O2"})", "reduced_gg", grid(2, 4, 59445, 0, -59445, 345000, 88) },
        { "GRIB2", R"({"grid":"O2"})", "reduced_gg", grid(2, 4, 59444409, 0, -59444409, 345000000, 88) },
        { "regular_gg_sfc_grib2", R"({"grid":"O2"})", "reduced_gg", grid(2, 4, 59444409, 0, -59444409, 345000000, 88) },
    }));
}


CASE("area")
{
    EXPECT(gridspec::check_all<encode_t>({
        // the rows of points (pl of these rows), their first/last points the westmost/eastmost
        { "reduced_gg_pl_32_grib1", R"({"area":[60,-10,0,30],"grid":"O32"})", "reduced_gg", grid(32, 22, 59998, -10000, 1395, 30000, 254), R"({"area":[59.9970201084913,-10,1.39530691081949,30],"grid":"O32"})" },
        { "reduced_gg_pl_32_grib2", R"({"area":[60,-10,0,30],"grid":"O32"})", "reduced_gg", grid(32, 22, 59997021, 350000000, 1395306, 30000000, 254), R"({"area":[59.9970201084913,350,1.39530691081949,390],"grid":"O32"})" },
        { "reduced_gg_pl_32_grib2", R"({"area":[60,350,0,390],"grid":"O32"})", "reduced_gg", grid(32, 22, 59997021, 350000000, 1395306, 30000000, 254), R"({"area":[59.9970201084913,350,1.39530691081949,390],"grid":"O32"})" },

        // hemispheres
        { "reduced_gg_pl_32_grib1", R"({"area":[90,0,0,360],"grid":"O32"})", "reduced_gg", grid(32, 32, 87864, 0, 1395, 357500, 2624), R"({"area":[90,0,1.39530691081949,360],"grid":"O32"})" },
        { "reduced_gg_pl_32_grib2", R"({"area":[90,0,0,360],"grid":"O32"})", "reduced_gg", grid(32, 32, 87863799, 0, 1395306, 357500000, 2624), R"({"area":[90,0,1.39530691081949,360],"grid":"O32"})" },
        { "reduced_gg_pl_32_grib2", R"({"area":[-20,0,-90,360],"grid":"N32"})", "reduced_gg", grid(32, 25, -20929574, 0, -87863799, 357187500, 2161), R"({"area":[-20.9295742544895,0,-90,360],"grid":"N32"})" },
    }));
}


CASE("rotated")
{
    const gridspec::keys_t pole{ { "latitudeOfSouthernPoleInDegrees", -40. }, { "longitudeOfSouthernPoleInDegrees", 30. } };

    EXPECT(gridspec::check_all<encode_t>({
        { "reduced_gg_pl_32_grib1", R"({"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})", "reduced_rotated_gg", pole },
        { "reduced_gg_pl_32_grib2", R"({"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})", "reduced_rotated_gg", pole },
        { "reduced_rotated_gg_pl_32_grib2", R"({"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})", "reduced_rotated_gg", pole },
        { "reduced_gg_pl_80_grib2", R"({"grid":"O80","projection":{"south_pole":[30,30],"type":"rotation"}})", "reduced_rotated_gg", grid(80, 160, 89141520, 0, -89141520, 358928572, 28480) },

        // an area of the rotated grid (first/last grid points in the rotated frame)
        { "reduced_gg_pl_32_grib1", R"({"area":[60,-10,0,30],"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})", "reduced_rotated_gg", grid(32, 22, 59998, -10000, 1395, 30000, 254), R"({"area":[59.9970201084913,-10,1.39530691081949,30],"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})" },
        { "reduced_gg_pl_32_grib2", R"({"area":[60,-10,0,30],"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})", "reduced_rotated_gg", grid(32, 22, 59997021, 350000000, 1395306, 30000000, 254), R"({"area":[59.9970201084913,350,1.39530691081949,390],"grid":"O32","projection":{"south_pole":[30,-40],"type":"rotation"}})" },

        // the rotation of the grid about the rotated pole
        { "reduced_gg_pl_32_grib2", R"({"grid":"O32","projection":{"rotation_angle":15,"south_pole":[30,-40],"type":"rotation"}})", "reduced_rotated_gg", { { "angleOfRotationInDegrees", 15. }, { "numberOfDataPoints", 5248 } } },

        // the south pole (unrotated) is not a rotation
        { "reduced_rotated_gg_pl_32_grib2", R"({"grid":"O32","projection":{"south_pole":[0,-90],"type":"rotation"}})", "reduced_gg", {}, R"({"grid":"O32"})" },
    }));
}


CASE("GRIB keys to gridSpec")
{
    // the sample reduced_gg_pl_32_grib2 is N32, global
    EXPECT(gridspec::check_all<decode_t>({
        { "reduced_gg_pl_32_grib2", {}, R"({"grid":"N32"})" },

        // Gaussian latitudes are matched to a millidegree at least (inwards too, as GRIB1 messages converted to GRIB2)
        { "reduced_gg_pl_32_grib2", { { "latitudeOfFirstGridPoint", 87863000 }, { "latitudeOfLastGridPoint", -87863000 } }, R"({"grid":"N32"})" },

        // the last longitude of a global grid is of the row of most points (expected, and inwards with a warning), also
        // rotated (GRIB1 millidegrees rounded inwards)
        { "reduced_gg_pl_32_grib2", { { "longitudeOfLastGridPoint", 357187000 } }, R"({"grid":"N32"})" },
        { "reduced_rotated_gg_pl_256_grib1", {}, R"({"grid":"N256","projection":{"south_pole":[0,0],"type":"rotation"}})" },
    }));
}


CASE("GRIB keys to gridSpec, of known wrongly encoded messages")
{
    // fixed by md5GridSection, then as other messages (snapped to the Gaussian latitudes): O2560 of the latitudes 89.972
    // (not 89.973, rounded outwards; beyond the millidegree of snapping), and of other latitudes (not fixed), an error
    auto o2560 = [](double lat) {
        return gridspec::keys_t{ { "gridSpec", R"({"grid":"O2560"})" },
                                 { "latitudeOfFirstGridPointInDegrees", lat },
                                 { "latitudeOfLastGridPointInDegrees", -lat },
                                 { "longitudeOfLastGridPointInDegrees", 359.964 } };
    };

    EXPECT(gridspec::check_all<decode_t>({
        { "reduced_gg_pl_128_grib1", o2560(89.972), R"({"grid":"O2560"})" },
        { "reduced_gg_pl_128_grib1", o2560(89.971), "error: Grid description is wrong or inconsistent" },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=reduced_gg.grib",
                                   {
                                       R"({"grid":"O8"})",  // #0
                                       R"({"grid":"O16"})",
                                       R"({"grid":"O20"})",
                                       R"({"grid":"N32"})",
                                       R"({"grid":"N32"})",
                                       R"({"grid":"O32"})",  // #5
                                       R"({"grid":"O32"})",
                                       R"({"grid":"N80"})",
                                       R"({"grid":"N80"})",
                                       R"({"grid":"O80"})",
                                       // N128 of a pl other than the classical
                                       R"({"grid":"N128","pl":[18,30,36,48,54,60,72,80,90,90,96,108,120,120,128,144,144,144,160,160,180,180,180,192,192,200,216,216,216,240,240,240,240,250,250,256,270,270,288,288,288,300,300,320,320,320,320,360,360,360,360,360,360,360,360,384,384,384,384,384,400,400,400,400,432,432,432,432,432,432,432,450,450,450,450,450,480,480,480,480,480,480,480,480,480,480,486,486,500,500,500,500,500,500,500,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,500,500,500,500,500,500,500,486,486,480,480,480,480,480,480,480,480,480,480,450,450,450,450,450,432,432,432,432,432,432,432,400,400,400,400,384,384,384,384,384,360,360,360,360,360,360,360,360,320,320,320,320,300,300,288,288,288,270,270,256,250,250,240,240,240,240,216,216,216,200,192,192,180,180,180,160,160,144,144,144,128,120,120,108,96,90,90,80,72,60,54,48,36,30,18]})",  // #10
                                       R"({"grid":"N160"})",
                                       R"({"grid":"N160"})",
                                       R"({"area":[90,0,-90,359.438],"grid":"O160"})",
                                       R"(error: Grid description is wrong or inconsistent)",
                                       R"({"grid":"N200"})",  // #15
                                       R"({"grid":"N200"})",
                                       R"({"grid":"N256"})",
                                       R"({"grid":"N320"})",
                                       R"({"grid":"N320"})",
                                       R"({"grid":"N320"})",  // #20
                                       R"({"grid":"N320"})",
                                       R"({"grid":"N400"})",
                                       R"({"area":[90,0,-90,359.775],"grid":"O400"})",
                                       R"({"grid":"O400"})",
                                       R"({"grid":"O400"})",  // #25
                                       R"({"grid":"N640"})",
                                       R"({"grid":"N640"})",
                                       R"({"grid":"N640"})",
                                       R"({"grid":"N640"})",
                                       R"({"grid":"N640"})",  // #30
                                       R"({"grid":"N640"})",
                                       R"({"grid":"O640"})",
                                       R"({"grid":"O640"})",
                                       R"({"grid":"O640"})",
                                       R"({"grid":"O1280"})",  // #35
                                       R"({"grid":"O1280"})",
                                       R"({"grid":"O1280"})",
                                       R"({"grid":"O1280"})",
                                       R"({"grid":"O1280"})",
                                       R"({"grid":"O1280"})",  // #40
                                       R"({"area":[90,180,-90,540],"grid":"N640"})",
                                       R"(error: Grid description is wrong or inconsistent)",
                                       R"(error: Grid description is wrong or inconsistent)",
                                       R"({"area":[69.9336077222471,350,36.0562248559007,391],"grid":"O640"})",
                                       R"({"area":[57.9851550599982,230,25.0917589855366,300],"grid":"O640"})",  // #45
                                       R"({"area":[69.9336077222471,350,36.0562248559007,391],"grid":"O640"})",
                                       R"({"area":[90,180,-90,540],"grid":"O640"})",
                                       R"(error: Grid description is wrong or inconsistent)",
                                       R"({"area":[43.9281488577447,91,21.0152267483818,143],"grid":"O640"})",
                                       R"({"area":[30.9957021787546,337,23.1237779083132,349],"grid":"O640"})",  // #50
                                       R"({"area":[-25.0917589855366,135,-46.880120286791,179],"grid":"O640"})",
                                       R"({"area":[69.9824223090677,340,15.0087870533333,540],"grid":"N1280"})",
                                       R"({"area":[69.9824223090677,340,15.0087870533333,420],"grid":"N1280"})",
                                       R"({"area":[57.9613339310375,-130,25.0615109127255,-60],"grid":"O1280"})",
                                       R"({"area":[9.94727573108748,350,0.0351493842156,360],"grid":"O1280"})",  // #55
                                       R"({"area":[90,180,-90,540],"grid":"O1280"})",
                                       R"({"area":[90,285,15.0087870533333,435],"grid":"O1280"})",
                                       R"({"area":[71.8804887530286,334,34.1300519836287,406],"grid":"O1280"})",
                                       R"({"area":[-24.9912121445865,135,-46.9947264210242,179],"grid":"O1280"})",
                                       R"({"area":[90,285,15.0087870533333,435],"grid":"O1280"})",  // #60
                                       R"({"area":[69.9824223090677,350,36.0281187134117,391],"grid":"O1280"})",
                                       R"({"area":[57.9613339310375,230,24.9912121445865,300],"grid":"O1280"})",
                                       R"({"area":[90,180.001,-90,540.001],"grid":"O1280"})",
                                       R"({"area":[0.456941994802685,6.75,-0.456941994802685,7.248],"grid":"O1280"})",
                                       R"({"area":[43.9718794382054,91,21.0544811260463,143],"grid":"O1280"})",  // #65
                                   },
                                   // the west is not of a grid point (set back as the westmost point)
                                   { 63 }));

    EXPECT(gridspec::check_decoded("gridspec/gridType=reduced_gg,N320.area.grib",
                                   { R"({"area":[4.91803128467449,75,-49.88288326186,180],"grid":"N320"})" }));

    EXPECT(gridspec::check_decoded("gridspec/gridType=reduced_rotated_gg.grib",
                                   {
                                       // the south pole at [0,-90] is not a rotation
                                       R"({"grid":"N80","projection":{"south_pole":[30,30],"type":"rotation"}})",  // #0
                                       R"({"grid":"N80"})",
                                       R"({"grid":"O80","projection":{"south_pole":[30,30],"type":"rotation"}})",
                                       R"({"grid":"O80"})",
                                       R"({"area":[27.9625200060835,0,-13.9110027017923,40],"grid":"N320","projection":{"south_pole":[-40,-22],"type":"rotation"}})",
                                   }));
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
