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

class SubString : public Expression
{
public:
    SubString(grib_context* c, const char* value, size_t start, size_t length);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override;
    int native_type(grib_handle*) override;
    string evaluate_string(grib_handle*, char*, size_t*, int*) override;
    const char* class_name() override { return "sub_string"; };

    // TODO(maee): Check this
    string get_name() override { return nullptr; }
    int evaluate_long(grib_handle*, long*) override { return 0; }
    int evaluate_double(grib_handle*, double*) override { return 0; }

private:
    char* value_ = nullptr;
};

}  // namespace eccodes::expression

grib_expression* new_sub_string_expression(grib_context* c, const char* value, size_t start, size_t length);
