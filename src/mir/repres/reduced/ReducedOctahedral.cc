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

#include "mir/repres/reduced/ReducedOctahedral.h"

#include <iostream>

// #include "atlas/Grid.h"
// #include "atlas/grids/grids.h"
// #include "atlas/grids/rgg/OctahedralRGG.h"
// #include "eckit/exception/Exceptions.h"
// #include "mir/param/MIRParametrisation.h"
// #include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace reduced {


ReducedOctahedral::ReducedOctahedral(size_t N):
    Octahedral(N) {

}

ReducedOctahedral::~ReducedOctahedral() {
}

ReducedOctahedral::ReducedOctahedral(long N, const util::BoundingBox &bbox):
    Octahedral(N, bbox) {
}

Representation *ReducedOctahedral::clone() const {
    return new ReducedOctahedral(N_, bbox_);
}

void ReducedOctahedral::print(std::ostream &out) const {
    out << "ReducedGGOctahedral[N" << N_ << ",bbox=" << bbox_ << "]";
}


} // namespace reduced
}  // namespace repres
}  // namespace mir

