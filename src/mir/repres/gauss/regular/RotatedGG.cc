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


#include "mir/repres/gauss/regular/RotatedGG.h"

#include <iostream>
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace regular {


RotatedGG::RotatedGG(const param::MIRParametrisation& parametrisation):
    Regular(parametrisation),
    rotation_(parametrisation) {
}


RotatedGG::RotatedGG(size_t N, const util::Rotation& rotation, const util::BoundingBox& bbox) :
    Regular(N, bbox),
    rotation_(rotation) {
}


RotatedGG::~RotatedGG() {
}


const Gridded* RotatedGG::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedGG(N_, rotation_, bbox);
}


void RotatedGG::print(std::ostream& out) const {
    out << "RotatedGG["
        <<  "N" << N_
        << ",bbox=" << bbox_
        << ",rotation=" << rotation_
        << "]";
}


void RotatedGG::makeName(std::ostream& out) const {
    Regular::makeName(out);
    rotation_.makeName(out);
}


bool RotatedGG::sameAs(const Representation& other) const {
    const RotatedGG* o = dynamic_cast<const RotatedGG*>(&other);
    return o && (rotation_ == o->rotation_) && RotatedGG::sameAs(other);
}


util::BoundingBox RotatedGG::extendedBoundingBox(const util::BoundingBox& bbox, double angle) const {

    // cropping bounding box after extending guarantees the representation can use it
    util::BoundingBox extended(bbox);
    Gridded::extendBoundingBox(extended, angle);
    return extended;
}


void RotatedGG::fill(grib_info& info) const  {
    Regular::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_GG;
}


void RotatedGG::fill(api::MIRJob& job) const  {
    Regular::fill(job);
    rotation_.fill(job);
}


Iterator* RotatedGG::iterator() const {
    auto Ni = [=](size_t){ return long(4 * N_); };
    return Gaussian::unrotatedIterator(Ni);
}


atlas::Grid RotatedGG::atlasGrid() const {
    return rotation_.rotate(Regular::atlasGrid());
}


namespace {
static RepresentationBuilder<RotatedGG> rotatedGG("rotated_gg"); // Name is what is returned by grib_api
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

