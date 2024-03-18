
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_codetable_title.h"
#include "grib_accessor_class_codetable.h"

grib_accessor_class_codetable_title_t _grib_accessor_class_codetable_title{"codetable_title"};
grib_accessor_class* grib_accessor_class_codetable_title = &_grib_accessor_class_codetable_title;

void grib_accessor_class_codetable_title_t::init(grib_accessor* a, const long len, grib_arguments* params){
    grib_accessor_codetable_title_t* self = (grib_accessor_codetable_title_t*)a;
    int n                               = 0;
    self->codetable                     = grib_arguments_get_name(grib_handle_of_accessor(a), params, n++);
    a->length                           = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_codetable_title_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_codetable_title_t::unpack_string(grib_accessor* a, char* buffer, size_t* len){
    grib_accessor_codetable_title_t* self = (grib_accessor_codetable_title_t*)a;
    grib_codetable* table               = NULL;

    size_t size = 1;
    long value;
    int err = GRIB_SUCCESS;
    char tmp[1024];
    size_t l                    = 1024;
    grib_accessor_codetable_t* ca = (grib_accessor_codetable_t*)grib_find_accessor(grib_handle_of_accessor(a), self->codetable);

    if ((err = ((grib_accessor*)ca)->unpack_long(&value, &size)) != GRIB_SUCCESS)
        return err;

    table = ca->table;

    if (table && (value >= 0) && (value < table->size) && table->entries[value].title) {
        strcpy(tmp, table->entries[value].title);
    }
    else {
        snprintf(tmp, sizeof(tmp), "%d", (int)value);
    }

    l = strlen(tmp) + 1;

    if (*len < l) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(buffer, tmp);
    *len = l;

    return GRIB_SUCCESS;
}
