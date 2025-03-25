/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "PackingType.h"

eccodes::AccessorBuilder<eccodes::accessor::PackingType> _grib_accessor_packing_type_builder{};

namespace eccodes::accessor
{

void PackingType::init(const long l, grib_arguments* args)
{
    Gen::init(l, args);
    int n         = 0;
    values_       = args->get_name(grib_handle_of_accessor(this), n++);
    packing_type_ = args->get_name(grib_handle_of_accessor(this), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

size_t PackingType::string_length()
{
    return 1024;
}

long PackingType::get_native_type()
{
    return GRIB_TYPE_STRING;
}

int PackingType::pack_string(const char* sval, size_t* len)
{
    grib_handle* h  = grib_handle_of_accessor(this);
    double* values  = NULL;
    grib_context* c = context_;
    size_t size     = 0;
    int err         = 0;

    if ((err = grib_get_size(h, values_, &size)) != GRIB_SUCCESS)
        return err;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, values_, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_string_internal(h, packing_type_, sval, len)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_double_array_internal(h, values_, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    grib_context_free(c, values);
    return GRIB_SUCCESS;
}

int PackingType::unpack_string(char* val, size_t* len)
{
    grib_handle* h = grib_handle_of_accessor(this);

    return grib_get_string(h, packing_type_, val, len);
}

}  // namespace eccodes::accessor
