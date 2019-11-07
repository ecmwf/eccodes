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


#include "mir/repres/gauss/reduced/RotatedClassic.h"

#include <iostream>

#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


RotatedClassic::RotatedClassic(size_t N, const util::Rotation& rotation, const util::BoundingBox& bbox, double angularPrecision):
    Classic(N, bbox, angularPrecision),
    rotation_(rotation) {
}


RotatedClassic::~RotatedClassic() = default;


void RotatedClassic::print(std::ostream &out) const {
    out << "RotatedClassic["
            "N=" << N_
        << ",bbox=" << bbox_
        << ",rotation=" << rotation_
        << "]";
}


void RotatedClassic::makeName(std::ostream& out) const {
    Classic::makeName(out);
    rotation_.makeName(out);
}


bool RotatedClassic::sameAs(const Representation&) const {
    NOTIMP;
}


Iterator* RotatedClassic::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedClassic::fill(grib_info &info) const  {
    Classic::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedClassic::fill(api::MIRJob&) const  {
    NOTIMP;
}


atlas::Grid RotatedClassic::atlasGrid() const {
    return rotation_.rotate(Classic::atlasGrid());
}


const Gridded* RotatedClassic::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedClassic(N_, rotation_, bbox, angularPrecision_);
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

