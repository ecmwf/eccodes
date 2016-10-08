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


#include "mir/repres/latlon/RegularLLOffset.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/OffsetGrid.h"
#include "mir/util/OffsetIterator.h"


namespace mir {
namespace repres {
namespace latlon {


RegularLLOffset::RegularLLOffset(const util::BoundingBox &bbox,
                                 const util::Increments &increments,
                                 double northwards, double eastwards):
    RegularLL(bbox, increments),
    northwards_(northwards),
    eastwards_(eastwards) {
}


RegularLLOffset::~RegularLLOffset() {
}


void RegularLLOffset::print(std::ostream &out) const {
    out << "RegularLLOffset[";
    RegularLL::print(out);
    out << ",northwards" << northwards_
        << ",eastwards" << eastwards_
        << "]";
}


// Called by RegularLL::crop()
const RegularLLOffset *RegularLLOffset::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::debug<LibMir>() << "Create cropped copy as RegularLLOffset bbox=" << bbox << std::endl;
    return new RegularLLOffset(bbox, increments_, northwards_, eastwards_);
}


Iterator *RegularLLOffset::unrotatedIterator() const {
    return new util::OffsetIterator(RegularLL::unrotatedIterator(), northwards_, eastwards_);
}


void RegularLLOffset::fill(grib_info &info) const  {
    RegularLL::fill(info);
}


void RegularLLOffset::fill(api::MIRJob &job) const  {
    RegularLL::fill(job);
}


atlas::grid::Grid *RegularLLOffset::atlasGrid() const {
    return new util::OffsetGrid(
               RegularLL::atlasGrid(),
               northwards_,
               eastwards_);
}


// namespace {
// static RepresentationBuilder<RegularLLOffset> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
// }


}  // namespace latlon
}  // namespace repres
}  // namespace mir

