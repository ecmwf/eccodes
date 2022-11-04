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
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init;dump
   MEMBERS=const char* date
   MEMBERS=const char* time
   MEMBERS=const char* step
   MEMBERS=const char* stepUnits
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

static int unpack_long(grib_accessor*, long* val, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_validity_date
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in validity_date */
    const char* date;
    const char* time;
    const char* step;
    const char* stepUnits;
    const char* year;
    const char* month;
    const char* day;
} grib_accessor_validity_date;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_validity_date = {
    &grib_accessor_class_long,                      /* super */
    "validity_date",                      /* name */
    sizeof(grib_accessor_validity_date),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    &dump,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    0,                  /* grib_pack procedures long */
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


grib_accessor_class* grib_accessor_class_validity_date = &_grib_accessor_class_validity_date;


static void init_class(grib_accessor_class* c)
{
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->value_count    =    (*(c->super))->value_count;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->get_native_type    =    (*(c->super))->get_native_type;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_long    =    (*(c->super))->pack_long;
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

/* Table of multipliers to convert step units to minutes */
static const double u2m[] = {
    1,            /* index 0:  minutes  */
    60,           /* index 1:  hour     */
    24 * 60,      /* index 2:  day      */
    24 * 60 * 30, /* index 3:  month    */
    -1,           /* index 4:  year     */
    -1,           /* index 5:  decade   */
    -1,           /* index 6:  30 years */
    -1,           /* index 7:  century  */
    -1,           /* index 8:  RESERVED */
    -1,           /* index 9:  RESERVED */
    3 * 60,       /* index 10: 3 hours  */
    6 * 60,       /* index 11: 6 hours  */
    12 * 60,      /* index 12: 12 hours */
    1 / 60.0,     /* index 13: seconds  */
    15,           /* index 14: 15 mins  */
    30            /* index 15: 30 mins  */
};

static long convert_to_minutes(long step, long stepUnits)
{
    double result = 0;
    if (stepUnits == 0)
        return step; /* unit=minutes so no change */
    if (stepUnits == 1)
        return step * 60; /* unit=hours */
    if (stepUnits == 13)
        return step / 60; /* unit=seconds */
    /* Assert( stepUnits < sizeof(u2m)/sizeof(u2m[0]) ); */

    result = step * u2m[stepUnits];
    return (long)result;
}

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_validity_date* self = (grib_accessor_validity_date*)a;
    grib_handle* hand                 = grib_handle_of_accessor(a);
    int n                             = 0;

    self->date      = grib_arguments_get_name(hand, c, n++);
    self->time      = grib_arguments_get_name(hand, c, n++);
    self->step      = grib_arguments_get_name(hand, c, n++);
    self->stepUnits = grib_arguments_get_name(hand, c, n++);
    self->year      = grib_arguments_get_name(hand, c, n++);
    self->month     = grib_arguments_get_name(hand, c, n++);
    self->day       = grib_arguments_get_name(hand, c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_validity_date* self = (grib_accessor_validity_date*)a;
    grib_handle* hand                 = grib_handle_of_accessor(a);
    int ret                           = 0;
    long date                         = 0;
    long time                         = 0;
    long step                         = 0;
    long stepUnits                    = 0;
    long hours = 0, minutes = 0, step_mins = 0, tmp, tmp_hrs;

    if (self->year) {
        long year, month, day;
        if ((ret = grib_get_long_internal(hand, self->year, &year)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_long_internal(hand, self->month, &month)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_long_internal(hand, self->day, &day)) != GRIB_SUCCESS)
            return ret;
        *val = year * 10000 + month * 100 + day;
        return GRIB_SUCCESS;
    }
    if ((ret = grib_get_long_internal(hand, self->date, &date)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->time, &time)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long(hand, self->step, &step)) != GRIB_SUCCESS) {
        if ((ret = grib_get_long_internal(hand, "endStep", &step)) != GRIB_SUCCESS) {
            return ret; /* See ECC-817 */
        }
    }

    if (self->stepUnits) {
        if ((ret = grib_get_long_internal(hand, self->stepUnits, &stepUnits)) != GRIB_SUCCESS)
            return ret;
        step_mins = convert_to_minutes(step, stepUnits);
    }

    minutes = time % 100;
    hours   = time / 100;
    tmp     = minutes + step_mins; /* add the step to our minutes */
    tmp_hrs = tmp / 60;            /* how many hours and mins is that? */
    hours += tmp_hrs;              /* increment hours */

    date = grib_date_to_julian(date);
    /* does the new 'hours' exceed 24? if so increment julian */
    while (hours >= 24) {
        date++;
        hours -= 24;
    }
    /* GRIB-29: Negative forecast time */
    while (hours < 0) {
        date--;
        hours += 24;
    }

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    *val = grib_julian_to_date(date);

    return GRIB_SUCCESS;
}
