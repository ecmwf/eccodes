
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
#include "grib_accessor_class_number_of_values.h"

grib_accessor_class_number_of_values_t _grib_accessor_class_number_of_values{"number_of_values"};
grib_accessor_class* grib_accessor_class_number_of_values = &_grib_accessor_class_number_of_values;


void grib_accessor_class_number_of_values_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_long_t::init(a, l, c);
    int n                                = 0;
    grib_accessor_number_of_values_t* self = (grib_accessor_number_of_values_t*)a;
    grib_handle* hand                    = grib_handle_of_accessor(a);

    self->values              = grib_arguments_get_name(hand, c, n++);
    self->bitsPerValue        = grib_arguments_get_name(hand, c, n++);
    self->numberOfPoints      = grib_arguments_get_name(hand, c, n++);
    self->bitmapPresent       = grib_arguments_get_name(hand, c, n++);
    self->bitmap              = grib_arguments_get_name(hand, c, n++);
    self->numberOfCodedValues = grib_arguments_get_name(hand, c, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;

    a->length = 0;
}

int grib_accessor_class_number_of_values_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_number_of_values_t* self = (grib_accessor_number_of_values_t*)a;
    int ret                              = GRIB_SUCCESS, i;
    long npoints = 0, bitmap_present = 0;
    size_t size = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfPoints, &npoints)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->bitmapPresent, &bitmap_present)) != GRIB_SUCCESS)
        return ret;

    if (bitmap_present) {
        double* bitmap;
        size   = npoints;
        bitmap = (double*)grib_context_malloc(a->context, sizeof(double) * size);
        if ((ret = grib_get_double_array_internal(grib_handle_of_accessor(a), self->bitmap, bitmap, &size)) != GRIB_SUCCESS) {
            grib_context_free(a->context, bitmap);
            return ret;
        }
        *val = 0;
        for (i = 0; i < size; i++)
            if (bitmap[i] != 0)
                (*val)++;

        grib_context_free(a->context, bitmap);
    }
    else {
        *val = npoints;
    }

    return ret;
}
