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


void Regular_fillGrib(grib_info& info) const
{
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_GG;

    info.grid.N                            = long(N_);
    info.grid.iDirectionIncrementInDegrees = getSmallestIncrement();
    info.grid.Ni                           = long(Ni_);
    info.grid.Nj                           = long(Nj_);

    bbox_.fillGrib(info);
}


}  // namespace eccodes::geo
