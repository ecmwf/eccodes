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
 *********************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long; pack_long
   IMPLEMENTS = init
   MEMBERS = const char* values
   MEMBERS = const char* bits_per_value
   MEMBERS = const char* changing_precision
   MEMBERS = const char* decimal_scale_factor
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_decimal_precision
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in decimal_precision */
    const char* values;
    const char* bits_per_value;
    const char* changing_precision;
    const char* decimal_scale_factor;
} grib_accessor_decimal_precision;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_decimal_precision = {
    &grib_accessor_class_long,                      /* super */
    "decimal_precision",                      /* name */
    sizeof(grib_accessor_decimal_precision),  /* size */
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
    &pack_long,                  /* pack_long */
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


grib_accessor_class* grib_accessor_class_decimal_precision = &_grib_accessor_class_decimal_precision;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* args)
{
    int n                                 = 0;
    grib_accessor_decimal_precision* self = (grib_accessor_decimal_precision*)a;

    self->bits_per_value       = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->decimal_scale_factor = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->changing_precision   = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->values               = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret                               = 0;
    grib_accessor_decimal_precision* self = (grib_accessor_decimal_precision*)a;
    grib_handle* h                        = grib_handle_of_accessor(a);

    if ((ret = grib_get_long_internal(h, self->decimal_scale_factor, val)) != GRIB_SUCCESS)
        return ret;

    *len = 1;
    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    long bitsPerValue                     = 0;
    double* values                        = NULL;
    size_t size                           = 0;
    int ret                               = 0;
    grib_accessor_decimal_precision* self = (grib_accessor_decimal_precision*)a;
    grib_context* c                       = a->context;
    grib_handle* h                        = grib_handle_of_accessor(a);

    if (!self->values) {
        if ((ret = grib_set_long_internal(h, self->bits_per_value, 0)) != GRIB_SUCCESS)
            return ret;

        if ((ret = grib_set_long_internal(h, self->decimal_scale_factor, *val)) != GRIB_SUCCESS)
            return ret;

        if ((ret = grib_set_long_internal(h, self->changing_precision, 1)) != GRIB_SUCCESS) {
            grib_context_free(c, values);
            return ret;
        }

        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_buffer_free(c, values);
        return ret;
    }

    if ((ret = grib_set_long_internal(h, self->decimal_scale_factor, *val)) != GRIB_SUCCESS) {
        grib_context_buffer_free(c, values);
        return ret;
    }

    if ((ret = grib_set_long_internal(h, self->bits_per_value, bitsPerValue)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_long_internal(h, self->changing_precision, 1)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    if ((ret = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS) {
        grib_context_buffer_free(c, values);
        return ret;
    }

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}
