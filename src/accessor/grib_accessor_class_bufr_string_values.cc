
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_bufr_string_values.h"
#include "grib_accessor_class_bufr_data_array.h"

grib_accessor_class_bufr_string_values_t _grib_accessor_class_bufr_string_values{"bufr_string_values"};
grib_accessor_class* grib_accessor_class_bufr_string_values = &_grib_accessor_class_bufr_string_values;


void grib_accessor_class_bufr_string_values_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_ascii_t::init(a, len, args);
    grib_accessor_bufr_string_values_t* self = (grib_accessor_bufr_string_values_t*)a;
    int n                                  = 0;
    self->dataAccessorName                 = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->dataAccessor                     = NULL;
    a->length                              = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_class_bufr_string_values_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string_array(dumper, a, NULL);
}

static grib_accessor* get_accessor(grib_accessor* a)
{
    grib_accessor_bufr_string_values_t* self = (grib_accessor_bufr_string_values_t*)a;
    if (!self->dataAccessor) {
        self->dataAccessor = grib_find_accessor(grib_handle_of_accessor(a), self->dataAccessorName);
    }
    return self->dataAccessor;
}

int grib_accessor_class_bufr_string_values_t::unpack_string_array(grib_accessor* a, char** buffer, size_t* len){
    grib_accessor* data        = 0;
    grib_context* c            = a->context;
    grib_vsarray* stringValues = NULL;
    size_t l = 0, tl;
    size_t i, j, n = 0;
    char** b = buffer;

    data = get_accessor(a);
    if (!data)
        return GRIB_NOT_FOUND;

    stringValues = accessor_bufr_data_array_get_stringValues(data);

    n = grib_vsarray_used_size(stringValues);

    tl = 0;
    for (j = 0; j < n; j++) {
        l = grib_sarray_used_size(stringValues->v[j]);
        tl += l;

        if (tl > *len)
            return GRIB_ARRAY_TOO_SMALL;

        for (i = 0; i < l; i++) {
            *(b++) = grib_context_strdup(c, stringValues->v[j]->v[i]);
        }
    }
    *len = tl;

    return GRIB_SUCCESS;
}

int grib_accessor_class_bufr_string_values_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_bufr_string_values_t::value_count(grib_accessor* a, long* rlen){
    grib_accessor* descriptors = get_accessor(a);
    return descriptors->value_count(rlen);}

void grib_accessor_class_bufr_string_values_t::destroy(grib_context* c, grib_accessor* a){
    grib_accessor_class_ascii_t::destroy(c, a);
}
