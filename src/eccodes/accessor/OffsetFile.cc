/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "OffsetFile.h"

eccodes::accessor::OffsetFile _grib_accessor_offset_file;
eccodes::Accessor* grib_accessor_offset_file = &_grib_accessor_offset_file;

namespace eccodes::accessor
{

void OffsetFile::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

int OffsetFile::unpack_double(double* val, size_t* len)
{
    *val = (double)(get_enclosing_handle()->offset);
    *len = 1;
    return GRIB_SUCCESS;
}

int OffsetFile::unpack_string(char* v, size_t* len)
{
    double val        = 0;
    size_t l          = 1;
    char repres[1024] = {0,};
    int err = 0;

    err = unpack_double(&val, &l);
    if (err) return err;

    snprintf(repres, sizeof(repres), "%.0f", val);

    l = strlen(repres) + 1;
    if (l > *len) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         class_name_, name_, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }
    grib_context_log(context_, GRIB_LOG_DEBUG, "%s: Casting double %s to string", __func__, name_);

    *len = l;

    strcpy(v, repres);
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
