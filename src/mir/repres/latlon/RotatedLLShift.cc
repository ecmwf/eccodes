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


#include "mir/repres/latlon/RotatedLLShift.h"

#include "mir/config/LibMir.h"
#include "mir/util/RotatedIterator.h"


namespace mir {
namespace repres {
namespace latlon {


RotatedLLShift::RotatedLLShift(const util::BoundingBox &bbox,
                               const util::Increments &increments,
                               const util::Rotation& rotation,
                               const util::Shift& shift):
    RegularLLShift(bbox, increments, shift),
    rotation_(rotation) {
}


RotatedLLShift::~RotatedLLShift() {
}


void RotatedLLShift::print(std::ostream &out) const {
    out << "RotatedLLShift[";
    RegularLLShift::print(out);
    out << ",rotation=" << rotation_
        << ",shift=" << shift_
        << "]";
}


void RotatedLLShift::makeName(std::ostream& out) const { NOTIMP; }
bool RotatedLLShift::sameAs(const Representation& other) const { NOTIMP; }


// Called by RotatedLLShift::crop()
const RotatedLLShift *RotatedLLShift::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::debug<LibMir>() << "Create cropped copy as RotatedLLShift bbox=" << bbox << std::endl;
    return new RotatedLLShift(bbox, increments_, rotation_, shift_);
}



Iterator *RotatedLLShift::rotatedIterator() const {
    return new util::RotatedIterator(RegularLLShift::rotatedIterator(), rotation_);
}


void RotatedLLShift::fill(grib_info &info) const  {
    RegularLLShift::fill(info);
    rotation_.fill(info);
}


void RotatedLLShift::fill(api::MIRJob &job) const  {
    RegularLLShift::fill(job);
    rotation_.fill(job);
}


atlas::Grid RotatedLLShift::atlasGrid() const {
    return rotation_.rotate(RegularLLShift::atlasGrid());
}


// namespace {
// static RepresentationBuilder<RotatedLLShift> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
// }


}  // namespace latlon
}  // namespace repres
}  // namespace mir

