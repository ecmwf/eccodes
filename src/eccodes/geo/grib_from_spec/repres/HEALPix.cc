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


namespace eccodes::geo
{


void HEALPix_fillGrib(grib_info& info) const
{
    info.grid.grid_type                          = GRIB_UTIL_GRID_SPEC_HEALPIX;
    info.grid.N                                  = static_cast<long>(grid_->Nside());
    info.grid.longitudeOfFirstGridPointInDegrees = 45.;

    info.extra_set("orderingConvention", grid_->order().c_str());
}


}  // namespace eccodes::geo
