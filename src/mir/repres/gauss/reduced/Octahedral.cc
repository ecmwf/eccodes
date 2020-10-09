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


#include "mir/repres/gauss/reduced/Octahedral.h"

#include "eckit/exception/Exceptions.h"

#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/util/Domain.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Octahedral::Octahedral(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Reduced(N, bbox, angularPrecision) {

    // adjust latitudes, longitudes and re-set bounding box
    Latitude n = bbox.north();
    Latitude s = bbox.south();
    correctSouthNorth(s, n);

    {
        atlas::util::Config config;
        config.set("name", "O" + std::to_string(N_));
        atlas::ReducedGaussianGrid grid(config);
        ASSERT(grid);

        setNj(grid.nx(), s, n);
    }

    Longitude w = bbox.west();
    Longitude e = bbox.east();
    correctWestEast(w, e);

    auto old(bbox_);
    bbox_ = util::BoundingBox(n, w, s, e);
    eckit::Log::debug<LibMir>() << "Octahedral BoundingBox:"
                                << "\n\t   " << old << "\n\t > " << bbox_ << std::endl;
}


Octahedral::~Octahedral() = default;


void Octahedral::fill(grib_info& info) const {
    Reduced::fill(info);
}


void Octahedral::fill(api::MIRJob& job) const {
    Reduced::fill(job);
    job.set("grid", "O" + std::to_string(N_));
}


void Octahedral::makeName(std::ostream& out) const {
    out << "O" << N_;
    bbox_.makeName(out);
}


bool Octahedral::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const Octahedral*>(&other);
    return (o != nullptr) && Reduced::sameAs(other);
}


atlas::Grid Octahedral::atlasGrid() const {
    return atlas::ReducedGaussianGrid("O" + std::to_string(N_), domain());
}


void Octahedral::fill(util::MeshGeneratorParameters& params) const {
    Gaussian::fill(params);
    params.set("triangulate", true);
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir
