/*
 * (C) Copyright 2025- ECMWF.
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

#include "eccodes/geo/BasicAngle.h"
#include "eccodes/geo/Geo.h"


namespace eccodes::geo
{


class GribFromSpec
{
public:
    static int set(codes_handle*&, const Spec&, const std::map<std::string, long>& extra = {}, const BasicAngle& = {});
};


}  // namespace eccodes::geo
