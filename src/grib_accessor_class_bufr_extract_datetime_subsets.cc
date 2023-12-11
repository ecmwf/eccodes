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
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init; get_native_type
   IMPLEMENTS = pack_long;
   MEMBERS    = const char* doExtractSubsets
   MEMBERS    = const char* numberOfSubsets
   MEMBERS    = const char* extractSubsetList
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_bufr_extract_datetime_subsets
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in bufr_extract_datetime_subsets */
    const char* doExtractSubsets;
    const char* numberOfSubsets;
    const char* extractSubsetList;
} grib_accessor_bufr_extract_datetime_subsets;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_extract_datetime_subsets = {
    &grib_accessor_class_gen,                      /* super */
    "bufr_extract_datetime_subsets",                      /* name */
    sizeof(grib_accessor_bufr_extract_datetime_subsets),  /* size */
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
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    0,                /* unpack_long */
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


grib_accessor_class* grib_accessor_class_bufr_extract_datetime_subsets = &_grib_accessor_class_bufr_extract_datetime_subsets;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    int n                                             = 0;
    grib_accessor_bufr_extract_datetime_subsets* self = (grib_accessor_bufr_extract_datetime_subsets*)a;

    a->length               = 0;
    self->doExtractSubsets  = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->numberOfSubsets   = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->extractSubsetList = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

/* Convert input date to Julian number. If date is invalid, return -1 */
static double date_to_julian(long year, long month, long day, long hour, long minute, double second)
{
    double result = 0; /* Julian number in units of days */

    /* For validating the date/time, we specify seconds as an integer */
    long lSecond = (long)second;
    grib_datetime_to_julian(year, month, day, hour, minute, lSecond, &result);

    {
        /* Check conversion worked by going other way */
        long year1, month1, day1, hour1, minute1, lSecond1;
        grib_julian_to_datetime(result, &year1, &month1, &day1, &hour1, &minute1, &lSecond1);
        if (year1 != year || month1 != month || day1 != day || minute1 != minute || lSecond1 != lSecond) {
            return -1; /* Failed. Invalid date*/
        }
    }
    /* Now get the proper Julian number specifying the seconds in double precision */
    grib_datetime_to_julian_d(year, month, day, hour, minute, second, &result);
    return result;
}

/* Helper function to construct the array of long for year, month, day etc. */
/* The boolean zero_on_error flag is for the case where codes_get fails and we fill with zeros */
static int build_long_array(grib_context* c, grib_handle* h, int compressed,
                            long** array, const char* key, long numberOfSubsets, int zero_on_error)
{
    int err = 0;
    long i;
    size_t n = numberOfSubsets;
    *array   = (long*)grib_context_malloc_clear(c, sizeof(long) * numberOfSubsets);
    if (compressed) {
        err = grib_get_long_array(h, key, *array, &n);
        if (zero_on_error) {
            if (err) {
                err         = 0;
                (*array)[0] = 0;
                n           = 1;
            }
        }
        if (err)
            return err;
        if (n != numberOfSubsets) {
            if (n == 1) {
                for (i = 1; i < numberOfSubsets; i++)
                    (*array)[i] = (*array)[0];
            }
            else {
                return GRIB_INTERNAL_ERROR;
            }
        }
    }
    else {
        /* uncompressed */
        char keystr[32] = {0,};
        size_t values_len = 0;
        for (i = 0; i < numberOfSubsets; ++i) {
            long lVal = 0;
            snprintf(keystr, sizeof(keystr), "#%ld#%s", i + 1, key);
            err = grib_get_size(h, keystr, &values_len);
            if (err)
                return err;
            if (values_len > 1)
                return GRIB_NOT_IMPLEMENTED;
            err = grib_get_long(h, keystr, &lVal);
            if (err)
                return err;
            (*array)[i] = lVal;
        }
    }
    return err;
}

static int select_datetime(grib_accessor* a)
{
    int ret                                           = 0;
    long compressed                                   = 0;
    grib_accessor_bufr_extract_datetime_subsets* self = (grib_accessor_bufr_extract_datetime_subsets*)a;
    grib_handle* h                                    = grib_handle_of_accessor(a);
    grib_context* c                                   = h->context;
    size_t n;
    double julianStart = 0, julianEnd = 0, julianDT = 0;
    char start_str[80] = {0,}, end_str[80] = {0,}, datetime_str[80] = {0,};
    long yearRank, monthRank, dayRank, hourRank, minuteRank, secondRank;
    long yearStart, monthStart, dayStart, hourStart, minuteStart, secondStart;
    long yearEnd, monthEnd, dayEnd, hourEnd, minuteEnd, secondEnd;
    long *year = NULL, *month = NULL, *day = NULL, *hour = NULL, *minute = NULL;
    double* second = NULL;
    long numberOfSubsets, i;
    grib_iarray* subsets = NULL;
    size_t nsubsets    = 0;
    char yearstr[32]   = "year";
    char monthstr[32]  = "month";
    char daystr[32]    = "day";
    char hourstr[32]   = "hour";
    char minutestr[32] = "minute";
    char secondstr[32] = "second";

    ret = grib_get_long(h, "compressedData", &compressed);
    if (ret) return ret;

    ret = grib_get_long(h, self->numberOfSubsets, &numberOfSubsets);
    if (ret) return ret;

    subsets = grib_iarray_new(c, numberOfSubsets, 10);

    ret = grib_set_long(h, "unpack", 1);
    if (ret) return ret;

    if (compressed) {
        ret = grib_get_long(h, "extractDateTimeYearRank", &yearRank);
        if (ret) return ret;
        snprintf(yearstr, sizeof(yearstr), "#%ld#year", yearRank);

        ret = grib_get_long(h, "extractDateTimeMonthRank", &monthRank);
        if (ret) return ret;
        snprintf(monthstr, sizeof(monthstr), "#%ld#month", monthRank);

        ret = grib_get_long(h, "extractDateTimeDayRank", &dayRank);
        if (ret) return ret;
        snprintf(daystr, sizeof(daystr), "#%ld#day", dayRank);

        ret = grib_get_long(h, "extractDateTimeHourRank", &hourRank);
        if (ret) return ret;
        snprintf(hourstr, sizeof(hourstr), "#%ld#hour", hourRank);

        ret = grib_get_long(h, "extractDateTimeMinuteRank", &minuteRank);
        if (ret) return ret;
        snprintf(minutestr, sizeof(minutestr), "#%ld#minute", minuteRank);

        ret = grib_get_long(h, "extractDateTimeSecondRank", &secondRank);
        if (ret) return ret;
        snprintf(secondstr, sizeof(secondstr), "#%ld#second", secondRank);
    }

    /* YEAR */
    ret = build_long_array(c, h, compressed, &year, yearstr, numberOfSubsets, 0);
    if (ret) return ret;

    /* MONTH */
    ret = build_long_array(c, h, compressed, &month, monthstr, numberOfSubsets, 0);
    if (ret) return ret;

    /* DAY */
    ret = build_long_array(c, h, compressed, &day, daystr, numberOfSubsets, 0);
    if (ret) return ret;

    /* HOUR */
    ret = build_long_array(c, h, compressed, &hour, hourstr, numberOfSubsets, 0);
    if (ret) return ret;

    /* MINUTE: Special treatment if error => set all entries to zero */
    ret = build_long_array(c, h, compressed, &minute, minutestr, numberOfSubsets, 1);
    if (ret) return ret;

    /* SECOND: Double array */
    n      = numberOfSubsets;
    second = (double*)grib_context_malloc_clear(c, sizeof(double) * numberOfSubsets);
    if (compressed) {
        ret = grib_get_double_array(h, secondstr, second, &n);
        if (ret) {
            ret       = 0;
            second[0] = 0;
            n         = 1;
            (void)ret;
        }
        if (n != numberOfSubsets) {
            if (n == 1) {
                for (i = 1; i < numberOfSubsets; i++)
                    second[i] = second[0];
            }
            else
                return GRIB_INTERNAL_ERROR;
        }
    }
    else {
        /* uncompressed */
        size_t values_len = 0;
        for (i = 0; i < numberOfSubsets; ++i) {
            snprintf(secondstr, sizeof(secondstr), "#%ld#second", i + 1);
            ret = grib_get_size(h, secondstr, &values_len);
            if (ret) {
                /* no 'second' key */
                for (i = 1; i < numberOfSubsets; i++)
                    second[i] = second[0];
            }
            else {
                if (values_len > 1)
                    return GRIB_NOT_IMPLEMENTED;
                ret = grib_get_double(h, secondstr, &(second[i]));
                if (ret)
                    return ret;
            }
        }
    }

    ret = grib_get_long(h, "extractDateTimeYearStart", &yearStart);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeMonthStart", &monthStart);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeDayStart", &dayStart);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeHourStart", &hourStart);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeMinuteStart", &minuteStart);
    if (ret)
        minuteStart = 0;
    ret = grib_get_long(h, "extractDateTimeSecondStart", &secondStart);
    if (ret)
        secondStart = 0;
    snprintf(start_str, sizeof(start_str), "%04ld/%02ld/%02ld %02ld:%02ld:%02ld",
             yearStart, monthStart, dayStart, hourStart, minuteStart, secondStart);
    if (c->debug) fprintf(stderr, "ECCODES DEBUG bufr_extract_datetime_subsets: start   =%s\n", start_str);
    julianStart = date_to_julian(yearStart, monthStart, dayStart, hourStart, minuteStart, secondStart);
    if (julianStart == -1) {
        grib_context_log(c, GRIB_LOG_ERROR, "Invalid start date/time: %s", start_str);
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    ret = grib_get_long(h, "extractDateTimeYearEnd", &yearEnd);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeMonthEnd", &monthEnd);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeDayEnd", &dayEnd);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeHourEnd", &hourEnd);
    if (ret) return ret;
    ret = grib_get_long(h, "extractDateTimeMinuteEnd", &minuteEnd);
    if (ret)
        minuteEnd = 0;
    ret = grib_get_long(h, "extractDateTimeSecondEnd", &secondEnd);
    if (ret)
        secondEnd = 0;
    snprintf(end_str, sizeof(end_str), "%04ld/%02ld/%02ld %02ld:%02ld:%02ld",
            yearEnd, monthEnd, dayEnd, hourEnd, minuteEnd, secondEnd);
    if (c->debug) fprintf(stderr, "ECCODES DEBUG bufr_extract_datetime_subsets: end     =%s\n", end_str);
    julianEnd = date_to_julian(yearEnd, monthEnd, dayEnd, hourEnd, minuteEnd, secondEnd);
    if (julianEnd == -1) {
        grib_context_log(c, GRIB_LOG_ERROR, "Invalid end date/time: %s", end_str);
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    if (julianEnd <= julianStart) {
        grib_context_log(c, GRIB_LOG_ERROR, "Wrong definition of time interval: end (%s) is not after start (%s)", end_str, start_str);
        ret = GRIB_INTERNAL_ERROR;
        goto cleanup;
    }

    for (i = 0; i < numberOfSubsets; i++) {
        if (second[i] == GRIB_MISSING_DOUBLE) {
            fprintf(stderr, "ECCODES WARNING: bufr_extract_datetime_subsets: Key '%s' is missing! Using zero instead\n", secondstr);
            second[i] = 0;
        }
        snprintf(datetime_str, sizeof(datetime_str), "%04ld/%02ld/%02ld %02ld:%02ld:%.3f",
                 year[i], month[i], day[i], hour[i], minute[i], second[i]);
        if (c->debug) fprintf(stderr, "ECCODES DEBUG bufr_extract_datetime_subsets: datetime=%s\n", datetime_str);
        julianDT = date_to_julian(year[i], month[i], day[i], hour[i], minute[i], second[i]);
        if (julianDT == -1) {
            grib_context_log(c, GRIB_LOG_ERROR, "Invalid date/time: %s", datetime_str);
            ret = GRIB_INTERNAL_ERROR;
            goto cleanup;
        }

        /*printf("SN: datetime_str=%s j=%.15f\t", datetime_str, julianDT);*/
        if (julianDT >= julianStart && julianEnd >= julianDT) {
            /*printf(" ....ADDING subset %ld\n",i);*/
            grib_iarray_push(subsets, i + 1);
        }
        else {
            /*printf(" ....Exclude subset %ld\n",i);*/
        }
    }

    nsubsets = grib_iarray_used_size(subsets);
    ret      = grib_set_long(h, "extractedDateTimeNumberOfSubsets", nsubsets);
    if (ret) return ret;

    if (nsubsets != 0) {
        long* subsets_ar = grib_iarray_get_array(subsets);
        ret = grib_set_long_array(h, self->extractSubsetList, subsets_ar, nsubsets);
        grib_context_free(c, subsets_ar);
        if (ret) return ret;

        ret = grib_set_long(h, self->doExtractSubsets, 1);
        if (ret) return ret;
    }

cleanup:
    grib_context_free(c, year);
    grib_context_free(c, month);
    grib_context_free(c, day);
    grib_context_free(c, hour);
    grib_context_free(c, minute);
    grib_context_free(c, second);
    grib_iarray_delete(subsets);
    subsets = NULL;

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    /*grib_accessor_bufr_extract_datetime_subsets *self =(grib_accessor_bufr_extract_datetime_subsets*)a;*/

    if (*len == 0)
        return GRIB_SUCCESS;
    return select_datetime(a);
}
