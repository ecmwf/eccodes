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

class Unop : public Expression {
public:
    Unop(grib_context* c, UnopLongProc long_func, UnopDoubleProc double_func, Expression* exp);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override;
    string get_name() override;
    int native_type(grib_handle*) const override;
    int evaluate_long(grib_handle*, long*) override;
    int evaluate_double(grib_handle*, double*) override;

    const char* class_name() const override { return "unop"; };

private:
    Expression* exp_            = nullptr;
    UnopLongProc long_func_     = nullptr;
    UnopDoubleProc double_func_ = nullptr;
};

}  // namespace eccodes::expression

grib_expression* new_unop_expression(grib_context* c, grib_unop_long_proc long_func, grib_unop_double_proc double_func, grib_expression* exp);
