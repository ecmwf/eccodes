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

#include "mir/config/LibMir.h"
#include "mir/util/RotatedGrid.h"
#include "mir/util/RotatedIterator.h"

namespace mir {
namespace repres {
namespace latlon {


RotatedLLOffset::RotatedLLOffset(const util::BoundingBox &bbox,
                                 const util::Increments &increments,
                                 const util::Rotation& rotation,
                                 double northwards,
                                 double eastwards):
    RegularLLOffset(bbox, increments, northwards, eastwards),
    rotation_(rotation) {
}


RotatedLLOffset::~RotatedLLOffset() {
}


void RotatedLLOffset::print(std::ostream &out) const {
    out << "RotatedLLOffset[";
    RegularLLOffset::print(out);
    out << ",rotation=" << rotation_
        << "]";
}


// Called by RotatedLLOffset::crop()
const RotatedLLOffset *RotatedLLOffset::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::debug<LibMir>() << "Create cropped copy as RotatedLLOffset bbox=" << bbox << std::endl;
    return new RotatedLLOffset(bbox, increments_, rotation_, northwards_, eastwards_);
}



Iterator *RotatedLLOffset::rotatedIterator() const {
    return new util::RotatedIterator(RegularLLOffset::rotatedIterator(), rotation_);
}


void RotatedLLOffset::fill(grib_info &info) const  {
    RegularLLOffset::fill(info);
    rotation_.fill(info);
}


void RotatedLLOffset::fill(api::MIRJob &job) const  {
    RegularLLOffset::fill(job);
    rotation_.fill(job);
}


atlas::grid::Grid *RotatedLLOffset::atlasGrid() const {
    return new util::RotatedGrid(
               RegularLLOffset::atlasGrid(),
               rotation_.south_pole_latitude(),
               rotation_.south_pole_longitude(),
               rotation_.south_pole_rotation_angle() );
}


// namespace {
// static RepresentationBuilder<RotatedLLOffset> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
// }


}  // namespace latlon
}  // namespace repres
}  // namespace mir

