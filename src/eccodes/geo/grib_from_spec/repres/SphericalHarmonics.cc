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


void SphericalHarmonics_fillGrib(grib_info& info) const
{
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type  = CODES_UTIL_GRID_SPEC_SH;
    info.grid.truncation = static_cast<long>(truncation_);

    // MIR-131: repacking must happen, so computeLaplacianOperator is set
    info.packing.packing_type             = CODES_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX;
    info.packing.computeLaplacianOperator = 1;
    info.packing.truncateLaplacian        = 1;
    // info.packing.laplacianOperator = 0;
}


}  // namespace eccodes::geo
