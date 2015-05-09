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

#include "mir/repres/reduced/RotatedFromPL.h"

#include <iostream>

#include "atlas/Grid.h"
#include "atlas/grids/grids.h"
#include "atlas/grids/ReducedGaussianGrid.h"
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"
#include "atlas/grids/RotatedGrid.h"

namespace mir {
namespace repres {
namespace reduced {


RotatedFromPL::RotatedFromPL(const param::MIRParametrisation &parametrisation):
    FromPL(parametrisation),
    rotation_(parametrisation) {
}


RotatedFromPL::~RotatedFromPL() {
}

RotatedFromPL::RotatedFromPL(long N, const std::vector<long> &pl, const util::BoundingBox &bbox, const util::Rotation& rotation):
    FromPL(N, pl, bbox),
    rotation_(rotation){
}

Representation *RotatedFromPL::clone() const {
    return new RotatedFromPL(N_, pl_, bbox_, rotation_);
}

void RotatedFromPL::print(std::ostream &out) const {
    out << "RotatedGGFromPL[N" << N_ << "]";
}

void RotatedFromPL::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj = N_ * 2; // Should be PL.size()
    info.grid.N = N_;

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

    // FIXME: Where are the PL set? Looks like grib_api has its own list
}

atlas::Grid *RotatedFromPL::atlasGrid() const {
    return new atlas::grids::RotatedGrid(FromPL::atlasGrid(),
                                         rotation_.south_pole_latitude(),
                                         rotation_.south_pole_longitude(),
                                         rotation_.south_pole_rotation_angle());
}

// namespace {
// static RepresentationBuilder<RotatedFromPL> reducedGGFromPL("reduced_gg"); // Name is what is returned by grib_api
// }

} // namespace reduced
}  // namespace repres
}  // namespace mir

