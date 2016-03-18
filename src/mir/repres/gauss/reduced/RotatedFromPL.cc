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


#include "mir/repres/gauss/reduced/RotatedFromPL.h"

#include <iostream>

#include "atlas/grid/deprecated/RotatedGrid.h"

#include "mir/util/Grib.h"
#include "mir/util/RotatedIterator.h"


namespace mir {
namespace repres {
namespace reduced {


RotatedFromPL::RotatedFromPL(const param::MIRParametrisation &parametrisation):
    FromPL(parametrisation),
    rotation_(parametrisation) {
}


RotatedFromPL::~RotatedFromPL() {
}


RotatedFromPL::RotatedFromPL(long N, const std::vector<long> &pl, const util::BoundingBox &bbox, const util::Rotation& rotation):
    FromPL(N, pl, bbox),
    rotation_(rotation) {
}


void RotatedFromPL::print(std::ostream &out) const {
    out << "RotatedGGFromPL[N" << N_ << "]";
}


void RotatedFromPL::fill(grib_info &info) const  {
#ifdef GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG
    FromPL::fill(info);
    rotation_.fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
#else
    NOTIMP;
#endif
}


void RotatedFromPL::fill(api::MIRJob &job) const  {
    NOTIMP;
}


atlas::grid::Grid *RotatedFromPL::atlasGrid() const {
    ASSERT(globalDomain()); // Atlas support needed for non global grids
    return new atlas::grid::deprecated::RotatedGrid(
                FromPL::atlasGrid(),
                rotation_.south_pole_latitude(),
                rotation_.south_pole_longitude(),
                rotation_.south_pole_rotation_angle() );
}


const Reduced* RotatedFromPL::cropped(const util::BoundingBox &bbox, const std::vector<long> &pl) const {
    return new RotatedFromPL(N_, pl, bbox, rotation_);
}


Iterator* RotatedFromPL::rotatedIterator() const {
    return new util::RotatedIterator(FromPL::unrotatedIterator(), rotation_);
}


namespace {
static RepresentationBuilder<RotatedFromPL> rotatedGG("reduced_rotated_gg"); // Name is what is returned by grib_api
}


}  // namespace reduced
}  // namespace repres
}  // namespace mir

