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


#include "mir/repres/gauss/reduced/Classic.h"

#include "atlas/grid.h"
#include "mir/api/MIRJob.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Classic::Classic(size_t N):
    Reduced(N) {
    adjustBoundingBoxEastWest(bbox_);
}


Classic::~Classic() {
}


Classic::Classic(size_t N, const util::BoundingBox &bbox):
    Reduced(N, bbox) {
    adjustBoundingBoxEastWest(bbox_);
}


void Classic::fill(grib_info &info) const  {
    Reduced::fill(info);
// NOTE: We assume that grib_api will put the proper PL
}

void Classic::fill(api::MIRJob &job) const  {
    Reduced::fill(job);
    std::stringstream os;
    os << "N" << N_;
    job.set("gridname", os.str());
}

void Classic::makeName(std::ostream& out) const {
    out << "N" << N_;
    bbox_.makeName(out);
}

bool Classic::sameAs(const Representation& other) const {
    const Classic* o = dynamic_cast<const Classic*>(&other);
    return o && Reduced::sameAs(other);
}


atlas::Grid Classic::atlasGrid() const {
    return atlas::grid::ReducedGaussianGrid("N" + std::to_string(N_), domain());
}

const std::vector<long>& Classic::pls() const {
    if (pl_.size() == 0) {

        atlas::Grid::Config config;
        config.set("name", "N" + std::to_string(N_));
        atlas::grid::ReducedGaussianGrid grid(config);
        ASSERT(grid);

        const std::vector<long>& pl = grid.nx();
        ASSERT(pl.size() == N_ * 2);
        for (size_t i = 0; i < pl.size(); i++) {
            ASSERT(pl[i] > 0);
        }

        pl_ = pl;
    }
    return pl_;
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

