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
   IMPLEMENTS = init;dump
   MEMBERS=const char* verification_yearmonth
   MEMBERS=const char* base_date
   MEMBERS=const char* day
   MEMBERS=const char* hour
   MEMBERS=const char* fcmonth
   MEMBERS=const char* check
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
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g1forecastmonth
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in g1forecastmonth */
    const char* verification_yearmonth;
    const char* base_date;
    const char* day;
    const char* hour;
    const char* fcmonth;
    const char* check;
} grib_accessor_g1forecastmonth;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_g1forecastmonth = {
    &grib_accessor_class_long,                      /* super */
    "g1forecastmonth",                      /* name */
    sizeof(grib_accessor_g1forecastmonth),  /* size */
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


grib_accessor_class* grib_accessor_class_g1forecastmonth = &_grib_accessor_class_g1forecastmonth;


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
    grib_accessor_g1forecastmonth* self = (grib_accessor_g1forecastmonth*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    int n = 0;
    const int count = grib_arguments_get_count(c);
    if (count == 6) { /* GRIB1 case -- this needs to be refactored */
        self->verification_yearmonth = grib_arguments_get_name(h, c, n++);
        self->base_date              = grib_arguments_get_name(h, c, n++);
        self->day                    = grib_arguments_get_name(h, c, n++);
        self->hour                   = grib_arguments_get_name(h, c, n++);
        self->fcmonth                = grib_arguments_get_name(h, c, n++);
        self->check                  = grib_arguments_get_name(h, c, n++);
    }
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int calculate_fcmonth(grib_accessor* a, 
        long verification_yearmonth, long base_date, long day, long hour, long* result)
{
    long base_yearmonth         = 0;
    long vyear  = 0;
    long vmonth = 0;
    long byear  = 0;
    long bmonth = 0;
    long fcmonth           = 0;

    base_yearmonth = base_date / 100;

    vyear  = verification_yearmonth / 100;
    vmonth = verification_yearmonth % 100;
    byear  = base_yearmonth / 100;
    bmonth = base_yearmonth % 100;

    fcmonth = (vyear - byear) * 12 + (vmonth - bmonth);
    if (day == 1 && hour == 0)
        fcmonth++;

    *result = fcmonth;
    return GRIB_SUCCESS;
}

static int unpack_long_edition2(grib_accessor* a, long* val, size_t* len)
{
    int err = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    long dataDate, verification_yearmonth;
    long year, month, day, hour, minute, second;
    long year2, month2, day2, hour2, minute2, second2;
    long forecastTime, indicatorOfUnitOfTimeRange;
    double jul_base, jul2, dstep;

    if ((err = grib_get_long(h, "year",  &year))  != GRIB_SUCCESS) return err;
    if ((err = grib_get_long(h, "month", &month)) != GRIB_SUCCESS) return err;
    if ((err = grib_get_long(h, "day",   &day))   != GRIB_SUCCESS) return err;
    if ((err = grib_get_long(h, "hour",   &hour))   != GRIB_SUCCESS) return err;
    if ((err = grib_get_long(h, "minute", &minute)) != GRIB_SUCCESS) return err;
    if ((err = grib_get_long(h, "second", &second)) != GRIB_SUCCESS) return err;

    if ((err = grib_get_long_internal(h, "dataDate", &dataDate)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(h, "forecastTime", &forecastTime)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, "indicatorOfUnitOfTimeRange", &indicatorOfUnitOfTimeRange)) != GRIB_SUCCESS)
        return err;
    if (indicatorOfUnitOfTimeRange != 1) { /* must be hour */
        grib_context_log(a->context, GRIB_LOG_ERROR, "indicatorOfUnitOfTimeRange must be 1 (hour)");
        return GRIB_DECODING_ERROR;
    }

    if ((err = grib_datetime_to_julian(year, month, day, hour, minute, second, &jul_base)) != GRIB_SUCCESS)
        return err;

    dstep = (((double)forecastTime) * 3600) / 86400; /* as a fraction of a day */
    jul2 = jul_base + dstep;

    if ((err = grib_julian_to_datetime(jul2, &year2, &month2, &day2, &hour2, &minute2, &second2)) != GRIB_SUCCESS)
        return err;

    verification_yearmonth = year2*100 + month2;
    if ((err = calculate_fcmonth(a, verification_yearmonth, dataDate, day, hour, val)) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}

static int unpack_long_edition1(grib_accessor* a, long* val, size_t* len)
{
    int err = 0;
    grib_accessor_g1forecastmonth* self = (grib_accessor_g1forecastmonth*)a;

    long verification_yearmonth = 0;
    long base_date              = 0;
    long day                    = 0;
    long hour                   = 0;
    long gribForecastMonth = 0;
    long check             = 0;
    long fcmonth           = 0;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a),
                                      self->verification_yearmonth, &verification_yearmonth)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->base_date, &base_date)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->day, &day)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->hour, &hour)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->fcmonth, &gribForecastMonth)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->check, &check)) != GRIB_SUCCESS)
        return err;

    if ((err = calculate_fcmonth(a, verification_yearmonth, base_date, day, hour, val)) != GRIB_SUCCESS)
        return err;

    /* Verification - compare gribForecastMonth with fcmonth */
    fcmonth = *val;
    if (gribForecastMonth != 0 && gribForecastMonth != fcmonth) {
        if (check) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s=%ld (%s-%s)=%ld", self->fcmonth,
                             gribForecastMonth, self->base_date, self->verification_yearmonth, fcmonth);
            Assert(gribForecastMonth == fcmonth);
        }
        else {
            *val = gribForecastMonth;
            return GRIB_SUCCESS;
        }
    }

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int err = 0;
    grib_handle* hand = grib_handle_of_accessor(a);
    long edition = 0;

    if ((err = grib_get_long(hand, "edition", &edition)) != GRIB_SUCCESS)
        return err;

    if (edition == 1)
        return unpack_long_edition1(a, val, len);
    if (edition == 2)
        return unpack_long_edition2(a, val, len);

    return GRIB_UNSUPPORTED_EDITION;
}

/* TODO: Check for a valid date */
static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_g1forecastmonth* self = (grib_accessor_g1forecastmonth*)a;
    return grib_set_long_internal(grib_handle_of_accessor(a), self->fcmonth, *val);
}
