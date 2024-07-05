
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_bufr_extract_subsets.h"

grib_accessor_class_bufr_extract_subsets_t _grib_accessor_class_bufr_extract_subsets{ "bufr_extract_subsets" };
grib_accessor_class* grib_accessor_class_bufr_extract_subsets = &_grib_accessor_class_bufr_extract_subsets;


void get_accessors(grib_accessor* a)
{
    grib_accessor_bufr_extract_subsets_t* self = (grib_accessor_bufr_extract_subsets_t*)a;
    const grib_handle* h = grib_handle_of_accessor(a);

    if (self->packAccessor)
        return;
    self->numericValuesAccessor = grib_find_accessor(h, self->numericValues);
    self->packAccessor          = grib_find_accessor(h, self->pack);
}

void grib_accessor_class_bufr_extract_subsets_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    grib_accessor_bufr_extract_subsets_t* self = (grib_accessor_bufr_extract_subsets_t*)a;
    int n = 0;

    a->length           = 0;
    self->numericValues = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->pack          = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

int grib_accessor_class_bufr_extract_subsets_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

int grib_accessor_class_bufr_extract_subsets_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_bufr_extract_subsets_t* self = (grib_accessor_bufr_extract_subsets_t*)a;
    size_t l = 1;
    long v[1];

    get_accessors(a);

    v[0] = 1;
    int err = self->packAccessor->pack_long(v, &l);
    if (err) {
        if (err == GRIB_ENCODING_ERROR)
            grib_context_log(a->context, GRIB_LOG_ERROR, "Could not extract subset(s).\n\tHint: Did you forget to set unpack=1?");
        return err;
    }

    return err;
}
