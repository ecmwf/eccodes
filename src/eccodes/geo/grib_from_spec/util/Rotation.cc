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


namespace mir::util {


void Rotation::fillGrib(grib_info& info) const {
    // Warning: scanning mode not considered

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;

    info.grid.latitudeOfSouthernPoleInDegrees  = rotation_.south_pole().lat;
    info.grid.longitudeOfSouthernPoleInDegrees = rotation_.south_pole().lon;

    // This is missing from the grib_spec
    // Remove that when supported
    if (!eckit::types::is_approximately_equal<double>(rotation_.angle(), 0.)) {
        info.extra_set("angleOfRotationInDegrees", rotation_.angle());
    }
}


}  // namespace mir::util
