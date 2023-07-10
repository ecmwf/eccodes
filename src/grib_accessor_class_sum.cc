/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_long
   IMPLEMENTS = unpack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = init
   MEMBERS=const char*    values
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_sum
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in sum */
    const char*    values;
} grib_accessor_sum;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_sum = {
    &grib_accessor_class_double,                      /* super */
    "sum",                      /* name */
    sizeof(grib_accessor_sum),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
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
    &unpack_double,              /* unpack_double */
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


grib_accessor_class* grib_accessor_class_sum = &_grib_accessor_class_sum;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    int n                   = 0;
    self->values            = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->length               = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    int ret                 = 0;
    size_t size             = 0;
    long* values            = 0;
    long i;
    long count = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    size = count;

    if (size == 0) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    values = (long*)grib_context_malloc_clear(a->context, sizeof(long) * size);
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    grib_get_long_array(grib_handle_of_accessor(a), self->values, values, &size);

    *val = 0;
    for (i = 0; i < size; i++)
        *val += values[i];

    grib_context_free(a->context, values);

    return GRIB_SUCCESS;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    int ret                 = 0;
    size_t size             = 0;
    double* values          = 0;
    long i;
    long count = 0;

    ret = value_count(a, &count);
    if (ret)
        return ret;
    size = count;

    if (size == 0) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    values = (double*)grib_context_malloc_clear(a->context, sizeof(double) * size);
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    ret = grib_get_double_array(grib_handle_of_accessor(a), self->values, values, &size);
    if (ret) {
        grib_context_free(a->context, values);
        return ret;
    }
    *val = 0;
    for (i = 0; i < size; i++)
        *val += values[i];

    grib_context_free(a->context, values);

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_sum* self = (grib_accessor_sum*)a;
    size_t n                = 0;
    int ret                 = 0;

    ret    = grib_get_size(grib_handle_of_accessor(a), self->values, &n);
    *count = n;

    if (ret)
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s is unable to get size of %s", a->name, self->values);

    return ret;
}
