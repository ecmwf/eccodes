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


#include "mir/repres/gauss/reduced/RotatedFromPL.h"

#include <iostream>
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


RotatedFromPL::RotatedFromPL(const param::MIRParametrisation &parametrisation):
    FromPL(parametrisation),
    rotation_(parametrisation) {
}


RotatedFromPL::RotatedFromPL(size_t N, const std::vector<long>& pl, const util::Rotation& rotation, const util::BoundingBox& bbox) :
    FromPL(N, pl, bbox),
    rotation_(rotation) {
}


RotatedFromPL::~RotatedFromPL() {
}


void RotatedFromPL::print(std::ostream &out) const {
    out << "RotatedFromPL["
            "N" << N_
        << ",bbox=" << bbox_
        << ",rotation=" << rotation_
        << "]";
}


void RotatedFromPL::makeName(std::ostream& out) const {
    FromPL::makeName(out);
    rotation_.makeName(out);
}


bool RotatedFromPL::sameAs(const Representation& other) const {
    const RotatedFromPL* o = dynamic_cast<const RotatedFromPL*>(&other);
    return o && (rotation_ == o->rotation_) && FromPL::sameAs(other);
}


util::BoundingBox RotatedFromPL::extendedBoundingBox(const util::BoundingBox& bbox) const {
    util::BoundingBox rotated = bbox.rotate(rotation_);
    return Reduced::extendedBoundingBox(rotated);
}


Iterator* RotatedFromPL::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedFromPL::fill(grib_info &info) const  {
    FromPL::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedFromPL::fill(api::MIRJob&) const  {
    NOTIMP;
}


atlas::Grid RotatedFromPL::atlasGrid() const {
    return rotation_.rotate(FromPL::atlasGrid());
}


const Gridded* RotatedFromPL::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedFromPL(N_, pls(), rotation_, bbox);
}


namespace {
static RepresentationBuilder<RotatedFromPL> rotatedFromPL("reduced_rotated_gg"); // Name is what is returned by grib_api
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

