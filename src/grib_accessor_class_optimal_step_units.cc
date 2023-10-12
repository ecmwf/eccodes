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
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = pack_long,unpack_long;dump
   IMPLEMENTS = pack_string,unpack_string;dump
   IMPLEMENTS = string_length
   IMPLEMENTS = init
   MEMBERS    = const char* forecast_time_value
   MEMBERS    = const char* forecast_time_unit
   MEMBERS    = const char* time_range_value 
   MEMBERS    = const char* time_range_unit
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
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_optimal_step_units
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in optimal_step_units */
    const char* forecast_time_value;
    const char* forecast_time_unit;
    const char* time_range_value;
    const char* time_range_unit;
} grib_accessor_optimal_step_units;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_optimal_step_units = {
    &grib_accessor_class_gen,                      /* super */
    "optimal_step_units",                      /* name */
    sizeof(grib_accessor_optimal_step_units),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    &string_length,              /* get length of string */
    0,                /* get number of values */
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


grib_accessor_class* grib_accessor_class_optimal_step_units = &_grib_accessor_class_optimal_step_units;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_optimal_step_units* self = (grib_accessor_optimal_step_units*)a;

    int n = 0;

    self->forecast_time_value = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->forecast_time_unit = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->time_range_value = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->time_range_unit= grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}


static size_t string_length(grib_accessor* a)
{
    return 255;
}

static long staticStepUnits = Unit{Unit::Value::MISSING}.value<long>();
static long staticForceStepUnits = Unit{Unit::Value::MISSING}.value<long>();

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_handle* h                   = grib_handle_of_accessor(a);
    try {
        if (!Unit::is_visible(Unit{*val}))
            throw std::runtime_error("Unit masked");
    }
    catch (std::exception& e) {
        auto visible_units = Unit::list_visible_units();
        std::string visible_units_str;
        for (auto& u : visible_units)
            visible_units_str += Unit{u}.value<std::string>() + ",";
        visible_units_str.pop_back();

        std::string msg = std::string{"Invalid unit: "} + std::to_string(*val) + " (" + e.what() + ")" + ". Available units are: " + visible_units_str;
        grib_context_log(a->context, GRIB_LOG_ERROR, msg.c_str());
        return GRIB_INVALID_ARGUMENT;
    }

    int ret;
    staticStepUnits = *val;
    if ((ret = grib_set_long_internal(h, "forceStepUnits", *val)) != GRIB_SUCCESS) {
        return ret;
    }

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    if (Unit{staticStepUnits} != Unit{Unit::Value::MISSING}) {
        *val = staticStepUnits;
        return GRIB_SUCCESS;
    }

    grib_accessor_optimal_step_units* self = (grib_accessor_optimal_step_units*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);

    auto forecast_time_opt = get_step(h, self->forecast_time_value, self->forecast_time_unit);
    auto time_range_opt = get_step(h, self->time_range_value, self->time_range_unit);

    if (forecast_time_opt && time_range_opt) {
        auto [step_a, step_b] = find_common_units(forecast_time_opt.value().optimize_unit(), (forecast_time_opt.value() + time_range_opt.value()).optimize_unit());
        *val = step_a.unit().value<long>();
    }
    else if (forecast_time_opt && !time_range_opt) {
        *val = forecast_time_opt.value().optimize_unit().unit().value<long>();
    }
    else if (!forecast_time_opt && time_range_opt) {
        *val = time_range_opt.value().optimize_unit().unit().value<long>();
    }
    else if (!forecast_time_opt && !time_range_opt) {
        *val = Unit{Unit::Value::HOUR}.value<long>();
    }

    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    try {
        long unit = Unit{val}.value<long>();

        if (!Unit::is_visible(Unit{val}))
            throw std::runtime_error("Unit masked");

        pack_long(a, &unit, len);
    }
    catch (std::exception& e) {
        auto visible_units = Unit::list_visible_units();
        std::string visible_units_str;
        for (auto& u : visible_units)
            visible_units_str += Unit{u}.value<std::string>() + ",";
        visible_units_str.pop_back();

        std::string msg = "Invalid unit: " + std::string(val) + " (" + e.what() + ")" + ". Available units are: " + visible_units_str;
        grib_context_log(a->context, GRIB_LOG_ERROR, msg.c_str());
        return GRIB_INVALID_ARGUMENT;
    }
    return GRIB_SUCCESS;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    int ret = 0;
    long unit = 0;
    size_t unit_len = 0;
    if ((ret = unpack_long(a, &unit, &unit_len)) != GRIB_SUCCESS)
        return ret;
    *len = snprintf(val, *len, "%s", Unit{unit}.value<std::string>().c_str());
    return GRIB_SUCCESS;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}
