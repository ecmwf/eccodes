/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/repres/regular/Regular.h"


#include <iostream>

#include "atlas/grids/GaussianGrid.h"

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace regular {

Regular::Regular(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("N", N_));
}

Regular::Regular(size_t N):
    N_(N) {

}


Regular::Regular(size_t N, const util::BoundingBox& bbox):
    N_(N),
    bbox_(bbox) {

}

Regular::~Regular() {
}


void Regular::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;
    info.grid.Nj = N_ * 2;

    double we = 90.0 / N_; // FIXME: Just a guess
    double ni = (bbox_.east() - bbox_.west()) / we;

    ASSERT(long(ni) == ni);
    info.grid.Ni = ni;
    info.grid.N = N_;
    info.grid.iDirectionIncrementInDegrees = we;

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    size_t j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = bbox_.global() ? 1 : 0;

}

atlas::Grid *Regular::atlasGrid() const {
    return new atlas::grids::GaussianGrid(N_);
}


void Regular::validate(const std::vector<double>& values) const {
    size_t nj = N_ * 2;
    size_t ni = (bbox_.east() - bbox_.west()) / (90.0 / N_);
    ASSERT(values.size() == ni * nj);
}


} // namespace regular
}  // namespace repres
}  // namespace mir

