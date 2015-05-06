/*
 * (C) Copyright 1996-2015 ECMWF.
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


#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"

#include "mir/repres/RotatedLL.h"

#include "atlas/grids/RotatedGrid.h"


namespace mir {
namespace repres {


RotatedLL::RotatedLL(const param::MIRParametrisation &parametrisation):
    RegularLL(parametrisation) {
}

RotatedLL::RotatedLL(const util::BoundingBox &bbox, const util::Increments &increments, const util::Rotation &rotation):
    RegularLL(bbox, increments),
    rotation_(rotation) {

}


// RotatedLL::RotatedLL() {
// }


RotatedLL::~RotatedLL() {
}


Representation *RotatedLL::clone() const {
    return new RotatedLL(bbox_, increments_, rotation_);
}

void RotatedLL::print(std::ostream &out) const {
    out << "RotatedLL[";
    LatLon::print(out);
    out  << ",rotation=" << rotation_
         << "]";
}


// Called by RegularLL::crop()
RegularLL* RotatedLL::cropped(const util::BoundingBox &bbox) const{
    eckit::Log::info() << "Create cropped copy as RotatedLL bbox=" << bbox << std::endl;
    return new RotatedLL(bbox, increments_, rotation_);
}


void RotatedLL::fill(grib_info &info) const  {
    RegularLL::fill(info);
    rotation_.fill(info);
}

atlas::Grid *RotatedLL::atlasGrid() const {
    return new atlas::grids::RotatedGrid(RegularLL::atlasGrid(),
            rotation_.south_pole_latitude(),
            rotation_.south_pole_longitude(),
            rotation_.south_pole_rotation_angle());
}

namespace {
static RepresentationBuilder<RotatedLL> rotatedLL("rotated_ll"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

