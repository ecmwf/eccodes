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
   IMPLEMENTS = init
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
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_budgdate
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in budgdate */
    const char* year;
    const char* month;
    const char* day;
} grib_accessor_budgdate;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_budgdate = {
    &grib_accessor_class_long,                      /* super */
    "budgdate",                      /* name */
    sizeof(grib_accessor_budgdate),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    0,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    &pack_long,                  /* grib_pack procedures long */
    &unpack_long,                /* grib_unpack procedures long */
    0,                /* grib_pack procedures double */
    0,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    0,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_budgdate = &_grib_accessor_class_budgdate;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->value_count    =    (*(c->super))->value_count;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->get_native_type    =    (*(c->super))->get_native_type;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_double    =    (*(c->super))->pack_double;
    c->unpack_double    =    (*(c->super))->unpack_double;
    c->pack_string    =    (*(c->super))->pack_string;
    c->unpack_string    =    (*(c->super))->unpack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_budgdate* self = (grib_accessor_budgdate*)a;
    int n                        = 0;

    self->year  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->month = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->day   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret                      = 0;
    grib_accessor_budgdate* self = (grib_accessor_budgdate*)a;

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

    val[0] = (1900 + year) * 10000 + month * 100 + day;

    return ret;
}

/* TODO: Check for a valid date */
static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int ret                      = 0;
    long v                       = val[0];
    grib_accessor_budgdate* self = (grib_accessor_budgdate*)a;

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

    year -= 1900;

    Assert(year < 255);

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->day, day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->month, month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->year, year)) != GRIB_SUCCESS)
        return ret;

    return ret;
}
