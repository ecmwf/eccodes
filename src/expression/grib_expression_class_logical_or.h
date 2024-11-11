/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_expression.h"

namespace eccodes::expression
{

class LogicalOr : public Expression
{
public:
    LogicalOr(grib_context* c, Expression* left, Expression* right);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) override;
    void add_dependency(grib_accessor* observer) override;
    int native_type(grib_handle*) override;
    int evaluate_long(grib_handle*, long*) override;
    int evaluate_double(grib_handle*, double*) override;
    const char* class_name() override { return "logical_or"; };

    // TODO(maee): Check this
    string get_name() override { return nullptr; }
    string evaluate_string(grib_handle*, char*, size_t*, int*) override { return nullptr; }

private:
    Expression* left_  = nullptr;
    Expression* right_ = nullptr;
};

}  // namespace eccodes::expression

grib_expression* new_logical_or_expression(grib_context* c, grib_expression* left, grib_expression* right);
