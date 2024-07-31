/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_g1shsimple_packing.h"

grib_accessor_class_data_g1shsimple_packing_t _grib_accessor_class_data_g1shsimple_packing{ "data_g1shsimple_packing" };
grib_accessor_class* grib_accessor_class_data_g1shsimple_packing = &_grib_accessor_class_data_g1shsimple_packing;


int grib_accessor_class_data_g1shsimple_packing_t::value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_g1shsimple_packing_t* self = (grib_accessor_data_g1shsimple_packing_t*)a;
    size_t len                                    = 0;
    int err                                       = 0;

    err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &len);
    len += 1;

    *count = len;
    return err;
}

int grib_accessor_class_data_g1shsimple_packing_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_data_g1shsimple_packing_t* self = (grib_accessor_data_g1shsimple_packing_t*)a;
    int err                                       = GRIB_SUCCESS;

    size_t coded_n_vals = 0;
    size_t n_vals       = 0;

    if ((err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    n_vals = coded_n_vals + 1;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->real_part, val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_get_double_array_internal(grib_handle_of_accessor(a), self->coded_values, val, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_data_g1shsimple_packing_t_bitmap : unpack_double : creating %s, %d values",
                     a->name, n_vals);

    *len = n_vals;

    return err;
}
