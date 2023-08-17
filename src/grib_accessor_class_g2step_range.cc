/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*********************************************
 *   Enrico Fucile
 *******************************************/

#include "grib_api_internal.h"
#include "step_optimizer.h"
#include <vector>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = pack_string;unpack_string;value_count
   IMPLEMENTS = pack_long;unpack_long;dump
   IMPLEMENTS = get_native_type;string_length
   IMPLEMENTS = init
   MEMBERS    = const char* startStep
   MEMBERS    = const char* endStep
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
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_g2step_range
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in g2step_range */
    const char* startStep;
    const char* endStep;
} grib_accessor_g2step_range;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_g2step_range = {
    &grib_accessor_class_gen,                      /* super */
    "g2step_range",                      /* name */
    sizeof(grib_accessor_g2step_range),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    &string_length,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    &pack_string,                /* pack_string */
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


grib_accessor_class* grib_accessor_class_g2step_range = &_grib_accessor_class_g2step_range;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;

    int n = 0;

    self->startStep = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->endStep   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    char buf[100];
    int ret     = 0;
    size_t size = 0;

    size_t stepOutputFormatSize = 128;
    char stepOutputFormat[stepOutputFormatSize];
    if ((ret = grib_get_string_internal(h, "stepOutputFormat", stepOutputFormat, &stepOutputFormatSize)) != GRIB_SUCCESS)
        return ret;

    if (strcmp(stepOutputFormat, "future") == 0) {
        long indicatorOfUnitOfTimeRange;
        if ((ret = grib_get_long_internal(h, "indicatorOfUnitOfTimeRange", &indicatorOfUnitOfTimeRange)) != GRIB_SUCCESS)
            return ret;

        long forecastTime;
        if ((ret = grib_get_long_internal(h, "forecastTime", &forecastTime)) != GRIB_SUCCESS)
            return ret;

        size_t stepUnitsSize = 128;
        char stepUnits[stepUnitsSize];
        if ((ret = grib_get_string_internal(h, "stepUnits", stepUnits, &stepUnitsSize)) != GRIB_SUCCESS)
            return ret;

        Step<long> step_a{forecastTime, indicatorOfUnitOfTimeRange};
        step_a.optimizeUnit();
        step_a.hide_hour_unit();

        if (self->endStep == NULL) {
            snprintf(buf, sizeof(buf), "%ld%s", step_a.value(), step_a.unit().to_string().c_str());
        }
        else {
            long indicatorOfUnitForTimeRange;
            long lengthOfTimeRange;
            ret = grib_get_long_internal(h, "indicatorOfUnitForTimeRange", &indicatorOfUnitForTimeRange);
            if (ret)
                return ret;
            ret = grib_get_long_internal(h, "lengthOfTimeRange", &lengthOfTimeRange);
            if (ret)
                return ret;

            Step<long> length{lengthOfTimeRange, indicatorOfUnitForTimeRange};
            Step<long> step_b = step_a + length;
            step_b.optimizeUnit();
            auto [a, b] = findCommonUnits(step_a, step_b);

            step_b.hide_hour_unit();
            a.hide_hour_unit();
            b.hide_hour_unit();

            if (a.value() == 0) {
                snprintf(buf, sizeof(buf), "0%s-%ld%s", step_b.unit().to_string().c_str(), step_b.value(), step_b.unit().to_string().c_str());
            }
            else {
                snprintf(buf, sizeof(buf), "%ld%s-%ld%s", a.value(), a.unit().to_string().c_str(), b.value(), b.unit().to_string().c_str());
            }
        }
    }
    else {
        grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
        grib_handle* h                   = grib_handle_of_accessor(a);
        int ret     = 0;
        size_t size = 0;
        long start = 0, theEnd = 0;

        ret = grib_get_long_internal(h, self->startStep, &start);
        if (ret)
            return ret;

        if (self->endStep == NULL) {
            snprintf(buf, sizeof(buf), "%ld", start);
        }
        else {
            if ((ret = grib_get_long_internal(h, self->endStep, &theEnd)) != GRIB_SUCCESS)
                return ret;

            if (start == theEnd) {
                snprintf(buf, sizeof(buf), "%ld", theEnd);
            }
            else {
                snprintf(buf, sizeof(buf), "%ld-%ld", start, theEnd);
            }
        }
    }

    size = strlen(buf) + 1;

    if (*len < size)
        return GRIB_ARRAY_TOO_SMALL;

    *len = size;

    memcpy(val, buf, size);

    return GRIB_SUCCESS;
}


static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret = 0;

    size_t stepOutputFormatSize = 128;
    char stepOutputFormat[stepOutputFormatSize];
    if ((ret = grib_get_string_internal(h, "stepOutputFormat", stepOutputFormat, &stepOutputFormatSize)) != GRIB_SUCCESS)
        return ret;

    if (strcmp(stepOutputFormat, "future") == 0) {
        std::vector<Step<long>> steps = parse_range<long>(val);
        if (steps.size() == 0) {
            return GRIB_INVALID_ARGUMENT;
        }
        if (steps.size() == 1) {
            steps[0].optimizeUnit();
            if ((ret = grib_set_long_internal(h, "indicatorOfUnitOfTimeRange", steps[0].unit().to_long())))
                return ret;
            if ((ret = grib_set_long_internal(h, "forecastTime", steps[0].value())))
                return ret;
        }
        else if (steps.size() == 2) {
            steps[0].optimizeUnit();
            steps[1].optimizeUnit();
            auto [s0, s1] = findCommonUnits(steps[0], steps[1]);

            if ((ret = grib_set_long_internal(h, "indicatorOfUnitOfTimeRange", s0.unit().to_long())))
                return ret;
            if ((ret = grib_set_long_internal(h, "forecastTime", s0.value())))
                return ret;

            if ((ret = grib_set_long_internal(h, "indicatorOfUnitForTimeRange", s1.unit().to_long())))
                return ret;
            if ((ret = grib_set_long_internal(h, "lengthOfTimeRange", s1.value())))
                return ret;
        }
        else {
            std::string msg = std::string("Invalid range: ") + val;
            throw std::runtime_error(msg);
        }
    }
    else {
        long start = 0, theEnd = -1;
        char *p = NULL, *q = NULL;

        start  = strtol(val, &p, 10);
        theEnd = start;

        if (*p != 0)
            theEnd = strtol(++p, &q, 10);
        if ((ret = grib_set_long_internal(h, self->startStep, start)))
            return ret;

        if (self->endStep != NULL) {
            if ((ret = grib_set_long_internal(h, self->endStep, theEnd)))
                return ret;
        }
    }

    return 0;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    return 255;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    char buff[100];
    size_t bufflen = 100;

    snprintf(buff, sizeof(buff), "%ld", *val);
    return pack_string(a, buff, &bufflen);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret = 0;

    size_t stepOutputFormatSize = 128;
    char stepOutputFormat[stepOutputFormatSize];
    if ((ret = grib_get_string_internal(h, "stepOutputFormat", stepOutputFormat, &stepOutputFormatSize)) != GRIB_SUCCESS)
        return ret;

    if (strcmp(stepOutputFormat, "future") == 0) {
        long unit = 0;
        long value = 0;

        if ((ret = grib_get_long_internal(h, "indicatorOfUnitOfTimeRange", &unit)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_long_internal(h, "forecastTime", &value)) != GRIB_SUCCESS)
            return ret;
        Step<long> start_step(value, unit);
        Step<long> step = start_step;

        if (self->endStep != NULL) {
            if ((ret = grib_get_long_internal(h, "indicatorOfUnitForTimeRange", &unit)) != GRIB_SUCCESS)
                return ret;
            if ((ret = grib_get_long_internal(h, "lengthOfTimeRange", &value)) != GRIB_SUCCESS)
                return ret;
            Step<long> end_step(value, unit);
            step = start_step + end_step;
        }

        step.optimizeUnit();
        *val = step.value();
    }
    else {
        char buff[100];
        size_t bufflen = 100;
        long start, theEnd;
        char* p = buff;
        char* q = NULL;
        if ((ret = unpack_string(a, buff, &bufflen)) != GRIB_SUCCESS)
            return ret;

        start  = strtol(buff, &p, 10);
        theEnd = start;
        if (*p != 0)
            theEnd = strtol(++p, &q, 10);

        *val = theEnd;
    }

    return 0;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}
