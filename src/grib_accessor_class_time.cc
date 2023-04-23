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
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_string
   IMPLEMENTS = init;dump
   MEMBERS=const char* hour
   MEMBERS=const char* minute
   MEMBERS=const char* second
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
static int unpack_string(grib_accessor*, char*, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_time
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in time */
    const char* hour;
    const char* minute;
    const char* second;
} grib_accessor_time;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_time = {
    &grib_accessor_class_long,                      /* super */
    "time",                      /* name */
    sizeof(grib_accessor_time),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
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


grib_accessor_class* grib_accessor_class_time = &_grib_accessor_class_time;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_time* self = (grib_accessor_time*)a;
    grib_handle* hand        = grib_handle_of_accessor(a);
    int n                    = 0;

    self->hour   = grib_arguments_get_name(hand, c, n++);
    self->minute = grib_arguments_get_name(hand, c, n++);
    self->second = grib_arguments_get_name(hand, c, n++);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret                  = 0;
    grib_accessor_time* self = (grib_accessor_time*)a;
    long hour                = 0;
    long minute              = 0;
    long second              = 0;
    grib_handle* hand        = grib_handle_of_accessor(a);

    if ((ret = grib_get_long_internal(hand, self->hour, &hour)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->minute, &minute)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->second, &second)) != GRIB_SUCCESS)
        return ret;

    /* We ignore the 'seconds' in our time calculation! */
    if (second != 0) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "Key %s (unpack_long): Truncating time: non-zero seconds(%ld) ignored", a->name, second);
    }

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    *val = hour * 100 + minute;

    if (hour == 255)
        *val = 12 * 100;

    if (hour != 255 && minute == 255)
        *val = hour * 100;

    return GRIB_SUCCESS;
}

/* TODO: Check for a valid date */

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int ret                  = 0;
    long v                   = val[0];
    grib_accessor_time* self = (grib_accessor_time*)a;
    grib_handle* hand        = grib_handle_of_accessor(a);
    long hour                = 0;
    long minute              = 0;
    long second              = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    hour   = v / 100;
    minute = v % 100;
    second = 0; /* We ignore the 'seconds' in our time calculation! */

    if ((ret = grib_set_long_internal(hand, self->hour, hour)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->minute, minute)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->second, second)) != GRIB_SUCCESS)
        return ret;

    return GRIB_SUCCESS;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    long v       = 0;
    size_t lsize = 1;

    unpack_long(a, &v, &lsize);

    if (*len < 5) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Key %s (unpack_string): Buffer too small", a->name);

        *len = 5;
        return GRIB_BUFFER_TOO_SMALL;
    }

    snprintf(val, 64, "%04ld", v);

    len[0] = 5;
    return GRIB_SUCCESS;
}
