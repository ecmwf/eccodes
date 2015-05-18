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

#include "atlas/grids/LonLatGrid.h"

#include "eckit/exception/Exceptions.h"

#include "mir/util/Grib.h"


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
RegularLL* RegularLL::cropped(const util::BoundingBox &bbox) const {
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

atlas::Grid *RegularLL::atlasGrid() const {

    return new atlas::grids::LonLatGrid(int(ni_),
                                        int(nj_),
                                        atlas::grids::LonLatGrid::INCLUDES_POLES);
}



namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}

}  // namespace latlon
}  // namespace repres
}  // namespace mir

