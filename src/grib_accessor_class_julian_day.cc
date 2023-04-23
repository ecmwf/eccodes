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
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = dump
   IMPLEMENTS = init
   MEMBERS    = const char *date
   MEMBERS    = const char *hour
   MEMBERS    = const char *minute
   MEMBERS    = const char *second
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_julian_day
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in julian_day */
    const char *date;
    const char *hour;
    const char *minute;
    const char *second;
} grib_accessor_julian_day;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_julian_day = {
    &grib_accessor_class_double,                      /* super */
    "julian_day",                      /* name */
    sizeof(grib_accessor_julian_day),  /* size */
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
    &pack_double,                /* pack_double */
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


grib_accessor_class* grib_accessor_class_julian_day = &_grib_accessor_class_julian_day;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_julian_day* self = (grib_accessor_julian_day*)a;
    int n                          = 0;

    self->date   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->hour   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->minute = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->second = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    const double v = *val;
    return pack_double(a, &v, len);
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_julian_day* self = (grib_accessor_julian_day*)a;
    int ret                        = 0;
    long hour                      = 0;
    long minute                    = 0;
    long second                    = 0;
    long date                      = 0;
    long year, month, day;

    ret = grib_julian_to_datetime(*val, &year, &month, &day, &hour, &minute, &second);
    if (ret != 0)
        return ret;

    date = year * 10000 + month * 100 + day;

    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->date, date);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->hour, hour);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->minute, minute);
    if (ret != 0)
        return ret;
    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->second, second);

    return ret;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret  = 0;
    double v = 0;

    ret  = unpack_double(a, &v, len);
    *val = (long)v;

    return ret;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    int ret = 0;
    long date, hour, minute, second;
    long year, month, day;
    grib_accessor_julian_day* self = (grib_accessor_julian_day*)a;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->date, &date);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->hour, &hour);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->minute, &minute);
    if (ret != GRIB_SUCCESS)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->second, &second);
    if (ret != GRIB_SUCCESS)
        return ret;

    year = date / 10000;
    date %= 10000;
    month = date / 100;
    date %= 100;
    day = date;

    ret = grib_datetime_to_julian(year, month, day, hour, minute, second, val);

    return ret;
}
