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


#include "mir/repres/latlon/RotatedLLOffset.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/OffsetGrid.h"
#include "mir/util/OffsetIterator.h"


namespace mir {
namespace repres {
namespace latlon {


RotatedLLOffset::RotatedLLOffset(const util::BoundingBox &bbox,
                                 const util::Increments &increments,
                                 const util::Rotation& rotation,
                                 double northwards,
                                 double eastwards):
    RotatedLL(bbox, increments, rotation),
    northwards_(northwards),
    eastwards_(eastwards) {
}


RotatedLLOffset::~RotatedLLOffset() {
}


void RotatedLLOffset::print(std::ostream &out) const {
    out << "RotatedLLOffset[";
    RotatedLL::print(out);
    out << ",northwards" << northwards_
        << ",eastwards" << eastwards_
        << "]";
}


// Called by RotatedLL::crop()
const RotatedLLOffset *RotatedLLOffset::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::debug<LibMir>() << "Create cropped copy as RotatedLLOffset bbox=" << bbox << std::endl;
    return new RotatedLLOffset(bbox, increments_, rotation_, northwards_, eastwards_);
}


Iterator *RotatedLLOffset::unrotatedIterator() const {
    return new util::OffsetIterator(RotatedLL::unrotatedIterator(), northwards_, eastwards_);
}

Iterator *RotatedLLOffset::rotatedIterator() const {
    return new util::OffsetIterator(RotatedLL::rotatedIterator(), northwards_, eastwards_);
}

atlas::grid::Grid *RotatedLLOffset::atlasGrid() const {
    return new util::OffsetGrid(
               RotatedLL::atlasGrid(),
               northwards_,
               eastwards_);
}


// namespace {
// static RepresentationBuilder<RotatedLLOffset> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
// }


}  // namespace latlon
}  // namespace repres
}  // namespace mir

