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


#include "mir/repres/gauss/reduced/RotatedClassic.h"

#include <ostream>

#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"


namespace mir::repres::gauss::reduced {


RotatedClassic::RotatedClassic(size_t N, const util::Rotation& rotation, const util::BoundingBox& bbox,
                               double angularPrecision) :
    Classic(N, bbox, angularPrecision), rotation_(rotation) {}


void RotatedClassic::print(std::ostream& out) const {
    out << "RotatedClassic["
           "N="
        << N_ << ",bbox=" << bbox_ << ",rotation=" << rotation_ << "]";
}


void RotatedClassic::makeName(std::ostream& out) const {
    Classic::makeName(out);
    rotation_.makeName(out);
}


bool RotatedClassic::sameAs(const Representation& /*unused*/) const {
    NOTIMP;
}


Iterator* RotatedClassic::iterator() const {
    return rotatedIterator(rotation_);
}


void RotatedClassic::fillGrib(grib_info& info) const {
    Classic::fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
}


void RotatedClassic::fillJob(api::MIRJob& job) const {
    Classic::fillJob(job);
    rotation_.fillJob(job);
}


atlas::Grid RotatedClassic::atlasGrid() const {
    return rotation_.rotate(Classic::atlasGrid());
}


const Gridded* RotatedClassic::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedClassic(N_, rotation_, bbox, angularPrecision_);
}


}  // namespace mir::repres::gauss::reduced
