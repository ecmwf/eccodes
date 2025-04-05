/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "CodetableTitle.h"
#include "Codetable.h"

eccodes::accessor::CodetableTitle _grib_accessor_codetable_title;
eccodes::Accessor* grib_accessor_codetable_title = &_grib_accessor_codetable_title;

namespace eccodes::accessor
{

void CodetableTitle::init(const long len, grib_arguments* params)
{
    Gen::init(len, params);
    int n      = 0;
    codetable_ = params->get_name(get_enclosing_handle(), n++);
    length_    = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

long CodetableTitle::get_native_type()
{
    return GRIB_TYPE_STRING;
}

int CodetableTitle::unpack_string(char* buffer, size_t* len)
{
    const grib_codetable* table = NULL;

    size_t size = 1;
    long value;
    int err = GRIB_SUCCESS;
    char tmp[1024];
    size_t l = 1024;
    accessor::Codetable* ca = (accessor::Codetable*)grib_find_accessor(grib_handle_of_accessor(this), codetable_);

    if ((err = ((grib_accessor*)ca)->unpack_long(&value, &size)) != GRIB_SUCCESS)
        return err;

    table = ca->codetable();

    if (table && (value >= 0) && (value < table->size) && table->entries[value].title) {
        strcpy(tmp, table->entries[value].title);
    }
    else {
        snprintf(tmp, sizeof(tmp), "%d", (int)value);
    }

    l = strlen(tmp) + 1;

    if (*len < l) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         class_name_, name_, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(buffer, tmp);
    *len = l;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
