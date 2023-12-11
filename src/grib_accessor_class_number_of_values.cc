/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**********************************
 *  Enrico Fucile
 **********************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init
   MEMBERS = const char* values
   MEMBERS = const char* bitsPerValue
   MEMBERS = const char* numberOfPoints
   MEMBERS = const char* bitmapPresent
   MEMBERS = const char* bitmap
   MEMBERS = const char* numberOfCodedValues
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_number_of_values
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in number_of_values */
    const char* values;
    const char* bitsPerValue;
    const char* numberOfPoints;
    const char* bitmapPresent;
    const char* bitmap;
    const char* numberOfCodedValues;
} grib_accessor_number_of_values;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_number_of_values = {
    &grib_accessor_class_long,                      /* super */
    "number_of_values",                      /* name */
    sizeof(grib_accessor_number_of_values),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_number_of_values = &_grib_accessor_class_number_of_values;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    int n                                = 0;
    grib_accessor_number_of_values* self = (grib_accessor_number_of_values*)a;
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

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_number_of_values* self = (grib_accessor_number_of_values*)a;
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
