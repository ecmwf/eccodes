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


#include "eccodes/geo/BoundingBox.h"

#include "eckit/geo/area/BoundingBox.h"


namespace eccodes::geo
{


BoundingBox::BoundingBox(const eckit::geo::area::BoundingBox& bbox) :
    BoundingBox(bbox.north, bbox.west, bbox.south, bbox.east)
{
}


void BoundingBox::fillGrib(grib_info& info) const
{
    // Warning: scanning mode not considered
    info.grid.latitudeOfFirstGridPointInDegrees  = north;
    info.grid.longitudeOfFirstGridPointInDegrees = west;
    info.grid.latitudeOfLastGridPointInDegrees   = south;
    info.grid.longitudeOfLastGridPointInDegrees  = east;

    info.extra_set("expandBoundingBox", 1L);
}


}  // namespace eccodes::geo
