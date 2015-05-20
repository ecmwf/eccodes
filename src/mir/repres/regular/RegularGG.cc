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

#include "mir/repres/regular/RegularGG.h"


#include <iostream>


#include "eckit/exception/Exceptions.h"


namespace mir {
namespace repres {
namespace regular {

RegularGG::RegularGG(const param::MIRParametrisation &parametrisation):
    Regular(parametrisation) {
}

RegularGG::RegularGG(size_t N):
    Regular(N) {
}


RegularGG::RegularGG(size_t N, const util::BoundingBox &bbox):
    Regular(N, bbox) {
}

Representation *RegularGG::clone() const {
    return new RegularGG(N_, bbox_);
}

RegularGG::~RegularGG() {
}


void RegularGG::print(std::ostream &out) const {
    out << "RegularGG[N" << N_ << ",bbox=" << bbox_ << "]";
}

Gridded *RegularGG::cropped(const util::BoundingBox &bbox) const {
    return new RegularGG(N_, bbox);
}


namespace {
static RepresentationBuilder<RegularGG> reducedGG("regular_gg"); // Name is what is returned by grib_api
}

}  // namespace regular
}  // namespace repres
}  // namespace mir

