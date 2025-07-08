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


namespace eccodes::geo
{


struct Rotation
{
    // -- Constructors

    Rotation(double south_pole_lat, double south_pole_lon, double south_pole_angle);

    // -- Methods

    void fillGrib(grib_info&) const;

private:
    // -- Members

    const double south_pole_lat_;
    const double south_pole_lon_;
    const double south_pole_angle_;
};


}  // namespace eccodes::geo
