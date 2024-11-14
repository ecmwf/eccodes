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

class Double : public Expression
{
public:
    Double(grib_context* c, double value);

    void destroy(grib_context*) override {}
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override {}
    int native_type(grib_handle*) override;
    int evaluate_long(grib_handle*, long*) override;
    int evaluate_double(grib_handle*, double*) override;

    const char* class_name() override { return "double"; };

private:
    double value_ = 0.0;
};

}  // namespace eccodes::expression

grib_expression* new_double_expression(grib_context* c, double value);
