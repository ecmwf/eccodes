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


using grib_handle = struct grib_handle;


namespace eccodes::geo
{


// env variable ECCODES_ECKIT_GEO levels controlling eckit::geo usage
enum EckitGeoLevel
{
    DISABLED = 0,  // (or undefined) don't use eckit::geo
    RESTRICTED,    //  use for selected functionality only
    ENABLED        //  use for everything
};

void eckit_main_init();

bool eckit_geo_use_for_iterator(const grib_handle*);
bool eckit_geo_use_for_gridspec(const grib_handle*);
bool eckit_geo_use_for_projstring(const grib_handle*);
bool eckit_geo_use_grib_fixes(const grib_handle*);


}  // namespace eccodes::geo
