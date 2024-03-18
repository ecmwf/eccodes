
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
#include "grib_accessor_class_optimal_step_units.h"
#include "step.h"
#include "step_utilities.h"

grib_accessor_class_optimal_step_units_t _grib_accessor_class_optimal_step_units{"optimal_step_units"};
grib_accessor_class* grib_accessor_class_optimal_step_units = &_grib_accessor_class_optimal_step_units;


void grib_accessor_class_optimal_step_units_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_optimal_step_units_t* self = (grib_accessor_optimal_step_units_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    int n = 0;

    self->forecast_time_value = grib_arguments_get_name(hand, c, n++);
    self->forecast_time_unit  = grib_arguments_get_name(hand, c, n++);
    self->time_range_value    = grib_arguments_get_name(hand, c, n++);
    self->time_range_unit     = grib_arguments_get_name(hand, c, n++);
    a->length = 0;
}

void grib_accessor_class_optimal_step_units_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

size_t grib_accessor_class_optimal_step_units_t::string_length(grib_accessor* a){
    return 255;
}

int grib_accessor_class_optimal_step_units_t::pack_expression(grib_accessor* a, grib_expression* e){
    const char* cval  = NULL;
    int ret           = 0;
    long lval         = 0;
    size_t len        = 1;
    grib_handle* hand = grib_handle_of_accessor(a);
    const char* cclass_name = a->cclass->name;

    if (strcmp(e->cclass->name, "long") == 0) {
        grib_expression_evaluate_long(hand, e, &lval); /* TODO: check return value */
        ret = a->pack_long(&lval, &len);    }
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
        ret = a->pack_string(cval, &len);    }
    return ret;
}

static long staticStepUnits = eccodes::Unit{eccodes::Unit::Value::MISSING}.value<long>();
static long staticForceStepUnits = eccodes::Unit{eccodes::Unit::Value::MISSING}.value<long>();

int grib_accessor_class_optimal_step_units_t::pack_long(grib_accessor* a, const long* val, size_t* len){
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

int grib_accessor_class_optimal_step_units_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    try {
        if (eccodes::Unit{staticStepUnits} != eccodes::Unit{eccodes::Unit::Value::MISSING}) {
            *val = staticStepUnits;
            return GRIB_SUCCESS;
        }

        const grib_accessor_optimal_step_units_t* self = (grib_accessor_optimal_step_units_t*)a;
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

int grib_accessor_class_optimal_step_units_t::pack_string(grib_accessor* a, const char* val, size_t* len){
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

int grib_accessor_class_optimal_step_units_t::unpack_string(grib_accessor* a, char* val, size_t* len){
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
int grib_accessor_class_optimal_step_units_t::is_missing(grib_accessor* a){
    return 0;
}

int grib_accessor_class_optimal_step_units_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}
