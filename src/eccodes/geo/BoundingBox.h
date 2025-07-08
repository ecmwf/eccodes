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


#pragma once

#include "eccodes/geo/Geo.h"


namespace eckit::geo::area
{
class BoundingBox;
}


namespace eccodes::geo
{


struct BoundingBox
{
    BoundingBox(double _north, double _west, double _south, double _east) :
        north(_north), west(_west), south(_south), east(_east) {}

    explicit BoundingBox(const ::eckit::geo::area::BoundingBox&);

    void fillGrib(grib_info&) const;

    const double north;
    const double west;
    const double south;
    const double east;
};


}  // namespace eccodes::geo
