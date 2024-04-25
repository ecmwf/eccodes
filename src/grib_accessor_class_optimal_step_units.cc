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
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = pack_long;unpack_long;dump
   IMPLEMENTS = pack_string;unpack_string;dump
   IMPLEMENTS = string_length;pack_expression;get_native_type;is_missing
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
static int is_missing(grib_accessor*);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int pack_expression(grib_accessor*, grib_expression*);
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
    &is_missing,                 /* is_missing */
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
    &pack_expression,            /* pack_expression */
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
    grib_handle* hand = grib_handle_of_accessor(a);
    int n = 0;

    self->forecast_time_value = grib_arguments_get_name(hand, c, n++);
    self->forecast_time_unit  = grib_arguments_get_name(hand, c, n++);
    self->time_range_value    = grib_arguments_get_name(hand, c, n++);
    self->time_range_unit     = grib_arguments_get_name(hand, c, n++);
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

static int pack_expression(grib_accessor* a, grib_expression* e)
{
    const char* cval  = NULL;
    int ret           = 0;
    long lval         = 0;
    size_t len        = 1;
    grib_handle* hand = grib_handle_of_accessor(a);
    const char* cclass_name = a->cclass->name;

    if (strcmp(e->cclass->name, "long") == 0) {
        grib_expression_evaluate_long(hand, e, &lval); /* TODO: check return value */
        ret = grib_pack_long(a, &lval, &len);
    }
    else {
        char tmp[1024];
        len  = sizeof(tmp);
        cval = grib_expression_evaluate_string(hand, e, tmp, &len, &ret);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                "%s.%s: Unable to evaluate string %s to be set in %s",
                cclass_name, __func__, grib_expression_get_name(e), a->name);
            return ret;
        }
        len = strlen(cval) + 1;
        //if (hand->context->debug)
        //    printf("ECCODES DEBUG grib_accessor_class_codetable::pack_expression %s %s\n", a->name, cval);
        ret = grib_pack_string(a, cval, &len);
    }
    return ret;
}

static long staticStepUnits = eccodes::Unit{eccodes::Unit::Value::MISSING}.value<long>();
static long staticForceStepUnits = eccodes::Unit{eccodes::Unit::Value::MISSING}.value<long>();

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_handle* h = grib_handle_of_accessor(a);
    auto supported_units = eccodes::Unit::list_supported_units();
    try {
        eccodes::Unit unit{*val}; // throws if not supported
        auto iter = std::find(supported_units.begin(), supported_units.end(), unit);
        if (iter == supported_units.end()) {
            throw std::runtime_error{"eccodes::Unit not supported"};
        }
    }
    catch (std::exception& e) {
        std::string supported_units_str;
        for (auto& u : supported_units)
            supported_units_str += eccodes::Unit{u}.value<std::string>() + ",";
        supported_units_str.pop_back();

        std::string msg = std::string{"Invalid unit: "} + std::to_string(*val) + " (" + e.what() + ")" +
                                      ". Available units are: " + supported_units_str;
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s", msg.c_str());
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
    try {
        if (eccodes::Unit{staticStepUnits} != eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            *val = staticStepUnits;
            return GRIB_SUCCESS;
        }

        const grib_accessor_optimal_step_units* self = (grib_accessor_optimal_step_units*)a;
        grib_handle* h = grib_handle_of_accessor(a);

        auto forecast_time_opt = get_step(h, self->forecast_time_value, self->forecast_time_unit);
        auto time_range_opt = get_step(h, self->time_range_value, self->time_range_unit);

        if (forecast_time_opt && time_range_opt) {
            auto [step_a, step_b] = find_common_units(forecast_time_opt.value().optimize_unit(),
                                                     (forecast_time_opt.value() + time_range_opt.value()).optimize_unit());
            *val = step_a.unit().value<long>();
        }
        else if (forecast_time_opt && !time_range_opt) {
            *val = forecast_time_opt.value().optimize_unit().unit().value<long>();
        }
        else if (!forecast_time_opt && time_range_opt) {
            *val = time_range_opt.value().optimize_unit().unit().value<long>();
        }
        else if (!forecast_time_opt && !time_range_opt) {
            *val = eccodes::Unit{eccodes::Unit::Value::HOUR}.value<long>();
        }
    }
    catch (std::exception& e) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s", e.what());
        return GRIB_INTERNAL_ERROR;
    }

    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    try {
        long unit = eccodes::Unit{val}.value<long>();
        pack_long(a, &unit, len);
    }
    catch (std::exception& e) {
        auto supported_units = eccodes::Unit::list_supported_units();
        std::string supported_units_str;
        for (auto& u : supported_units)
            supported_units_str += eccodes::Unit{u}.value<std::string>() + ",";
        supported_units_str.pop_back();

        std::string msg = "Invalid unit: " + std::string(val) + " (" + e.what() + ")" + ". Available units are: " + supported_units_str;
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s", msg.c_str());
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
    *len = snprintf(val, *len, "%s", eccodes::Unit{unit}.value<std::string>().c_str());
    return GRIB_SUCCESS;
}

// Step units are never missing
// If the user does not specify a step unit, we default to hours
static int is_missing(grib_accessor* a)
{
    return 0;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}
