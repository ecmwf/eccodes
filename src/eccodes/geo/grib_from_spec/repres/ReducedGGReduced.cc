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


void Reduced_fillGrib(grib_info& info) const
{
    // See copy_spec_from_ksec.c in libemos for info

    const auto& pl = pls();

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj        = long(Nj_);
    info.grid.N         = long(N_);
    info.grid.pl        = &pl[k_];
    info.grid.pl_size   = long(Nj_);

    for (size_t i = k_; i < k_ + Nj_; i++) {
        ASSERT(pl[i] > 0);
    }

    bbox_.fillGrib(info);
}


}  // namespace eccodes::geo
