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

#include "atlas/Grid.h"
#include "atlas/grids/grids.h"
#include "atlas/grids/rgg/OctahedralRGG.h"
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"
#include "atlas/grids/RotatedGrid.h"

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
    NOTIMP; // Check grib flag

}

atlas::Grid *RotatedOctahedral::atlasGrid() const {
    return new atlas::grids::RotatedGrid(Octahedral::atlasGrid(),
                                         rotation_.south_pole_latitude(),
                                         rotation_.south_pole_longitude(),
                                         rotation_.south_pole_rotation_angle());
}


} // namespace reduced
}  // namespace repres
}  // namespace mir

