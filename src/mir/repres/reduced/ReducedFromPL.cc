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

#include "mir/repres/reduced/ReducedFromPL.h"

#include <iostream>

#include "atlas/Grid.h"
#include "atlas/grids/grids.h"
#include "atlas/grids/ReducedGaussianGrid.h"
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace reduced {


ReducedFromPL::ReducedFromPL(const param::MIRParametrisation &parametrisation):
    FromPL(parametrisation) {

}


ReducedFromPL::~ReducedFromPL() {
}

ReducedFromPL::ReducedFromPL(long N, const std::vector<long> &pl, const util::BoundingBox &bbox):
    FromPL(N, pl, bbox) {
}

Representation *ReducedFromPL::clone() const {
    return new ReducedFromPL(N_, pl_, bbox_);
}

void ReducedFromPL::print(std::ostream &out) const {
    out << "ReducedGGFromPL[N" << N_ << "]";
}

namespace {
static RepresentationBuilder<ReducedFromPL> reducedGGFromPL("reduced_gg"); // Name is what is returned by grib_api
}

} // namespace reduced
}  // namespace repres
}  // namespace mir

