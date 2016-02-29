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

#include "atlas/grid/ReducedGaussianGrid.h"
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

FromPL::FromPL(const std::vector<long> &pl):
    Reduced(pl.size()/2),
    pl_(pl) {
}

void FromPL::fill(grib_info &info) const  {
    Reduced::fill(info);
}

void FromPL::fill(api::MIRJob &job) const  {
    Reduced::fill(job);
}

atlas::grid::Grid *FromPL::atlasGrid() const {
    ASSERT(globalDomain()); // Atlas support needed for non global grids
    ASSERT (pl_.size() > 0);
    return new atlas::grid::ReducedGaussianGrid(N_, &pl_[0]);
}


const std::vector<long> &FromPL::pls() const {
    return pl_;
}

}  // namespace reduced
}  // namespace repres
}  // namespace mir

