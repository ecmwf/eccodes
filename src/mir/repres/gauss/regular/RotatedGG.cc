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


#include "mir/repres/gauss/regular/RotatedGG.h"

#include <iostream>
#include "atlas/grid.h"
#include "mir/util/Grib.h"
#include "mir/util/RotatedIterator.h"


namespace mir {
namespace repres {
namespace regular {


RotatedGG::RotatedGG(const param::MIRParametrisation &parametrisation):
    Regular(parametrisation),
    rotation_(parametrisation) {
}


RotatedGG::~RotatedGG() {
}


RotatedGG::RotatedGG(size_t N, const util::BoundingBox &bbox, const util::Rotation& rotation):
    Regular(N, bbox),
    rotation_(rotation) {
}


const Gridded *RotatedGG::cropped(const util::BoundingBox &bbox) const {
    return new RotatedGG(N_, bbox, rotation_);
}


void RotatedGG::print(std::ostream &out) const {
    out << "RotatedGG[N" << N_ << ",bbox=" << bbox_ << ",rotation" << rotation_ << "]";
}


void RotatedGG::fill(grib_info &info) const  {
    Regular::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_GG;
}


void RotatedGG::fill(api::MIRJob &job) const  {
    Regular::fill(job);
    rotation_.fill(job);
}


Iterator* RotatedGG::rotatedIterator() const {
    return new util::RotatedIterator(Regular::unrotatedIterator(), rotation_);
}


atlas::Grid RotatedGG::atlasGrid() const {
    return rotation_.rotate(Regular::atlasGrid());
}


namespace {
static RepresentationBuilder<RotatedGG> rotatedGG("rotated_gg"); // Name is what is returned by grib_api
}


}  // namespace reduced
}  // namespace repres
}  // namespace mir

