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
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_evaluate
   IMPLEMENTS = unpack_double
   IMPLEMENTS = unpack_string
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_round
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in evaluate */
    grib_arguments* arg;
    /* Members defined in round */
} grib_accessor_round;

extern grib_accessor_class* grib_accessor_class_evaluate;

static grib_accessor_class _grib_accessor_class_round = {
    &grib_accessor_class_evaluate,                      /* super */
    "round",                      /* name */
    sizeof(grib_accessor_round),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    0,                       /* init */
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
    0,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    &unpack_string,              /* unpack_string */
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


grib_accessor_class* grib_accessor_class_round = &_grib_accessor_class_round;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_round* self = (grib_accessor_round*)a;

    int ret = GRIB_SUCCESS;

    size_t replen             = 0;
    double rounding_precision = 0;
    double rounded            = 0;
    double toround            = 0;

    const char* oval = NULL;
    oval             = grib_arguments_get_name(grib_handle_of_accessor(a), self->arg, 0);

    if ((ret = grib_get_double_internal(grib_handle_of_accessor(a), oval, &toround)) != 0)
        return ret;

    rounding_precision = grib_arguments_get_long(grib_handle_of_accessor(a), self->arg, 1);

    rounded = floor(rounding_precision * toround + 0.5) / rounding_precision;

    *len = replen;

    *val = rounded;

    return ret;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    char result[1024];
    int ret       = GRIB_SUCCESS;
    size_t replen = 1;

    double value = 0;

    ret = unpack_double(a, &value, &replen);

    snprintf(result, sizeof(result), "%.3f", value);

    replen = strlen(result) + 1;

    if (*len < replen) {
        *len = replen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = replen;

    snprintf(val, 1024, "%s", result);

    return ret;
}
