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

#include "grib_api_internal.h"
#include <functional>

namespace eccodes::expression {

class Expression {
public:
    using string = const char*;

    using PackProc        = std::function<int(grib_handle*, const double*, size_t, void*, size_t*)>;
    using UnpackProc      = std::function<int(grib_handle*, const void*, size_t, double*, size_t*)>;
    using BinopLongProc   = std::function<long(long, long)>;
    using UnopLongProc    = std::function<long(long)>;
    using BinopDoubleProc = std::function<double(double, double)>;
    using UnopDoubleProc  = std::function<double(double)>;
    using BinopStringProc = std::function<int(char*, char*)>;

    virtual ~Expression() {};
    virtual void destroy(grib_context*);
    virtual void print(grib_context*, grib_handle*, FILE*);
    virtual void add_dependency(grib_accessor* observer);
    virtual string get_name() const;
    virtual int native_type(grib_handle*) const;
    virtual int evaluate_long(grib_handle*, long*);
    virtual int evaluate_double(grib_handle*, double*);
    virtual string evaluate_string(grib_handle*, char*, size_t*, int*);

    virtual const char* class_name() const = 0;
};

};  // namespace eccodes::expression

using grib_expression = eccodes::expression::Expression;

namespace eccodes {
using Expression = eccodes::expression::Expression;
}  // namespace eccodes

void grib_expression_free(grib_context* ctx, grib_expression* g);
