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


// gridSpec figure (GRIB2 shapeOfTheEarth, GRIB1 earthIsOblate), of any grid type; the figure of projected grids is part
// of their geometry, see gridspec_lambert.cc and others


#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Grid.h"
#include "eckit/spec/Custom.h"

#include "eccodes/geo/GribToSpec.h"

#include "gridspec.h"


CASE("shapeOfTheEarth")
{
    // GRIB2 Code table 3.2 (shape of the reference system) round trip:
    //   gridSpec (with a figure) -> GRIB -> gridSpec, entirely in memory

    struct test_t
    {
        const std::string figure;    // gridSpec "figure" value, empty for none (the default figure)
        const long shapeOfTheEarth;  // as encoded from the gridSpec above
        const std::string back;      // gridSpec "figure" value, as decoded back from the GRIB
        const double a;              // eckit::geo::Figure built back from the GRIB
        const double b;
        const long shapeOfTheEarth_back;  // as re-encoded from the gridSpec decoded back
    };

    // NOTE: figures "earth" (shapeOfTheEarth=6) and "grib1" (shapeOfTheEarth=0) are both default; "grib1" is not reachable from a gridSpec
    for (const auto& test : std::vector<test_t>{
             { R"("grib1")", 6, "", 6371229., 6371229., 6 },
             { R"({"r":6000000})", 1, R"({"r":6000000})", 6000000., 6000000., 1 },
             { R"("iau1965")", 2, R"("iau1965")", 6378160., 6356775., 2 },
             { R"({"a":6378137,"b":6356752})", 7, R"({"a":6378137,"b":6356752})", 6378137., 6356752., 7 },
             { R"("grs80")", 4, R"("grs80")", 6378137., 6356752.314140, 4 },
             { R"("wgs84")", 5, R"("wgs84")", 6378137., 6356752.314245, 5 },
             { "", 6, "", 6371229., 6371229., 6 },
             { R"("wgs84_sphere")", 8, R"("wgs84_sphere")", 6371200., 6371200., 8 },
             { R"("sun")", 11, R"("sun")", 695990000., 695990000., 11 },
         }) {
        auto gridspec_with = [](const std::string& figure) {
            return figure.empty() ? R"({"area":[60,0,0,30],"grid":[2,2]})"
                                  : R"({"area":[60,0,0,30],"figure":)" + figure + R"(,"grid":[2,2]})";
        };

        auto figure_spec = [](const std::string& figure) {
            return R"({"figure":)" + (figure.empty() ? R"("earth")" : figure) + "}";
        };

        const auto user = gridspec_with(test.figure);

        auto h = gridspec::sample("regular_ll_sfc_grib2");
        EXPECT(gridspec::set_gridspec(h.get(), user) == CODES_SUCCESS);

        long shapeOfTheEarth = -1;
        ASSERT(CODES_SUCCESS == codes_get_long(h.get(), "shapeOfTheEarth", &shapeOfTheEarth));

        EXPECT_EQUAL(shapeOfTheEarth, test.shapeOfTheEarth);

        // the figure is only part of the gridSpec if it isn't a default figure
        const auto gridSpec = gridspec::get_gridspec(h.get());
        EXPECT_EQUAL(gridSpec, gridspec_with(test.back));

        // the decoded gridSpec builds a grid carrying that same figure...
        std::unique_ptr<const ::eckit::geo::Grid> grid(::eckit::geo::GridFactory::make_from_string(gridSpec));
        ASSERT(grid);

        std::unique_ptr<const ::eckit::spec::Custom> spec(grid->figure().spec());
        EXPECT_EQUAL(spec->str(), figure_spec(test.back));

        EXPECT(::eckit::types::is_approximately_equal(grid->figure().a(), test.a));
        EXPECT(::eckit::types::is_approximately_equal(grid->figure().b(), test.b));

        // ...which re-encodes to the same shape of the reference system
        auto h2 = gridspec::sample("regular_ll_sfc_grib2");
        EXPECT(gridspec::set_gridspec(h2.get(), grid->spec_str()) == CODES_SUCCESS);

        long shapeOfTheEarth2 = -1;
        ASSERT(CODES_SUCCESS == codes_get_long(h2.get(), "shapeOfTheEarth", &shapeOfTheEarth2));

        EXPECT_EQUAL(shapeOfTheEarth2, test.shapeOfTheEarth_back);
    }
}


CASE("figure")
{
    struct test_t
    {
        const std::string path;
        const std::vector<std::pair<size_t, std::string>> figures;  // [(count, figure), (count, figure), ...]
    };

    for (const auto& test : std::vector<test_t>{
             { "gridType=healpix.grib", { { 4, R"("grib1")" }, { 1, R"("earth")" } } },
             { "gridType=reduced_gg,N320.area.grib", { { 1, R"("earth")" } } },
             { "gridType=reduced_ll.grib", { { 4, R"("grib1")" } } },
             { "gridType=reduced_rotated_gg.grib", { { 5, R"("grib1")" } } },
             { "gridType=regular_gg.grib", { { 7, R"("grib1")" }, { 1, R"("earth")" }, { 5, R"("grib1")" } } },
             { "gridType=regular_ll,isECMWFPostGRIB2MigrationMessage=1.grib", { { 1, R"("grib1")" } } },
             { "gridType=regular_ll,scanningMode=96.grib", { { 1, R"("earth")" } } },
             { "gridType=regular_ll.grib", { { 2, R"("grib1")" }, { 1, R"("earth")" }, { 1, R"("grib1")" }, { 1, R"("earth")" }, { 1, R"("grib1")" }, { 1, R"("earth")" }, { 1, R"("grib1")" }, { 1, R"("earth")" }, { 2, R"("grib1")" }, { 1, R"("earth")" }, { 2, R"("grib1")" }, { 1, R"("earth")" }, { 7, R"("grib1")" }, { 1, R"("earth")" }, { 2, R"("grib1")" }, { 1, R"("earth")" }, { 1, R"("grib1")" }, { 1, R"("earth")" }, { 7, R"("grib1")" }, { 2, R"("earth")" }, { 5, R"("grib1")" }, { 1, R"("earth")" }, { 1, R"("grib1")" }, { 3, R"({"R":6371229})" }, { 1, R"("earth")" }, { 2, R"({"R":6371229})" }, { 3, R"("grib1")" } } },
             { "gridType=rotated_gg.grib", { { 4, R"("grib1")" } } },
             { "gridType=sh.grib", { { 1, R"("grib1")" } } },
             { "gridType=unstructured_grid,icon.grib", { { 1, R"("earth")" } } },
             { "gridType=unstructured_grid,orca.grib", { { 6, R"("earth")" } } },
         }) {
        std::vector<std::string> figures;
        for (const auto& [n, figure] : test.figures) {
            figures.insert(figures.end(), n, figure);
        }

        if (!gridspec::has_data("gridspec/" + test.path)) {
            continue;
        }

        size_t count = 0;
        for (const auto& handle : gridspec::messages("gridspec/" + test.path)) {
            ASSERT(count < figures.size());
            auto* h = handle.get();
            const auto& figure_expected = figures[count++];

            std::unique_ptr<const ::eckit::geo::Figure> expected(
                ::eckit::geo::FigureFactory::make_from_string(R"({"figure":)" + figure_expected + "}"));
            ASSERT(expected);

            // the figure, as decoded from the GRIB
            long edition = 0;
            EXPECT(codes_get_long(h, "edition", &edition) == CODES_SUCCESS);

            eccodes::geo::GribToSpec spec(h);
            EXPECT(spec.has("figure"));

            std::unique_ptr<const ::eckit::geo::Figure> figure(::eckit::geo::FigureFactory::build(spec));
            ASSERT(figure);

            EXPECT(*figure == *expected);
            EXPECT(edition == 2 || figure->is_default());

            // the figure, as part of the gridSpec (only if not a default figure)
            const auto gridSpec = gridspec::get_gridspec(h);
            const bool has_figure = gridSpec.find(R"("figure":)") != std::string::npos;
            EXPECT(has_figure == !expected->is_default());

            if (has_figure) {
                std::unique_ptr<const ::eckit::geo::Grid> grid(::eckit::geo::GridFactory::make_from_string(gridSpec));
                ASSERT(grid);

                EXPECT(grid->figure() == *expected);
            }
        }

        EXPECT_EQUAL(count, figures.size());
    }
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
