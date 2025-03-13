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
   IMPLEMENTS = unpack_long;pack_long;init
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
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_g2date
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in g2date */
    const char* century;
    const char* year;
    const char* month;
    const char* day;
} grib_accessor_g2date;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_g2date = {
    &grib_accessor_class_long,                      /* super */
    "g2date",                      /* name */
    sizeof(grib_accessor_g2date),  /* size */
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


grib_accessor_class* grib_accessor_class_g2date = &_grib_accessor_class_g2date;

/* END_CLASS_IMP */
static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_g2date* self = (grib_accessor_g2date*)a;
    int n                      = 0;

    self->year  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->month = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->day   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    const grib_accessor_g2date* self = (grib_accessor_g2date*)a;

    int ret = 0;
    long year  = 0;
    long month = 0;
    long day   = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->day, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->month, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->year, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    val[0] = year * 10000 + month * 100 + day;

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    const grib_accessor_g2date* self = (grib_accessor_g2date*)a;

    int ret = GRIB_SUCCESS;
    long v = val[0];
    long year  = 0;
    long month = 0;
    long day   = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    year = v / 10000;
    v %= 10000;
    month = v / 100;
    v %= 100;
    day = v;

    if (!is_date_valid(year, month, day, 0, 0, 0)) {
        // ECC-1777: For now just a warning. Will later change to an error
        fprintf(stderr, "ECCODES WARNING :  %s:%s: Date is not valid! year=%ld month=%ld day=%ld\n",
                a->cclass->name, __func__, year, month, day);
    }

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->day, day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->month, month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->year, year)) != GRIB_SUCCESS)
        return ret;

    return GRIB_SUCCESS;
}
