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

namespace eccodes::expression {

class Accessor : public Expression
{
public:
    Accessor(grib_context* c, const char* name, long start, size_t length);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override;
    string get_name() override;
    int native_type(grib_handle*) override;
    int evaluate_long(grib_handle*, long*) override;
    int evaluate_double(grib_handle*, double*) override;
    string evaluate_string(grib_handle*, char*, size_t*, int*) override;
    const char* class_name() override { return "accessor"; };

private:
    char* name_;
    long start_;
    size_t length_;
};

};  // namespace eccodes::expression

grib_expression* new_accessor_expression(grib_context* c, const char* name, long start, size_t length);
