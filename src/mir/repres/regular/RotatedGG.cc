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

#include "mir/repres/regular/RotatedGG.h"

#include <iostream>

#include "mir/util/Grib.h"
#include "atlas/grids/RotatedGrid.h"

namespace mir {
namespace repres {
namespace regular {


RotatedGG::RotatedGG(const param::MIRParametrisation &parametrisation):
    Regular(parametrisation),
    rotation_(parametrisation) {
}


RotatedGG::~RotatedGG() {
}

RotatedGG::RotatedGG(size_t N, const util::BoundingBox &bbox, const util::Rotation& rotation):
    Regular(N, bbox),
    rotation_(rotation) {
}

Representation *RotatedGG::clone() const {
    return new RotatedGG(N_, bbox_, rotation_);
}

void RotatedGG::print(std::ostream &out) const {
    out << "RotatedGG[N" << N_ << ",bbox=" << bbox_ << ",rotation" << rotation_ << "]";
}

void RotatedGG::fill(grib_info &info) const  {
    Regular::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_GG;
}

atlas::Grid *RotatedGG::atlasGrid() const {
    return new atlas::grids::RotatedGrid(Regular::atlasGrid(),
                                         rotation_.south_pole_latitude(),
                                         rotation_.south_pole_longitude(),
                                         rotation_.south_pole_rotation_angle());
}


namespace {
static RepresentationBuilder<RotatedGG> rotatedGG("rotated_gg"); // Name is what is returned by grib_api
}


// namespace {
// static RepresentationBuilder<RotatedGG> reducedGGFromPL("reduced_gg"); // Name is what is returned by grib_api
// }

} // namespace reduced
}  // namespace repres
}  // namespace mir

