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


#include "mir/repres/latlon/RegularLLShift.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/ShiftIterator.h"


namespace mir {
namespace repres {
namespace latlon {


RegularLLShift::RegularLLShift(const util::BoundingBox &bbox,
                               const util::Increments &increments,
                               const util::Shift& shift):
    RegularLL(bbox, increments),
    shift_(shift) {
}


RegularLLShift::~RegularLLShift() {
}


void RegularLLShift::print(std::ostream &out) const {
    out << "RegularLLShift[";
    RegularLL::print(out);
    out << ",shift=" << shift_
        << "]";
}


void RegularLLShift::makeName(std::ostream& out) const {
    RegularLL::makeName(out);
    shift_.makeName(out);
}

bool RegularLLShift::sameAs(const Representation& other) const { NOTIMP; }


// Called by RegularLL::crop()
const RegularLLShift *RegularLLShift::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::debug<LibMir>() << "Create cropped copy as RegularLLShift bbox=" << bbox << std::endl;
    return new RegularLLShift(bbox, increments_, shift_);
}


Iterator *RegularLLShift::unrotatedIterator() const {
    return new util::ShiftIterator(RegularLL::unrotatedIterator(), shift_);
}


// namespace {
// static RepresentationBuilder<RegularLLShift> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
// }


}  // namespace latlon
}  // namespace repres
}  // namespace mir

