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


#include "mir/repres/gauss/reduced/RotatedFromPL.h"

#include <ostream>

#include "mir/util/Atlas.h"
#include "mir/util/Grib.h"


namespace mir::repres::gauss::reduced {


RotatedFromPL::RotatedFromPL(const param::MIRParametrisation& parametrisation) :
    FromPL(parametrisation), rotation_(parametrisation) {}


RotatedFromPL::RotatedFromPL(size_t N, const std::vector<long>& pl, const util::Rotation& rotation,
                             const util::BoundingBox& bbox, double angularPrecision) :
    FromPL(N, pl, bbox, angularPrecision), rotation_(rotation) {}


void RotatedFromPL::print(std::ostream& out) const {
    out << "RotatedFromPL["
           "N="
        << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}


void RotatedFromPL::makeName(std::ostream& out) const {
    FromPL::makeName(out);
    rotation_.makeName(out);
}


bool RotatedFromPL::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const RotatedFromPL*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && FromPL::sameAs(other);
}


Iterator* RotatedFromPL::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedFromPL::fillGrib(grib_info& info) const {
    FromPL::fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedFromPL::fillJob(api::MIRJob& job) const {
    FromPL::fillJob(job);
    rotation_.fillJob(job);
}


atlas::Grid RotatedFromPL::atlasGrid() const {
    return rotate_atlas_grid(rotation_.rotation(), FromPL::atlasGrid());
}


const Gridded* RotatedFromPL::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedFromPL(N_, pls(), rotation_, bbox, angularPrecision_);
}


static const RepresentationBuilder<RotatedFromPL> rotatedFromPL("reduced_rotated_gg");


}  // namespace mir::repres::gauss::reduced
