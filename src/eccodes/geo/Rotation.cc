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


#include "eccodes/geo/Rotation.h"

#include "eckit/geo/projection/Rotation.h"
#include "eckit/types/FloatCompare.h"


namespace eccodes::geo
{


Rotation::Rotation(const ::eckit::geo::projection::Rotation& r) :
    Rotation(r.south_pole().lat,
             r.south_pole().lon,
             r.angle())
{
}


Rotation::Rotation(double south_pole_lat, double south_pole_lon, double south_pole_angle) :
    south_pole_lat_(south_pole_lat), south_pole_lon_(south_pole_lon), south_pole_angle_(south_pole_angle) {}


void Rotation::fillGrib(grib_info& info) const
{
    // Warning: scanning mode not considered

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;

    info.grid.latitudeOfSouthernPoleInDegrees  = south_pole_lat_;
    info.grid.longitudeOfSouthernPoleInDegrees = south_pole_lon_;

    // This is missing from the grib_spec
    // Remove that when supported
    if (!eckit::types::is_approximately_equal<double>(south_pole_angle_, 0.)) {
        info.extra_set("angleOfRotationInDegrees", south_pole_angle_);
    }
}


}  // namespace eccodes::geo
