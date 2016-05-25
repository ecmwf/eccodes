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


#include "mir/repres/latlon/RegularLL.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Types.h"

#include "atlas/grid/global/lonlat/RegularLonLat.h"

#include "mir/util/Grib.h"
#include "mir/util/Compare.h"
#include "mir/log/MIR.h"


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


// Called by RegularLL::crop()
const RegularLL *RegularLL::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::trace<MIR>() << "Create cropped copy as RegularLL bbox=" << bbox << std::endl;
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


void RegularLL::fill(api::MIRJob &job) const  {
    LatLon::fill(job);
}


static bool check(double x, double dx) {
    double a = (x > 0 ? x : -x) / dx;
    return eckit::FloatCompare<double>::isApproximatelyEqual(size_t(a), a);
}


atlas::grid::Grid *RegularLL::atlasGrid() const {

    eckit::Log::trace<MIR>() << "RegularLL::atlasGrid BBox is " << bbox_ << std::endl;

    atlas::grid::Grid *grid = new atlas::grid::global::lonlat::RegularLonLat(
        (const size_t) ni_,
        (const size_t) nj_ );

    // FIXME: an assertion for shift global grids
    ASSERT(bbox_.north() == 90);
    ASSERT(bbox_.south() == -90);
    ASSERT(bbox_.east() == 360 - increments_.west_east());
    ASSERT(bbox_.west() == 0);

    grid->domain(atlasDomain());
    return grid;
}


namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

