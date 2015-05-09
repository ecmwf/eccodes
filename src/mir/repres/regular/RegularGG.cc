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

#include "mir/repres/RegularGG.h"


#include <iostream>

#include "atlas/grids/GaussianGrid.h"

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace regular {

RegularGG::RegularGG(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("N", N_));
}

RegularGG::RegularGG(size_t N_):
    N_(N_) {

}

Representation *RegularGG::clone() const {
    return new RegularGG(N_);
}

RegularGG::RegularGG() {
}


RegularGG::~RegularGG() {
}


void RegularGG::print(std::ostream &out) const {
    out << "RegularGG[N" << N_ << "]";
}

void RegularGG::fill(grib_info &info) const  {

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

atlas::Grid *RegularGG::atlasGrid() const {
    return new atlas::grids::GaussianGrid(N_);
}


namespace {
static RepresentationBuilder<RegularGG> reducedGG("regular_gg"); // Name is what is returned by grib_api
}

} // namespace regular
}  // namespace repres
}  // namespace mir

