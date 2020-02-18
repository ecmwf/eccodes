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


#include "mir/repres/gauss/reduced/RotatedFromPL.h"

#include <iostream>
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


RotatedFromPL::RotatedFromPL(const param::MIRParametrisation& parametrisation) :
    FromPL(parametrisation),
    rotation_(parametrisation) {}


RotatedFromPL::RotatedFromPL(size_t N, const std::vector<long>& pl, const util::Rotation& rotation,
                             const util::BoundingBox& bbox, double angularPrecision) :
    FromPL(N, pl, bbox, angularPrecision),
    rotation_(rotation) {}


RotatedFromPL::~RotatedFromPL() = default;


void RotatedFromPL::print(std::ostream& out) const {
    out << "RotatedFromPL["
           "N="
        << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}


void RotatedFromPL::makeName(std::ostream& out) const {
    FromPL::makeName(out);
    rotation_.makeName(out);
}


bool RotatedFromPL::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const RotatedFromPL*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && FromPL::sameAs(other);
}


Iterator* RotatedFromPL::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedFromPL::fill(grib_info& info) const {
    FromPL::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedFromPL::fill(api::MIRJob&) const {
    NOTIMP;
}


atlas::Grid RotatedFromPL::atlasGrid() const {
    return rotation_.rotate(FromPL::atlasGrid());
}


const Gridded* RotatedFromPL::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedFromPL(N_, pls(), rotation_, bbox, angularPrecision_);
}


static RepresentationBuilder<RotatedFromPL> rotatedFromPL("reduced_rotated_gg");


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir
