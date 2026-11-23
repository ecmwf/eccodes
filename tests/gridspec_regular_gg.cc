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


// gridSpec of gridType=regular_gg and rotated_gg


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"

#include "eccodes/geo/GribToSpec.h"

#include "gridspec.h"


using gridspec::decode_t;
using gridspec::encode_error_t;
using gridspec::encode_t;

using grid = gridspec::regular_t;


// Gaussian latitudes are encoded rounded outwards (GRIB1 millidegrees, GRIB2 microdegrees): the first latitude of F48
// is 88.5721685, encoded as 88.573 or 88.572169


CASE("global")
{
    EXPECT(gridspec::check_all<encode_t>({
        { "regular_gg_sfc_grib1", R"({"grid":"F48"})", "regular_gg", grid(192, 96, 88573, 0, -88573, 358125, { { "N", 48 }, { "iDirectionIncrement", 1875 } }) },
        { "regular_gg_sfc_grib2", R"({"grid":"F48"})", "regular_gg", grid(192, 96, 88572169, 0, -88572169, 358125000, { { "N", 48 }, { "iDirectionIncrement", 1875000 } }) },
        { "regular_gg_sfc_grib1", R"({"grid":"F1280"})", "regular_gg", grid(5120, 2560, 89947, 0, -89947, 359930) },
        { "regular_gg_sfc_grib2", R"({"grid":"F1280"})", "regular_gg", grid(5120, 2560, 89946188, 0, -89946188, 359929688) },
        { "regular_gg_sfc_grib1", R"({"grid":"F2"})", "regular_gg", grid(8, 4, 59445, 0, -59445, 315000) },
        { "regular_gg_sfc_grib2", R"({"grid":"F1"})", "regular_gg", grid(4, 2, 35264390, 0, -35264390, 270000000, { { "iDirectionIncrement", 90000000 } }) },

        // other than 4N longitudes (the increment is of these)
        { "regular_gg_sfc_grib1", R"({"grid":"F95","ni":384})", "regular_gg", grid(384, 190, 89277, 0, -89277, 359063, { { "iDirectionIncrement", 938 } }) },
        { "regular_gg_sfc_grib2", R"({"grid":"F95","ni":384})", "regular_gg", grid(384, 190, 89276713, 0, -89276713, 359062500, { { "iDirectionIncrement", 937500 } }) },

        // set on a message of another grid
        { "GRIB1", R"({"grid":"F48"})", "regular_gg", grid(192, 96, 88573, 0, -88573, 358125) },
        { "GRIB2", R"({"grid":"F48"})", "regular_gg", grid(192, 96, 88572169, 0, -88572169, 358125000) },
    }));

    // GRIB1 encodes increments up to 65.535 degrees
    EXPECT(gridspec::check_all<encode_error_t>({
        { "regular_gg_sfc_grib1", R"({"grid":"F1"})", CODES_GEOCALCULUS_PROBLEM },
    }));
}


CASE("area")
{
    EXPECT(gridspec::check_all<encode_t>({
        // the area is cropped to the grid's points
        { "regular_gg_sfc_grib1", R"({"area":[60,-10,0,30],"grid":"F48"})", "regular_gg", grid(22, 32, 58756, -9375, 932, 30000), R"({"area":[58.7552092693799,-9.375,0.932629967838,30],"grid":"F48"})" },
        { "regular_gg_sfc_grib2", R"({"area":[60,-10,0,30],"grid":"F48"})", "regular_gg", grid(22, 32, 58755210, 350625000, 932629, 30000000), R"({"area":[58.7552092693799,350.625,0.932629967838,390],"grid":"F48"})" },

        // hemispheres, a band, and a column
        { "regular_gg_sfc_grib2", R"({"area":[90,0,0,360],"grid":"F48"})", "regular_gg", grid(192, 48, 88572169, 0, 932629, 358125000), R"({"area":[90,0,0.932629967838,360],"grid":"F48"})" },
        { "regular_gg_sfc_grib2", R"({"area":[0,0,-90,360],"grid":"F48"})", "regular_gg", grid(192, 48, -932629, 0, -88572169, 358125000), R"({"area":[-0.932629967838,0,-90,360],"grid":"F48"})" },
        { "regular_gg_sfc_grib2", R"({"area":[10,0,-10,360],"grid":"F48"})", "regular_gg", grid(192, 10, 8393669, 0, -8393669, 358125000), R"({"area":[8.39366890769238,0,-8.39366890769238,360],"grid":"F48"})" },
        { "regular_gg_sfc_grib2", R"({"area":[0,0,-90,0],"grid":"F48"})", "regular_gg", grid(1, 48, -932629, 0, -88572169, 0), R"({"area":[-0.932629967838,0,-90,0],"grid":"F48"})" },

        // of other than 4N longitudes
        { "regular_gg_sfc_grib1", R"({"area":[30,10,-30,50],"grid":"F95","ni":384})", "regular_gg", grid(43, 64, 29764, 10312, -29764, 49688), R"({"area":[29.7636666711173,10.3125,-29.7636666711173,49.6875],"grid":"F95","ni":384})" },
        { "regular_gg_sfc_grib2", R"({"area":[30,10,-30,50],"grid":"F95","ni":384})", "regular_gg", grid(43, 64, 29763667, 10312500, -29763667, 49687500), R"({"area":[29.7636666711173,10.3125,-29.7636666711173,49.6875],"grid":"F95","ni":384})" },
    }));
}


CASE("order")
{
    // the first/last grid points are rounded outwards, in the scanning directions
    EXPECT(gridspec::check_all<encode_t>({
        { "regular_gg_sfc_grib1", R"({"grid":"F48","order":"i+j+"})", "regular_gg", grid(192, 96, -88573, 0, 88573, 358125, { { "scanningMode", 64 } }) },
        { "regular_gg_sfc_grib1", R"({"grid":"F48","order":"i-j-"})", "regular_gg", grid(192, 96, 88573, 358125, -88573, 0, { { "scanningMode", 128 } }) },
        { "regular_gg_sfc_grib1", R"({"grid":"F48","order":"j+i+"})", "regular_gg", grid(192, 96, -88573, 0, 88573, 358125, { { "scanningMode", 96 } }) },
        { "regular_gg_sfc_grib2", R"({"grid":"F48","order":"i+j+"})", "regular_gg", grid(192, 96, -88572169, 0, 88572169, 358125000, { { "scanningMode", 64 } }) },
        { "regular_gg_sfc_grib2", R"({"grid":"F48","order":"i-j-"})", "regular_gg", grid(192, 96, 88572169, 358125000, -88572169, 0, { { "scanningMode", 128 } }) },
        { "regular_gg_sfc_grib2", R"({"grid":"F48","order":"j+i+"})", "regular_gg", grid(192, 96, -88572169, 0, 88572169, 358125000, { { "scanningMode", 96 } }) },
        { "regular_gg_sfc_grib2", R"({"grid":"F48","order":"i+-j+"})", "regular_gg", grid(192, 96, -88572169, 0, 88572169, 358125000, { { "scanningMode", 80 } }) },
        { "regular_gg_sfc_grib1", R"({"area":[60,-10,0,30],"grid":"F48","order":"i+j+"})", "regular_gg", grid(22, 32, 932, -9375, 58756, 30000, { { "scanningMode", 64 } }), R"({"area":[58.7552092693799,-9.375,0.932629967838,30],"grid":"F48","order":"i+j+"})" },
        { "regular_gg_sfc_grib2", R"({"area":[60,-10,0,30],"grid":"F48","order":"i+j+"})", "regular_gg", grid(22, 32, 932629, 350625000, 58755210, 30000000, { { "scanningMode", 64 } }), R"({"area":[58.7552092693799,350.625,0.932629967838,390],"grid":"F48","order":"i+j+"})" },
    }));
}


CASE("rotated")
{
    const gridspec::keys_t pole{ { "latitudeOfSouthernPoleInDegrees", -40. }, { "longitudeOfSouthernPoleInDegrees", 30. } };

    EXPECT(gridspec::check_all<encode_t>({
        { "regular_gg_sfc_grib1", R"({"grid":"F48","projection":{"south_pole":[30,-40],"type":"rotation"}})", "rotated_gg", pole },
        { "regular_gg_sfc_grib2", R"({"grid":"F48","projection":{"south_pole":[30,-40],"type":"rotation"}})", "rotated_gg", pole },
        { "rotated_gg_sfc_grib2", R"({"grid":"F48","projection":{"south_pole":[30,-40],"type":"rotation"}})", "rotated_gg", pole },

        // an area of the rotated grid (first/last grid points in the rotated frame)
        { "regular_gg_sfc_grib2", R"({"area":[60,-10,0,30],"grid":"F48","projection":{"south_pole":[30,-40],"type":"rotation"}})", "rotated_gg", grid(22, 32, 58755210, 350625000, 932629, 30000000), R"({"area":[58.7552092693799,350.625,0.932629967838,390],"grid":"F48","projection":{"south_pole":[30,-40],"type":"rotation"}})" },

        // the rotation of the grid about the rotated pole
        { "regular_gg_sfc_grib2", R"({"grid":"F48","projection":{"rotation_angle":15,"south_pole":[30,-40],"type":"rotation"}})", "rotated_gg", { { "angleOfRotationInDegrees", 15. }, { "numberOfDataPoints", 18432 } } },

        // the south pole (unrotated) is not a rotation
        { "rotated_gg_sfc_grib2", R"({"grid":"F48","projection":{"south_pole":[0,-90],"type":"rotation"}})", "regular_gg", {}, R"({"grid":"F48"})" },
    }));
}


CASE("GRIB keys to gridSpec")
{
    // the sample regular_gg_sfc_grib2 is F32, global
    EXPECT(gridspec::check_all<decode_t>({
        { "regular_gg_sfc_grib2", {}, R"({"grid":"F32"})" },
        { "rotated_gg_sfc_grib2", {}, R"({"grid":"F32","projection":{"south_pole":[0,0],"type":"rotation"}})" },

        // Gaussian latitudes are matched to a millidegree at least (inwards too, as GRIB1 messages converted to GRIB2)
        { "regular_gg_sfc_grib2", { { "latitudeOfFirstGridPoint", 87864000 }, { "latitudeOfLastGridPoint", -87864000 } }, R"({"grid":"F32"})" },
        { "regular_gg_sfc_grib2", { { "latitudeOfFirstGridPoint", 87863000 }, { "latitudeOfLastGridPoint", -87863000 } }, R"({"grid":"F32"})" },

        // longitudes are matched to the grid's, to a millidegree at least
        { "regular_gg_sfc_grib2", { { "longitudeOfLastGridPoint", 357187000 } }, R"({"grid":"F32"})" },

        // other than 4N longitudes, by the increment
        { "regular_gg_sfc_grib2", { { "Ni", 120 }, { "numberOfDataPoints", 120 * 64 }, { "iDirectionIncrement", 3000000 }, { "longitudeOfLastGridPoint", 357000000 } }, R"({"grid":"F32","ni":120})" },

        // the scanning mode
        { "regular_gg_sfc_grib2", { { "jScansPositively", 1 }, { "latitudeOfFirstGridPoint", -87863799 }, { "latitudeOfLastGridPoint", 87863799 } }, R"({"grid":"F32","order":"i+j+"})" },
        { "regular_gg_sfc_grib2", { { "iScansNegatively", 1 }, { "longitudeOfFirstGridPoint", 357187500 }, { "longitudeOfLastGridPoint", 0 } }, R"({"grid":"F32","order":"i-j-"})" },
    }));
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=regular_gg.grib",
                                   {
                                       R"({"area":[69.9460806469834,-60,21.4504750373982,60],"grid":"F48"})",
                                       R"({"area":[90,0,-77.4058880820788,360],"grid":"F48"})",
                                       R"({"area":[26.1408367776387,0,-26.1408367776387,360],"grid":"F60"})",
                                       R"({"area":[68.9712395389358,-59.625,20.747558233616,59.625],"grid":"F80"})",
                                       R"({"area":[90,0,-77.9426242466729,360],"grid":"F80"})",
                                       R"({"area":[75.1346965469112,-64.96875,15.1112834749574,64.96875],"grid":"F640"})",
                                       R"({"area":[-89.8898295555062,0,-90,360],"grid":"F2250"})",
                                       R"({"grid":"F8"})",
                                       R"({"grid":"F32"})",
                                       R"({"grid":"F48"})",
                                       R"({"grid":"F80"})",
                                       R"({"grid":"F95","ni":384})",  // Ni=384, not 4N
                                       R"({"grid":"F640"})",
                                   }));

    EXPECT(gridspec::check_decoded("gridspec/gridType=rotated_gg.grib",
                                   {
                                       R"({"grid":"F48","projection":{"south_pole":[30,30],"type":"rotation"}})",
                                       R"({"grid":"F48"})",
                                       R"({"grid":"F80"})",
                                       R"({"grid":"F80","projection":{"south_pole":[30,30],"type":"rotation"}})",
                                   }));
}


CASE("rotated coordinates")
{
    if (!gridspec::has_data("gridspec/gridType=rotated_gg.grib")) {
        return;
    }

    // first points of the first message (F48, south pole at [30,30])
    const std::vector<eckit::geo::PointLonLat> points_ref{
        { -150.00000000000000, -28.57216851400726 },
        { -150.05319064425672, -28.57292230625801 },
        { -150.10632666115495, -28.57518290821670 },
        { -150.15935347266884, -28.57894799620847 },
        { -150.21221659941676, -28.58421369979395 },
        { -150.26486170993135, -28.59097460529629 },
        { -150.31723466986631, -28.59922376073626 },
        { -150.36928159111906, -28.60895268217335 },
        { -150.42094888084813, -28.62015136144922 },
        { -150.47218329036292, -28.63280827532991 },
    };

    const auto handles = gridspec::messages("gridspec/gridType=rotated_gg.grib");
    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(eccodes::geo::GribToSpec(handles.front().get())));

    const auto points = grid->to_points();
    for (size_t i = 0; i < points_ref.size(); ++i) {
        EXPECT(eckit::geo::points_equal(points[i], points_ref[i], 1e-6));
    }
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
