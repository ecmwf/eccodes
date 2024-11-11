/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "grib_expression.h"

namespace eccodes::expression
{

class Functor : public Expression
{
public:
    Functor(grib_context* c, const char* name, grib_arguments* args);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override;
    int native_type(grib_handle*) override;
    int evaluate_long(grib_handle*, long*) override;

    // TODO(maee): Check this
    string get_name() override { return nullptr; }
    int evaluate_double(grib_handle*, double*) override { return 0; }
    string evaluate_string(grib_handle*, char*, size_t*, int*) override { return nullptr; }
    const char* class_name() override { return "functor"; };

private:
    char* name_ = nullptr;
    grib_arguments* args_ = nullptr;

    friend Expression* new_func_expression(grib_context* c, const char* name, grib_arguments* args);
};

Expression* new_func_expression(grib_context* c, const char* name, grib_arguments* args);

}  // namespace eccodes::expression


grib_expression* new_func_expression(grib_context* c, const char* name, grib_arguments* args);
