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

#include "mir/repres/ReducedLL.h"

#include <eckit/parser/Tokenizer.h>

#include "atlas/Grid.h"
#include "atlas/GridSpec.h"
#include "atlas/grids/ReducedLonLatGrid.h"
#include <eckit/parser/Tokenizer.h>

namespace mir {
namespace repres {


ReducedLL::ReducedLL(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
}


ReducedLL::ReducedLL() {
}


ReducedLL::~ReducedLL() {
}


void ReducedLL::print(std::ostream &out) const {
    out << "ReducedLL["
        << "]";
}


void ReducedLL::fill(grib_info &info) const  {
    NOTIMP;
}

atlas::Grid* ReducedLL::atlasGrid() const {
    // FIXME: ask atlas to support long instead of int
    std::vector<int> pl(pl_.size());
    for(size_t i= 0; i < pl_.size(); i++) {
        pl[i] = pl_[i];
    }
    return new atlas::grids::ReducedLonLatGrid(pl.size(), &pl[0], atlas::grids::ReducedLonLatGrid::INCLUDES_POLES);
}

namespace {
static RepresentationBuilder<ReducedLL> reducedLL("reduced_ll"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

