/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <map>
#include <string>

#include "eccodes/geo/Geo.h"


namespace eccodes::geo
{


class GribFromSpec
{
public:
    enum ProjectionType
    {
        UNROTATED = 0,
        ROTATED,
        LAMBERT_AZIMUTHAL_EQUAL_AREA,
        LAMBERT_CONFORMAL_CONIC,
        POLAR_STEREOGRAPHIC,
    };

    enum LatitudeType
    {
        LAT_REGULAR = 0,
        LAT_GAUSSIAN,
        LAT_UNSTRUCTURED,
    };

    enum LongitudeType
    {
        LON_REGULAR = 0,
        LON_REDUCED,
        LON_UNSTRUCTURED,
    };

    enum BasicAngleType
    {
        DECIMAL,
        AS_INPUT,
        FRACTION
    };

    explicit GribFromSpec(BasicAngleType basicAngleType = DECIMAL) :
        basicAngleFormat_(basicAngleType) {}

    int set(codes_handle*, const Spec&, const std::map<std::string, long>& extra = {});

private:
    const BasicAngleType basicAngleFormat_;
};


}  // namespace eccodes::geo
