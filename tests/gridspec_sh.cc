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


// gridSpec of gridType=sh (spherical harmonics)


#include <string>
#include <utility>
#include <vector>

#include "gridspec.h"


using gridspec::encode_error_t;
using gridspec::encode_t;


CASE("gridSpec to GRIB and back")
{
    // the number of values is of the real coefficients, (T+1)(T+2)
    EXPECT(gridspec::check_all<encode_t>({
        { "sh_ml_grib1", R"({"grid":"T19"})", "sh", { { "J", 19 }, { "K", 19 }, { "M", 19 }, { "packingType", "spectral_complex" }, { "numberOfValues", 420 } } },
        { "sh_ml_grib2", R"({"grid":"T19"})", "sh", { { "J", 19 }, { "K", 19 }, { "M", 19 }, { "packingType", "spectral_complex" }, { "numberOfValues", 420 } } },
        { "sh_ml_grib2", R"({"grid":"T639"})", "sh", { { "J", 639 }, { "numberOfValues", 410240 } } },
        { "sh_ml_grib2", R"({"truncation":63,"type":"sh"})", "sh", { { "J", 63 }, { "numberOfValues", 4160 } }, R"({"grid":"T63"})" },
    }));

    // in place, spectral data is not converted from gridded data
    EXPECT(gridspec::check_all<encode_error_t>({
        { "GRIB1", R"({"grid":"T19"})", CODES_GEOCALCULUS_PROBLEM },
        { "GRIB2", R"({"grid":"T19"})", CODES_GEOCALCULUS_PROBLEM },
        { "regular_gg_sfc_grib2", R"({"grid":"T19"})", CODES_GEOCALCULUS_PROBLEM },
    }));
}


CASE("gridded on spectral messages")
{
    // gridded data is converted from spectral data (the values a constant field as many as the points, for the caller to
    // replace)
    EXPECT(gridspec::check_all<encode_t>({
        { "sh_ml_grib2", R"({"grid":"O32"})", "reduced_gg", { { "packingType", "grid_simple" }, { "numberOfDataPoints", 5248 } } },
        { "sh_ml_grib2", R"({"grid":"H2"})", "healpix", { { "packingType", "grid_simple" }, { "numberOfDataPoints", 48 } } },
    }));

    // GRIB1 numberOfDataPoints is of the values, as many as the points
    for (const auto& [spec, n] : std::vector<std::pair<std::string, size_t>>{ { R"({"grid":"O32"})", 5248 }, { R"({"grid":[1,1]})", 65160 } }) {
        auto h = gridspec::sample("sh_ml_grib1");
        EXPECT(gridspec::set_gridspec(h.get(), spec) == CODES_SUCCESS);
        EXPECT(gridspec::mismatches(h.get(), { { "packingType", "grid_simple" } }).empty());
        EXPECT_EQUAL(gridspec::get_long(h.get(), "numberOfDataPoints"), static_cast<long>(n));
        EXPECT_EQUAL(gridspec::get_gridspec(h.get()), spec);
    }
}


CASE("GRIB to gridSpec")
{
    EXPECT(gridspec::check_decoded("gridspec/gridType=sh.grib", { R"({"grid":"T19"})" }));
}

int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
