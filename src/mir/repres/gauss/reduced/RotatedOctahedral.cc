/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/gauss/reduced/RotatedOctahedral.h"

#include <ostream>

#include "mir/util/Atlas.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


RotatedOctahedral::RotatedOctahedral(size_t N, const util::Rotation& rotation, const util::BoundingBox& bbox,
                                     double angularPrecision) :
    Octahedral(N, bbox, angularPrecision), rotation_(rotation) {}


void RotatedOctahedral::print(std::ostream& out) const {
    out << "RotatedOctahedral["
           "N="
        << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}


void RotatedOctahedral::makeName(std::ostream& out) const {
    Octahedral::makeName(out);
    rotation_.makeName(out);
}


bool RotatedOctahedral::sameAs(const Representation&) const {
    NOTIMP;
}


Iterator* RotatedOctahedral::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedOctahedral::fill(grib_info& info) const {
    Octahedral::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedOctahedral::fill(api::MIRJob& job) const {
    Octahedral::fill(job);
    rotation_.fill(job);
}


atlas::Grid RotatedOctahedral::atlasGrid() const {
    return rotation_.rotate(Octahedral::atlasGrid());
}


const Gridded* RotatedOctahedral::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedOctahedral(N_, rotation_, bbox, angularPrecision_);
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir
