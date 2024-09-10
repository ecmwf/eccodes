
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

AccessorBuilder<grib_accessor_bufr_extract_subsets_t> _grib_accessor_bufr_extract_subsets_builder{};

void get_accessors(grib_accessor* a)
{
    const grib_handle* h                       = grib_handle_of_accessor(a);
    grib_accessor_bufr_extract_subsets_t* self = (grib_accessor_bufr_extract_subsets_t*)a;

    if (self->packAccessor_)
        return;
    self->numericValuesAccessor_ = grib_find_accessor(h, self->numericValues_);
    self->packAccessor_          = grib_find_accessor(h, self->pack_);
}

void grib_accessor_bufr_extract_subsets_t::init(const long len, grib_arguments* arg)
{
    grib_accessor_gen_t::init(len, arg);
    int n = 0;

    length_        = 0;
    numericValues_ = grib_arguments_get_name(grib_handle_of_accessor(this), arg, n++);
    pack_          = grib_arguments_get_name(grib_handle_of_accessor(this), arg, n++);
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;

    numericValuesAccessor_ = NULL;
    packAccessor_          = NULL;
}

long grib_accessor_bufr_extract_subsets_t::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int grib_accessor_bufr_extract_subsets_t::pack_long(const long* val, size_t* len)
{
    size_t l = 1;
    long v[1];

    get_accessors(this);

    v[0]    = 1;
    int err = packAccessor_->pack_long(v, &l);
    if (err) {
        if (err == GRIB_ENCODING_ERROR)
            grib_context_log(context_, GRIB_LOG_ERROR, "Could not extract subset(s).\n\tHint: Did you forget to set unpack=1?");
        return err;
    }

    return err;
}
