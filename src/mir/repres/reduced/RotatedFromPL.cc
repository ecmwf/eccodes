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
    rotation_(rotation) {
}

Representation *RotatedFromPL::clone() const {
    return new RotatedFromPL(N_, pl_, bbox_, rotation_);
}

void RotatedFromPL::print(std::ostream &out) const {
    out << "RotatedGGFromPL[N" << N_ << "]";
}

void RotatedFromPL::fill(grib_info &info) const  {
    FromPL::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_GG;
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

