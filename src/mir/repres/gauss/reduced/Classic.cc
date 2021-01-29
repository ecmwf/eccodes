/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/gauss/reduced/Classic.h"

#include "mir/api/MIRJob.h"
#include "mir/util/Domain.h"
#include "mir/util/Log.h"
#include "mir/util/Types.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Classic::Classic(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Reduced(N, bbox, angularPrecision) {

    // adjust latitudes, longitudes and re-set bounding box
    Latitude n = bbox.north();
    Latitude s = bbox.south();
    correctSouthNorth(s, n);

    setNj(pls("N" + std::to_string(N_)), s, n);

    Longitude w = bbox.west();
    Longitude e = bbox.east();
    correctWestEast(w, e);

    auto old(bbox_);
    bbox_ = util::BoundingBox(n, w, s, e);
    Log::debug() << "Classic BoundingBox:"
                 << "\n\t   " << old << "\n\t > " << bbox_ << std::endl;
}


Classic::~Classic() = default;


void Classic::fill(grib_info& info) const {
    Reduced::fill(info);
}


void Classic::fill(api::MIRJob& job) const {
    Reduced::fill(job);
    job.set("grid", "N" + std::to_string(N_));
}


void Classic::makeName(std::ostream& out) const {
    out << "N" << N_;
    bbox_.makeName(out);
}


bool Classic::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const Classic*>(&other);
    return (o != nullptr) && Reduced::sameAs(other);
}


atlas::Grid Classic::atlasGrid() const {
    return atlas::ReducedGaussianGrid("N" + std::to_string(N_), domain());
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir
