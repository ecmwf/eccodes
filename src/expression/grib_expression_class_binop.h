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

class Binop : public Expression
{
public:
    Binop(grib_context* c, BinopLongProc long_func, BinopDoubleProc double_func, Expression* left, Expression* right);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override;
    int native_type(grib_handle*) override;
    int evaluate_long(grib_handle*, long*) override;
    int evaluate_double(grib_handle*, double*) override;

    // TODO(maee): Check this
    string get_name() override { return nullptr; }
    string evaluate_string(grib_handle*, char*, size_t*, int*) override { return nullptr; }
    const char* class_name() override { return "binop"; };

private:

    Expression* left_ = nullptr;
    Expression* right_ = nullptr;
    BinopLongProc long_func_ = nullptr;
    BinopDoubleProc double_func_ = nullptr;
    BinopStringProc string_func_ = nullptr;
};

};  // namespace eccodes::expression

grib_expression* new_binop_expression(grib_context* c,
                                      eccodes::Expression::BinopLongProc long_func,
                                      eccodes::Expression::BinopDoubleProc double_func,
                                      grib_expression* left, grib_expression* right);
