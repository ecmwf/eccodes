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
#include "step.h"
#include "step_utilities.h"
#include <vector>
#include <sstream>
#include <iostream>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = pack_string;unpack_string;value_count
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = get_native_type;string_length
   IMPLEMENTS = init
   MEMBERS    = const char* start_step
   MEMBERS    = const char* end_step
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
    const char* start_step;
    const char* end_step;
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
    0,                /* pack_double */
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

    self->start_step = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->end_step   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->length = 0;
}

//static void dump(grib_accessor* a, grib_dumper* dumper)
//{
    //grib_dump_string(dumper, a, NULL);
//}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret     = 0;
    size_t size = 0;
    double start_step_value = 0;
    double end_step_value = 0;
    long step_units;

    int show_hours = a->context->grib_hourly_steps_with_units;

    if ((ret = grib_get_double_internal(h, self->start_step, &start_step_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret= grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
        return ret;
    try {
        if (eccodes::Unit{step_units} == eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            if ((ret = grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
                return ret;
        }

        char fp_format[128] = "%g";
        size_t fp_format_len = sizeof(fp_format);
        if ((ret = grib_get_string_internal(h, "formatForDoubles", fp_format, &fp_format_len)) != GRIB_SUCCESS)
            return ret;
        std::stringstream ss;

        eccodes::Step start_step{start_step_value, step_units};
        if (self->end_step == NULL) {
            ss << start_step.value<std::string>(fp_format, show_hours);
        }
        else {
            if ((ret = grib_get_double_internal(h, self->end_step, &end_step_value)) != GRIB_SUCCESS)
                return ret;

            eccodes::Step end_step{end_step_value, step_units};

            if (start_step_value == end_step_value) {
                ss << end_step.value<std::string>(fp_format, show_hours);
            }
            else {
                ss << start_step.value<std::string>(fp_format, show_hours) << "-" << end_step.value<std::string>(fp_format, show_hours);
            }
        }

        size = ss.str().size() + 1;

        if (*len < size)
            return GRIB_ARRAY_TOO_SMALL;

        *len = size;

        memcpy(val, ss.str().c_str(), size);
    }
    catch (std::exception& e) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_g2step_range::unpack_string: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}


// Step range format: <start_step>[-<end_step>]
// <start_step> and <end_step> can be in different units
// stepRange="X" in instantaneous field is equivalent to set step=X
// stepRange="X" in accumulated field is equivalent to startStep=X, endStep=startStep
static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret = 0;

    long force_step_units;
    if ((ret = grib_get_long_internal(h, "forceStepUnits", &force_step_units)) != GRIB_SUCCESS)
        return ret;

    try {
        std::vector<eccodes::Step> steps = parse_range(val, eccodes::Unit{force_step_units});
        if (steps.size() == 0) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "Could not parse step range: %s", val);
            return GRIB_INVALID_ARGUMENT;
        }

        eccodes::Step step_0;
        eccodes::Step step_1;
        if (eccodes::Unit{force_step_units} == eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            if (steps.size() > 1)
                std::tie(step_0, step_1) = find_common_units(steps[0].optimize_unit(), steps[1].optimize_unit());
            else
                step_0 = steps[0].optimize_unit();
        }
        else {
            step_0 = eccodes::Step{steps[0].value<long>(eccodes::Unit{force_step_units}), eccodes::Unit{force_step_units}};
            if (steps.size() > 1) {
                step_1 = eccodes::Step{steps[1].value<long>(eccodes::Unit{force_step_units}), eccodes::Unit{force_step_units}};
            }
        }

        if ((ret = grib_set_long_internal(h, "startStepUnit", step_0.unit().value<long>())))
            return ret;
        if ((ret = set_step(h, "forecastTime" , "indicatorOfUnitOfTimeRange", step_0)) != GRIB_SUCCESS)
            return ret;

        if (self->end_step != NULL) {
            if (steps.size() > 1) {
                if ((ret = grib_set_long_internal(h, "endStepUnit", step_1.unit().value<long>())))
                    return ret;
                if ((ret = grib_set_long_internal(h, self->end_step, step_1.value<long>())))
                    return ret;
            } else {
                if ((ret = grib_set_long_internal(h, "endStepUnit", step_0.unit().value<long>())))
                    return ret;
                if ((ret = grib_set_long_internal(h, self->end_step, step_0.value<long>())))
                    return ret;
            }
        }
    }
    catch (std::exception& e) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_class_g2step_range::pack_string: %s", e.what());
        return GRIB_INVALID_ARGUMENT;
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


static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret     = 0;
    long end_start_value = 0;
    long end_step_value = 0;
    long step_units = 0;

    if ((ret = grib_get_long_internal(h, self->start_step, &end_start_value)) != GRIB_SUCCESS)
        return ret;
    try {
        if ((ret = grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
            throw std::runtime_error("Failed to get stepUnits");
        if (eccodes::Unit{step_units} == eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            if ((ret = grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
                return ret;
        }

        eccodes::Step start_step{end_start_value, step_units};
        if (self->end_step == NULL) {
            *val = start_step.value<long>();
        }
        else {
            if ((ret = grib_get_long_internal(h, self->end_step, &end_step_value)) != GRIB_SUCCESS)
                return ret;
            eccodes::Step end_step{end_step_value, step_units};
            *val = end_step.value<long>();
        }
    }
    catch (std::exception& e) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Failed to unpack step range: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}


static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_g2step_range* self = (grib_accessor_g2step_range*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret     = 0;
    double end_start_value = 0;
    double end_step_value = 0;
    long step_units = 0;

    if ((ret = grib_get_double_internal(h, self->start_step, &end_start_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
        throw std::runtime_error("Failed to get stepUnits");

    try {
        if (eccodes::Unit{step_units} == eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            if ((ret = grib_get_long_internal(h, "stepUnits", &step_units)) != GRIB_SUCCESS)
                return ret;
        }

        eccodes::Step start_step{end_start_value, step_units};
        if (self->end_step == NULL) {
            *val = start_step.value<long>();
        }
        else {
            if ((ret = grib_get_double_internal(h, self->end_step, &end_step_value)) != GRIB_SUCCESS)
                return ret;
            eccodes::Step end_step{end_step_value, step_units};
            *val = end_step.value<double>();
        }
    }
    catch (std::exception& e) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grid_accessor_g2step_range::unpack_double: %s", e.what());
        return GRIB_DECODING_ERROR;
    }

    return GRIB_SUCCESS;
}


static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}
