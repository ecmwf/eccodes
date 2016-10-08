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
#include "eckit/types/FloatCompare.h"
#include "atlas/grid/lonlat/RegularLonLat.h"
#include "atlas/grid/lonlat/ShiftedLat.h"
#include "atlas/grid/lonlat/ShiftedLon.h"
#include "atlas/grid/lonlat/ShiftedLonLat.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"
#include "mir/util/OffsetGrid.h"


namespace mir {
namespace repres {
namespace latlon {


RegularLL::RegularLL(const param::MIRParametrisation &parametrisation):
    LatLon(parametrisation),
    bboxDefinesGrid_(false) {
    parametrisation.get("bounding-box-defines-grid", bboxDefinesGrid_);
}


RegularLL::RegularLL(const util::BoundingBox &bbox,
                     const util::Increments &increments,
                     bool bboxDefinesGrid) :
    LatLon(bbox, increments),
    bboxDefinesGrid_(bboxDefinesGrid) {
}


RegularLL::~RegularLL() {
}


// Called by RegularLL::crop()
const RegularLL *RegularLL::cropped(const util::BoundingBox &bbox) const {
    // eckit::Log::debug<LibMir>() << "Create cropped copy as RegularLL bbox=" << bbox << std::endl;
    return new RegularLL(bbox, increments_, bboxDefinesGrid_);
}


void RegularLL::print(std::ostream &out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << ",bboxDefinesGrid=" << bboxDefinesGrid_
        << "]";
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


atlas::grid::lonlat::Shift RegularLL::atlasShift() const {
    typedef eckit::FloatCompare<double> cmp;

    // locate latitude/longitude origin via accumulation of increments, in range [0,inc[
    // NOTE: shift is assumed half-increment origin dispacement; Domain is checked for
    // global NS/EW range (this could/should be revised).
    const double inc_we = increments_.west_east();
    const double inc_sn = increments_.south_north();
    ASSERT(cmp::isStrictlyGreater(inc_we, 0));
    ASSERT(cmp::isStrictlyGreater(inc_sn, 0));

    int i = 0, j = 0;
    while (bbox_.west()  + i * inc_we < inc_we) { ++i; }
    while (bbox_.west()  + i * inc_we > inc_we) { --i; }
    while (bbox_.south() + j * inc_sn < inc_sn) { ++j; }
    while (bbox_.south() + j * inc_sn > inc_sn) { --j; }
    const double
    lon_origin = bbox_.west()  + i * inc_we,
    lat_origin = bbox_.south() + j * inc_sn;

    const atlas::grid::Domain dom = atlasDomain();
    const bool
    includesBothPoles = dom.includesPoleNorth() && dom.includesPoleSouth(),
    isShiftedLon = dom.isPeriodicEastWest() && cmp::isApproximatelyEqual(lon_origin, inc_we / 2.),
    isShiftedLat = includesBothPoles        && cmp::isApproximatelyEqual(lat_origin, inc_sn / 2.);

    return atlas::grid::lonlat::Shift(isShiftedLon, isShiftedLat);
}


atlas::grid::Grid* RegularLL::atlasGrid() const {
    using namespace atlas::grid::lonlat;
    const Shift shift = atlasShift();

    if (bboxDefinesGrid_) {
        ASSERT(!shift(Shift::LON | Shift::LAT));
        ASSERT(!shift(Shift::LON));
        ASSERT(!shift(Shift::LAT));

        return new util::OffsetGrid(new RegularLonLat (ni_, nj_, atlasDomain()), 0.5, 0.5);

    }

    // return non-shifted/shifted grid
    return shift(Shift::LON | Shift::LAT) ? static_cast<LonLat*>(new ShiftedLonLat (ni_, nj_, atlasDomain()))
           : shift(Shift::LON) ?            static_cast<LonLat*>(new ShiftedLon    (ni_, nj_, atlasDomain()))
           : shift(Shift::LAT) ?            static_cast<LonLat*>(new ShiftedLat    (ni_, nj_, atlasDomain()))
           :                               static_cast<LonLat*>(new RegularLonLat (ni_, nj_, atlasDomain()));
}


namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

