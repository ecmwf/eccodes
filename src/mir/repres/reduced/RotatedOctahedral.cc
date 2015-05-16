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

#include "mir/repres/reduced/RotatedOctahedral.h"

#include <iostream>

#include "mir/util/Grib.h"
#include "atlas/grids/RotatedGrid.h"
#include "mir/repres/reduced/RotatedFromPL.h"

namespace mir {
namespace repres {
namespace reduced {



RotatedOctahedral::~RotatedOctahedral() {
}

RotatedOctahedral::RotatedOctahedral(long N, const util::BoundingBox &bbox, const util::Rotation& rotation):
    Octahedral(N, bbox),
    rotation_(rotation) {

}

Representation *RotatedOctahedral::clone() const {
    return new RotatedOctahedral(N_, bbox_, rotation_);
}

void RotatedOctahedral::print(std::ostream &out) const {
    out << "RotatedGGOctahedral[N" << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}

void RotatedOctahedral::fill(grib_info &info) const  {
    Octahedral::fill(info);
    rotation_.fill(info);
#if 0
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_OCTAHEDRAL_GG
#else
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_GG;

#endif

}

atlas::Grid *RotatedOctahedral::atlasGrid() const {
    return new atlas::grids::RotatedGrid(Octahedral::atlasGrid(),
                                         rotation_.south_pole_latitude(),
                                         rotation_.south_pole_longitude(),
                                         rotation_.south_pole_rotation_angle());
}


Gaussian *RotatedOctahedral::cropped(const util::BoundingBox &bbox, const std::vector<long> &pl) const {
    // We lose the RotatedOctahedral nature of the grid
    return new RotatedFromPL(N_, pl, bbox, rotation_);
}

} // namespace reduced
}  // namespace repres
}  // namespace mir

