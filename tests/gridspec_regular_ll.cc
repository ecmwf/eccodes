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


// gridSpec of gridType=regular_ll and rotated_ll


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/spec/Custom.h"

#include "eccodes/geo/GribFromSpec.h"
#include "eccodes/geo/GribToSpec.h"

#include "gridspec.h"


using gridspec::decode_t;
using gridspec::encode_t;
using gridspec::keys_t;


using grid = gridspec::regular_t;


CASE("global")
{
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB1", R"({"grid":[1,1]})", "regular_ll", grid(360, 181, 90000, 0, -90000, 359000, { { "iDirectionIncrement", 1000 }, { "jDirectionIncrement", 1000 }, { "scanningMode", 0 } }) },
        { "GRIB2", R"({"grid":[1,1]})", "regular_ll", grid(360, 181, 90000000, 0, -90000000, 359000000, { { "iDirectionIncrement", 1000000 }, { "jDirectionIncrement", 1000000 }, { "scanningMode", 0 } }) },
        { "GRIB1", R"({"grid":[0.25,0.25]})", "regular_ll", grid(1440, 721, 90000, 0, -90000, 359750) },
        { "GRIB2", R"({"grid":[0.25,0.25]})", "regular_ll", grid(1440, 721, 90000000, 0, -90000000, 359750000) },
        { "GRIB1", R"({"grid":[0.7,0.7]})", "regular_ll", grid(515, 257, 89600, 0, -89600, 359800) },
        { "GRIB2", R"({"grid":[0.7,0.7]})", "regular_ll", grid(515, 257, 89600000, 0, -89600000, 359800000) },

        // different increments west-east and south-north
        { "GRIB1", R"({"grid":[2,4]})", "regular_ll", grid(180, 45, 88000, 0, -88000, 358000, { { "iDirectionIncrement", 2000 }, { "jDirectionIncrement", 4000 } }) },
        { "GRIB2", R"({"grid":[2,4]})", "regular_ll", grid(180, 45, 88000000, 0, -88000000, 358000000, { { "iDirectionIncrement", 2000000 }, { "jDirectionIncrement", 4000000 } }) },

        // shifted from the origin
        { "GRIB1", R"({"grid":[1,1],"reference":[0.5,0.5]})", "regular_ll", grid(360, 180, 89500, 500, -89500, 359500) },
        { "GRIB2", R"({"grid":[1,1],"reference":[0.5,0.5]})", "regular_ll", grid(360, 180, 89500000, 500000, -89500000, 359500000) },

        // increments not encodable in GRIB1 millidegrees (GRIB-863) are not given, but the grid is exact
        { "GRIB1", R"({"grid":[0.333333333333333,0.333333333333333]})", "regular_ll", grid(1080, 541, 90000, 0, -90000, 359667, { { "ijDirectionIncrementGiven", 0 } }) },
        { "GRIB2", R"({"grid":[0.333333333333333,0.333333333333333]})", "regular_ll", grid(1080, 541, 90000000, 0, -90000000, 359666667, { { "ijDirectionIncrementGiven", 1 }, { "iDirectionIncrement", 333333 }, { "jDirectionIncrement", 333333 } }) },
    }));
}


CASE("area")
{
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB1", R"({"area":[60,0,0,30],"grid":[2,2]})", "regular_ll", grid(16, 31, 60000, 0, 0, 30000) },
        { "GRIB2", R"({"area":[60,0,0,30],"grid":[2,2]})", "regular_ll", grid(16, 31, 60000000, 0, 0, 30000000) },

        // an area not on the grid is cropped to its points (the reference is the first point)
        { "GRIB1", R"({"area":[60.3,0.2,0.1,30.7],"grid":[1,1]})", "regular_ll", grid(31, 61, 60100, 200, 100, 30200), R"({"area":[60.1,0.2,0.1,30.2],"grid":[1,1],"reference":[0.2,0.1]})" },
        { "GRIB2", R"({"area":[60.3,0.2,0.1,30.7],"grid":[1,1]})", "regular_ll", grid(31, 61, 60100000, 200000, 100000, 30200000), R"({"area":[60.1,0.2,0.1,30.2],"grid":[1,1],"reference":[0.2,0.1]})" },

        // across the date line: GRIB1 encodes negative longitudes, GRIB2 does not (the same grid)
        { "GRIB1", R"({"area":[10,-10,-10,10],"grid":[1,1]})", "regular_ll", grid(21, 21, 10000, -10000, -10000, 10000) },
        { "GRIB2", R"({"area":[10,-10,-10,10],"grid":[1,1]})", "regular_ll", grid(21, 21, 10000000, 350000000, -10000000, 10000000), R"({"area":[10,350,-10,370],"grid":[1,1]})" },
        { "GRIB1", R"({"area":[10,350,-10,370],"grid":[1,1]})", "regular_ll", grid(21, 21, 10000, 350000, -10000, 370000) },
        { "GRIB2", R"({"area":[10,350,-10,370],"grid":[1,1]})", "regular_ll", grid(21, 21, 10000000, 350000000, -10000000, 10000000) },

        // global, from a west other than 0
        { "GRIB1", R"({"area":[90,-180,-90,180],"grid":[1,1]})", "regular_ll", grid(360, 181, 90000, -180000, -90000, 179000) },
        { "GRIB2", R"({"area":[90,-180,-90,180],"grid":[1,1]})", "regular_ll", grid(360, 181, 90000000, 180000000, -90000000, 179000000), R"({"area":[90,180,-90,540],"grid":[1,1]})" },

        // high resolution: GRIB1 increments are not given (GRIB-863)
        { "GRIB1", R"({"area":[1,0,0,1],"grid":[0.0005,0.0005]})", "regular_ll", grid(2001, 2001, 1000, 0, 0, 1000, { { "ijDirectionIncrementGiven", 0 } }) },
        { "GRIB2", R"({"area":[1,0,0,1],"grid":[0.0005,0.0005]})", "regular_ll", grid(2001, 2001, 1000000, 0, 0, 1000000, { { "iDirectionIncrement", 500 }, { "jDirectionIncrement", 500 } }) },
        { "GRIB2", R"({"area":[0.001,0,0,0.001],"grid":[0.0001,0.0001]})", "regular_ll", grid(11, 11, 1000, 0, 0, 1000, { { "iDirectionIncrement", 100 }, { "jDirectionIncrement", 100 } }) },

        // ECC-2318: a single row, column or point keeps the increment given, if any (0 otherwise)
        { "GRIB1", R"({"area":[60,0,60,30],"grid":[2,2]})", "regular_ll", grid(16, 1, 60000, 0, 60000, 30000, { { "jDirectionIncrement", 2000 } }) },
        { "GRIB2", R"({"area":[60,0,60,30],"grid":[2,2]})", "regular_ll", grid(16, 1, 60000000, 0, 60000000, 30000000, { { "jDirectionIncrement", 2000000 } }) },
        { "GRIB2", R"({"area":[60,0,0,0],"grid":[2,2]})", "regular_ll", grid(1, 31, 60000000, 0, 0, 0, { { "iDirectionIncrement", 2000000 } }) },
        { "GRIB2", R"({"area":[0,0,0,0],"grid":[1,1]})", "regular_ll", grid(1, 1, 0, 0, 0, 0) },
        { "GRIB2", R"({"area":[0,0,0,0],"grid":[0,0]})", "regular_ll", grid(1, 1, 0, 0, 0, 0, { { "iDirectionIncrement", 0 }, { "jDirectionIncrement", 0 } }) },
    }));
}


CASE("order")
{
    // the scanning mode: i/j directions (iScansNegatively, jScansPositively), j consecutive (jPointsAreConsecutive) and
    // alternating rows (alternativeRowScanning, GRIB2 only)
    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB1", R"({"grid":[1,1],"order":"i+j-"})", "regular_ll", grid(360, 181, 90000, 0, -90000, 359000, { { "scanningMode", 0 } }), R"({"grid":[1,1]})" },
        { "GRIB1", R"({"grid":[1,1],"order":"i+j+"})", "regular_ll", grid(360, 181, -90000, 0, 90000, 359000, { { "scanningMode", 64 } }) },
        { "GRIB1", R"({"grid":[1,1],"order":"i-j-"})", "regular_ll", grid(360, 181, 90000, 359000, -90000, 0, { { "scanningMode", 128 } }) },
        { "GRIB1", R"({"grid":[1,1],"order":"i-j+"})", "regular_ll", grid(360, 181, -90000, 359000, 90000, 0, { { "scanningMode", 192 } }) },
        { "GRIB1", R"({"grid":[1,1],"order":"j+i+"})", "regular_ll", grid(360, 181, -90000, 0, 90000, 359000, { { "scanningMode", 96 } }) },
        { "GRIB1", R"({"grid":[1,1],"order":"j-i+"})", "regular_ll", grid(360, 181, 90000, 0, -90000, 359000, { { "scanningMode", 32 } }) },

        { "GRIB2", R"({"grid":[1,1],"order":"i+j+"})", "regular_ll", grid(360, 181, -90000000, 0, 90000000, 359000000, { { "scanningMode", 64 } }) },
        { "GRIB2", R"({"grid":[1,1],"order":"i-j-"})", "regular_ll", grid(360, 181, 90000000, 359000000, -90000000, 0, { { "scanningMode", 128 } }) },
        { "GRIB2", R"({"grid":[1,1],"order":"i-j+"})", "regular_ll", grid(360, 181, -90000000, 359000000, 90000000, 0, { { "scanningMode", 192 } }) },
        { "GRIB2", R"({"grid":[1,1],"order":"j+i+"})", "regular_ll", grid(360, 181, -90000000, 0, 90000000, 359000000, { { "scanningMode", 96 } }) },
        { "GRIB2", R"({"grid":[1,1],"order":"j-i+"})", "regular_ll", grid(360, 181, 90000000, 0, -90000000, 359000000, { { "scanningMode", 32 } }) },
        { "GRIB2", R"({"grid":[1,1],"order":"i+-j+"})", "regular_ll", grid(360, 181, -90000000, 0, 90000000, 359000000, { { "scanningMode", 80 } }) },
        { "GRIB2", R"({"area":[20,20,17,24],"grid":[1,1],"order":"i-j-"})", "regular_ll", grid(5, 4, 20000000, 24000000, 17000000, 20000000, { { "scanningMode", 128 } }) },
    }));

    // alternating rows are GRIB2 only
    EXPECT(gridspec::check_all<gridspec::encode_error_t>({
        { "GRIB1", R"({"grid":[1,1],"order":"i+-j+"})", CODES_GEOCALCULUS_PROBLEM },
    }));

    SECTION("a message's alternating rows are unset")
    {
        auto h = gridspec::sample("GRIB2");
        EXPECT(gridspec::set_gridspec(h.get(), R"({"grid":[1,1],"order":"i+-j+"})") == CODES_SUCCESS);
        EXPECT_EQUAL(gridspec::get_long(h.get(), "alternativeRowScanning"), 1);
        EXPECT(gridspec::set_gridspec(h.get(), R"({"grid":[1,1],"order":"i+j+"})") == CODES_SUCCESS);
        EXPECT_EQUAL(gridspec::get_long(h.get(), "alternativeRowScanning"), 0);
        EXPECT_EQUAL(gridspec::get_gridspec(h.get()), R"({"grid":[1,1],"order":"i+j+"})");
    }

    SECTION("a message's scanning mode is replaced")
    {
        if (!gridspec::has_data("gridspec/gridType=regular_ll,scanningMode=96.grib")) {
            return;
        }

        const auto handles = gridspec::messages("gridspec/gridType=regular_ll,scanningMode=96.grib");
        auto h = gridspec::clone(handles.front().get());

        EXPECT_EQUAL(gridspec::get_long(h.get(), "scanningMode"), 96);
        EXPECT(gridspec::set_gridspec(h.get(), R"({"area":[1,0,0,2],"grid":[1,1]})") == CODES_SUCCESS);
        EXPECT_EQUAL(gridspec::get_long(h.get(), "scanningMode"), 0);
        EXPECT_EQUAL(gridspec::get_gridspec(h.get()), R"({"area":[1,0,0,2],"grid":[1,1]})");
    }
}


CASE("rotated")
{
    const keys_t pole{ { "latitudeOfSouthernPoleInDegrees", -40. }, { "longitudeOfSouthernPoleInDegrees", 30. } };

    EXPECT(gridspec::check_all<encode_t>({
        { "GRIB1", R"({"grid":[1,1],"projection":{"south_pole":[30,-40],"type":"rotation"}})", "rotated_ll", pole },
        { "GRIB2", R"({"grid":[1,1],"projection":{"south_pole":[30,-40],"type":"rotation"}})", "rotated_ll", pole },

        // the rotation of the grid about the rotated pole
        { "GRIB1", R"({"grid":[1,1],"projection":{"rotation_angle":15,"south_pole":[30,-40],"type":"rotation"}})", "rotated_ll", { { "angleOfRotationInDegrees", 15. } } },
        { "GRIB2", R"({"grid":[1,1],"projection":{"rotation_angle":15,"south_pole":[30,-40],"type":"rotation"}})", "rotated_ll", { { "angleOfRotationInDegrees", 15. }, { "numberOfDataPoints", 360 * 181 } } },
        { "GRIB2", R"({"grid":[1,1],"rotation":[-40,30],"rotation_angle":15})", "rotated_ll", { { "angleOfRotationInDegrees", 15. } }, R"({"grid":[1,1],"projection":{"rotation_angle":15,"south_pole":[30,-40],"type":"rotation"}})" },

        // the south pole as MARS (rotation=lat/lon)
        { "GRIB2", R"({"grid":[1,1],"rotation":[-40,30]})", "rotated_ll", pole, R"({"grid":[1,1],"projection":{"south_pole":[30,-40],"type":"rotation"}})" },

        // the south pole (unrotated) is not a rotation
        { "GRIB1", R"({"grid":[1,1],"projection":{"south_pole":[0,-90],"type":"rotation"}})", "regular_ll", {}, R"({"grid":[1,1]})" },
        { "GRIB2", R"({"grid":[1,1],"projection":{"south_pole":[0,-90],"type":"rotation"}})", "regular_ll", {}, R"({"grid":[1,1]})" },

        // the north pole is a rotation (GRIB2 longitudes are not negative)
        { "GRIB1", R"({"grid":[1,1],"projection":{"south_pole":[-90,0],"type":"rotation"}})", "rotated_ll", {} },
        { "GRIB2", R"({"grid":[1,1],"projection":{"south_pole":[-90,0],"type":"rotation"}})", "rotated_ll", {}, R"({"grid":[1,1],"projection":{"south_pole":[270,0],"type":"rotation"}})" },

        // an area of the rotated grid (first/last grid points in the rotated frame)
        { "GRIB1", R"({"area":[26.65,5.75,-13.25,30.45],"grid":[0.1,0.1],"projection":{"south_pole":[320,-22],"type":"rotation"}})", "rotated_ll", grid(248, 400, 26650, 5750, -13250, 30450), R"({"area":[26.65,5.75,-13.25,30.45],"grid":[0.1,0.1],"projection":{"south_pole":[320,-22],"type":"rotation"},"reference":[0.05,0.05]})" },
        { "GRIB2", R"({"area":[26.65,5.75,-13.25,30.45],"grid":[0.1,0.1],"projection":{"south_pole":[320,-22],"type":"rotation"}})", "rotated_ll", grid(248, 400, 26650000, 5750000, -13250000, 30450000), R"({"area":[26.65,5.75,-13.25,30.45],"grid":[0.1,0.1],"projection":{"south_pole":[320,-22],"type":"rotation"},"reference":[0.05,0.05]})" },
    }));
}


CASE("GRIB keys to gridSpec")
{
    const std::string OFFSET_BY_A_FRACTION = R"({"area":[60.1666666666667,0.166666666666667,0.166666666666667,60.1666666666667],"grid":[0.333333333333333,0.333333333333333],"reference":[0.166666666666667,0.166666666666667]})";

    // the sample GRIB2 is regular_ll, area [60,0,0,30], increments 2/2 degrees
    EXPECT(gridspec::check_all<decode_t>({
        { "GRIB2", {}, R"({"area":[60,0,0,30],"grid":[2,2]})" },

        // ECC-2318: a single row, column or point keeps the increment given, if any (0 otherwise)
        { "GRIB2", { { "Ni", 16 }, { "Nj", 1 }, { "numberOfDataPoints", 16 }, { "latitudeOfLastGridPointInDegrees", 60. } }, R"({"area":[60,0,60,30],"grid":[2,2]})" },
        { "GRIB2", { { "Ni", 1 }, { "Nj", 31 }, { "numberOfDataPoints", 31 }, { "longitudeOfLastGridPointInDegrees", 0. } }, R"({"area":[60,0,0,0],"grid":[2,2]})" },
        { "GRIB2", { { "Ni", 1 }, { "Nj", 1 }, { "numberOfDataPoints", 1 }, { "latitudeOfLastGridPointInDegrees", 60. }, { "longitudeOfLastGridPointInDegrees", 0. } }, R"({"area":[60,0,60,0],"grid":[2,2]})" },
        { "GRIB2", { { "Ni", 16 }, { "Nj", 1 }, { "numberOfDataPoints", 16 }, { "latitudeOfLastGridPointInDegrees", 60. }, { "ijDirectionIncrementGiven", 0 } }, R"({"area":[60,0,60,30],"grid":[2,0]})" },

        // increments not given are calculated from the first/last grid points
        { "GRIB2", { { "ijDirectionIncrementGiven", 0 } }, R"({"area":[60,0,0,30],"grid":[2,2]})" },

        // increments given are those calculated, to the angular precision: the grid is as by its first/last points
        { "GRIB2", { { "Ni", 1080 }, { "Nj", 541 }, { "numberOfDataPoints", 1080 * 541 }, { "latitudeOfFirstGridPointInDegrees", 90. }, { "longitudeOfFirstGridPointInDegrees", 0. }, { "latitudeOfLastGridPointInDegrees", -90. }, { "longitudeOfLastGridPointInDegrees", 359.666667 }, { "iDirectionIncrementInDegrees", 0.333333 }, { "jDirectionIncrementInDegrees", 0.333333 } }, R"({"grid":[0.333333333333333,0.333333333333333]})" },

        // a division of the globe given to a millidegree (GRIB1, 1/3 degree as 0.333, of the sample's 360x181 points) is exact
        // a grid offset by a fraction of a repeating fraction increment (1/6 of 1/3 degree), of the first/last points rounded
        // (outwards) to the angular precision, is of that offset: the same, of either edition or of a basic angle (of the
        // default precision, or exact)
        { "GRIB1", { { "Ni", 181 }, { "Nj", 181 }, { "latitudeOfFirstGridPoint", 60167 }, { "longitudeOfFirstGridPoint", 166 }, { "latitudeOfLastGridPoint", 166 }, { "longitudeOfLastGridPoint", 60167 }, { "iDirectionIncrement", 333 }, { "jDirectionIncrement", 333 } }, OFFSET_BY_A_FRACTION },
        { "GRIB2", { { "Ni", 181 }, { "Nj", 181 }, { "numberOfDataPoints", 181 * 181 }, { "latitudeOfFirstGridPoint", 60166667 }, { "longitudeOfFirstGridPoint", 166666 }, { "latitudeOfLastGridPoint", 166666 }, { "longitudeOfLastGridPoint", 60166667 }, { "iDirectionIncrement", 333333 }, { "jDirectionIncrement", 333333 } }, OFFSET_BY_A_FRACTION },
        { "GRIB2", { { "basicAngleOfTheInitialProductionDomain", 1 }, { "subdivisionsOfBasicAngle", 1000000 }, { "Ni", 181 }, { "Nj", 181 }, { "numberOfDataPoints", 181 * 181 }, { "latitudeOfFirstGridPoint", 60166667 }, { "longitudeOfFirstGridPoint", 166666 }, { "latitudeOfLastGridPoint", 166666 }, { "longitudeOfLastGridPoint", 60166667 }, { "iDirectionIncrement", 333333 }, { "jDirectionIncrement", 333333 } }, OFFSET_BY_A_FRACTION },
        { "GRIB2", { { "basicAngleOfTheInitialProductionDomain", 0 }, { "subdivisionsOfBasicAngle", 6 }, { "Ni", 181 }, { "Nj", 181 }, { "numberOfDataPoints", 181 * 181 }, { "latitudeOfFirstGridPoint", 361 }, { "longitudeOfFirstGridPoint", 1 }, { "latitudeOfLastGridPoint", 1 }, { "longitudeOfLastGridPoint", 361 }, { "iDirectionIncrement", 2 }, { "jDirectionIncrement", 2 } }, OFFSET_BY_A_FRACTION },

        // ... but not if further than the precision (as encoded)
        { "GRIB2", { { "Ni", 181 }, { "Nj", 181 }, { "numberOfDataPoints", 181 * 181 }, { "latitudeOfFirstGridPoint", 60166000 }, { "longitudeOfFirstGridPoint", 166000 }, { "latitudeOfLastGridPoint", 166000 }, { "longitudeOfLastGridPoint", 60166000 }, { "iDirectionIncrement", 333333 }, { "jDirectionIncrement", 333333 } }, R"({"area":[60.166,0.166,0.166,60.166],"grid":[0.333333333333333,0.333333333333333],"reference":[0.166,0.166]})" },

        { "GRIB1", { { "Ni", 360 }, { "Nj", 181 }, { "latitudeOfFirstGridPoint", 90000 }, { "longitudeOfFirstGridPoint", 0 }, { "latitudeOfLastGridPoint", 30000 }, { "longitudeOfLastGridPoint", 119667 }, { "iDirectionIncrement", 333 }, { "jDirectionIncrement", 333 } }, R"({"area":[90,0,30,119.666666666667],"grid":[0.333333333333333,0.333333333333333]})" },

        // an increment given exactly, of first/last points imprecise (by a few angular precisions), is the increment
        { "GRIB2", { { "Ni", 2560 }, { "Nj", 1920 }, { "numberOfDataPoints", 2560 * 1920 }, { "jScansPositively", 1 }, { "latitudeOfFirstGridPoint", -89953128 }, { "longitudeOfFirstGridPoint", 70312 }, { "latitudeOfLastGridPoint", 89953128 }, { "longitudeOfLastGridPoint", 359929696 }, { "iDirectionIncrement", 140625 }, { "jDirectionIncrement", 93750 } }, R"({"grid":[0.140625,0.09375],"order":"i+j+","reference":[0.0703125,0.046872]})" },

        // an increment given inconsistently with the first/last points (warning) is calculated from them
        { "GRIB2", { { "iDirectionIncrement", 3000000 } }, R"({"area":[60,0,0,30],"grid":[2,2]})" },

        // longitudes around the globe are periodic (to 3/2 the angular precision), of the exact increment
        { "GRIB2", { { "Ni", 1024 }, { "Nj", 31 }, { "numberOfDataPoints", 1024 * 31 }, { "longitudeOfLastGridPointInDegrees", 359.648438 }, { "iDirectionIncrementInDegrees", 0.351562 } }, R"({"area":[60,0,0,360],"grid":[0.3515625,2]})" },

        // the scanning mode
        { "GRIB2", { { "jScansPositively", 1 }, { "latitudeOfFirstGridPointInDegrees", 0. }, { "latitudeOfLastGridPointInDegrees", 60. } }, R"({"area":[60,0,0,30],"grid":[2,2],"order":"i+j+"})" },
        { "GRIB2", { { "iScansNegatively", 1 }, { "longitudeOfFirstGridPointInDegrees", 30. }, { "longitudeOfLastGridPointInDegrees", 0. } }, R"({"area":[60,0,0,30],"grid":[2,2],"order":"i-j-"})" },
        { "GRIB2", { { "jPointsAreConsecutive", 1 } }, R"({"area":[60,0,0,30],"grid":[2,2],"order":"j-i+"})" },

        // inconsistent
        { "GRIB2", { { "numberOfDataPoints", 1 } }, "error: Problem with calculation of geographic attributes" },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=regular_ll.grib",
                                   {
                                       R"({"area":[1,0,0,1],"grid":[1,1]})",
                                       R"({"area":[2,0,0,2],"grid":[2,2]})",
                                       R"({"area":[1,0,0,1],"grid":[1,1]})",
                                       R"({"area":[1,0,0,1],"grid":[0.5,0.5]})",
                                       R"({"area":[1,0,0,1],"grid":[0.5,0.5]})",
                                       R"({"area":[90,0,84,6],"grid":[2,2]})",
                                       R"({"area":[3,0,0,3],"grid":[1,1]})",
                                       R"({"grid":[10,10]})",
                                       R"({"grid":[10,10]})",
                                       R"({"grid":[9,9]})",
                                       R"({"grid":[9,9]})",
                                       R"({"grid":[9,9]})",
                                       R"({"area":[69,-60,21,60],"grid":[3,3]})",
                                       R"({"area":[19,6,-11,366],"grid":[5,5],"reference":[1,4]})",
                                       R"({"area":[72,325,24,434],"grid":[1,1]})",
                                       R"({"grid":[3,3]})",
                                       R"({"area":[90,0,-78,360],"grid":[3,3]})",
                                       R"({"area":[75,-65,15,65],"grid":[1,1]})",
                                       R"({"area":[47.1,-12.9,33,8.1],"grid":[0.15,0.15]})",
                                       R"({"area":[73.5,-27,33,45],"grid":[0.5,0.5]})",
                                       R"({"grid":[2,4]})",
                                       R"({"grid":[2,2]})",
                                       R"({"grid":[2,2]})",
                                       R"({"area":[62.375,-12,48.042,13],"grid":[0.125,0.0833313953488372],"order":"j+i+","reference":[0,0.0431162790697674]})",
                                       R"({"area":[62.333,-11.9375,48,13.0625],"grid":[0.125,0.0833313953488372],"order":"j+i+","reference":[0.0625,0.00111627906976744]})",
                                       R"({"grid":[1.5,1.5]})",
                                       R"({"grid":[1.40625,0.462724935732648],"reference":[0,0.231362467866324]})",
                                       R"({"grid":[1.25,0.833333333333333],"order":"i+j+"})",
                                       R"({"area":[90,0,-78,360],"grid":[1,1]})",
                                       R"({"grid":[1,1],"reference":[0.5,0.5]})",
                                       R"({"grid":[1,1]})",
                                       R"({"area":[73,-27,33,45],"grid":[0.2,0.2]})",
                                       R"({"area":[73,-27,33,45],"grid":[0.1,0.1]})",
                                       R"({"grid":[0.45,0.3],"order":"i+j+","reference":[0.225,0.15]})",
                                       R"({"grid":[0.45,0.3],"order":"i+j+"})",
                                       R"({"area":[80.0390625,0.175781,-80.0390625,359.823707],"grid":[0.351562,0.234375],"order":"i+j+","reference":[0.175781,0.1171875]})",
                                       R"({"area":[80.0390625,0,-80.0390625,360],"grid":[0.3515625,0.234375],"order":"i+j+","reference":[0.17578125,0.1171875]})",
                                       R"({"area":[89.55,0,-90,360],"grid":[0.35,0.34931906614786],"reference":[0,0.12431906614786]})",
                                       R"({"area":[90,-179.875,-90,180.125],"grid":[0.25,0.25],"order":"i+j+","reference":[0.125,0.125]})",
                                       R"({"grid":[0.1,0.1],"reference":[0.05,0.05]})",
                                       R"({"grid":[0.00833333333333333,0.00833333333333333],"reference":[0.00416666666666667,0.00416666666666667]})",
                                       R"({"area":[73,-27,33,45],"grid":[4,4],"reference":[1,1]})",
                                       R"({"area":[10,359.5,-10,377.5],"grid":[0.5,0.5]})",
                                       R"({"area":[90,182.5,-90,542.5],"grid":[5,5],"reference":[2.5,2.5]})",
                                       R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0.9375,0.625]})",
                                       R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0,0.625]})",
                                       R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0.9375,0]})",
                                       R"({"grid":[1.25,0.833333333333333],"order":"i+j+","reference":[0.625,0.416666666666667]})",
                                       R"({"grid":[0.5625,0.375],"order":"i+j+","reference":[0.28125,0.1875]})",
                                       R"({"grid":[0.140625,0.09375],"order":"i+j+","reference":[0.0703125,0.046872]})",
                                       R"({"area":[20,20,17,24],"grid":[1,1]})",
                                       R"({"area":[20,20,17,24],"grid":[1,1],"order":"i-j-"})",
                                       R"({"area":[20,20,17,24],"grid":[1,1],"order":"i+j+"})",
                                   }));

    EXPECT(gridspec::check_decoded("gridspec/gridType=rotated_ll.grib",
                                   {
                                       // the south pole at [0,-90] is not a rotation
                                       R"({"area":[90,0,-78,360],"grid":[3,3]})",
                                       R"({"area":[69,-60,21,60],"grid":[3,3]})",
                                       R"({"area":[90,0,-78,360],"grid":[3,3],"projection":{"south_pole":[30,30],"type":"rotation"}})",
                                       R"({"area":[69,-60,21,60],"grid":[3,3],"projection":{"south_pole":[30,30],"type":"rotation"}})",
                                       R"({"area":[5.5,356.2,-8.5,367.7],"grid":[0.02,0.02],"order":"i+j+","projection":{"south_pole":[10,-47],"type":"rotation"}})",
                                       R"({"grid":[3,3]})",
                                       R"({"grid":[3,3],"projection":{"south_pole":[30,30],"type":"rotation"}})",
                                   }));

    EXPECT(gridspec::check_decoded("gridspec/gridType=regular_ll,scanningMode=96.grib", { R"({"area":[1,0,0,2],"grid":[1,1],"order":"j+i+"})" }));
    EXPECT(gridspec::check_decoded("gridspec/gridType=regular_ll,isECMWFPostGRIB2MigrationMessage=1.grib", { R"({"area":[0,0,0,0],"grid":[1,1]})" }));
}


CASE("GRIB to gridSpec, set on the message")
{
    // a gridSpec set on a message of another grid (ECC-2318: a single row or column keeps its increment), as decoded
    if (!gridspec::has_data("gridspec/gridType=regular_ll,isECMWFPostGRIB2MigrationMessage=1.grib")) {
        return;
    }

    const auto handles = gridspec::messages("gridspec/gridType=regular_ll,isECMWFPostGRIB2MigrationMessage=1.grib");

    for (const auto& [user, spec, shape] : std::vector<std::tuple<std::string, std::string, std::vector<size_t>>>{
             { "{grid:[0,0]}", R"({"area":[90,0,90,0],"grid":[0,0]})", { 1, 1 } },
             { "{area:[0,0,0,0], grid:[0,0]}", R"({"area":[0,0,0,0],"grid":[0,0]})", { 1, 1 } },
             { "{area:[0,0,0,0], grid:[1,1]}", R"({"area":[0,0,0,0],"grid":[1,1]})", { 1, 1 } },
             { "{area:[60,0,60,30], grid:[2,2]}", R"({"area":[60,0,60,30],"grid":[2,2]})", { 1, 16 } },
             { "{area:[60,0,0,0], grid:[2,2]}", R"({"area":[60,0,0,0],"grid":[2,2]})", { 31, 1 } },
             { "{area:[60,0,0,30], grid:[2,2]}", R"({"area":[60,0,0,30],"grid":[2,2]})", { 31, 16 } },
         }) {
        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(user));
        EXPECT(grid->shape() == shape);

        auto h = gridspec::clone(handles.front().get());
        EXPECT(gridspec::set_gridspec(h.get(), user) == CODES_SUCCESS);
        EXPECT_EQUAL(gridspec::get_gridspec(h.get()), spec);
    }
}


CASE("scanningMode=96 coordinates")
{
    if (!gridspec::has_data("gridspec/gridType=regular_ll,scanningMode=96.grib")) {
        return;
    }

    const auto handles = gridspec::messages("gridspec/gridType=regular_ll,scanningMode=96.grib");
    auto* h = handles.front().get();

    size_t n = 0;
    EXPECT(codes_get_size(h, "values", &n) == CODES_SUCCESS && n == 6);

    std::vector<double> values(n);
    EXPECT(codes_get_double_array(h, "values", values.data(), &n) == CODES_SUCCESS);

    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(eccodes::geo::GribToSpec(h)));
    EXPECT(grid->order() == "j+i+");

    // columns first, south to north
    const auto [lats, lons] = grid->to_latlons();
    const std::vector<double> lats_ref{ 0, 1, 0, 1, 0, 1 };
    const std::vector<double> lons_ref{ 0, 0, 1, 1, 2, 2 };
    for (size_t i = 0; i < n; ++i) {
        EXPECT(eckit::types::is_approximately_equal(values[i], static_cast<double>(i), 1e-6));
        EXPECT(eckit::types::is_approximately_equal(lats[i], lats_ref[i], eckit::geo::PointLonLat::EPS));
        EXPECT(eckit::types::is_approximately_equal(lons[i], lons_ref[i], eckit::geo::PointLonLat::EPS));
    }
}


CASE("GribFromSpec: extra keys")
{
    // keys set with the geometry (not part of the gridSpec)
    for (const auto* name : { "GRIB1", "GRIB2" }) {
        auto h = gridspec::sample(name);
        auto* result = eccodes::geo::GribFromSpec::set(h.get(), eckit::spec::Custom{ { "grid", "1/1" } }, { { "paramId", 167 } });
        EXPECT(result == h.get());

        EXPECT_EQUAL(gridspec::get_long(result, "paramId"), 167);
        EXPECT_EQUAL(gridspec::get_gridspec(result), R"({"grid":[1,1]})");
    }
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
