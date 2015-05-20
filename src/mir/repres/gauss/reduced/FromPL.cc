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

#include "mir/repres/gauss/reduced/FromPL.h"

#include "atlas/grids/ReducedGaussianGrid.h"
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace reduced {


FromPL::FromPL(const param::MIRParametrisation &parametrisation): Reduced(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
}

FromPL::~FromPL() {
}

FromPL::FromPL(size_t N, const std::vector<long> &pl, const util::BoundingBox &bbox):
    Reduced(N, bbox),
    pl_(pl) {

}

void FromPL::fill(grib_info &info) const  {
    Reduced::fill(info);
}

atlas::Grid *FromPL::atlasGrid() const {
    ASSERT (pl_.size() > 0);
    // FIXME: ask atlas to support long instead of int
    std::vector<int> pl(pl_.size());
    for (size_t i = 0; i < pl_.size(); i++) {
        pl[i] = pl_[i];
    }
    return new atlas::grids::ReducedGaussianGrid(N_, &pl[0]);
}


const std::vector<long> &FromPL::pls() const {
    return pl_;
}

}  // namespace reduced
}  // namespace repres
}  // namespace mir

