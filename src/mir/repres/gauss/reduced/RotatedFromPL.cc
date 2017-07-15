/*
 * (C) Copyright 1996-2016 ECMWF.
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


RotatedFromPL::~RotatedFromPL() {
}


RotatedFromPL::RotatedFromPL(long N, const std::vector<long> &pl, const util::BoundingBox &bbox, const util::Rotation& rotation):
    FromPL(N, pl, bbox),
    rotation_(rotation) {
}


void RotatedFromPL::print(std::ostream &out) const {
    out << "RotatedGGFromPL[N" << N_ << "]";
}


Iterator* RotatedFromPL::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedFromPL::fill(grib_info &info) const  {
    FromPL::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedFromPL::fill(api::MIRJob &job) const  {
    NOTIMP;
}


atlas::Grid RotatedFromPL::atlasGrid() const {
    return rotation_.rotate(FromPL::atlasGrid());
}


const Reduced* RotatedFromPL::cropped(const util::BoundingBox &bbox, const std::vector<long> &pl) const {
    return new RotatedFromPL(N_, pl, bbox, rotation_);
}


void RotatedFromPL::makeName(std::ostream& out) const {
    FromPL::makeName(out);
    rotation_.makeName(out);
}


bool RotatedFromPL::sameAs(const Representation& other) const {
    const RotatedFromPL* o = dynamic_cast<const RotatedFromPL*>(&other);
    return o && (rotation_ == o->rotation_) && FromPL::sameAs(other);
}


namespace {
static RepresentationBuilder<RotatedFromPL> rotatedGG("reduced_rotated_gg"); // Name is what is returned by grib_api
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

