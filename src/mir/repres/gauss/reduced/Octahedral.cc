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

#include "mir/api/MIRJob.h"
#include "mir/util/Domain.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Types.h"


namespace mir::repres::gauss::reduced {


Octahedral::Octahedral(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Reduced(N, bbox, angularPrecision) {

    // adjust latitudes, longitudes and re-set bounding box
    Latitude n = bbox.north();
    Latitude s = bbox.south();
    correctSouthNorth(s, n);

    setNj(pls("O" + std::to_string(N_)), s, n);

    Longitude w = bbox.west();
    Longitude e = bbox.east();
    correctWestEast(w, e);

    auto old(bbox_);
    bbox_ = util::BoundingBox(n, w, s, e);
    Log::debug() << "Octahedral BoundingBox:" << "\n\t   " << old << "\n\t > " << bbox_ << std::endl;
}


void Octahedral::fillGrib(grib_info& info) const {
    Reduced::fillGrib(info);
}


void Octahedral::fillJob(api::MIRJob& job) const {
    Reduced::fillJob(job);
    job.set("grid", "O" + std::to_string(N_));
}


void Octahedral::makeName(std::ostream& out) const {
    out << "O" << N_;
    bbox_.makeName(out);
}


bool Octahedral::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const Octahedral*>(&other);
    return (o != nullptr) && Reduced::sameAs(other);
}


atlas::Grid Octahedral::atlasGrid() const {
    return atlas::ReducedGaussianGrid("O" + std::to_string(N_), domain());
}


void Octahedral::fillMeshGen(util::MeshGeneratorParameters& params) const {
    Gaussian::fillMeshGen(params);
    params.set("triangulate", true);
}


}  // namespace mir::repres::gauss::reduced
