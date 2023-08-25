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
#include "step.h"
#include "step_utilities.h"
#include <vector>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = pack_string;unpack_string;value_count
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_string;pack_string
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
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*, long*);
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
    0,                       /* dump */
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
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
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
    long start_value = 0;
    long end_value = 0;
    long step_units_value = 0;

    if ((ret = grib_get_long_internal(h, self->startStep, &start_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "stepUnits", &step_units_value)) != GRIB_SUCCESS)
        return ret;


    Step start_step = Step(start_value, step_units_value);
    start_step.hide_hour_unit();
    if (self->endStep == NULL) {
        if (is_future_output_enabled(h)) {
            snprintf(buf, sizeof(buf), "%s", start_step.to_string().c_str());
        }
        else {
            snprintf(buf, sizeof(buf), "%ld", start_value);
        }
    }
    else {
        if ((ret = grib_get_long_internal(h, self->endStep, &end_value)) != GRIB_SUCCESS)
            return ret;

        if (is_future_output_enabled(h)) {
            Step end_step = Step(end_value, step_units_value);
            end_step.hide_hour_unit();
            if (start_value == end_value) {
                snprintf(buf, sizeof(buf), "%s", end_step.to_string().c_str());
            }
            else {
                snprintf(buf, sizeof(buf), "%s-%s", start_step.to_string().c_str(), end_step.to_string().c_str());
            }
        }
        else {
            if (start_value == end_value) {
                snprintf(buf, sizeof(buf), "%ld", end_value);
            }
            else {
                snprintf(buf, sizeof(buf), "%ld-%ld", start_value, end_value);
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

    std::vector<Step> steps = parse_range(val);
    if (steps.size() == 0)
        return GRIB_INVALID_ARGUMENT;

    Step step_0 = steps[0];
    Step step_1;
    if (steps.size() > 1) {
        std::tie(step_0, step_1) = find_common_units(steps[0].optimize_unit(), steps[1].optimize_unit());
        if ((ret = grib_set_long_internal(h, "stepUnits", step_0.unit().to_long())))
            return ret;
    }

    if ((ret = grib_set_long_internal(h, self->startStep, step_0.value<long>())))
        return ret;

    if ((self->endStep != NULL) && (steps.size() > 1)) {
        if ((ret = grib_set_long_internal(h, self->endStep, step_1.value<long>())))
            return ret;
    }
    return GRIB_SUCCESS;
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

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    // TODO(EB)
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    char buf[100];
    int ret     = 0;
    size_t size = 0;
    long start_value = 0;
    long end_value = 0;
    long step_units_value = 0;

    if ((ret = grib_get_long_internal(h, self->startStep, &start_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "stepUnits", &step_units_value)) != GRIB_SUCCESS)
        return ret;

    Step start_step = Step(start_value, step_units_value);
    start_step.hide_hour_unit();
    if (self->endStep == NULL) {
        *val = start_step.value<double>();
    }
    else {
        if ((ret = grib_get_long_internal(h, self->endStep, &end_value)) != GRIB_SUCCESS)
            return ret;
        Step end_step = Step(end_value, step_units_value);
        *val = end_step.value<double>();
    }

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    char buf[100];
    int ret     = 0;
    size_t size = 0;
    long start_value = 0;
    long end_value = 0;
    long step_units_value = 0;

    if ((ret = grib_get_long_internal(h, self->startStep, &start_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "stepUnits", &step_units_value)) != GRIB_SUCCESS)
        return ret;

    Step start_step = Step(start_value, step_units_value);
    start_step.hide_hour_unit();
    if (self->endStep == NULL) {
        *val = start_step.value<long>();
    }
    else {
        if ((ret = grib_get_long_internal(h, self->endStep, &end_value)) != GRIB_SUCCESS)
            return ret;
        Step end_step = Step(end_value, step_units_value);
        *val = end_step.value<long>();
    }

    return GRIB_SUCCESS;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}
