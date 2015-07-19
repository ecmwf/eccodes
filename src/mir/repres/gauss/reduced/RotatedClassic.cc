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

#include "mir/repres/gauss/reduced/RotatedClassic.h"

#include <iostream>

#include "mir/util/Grib.h"
#include "atlas/grids/RotatedGrid.h"
#include "mir/repres/gauss/reduced/RotatedFromPL.h"
#include "mir/util/RotatedIterator.h"

namespace mir {
namespace repres {
namespace reduced {



RotatedClassic::~RotatedClassic() {
}

RotatedClassic::RotatedClassic(long N, const util::BoundingBox &bbox, const util::Rotation& rotation):
    Classic(N, bbox),
    rotation_(rotation) {

}

void RotatedClassic::print(std::ostream &out) const {
    out << "RotatedGGClassic[N" << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_  << "]";
}

void RotatedClassic::fill(grib_info &info) const  {
#ifdef GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG
    Classic::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
#else
    NOTIMP;
#endif
}


Iterator* RotatedClassic::iterator(bool unrotated) const {
    if (unrotated) {
        return Classic::iterator(unrotated);
    } else {
        return new util::RotatedIterator(Classic::iterator(unrotated), rotation_);
    }
}

atlas::Grid *RotatedClassic::atlasGrid() const {
    ASSERT(globalDomain()); // Atlas support needed for non global grids
    return new atlas::grids::RotatedGrid(Classic::atlasGrid(),
                                         rotation_.south_pole_latitude(),
                                         rotation_.south_pole_longitude(),
                                         rotation_.south_pole_rotation_angle());
}

const Reduced *RotatedClassic::cropped(const util::BoundingBox &bbox, const std::vector<long> &pl) const {
    // We lose the RotatedClassic nature of the grid
    return new RotatedFromPL(N_, pl, bbox, rotation_);
}

}  // namespace reduced
}  // namespace repres
}  // namespace mir

