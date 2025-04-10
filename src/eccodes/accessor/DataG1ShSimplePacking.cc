/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataG1ShSimplePacking.h"

eccodes::accessor::DataG1ShSimplePacking _grib_accessor_data_g1shsimple_packing;
eccodes::Accessor* grib_accessor_data_g1shsimple_packing = &_grib_accessor_data_g1shsimple_packing;

namespace eccodes::accessor
{

int DataG1ShSimplePacking::value_count(long* count)
{
    size_t len = 0;
    int err    = 0;

    err = grib_get_size(get_enclosing_handle(), coded_values_, &len);
    len += 1;

    *count = len;
    return err;
}

int DataG1ShSimplePacking::unpack_double(double* val, size_t* len)
{
    int err = GRIB_SUCCESS;

    size_t coded_n_vals = 0;
    size_t n_vals       = 0;

    if ((err = grib_get_size(get_enclosing_handle(), coded_values_, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    n_vals = coded_n_vals + 1;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_double_internal(get_enclosing_handle(), real_part_, val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_get_double_array_internal(get_enclosing_handle(), coded_values_, val, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    grib_context_log(context_, GRIB_LOG_DEBUG,
                     "DataG1shsimplePacking_bitmap : unpack_double : creating %s, %d values",
                     name_, n_vals);

    *len = n_vals;

    return err;
}

}  // namespace eccodes::accessor
