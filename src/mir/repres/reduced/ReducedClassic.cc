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

#include "mir/repres/reduced/ReducedClassic.h"

#include <iostream>

namespace mir {
namespace repres {
namespace reduced {


ReducedClassic::ReducedClassic(size_t N):
    Classic(N) {

}

ReducedClassic::~ReducedClassic() {
}

ReducedClassic::ReducedClassic(long N, const util::BoundingBox &bbox):
    Classic(N, bbox) {

}

Representation *ReducedClassic::clone() const {
    return new ReducedClassic(N_, bbox_);
}

void ReducedClassic::print(std::ostream &out) const {
    out << "ReducedGGClassic[N" << N_ << ",bbox=" << bbox_ << "]";
}




} // namespace reduced
}  // namespace repres
}  // namespace mir

