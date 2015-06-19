/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "mir/repres/latlon/RegularLL.h"

#include <iostream>

#include "atlas/grids/LocalGrid.h"
#include "atlas/grids/LonLatGrid.h"

#include "eckit/exception/Exceptions.h"

#include "mir/util/Grib.h"
#include "mir/util/Compare.h"


namespace mir {
namespace repres {
namespace latlon {


RegularLL::RegularLL(const param::MIRParametrisation &parametrisation):
    LatLon(parametrisation) {
}


RegularLL::RegularLL(const util::BoundingBox &bbox,
                     const util::Increments &increments):
    LatLon(bbox, increments) {
}


RegularLL::~RegularLL() {
}

Representation *RegularLL::clone() const {
    return new RegularLL(bbox_, increments_);
}


// Called by RegularLL::crop()
RegularLL *RegularLL::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::info() << "Create cropped copy as RegularLL bbox=" << bbox << std::endl;
    return new RegularLL(bbox, increments_);
}


void RegularLL::print(std::ostream &out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
}


void RegularLL::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    LatLon::fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;

}

static bool check(double x, double dx) {
    double a = (x > 0 ? x : -x) / dx;
    return eckit::FloatCompare<double>::isApproxEqual(size_t(a), a);
}


atlas::Grid *RegularLL::atlasGrid() const {

    eckit::Log::info() << "RegularLL::atlasGrid BBox is " << bbox_ << std::endl;

    atlas::Grid *grid = new atlas::grids::LonLatGrid(int(ni_),
            int(nj_),
            atlas::grids::LonLatGrid::INCLUDES_POLES);

    if (globalDomain()) {

        // FIXME: an assertion for shift global grids
        ASSERT(bbox_.north() == 90);
        ASSERT(bbox_.south() == -90);
        ASSERT(bbox_.east() == 360 - increments_.west_east());
        ASSERT(bbox_.west() == 0);
    } else {
        // FIXME: assert if non-global shifted grid
        ASSERT(check(bbox_.north(), increments_.south_north()));
        ASSERT(check(bbox_.south(), increments_.south_north()));
        ASSERT(check(bbox_.west(), increments_.west_east()));
        ASSERT(check(bbox_.east(), increments_.west_east()));

        atlas::Domain domain(bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east());
        grid = new atlas::grids::LocalGrid(grid, domain);
    }

    eckit::Log::info() << "RegularLL::atlasGrid is " << *grid << " BoundBox " << bbox_ << std::endl;

    return grid;

}



namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}

}  // namespace latlon
}  // namespace repres
}  // namespace mir

