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


#include "mir/repres/gauss/reduced/FromPL.h"

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"
#include "eckit/utils/MD5.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


static bool checkPl(const std::vector<long>& pl) {
    return *std::min_element(pl.begin(), pl.end()) >= 2;
}


FromPL::FromPL(const param::MIRParametrisation &parametrisation) : Reduced(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
    checkPl(pl_);
    adjustBoundingBoxEastWest(bbox_);
}


FromPL::~FromPL() {
}


FromPL::FromPL(size_t N, const std::vector<long> &pl, const util::BoundingBox &bbox) :
    Reduced(N, bbox),
    pl_(pl) {
    checkPl(pl_);
    adjustBoundingBoxEastWest(bbox_);
}


FromPL::FromPL(const std::vector<long> &pl):
    Reduced(pl.size() / 2),
    pl_(pl) {
    checkPl(pl_);
    adjustBoundingBoxEastWest(bbox_);
}


void FromPL::makeName(std::ostream& out) const {
    out << "R" << N_ << "-";

    eckit::MD5 md5;
    for (auto j = pl_.begin(); j != pl_.end(); ++j) {
        md5 << *j;
    }

    out << std::string(md5);
    bbox_.makeName(out);
}


bool FromPL::sameAs(const Representation& other) const {
    const FromPL* o = dynamic_cast<const FromPL*>(&other);
    return o && (pl_ == o->pl_) && Reduced::sameAs(other);
}


void FromPL::fill(grib_info &info) const  {
    Reduced::fill(info);
}


void FromPL::fill(api::MIRJob &job) const  {
    Reduced::fill(job);
}


atlas::Grid FromPL::atlasGrid() const {
    return atlas::grid::ReducedGaussianGrid(pl_, domain());
}


const std::vector<long> &FromPL::pls() const {
    return pl_;
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

