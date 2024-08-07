/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_shsimple_packing.h"

grib_accessor_class_data_shsimple_packing_t _grib_accessor_class_data_shsimple_packing{ "data_shsimple_packing" };
grib_accessor_class* grib_accessor_class_data_shsimple_packing = &_grib_accessor_class_data_shsimple_packing;


void grib_accessor_class_data_shsimple_packing_t::init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_class_gen_t::init(a, v, args);
    grib_accessor_data_shsimple_packing_t* self = (grib_accessor_data_shsimple_packing_t*)a;

    self->coded_values = grib_arguments_get_name(grib_handle_of_accessor(a), args, 0);
    self->real_part    = grib_arguments_get_name(grib_handle_of_accessor(a), args, 1);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;

    a->length = 0;
}

void grib_accessor_class_data_shsimple_packing_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper, a);
}

int grib_accessor_class_data_shsimple_packing_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_shsimple_packing_t* self = (grib_accessor_data_shsimple_packing_t*)a;
    int err                                     = GRIB_SUCCESS;

    size_t coded_n_vals = *len - 1;
    size_t n_vals       = *len;

    self->dirty = 1;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((err = grib_set_double_internal(grib_handle_of_accessor(a), self->real_part, *val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_set_double_array_internal(grib_handle_of_accessor(a), self->coded_values, val, coded_n_vals)) != GRIB_SUCCESS)
        return err;

    *len = n_vals;

    return err;
}

int grib_accessor_class_data_shsimple_packing_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}
