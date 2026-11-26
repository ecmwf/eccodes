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


// gridSpec set and got as a spec (codes_set_spec, codes_get_spec), as of an eckit::geo::Grid


#include <memory>
#include <stdexcept>
#include <string>

#include "eckit/spec/Custom.h"

#include "eccodes/Spec.h"

#include "gridspec.h"


CASE("set and get, as the gridSpec string")
{
    for (const auto* name : { "GRIB1", "GRIB2" }) {
        for (const auto* str : { R"({"grid":[2,2]})", R"({"area":[60,0,0,30],"grid":[2,2]})", R"({"grid":"O32"})", R"({"grid":"F32"})" }) {
            std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(str));

            auto h = gridspec::sample(name);
            EXPECT(codes_set_spec(h.get(), "gridSpec", grid->spec()) == CODES_SUCCESS);

            const auto& spec = codes_get_spec(h.get(), "gridSpec");
            EXPECT_EQUAL(spec.str(), gridspec::get_gridspec(h.get()));
            EXPECT_EQUAL(spec.str(), grid->spec_str());

            // the same message as by the string
            auto s = gridspec::sample(name);
            EXPECT(gridspec::set_gridspec(s.get(), str) == CODES_SUCCESS);
            EXPECT_EQUAL(gridspec::get_gridspec(s.get()), spec.str());

            // the grid of the message
            std::unique_ptr<const eckit::geo::Grid> back(eckit::geo::GridFactory::build(spec));
            EXPECT(*back == *grid);
        }
    }
}


CASE("get: of the message as changed")
{
    auto h = gridspec::sample("GRIB2");

    std::unique_ptr<const eckit::geo::Grid> a(eckit::geo::GridFactory::make_from_string(R"({"grid":"O32"})"));
    std::unique_ptr<const eckit::geo::Grid> b(eckit::geo::GridFactory::make_from_string(R"({"grid":"H4"})"));

    EXPECT(codes_set_spec(h.get(), "gridSpec", a->spec()) == CODES_SUCCESS);
    EXPECT_EQUAL(codes_get_spec(h.get(), "gridSpec").str(), a->spec_str());

    EXPECT(codes_set_spec(h.get(), "gridSpec", b->spec()) == CODES_SUCCESS);
    EXPECT_EQUAL(codes_get_spec(h.get(), "gridSpec").str(), b->spec_str());
}


CASE("errors")
{
    auto h = gridspec::sample("GRIB2");
    const auto before = gridspec::get_gridspec(h.get());

    std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(R"({"grid":[1,1]})"));
    EXPECT(codes_set_spec(h.get(), "nonExistingKey", grid->spec()) == CODES_NOT_FOUND);
    EXPECT(codes_set_spec(h.get(), "edition", grid->spec()) == CODES_WRONG_TYPE);

    // not a grid, the message unchanged
    const eckit::spec::Custom unknown{ { "grid", std::string("X99") } };
    EXPECT(codes_set_spec(h.get(), "gridSpec", unknown) == CODES_GEOCALCULUS_PROBLEM);
    EXPECT_EQUAL(gridspec::get_gridspec(h.get()), before);

    EXPECT_THROWS_AS(codes_get_spec(h.get(), "nonExistingKey"), std::runtime_error);
    EXPECT_THROWS_AS(codes_get_spec(h.get(), "edition"), std::runtime_error);
}


int main(int argc, char* argv[])
{
    return gridspec::main(argc, argv);
}
