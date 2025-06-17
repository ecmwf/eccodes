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


#include "mir/repres/gauss/regular/RotatedGG.h"

#include <ostream>
#include <utility>

#include "mir/repres/gauss/GaussianIterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Grib.h"


namespace mir::repres::gauss::regular {


RotatedGG::RotatedGG(const param::MIRParametrisation& parametrisation) :
    Regular(parametrisation), rotation_(parametrisation) {}


RotatedGG::RotatedGG(size_t N, const util::Rotation& rotation, const util::BoundingBox& bbox, double angularPrecision) :
    Regular(N, bbox, angularPrecision), rotation_(rotation) {}


void RotatedGG::print(std::ostream& out) const {
    out << "RotatedGG["
           "N="
        << N_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}


bool RotatedGG::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const RotatedGG*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && Regular::sameAs(other);
}


Iterator* RotatedGG::iterator() const {
    std::vector<long> pl(N_ * 2, long(4 * N_));
    return new gauss::GaussianIterator(latitudes(), std::move(pl), bbox_, N_, Nj_, k_, rotation_);
}


const Gridded* RotatedGG::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedGG(N_, rotation_, bbox, angularPrecision_);
}


void RotatedGG::makeName(std::ostream& out) const {
    Regular::makeName(out);
    rotation_.makeName(out);
}


void RotatedGG::fillGrib(grib_info& info) const {
    Regular::fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_GG;
}


void RotatedGG::fillJob(api::MIRJob& job) const {
    Regular::fillJob(job);
    rotation_.fillJob(job);
}


atlas::Grid RotatedGG::atlasGrid() const {
    return rotate_atlas_grid(rotation_.rotation(), Regular::atlasGrid());
}


static const RepresentationBuilder<RotatedGG> rotatedGG("rotated_gg");


}  // namespace mir::repres::gauss::regular
