/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/************************************
 *  Enrico Fucile
 ***********************************/

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long;pack_long;unpack_string
   IMPLEMENTS = init;dump;value_count
   MEMBERS=const char* century
   MEMBERS=const char* year
   MEMBERS=const char* month
   MEMBERS=const char* day
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
static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g1date
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in g1date */
    const char* century;
    const char* year;
    const char* month;
    const char* day;
} grib_accessor_g1date;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_g1date = {
    &grib_accessor_class_long,                      /* super */
    "g1date",                      /* name */
    sizeof(grib_accessor_g1date),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
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


grib_accessor_class* grib_accessor_class_g1date = &_grib_accessor_class_g1date;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_g1date* self = (grib_accessor_g1date*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);
    int n                      = 0;

    self->century = grib_arguments_get_name(hand, c, n++);
    self->year    = grib_arguments_get_name(hand, c, n++);
    self->month   = grib_arguments_get_name(hand, c, n++);
    self->day     = grib_arguments_get_name(hand, c, n++);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret                    = 0;
    grib_accessor_g1date* self = (grib_accessor_g1date*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);

    long year    = 0;
    long century = 0;
    long month   = 0;
    long day     = 0;

    if ((ret = grib_get_long_internal(hand, self->century, &century)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->day, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->month, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->year, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    *val = ((century - 1) * 100 + year) * 10000 + month * 100 + day;

    if (year == 255 && day == 255 && month >= 1 && month <= 12) {
        *val = month;
    }

    if (year == 255 && day != 255 && month >= 1 && month <= 12) {
        *val = month * 100 + day;
    }

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int ret                    = 0;
    long v                     = val[0];
    grib_accessor_g1date* self = (grib_accessor_g1date*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);

    long year    = 0;
    long century = 0;
    long month   = 0;
    long day     = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    {
        long d = grib_julian_to_date((long)grib_date_to_julian(v));
        if (v != d) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_g1date: pack_long invalid date %ld, changed to %ld", v, d);
            return GRIB_ENCODING_ERROR;
        }
    }

    century = v / 1000000;
    v %= 1000000;
    year = v / 10000;
    v %= 10000;
    month = v / 100;
    v %= 100;
    day = v;

    if (year == 0)
        year = 100;
    else
        century++;

    if ((ret = grib_set_long_internal(hand, self->century, century)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->day, day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->month, month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->year, year)) != GRIB_SUCCESS)
        return ret;

    return GRIB_SUCCESS;
}

static const char* months[] = {
    "jan",
    "feb",
    "mar",
    "apr",
    "may",
    "jun",
    "jul",
    "aug",
    "sep",
    "oct",
    "nov",
    "dec",
};

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    int ret = 0;
    char tmp[1024];
    grib_accessor_g1date* self = (grib_accessor_g1date*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);
    long year                  = 0;
    long century               = 0;
    long month                 = 0;
    long day                   = 0;
    size_t l;

    if ((ret = grib_get_long_internal(hand, self->century, &century)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->day, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->month, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->year, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    if (year == 255 && day == 255 && month >= 1 && month <= 12) {
        strcpy(tmp, months[month - 1]);
    }
    else if (year == 255 && month >= 1 && month <= 12) {
        snprintf(tmp, sizeof(tmp), "%s-%02ld", months[month - 1], day);
        /* snprintf(tmp,sizeof(tmp),"%02ld-%02ld",month,day); */
    }
    else {
        long x = ((century - 1) * 100 + year) * 10000 + month * 100 + day;
        snprintf(tmp, sizeof(tmp), "%ld", x);
    }

    l = strlen(tmp) + 1;
    if (*len < l) {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    *len = l;
    strcpy(val, tmp);

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}
