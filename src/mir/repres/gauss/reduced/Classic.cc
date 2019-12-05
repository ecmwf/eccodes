/*
 * (C) Copyright 1996- ECMWF.
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

#include "eckit/exception/Exceptions.h"

#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/util/Domain.h"


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

    {
        atlas::util::Config config;
        config.set("name", "N" + std::to_string(N_));
        atlas::ReducedGaussianGrid grid(config);
        ASSERT(grid);

        setNj(grid.nx(), s, n);
    }

    Longitude w = bbox.west();
    Longitude e = bbox.east();
    correctWestEast(w, e);

    auto old(bbox_);
    bbox_ = util::BoundingBox(n, w, s, e);
    eckit::Log::debug<LibMir>() << "Classic BoundingBox:"
                                << "\n\t   " << old << "\n\t > " << bbox_ << std::endl;
}


Classic::~Classic() = default;


void Classic::fill(grib_info& info) const {
    Reduced::fill(info);
}


void Classic::fill(api::MIRJob& job) const {
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
    auto o = dynamic_cast<const Classic*>(&other);
    return o && Reduced::sameAs(other);
}


atlas::Grid Classic::atlasGrid() const {
    return atlas::ReducedGaussianGrid("N" + std::to_string(N_), domain());
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

