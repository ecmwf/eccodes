/*
 * (C) Copyright 1996- ECMWF.
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


#include "mir/repres/gauss/reduced/RotatedOctahedral.h"

#include <iostream>

#include "mir/repres/gauss/reduced/RotatedFromPL.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


RotatedOctahedral::RotatedOctahedral(size_t N, const util::BoundingBox &bbox, const util::Rotation& rotation):
    Octahedral(N, bbox),
    rotation_(rotation) {
}


RotatedOctahedral::~RotatedOctahedral() {
}


void RotatedOctahedral::print(std::ostream &out) const {
    out << "RotatedGGOctahedral[N" << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}


void RotatedOctahedral::makeName(std::ostream& out) const {
    Octahedral::makeName(out);
    rotation_.makeName(out);
}


bool RotatedOctahedral::sameAs(const Representation& other) const { NOTIMP; }


Iterator* RotatedOctahedral::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedOctahedral::fill(grib_info& info) const  {
    Octahedral::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedOctahedral::fill(api::MIRJob&) const  {
    NOTIMP;
}


atlas::Grid RotatedOctahedral::atlasGrid() const {
    return rotation_.rotate(Octahedral::atlasGrid());
}


const Reduced *RotatedOctahedral::cropped(const util::BoundingBox &bbox, const std::vector<long> &pl) const {
    // We lose the RotatedOctahedral nature of the grid
    return new RotatedFromPL(N_, pl, bbox, rotation_, false);
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

