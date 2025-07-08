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


void RegularGrid_fillGrib(grib_info& info) const
{
    // shape of the reference system
    shape_.fillGrib(info, grid_.projection().spec());

    // scanningMode
    info.grid.iScansNegatively = x_.front() < x_.back() ? 0L : 1L;
    info.grid.jScansPositively = y_.front() < y_.back() ? 1L : 0L;
}


}  // namespace eccodes::geo
