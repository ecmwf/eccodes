/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "Expression.h"

namespace eccodes::expression {

class String : public Expression {
public:
    String(grib_context*, const char*);

    void destroy(grib_context*) override;
    void print(grib_context*, grib_handle*, FILE*) const override;
    void add_dependency(grib_accessor*) override;
    int native_type(grib_handle*) const override;
    const char* evaluate_string(grib_handle*, char*, size_t*, int*) const override;

    const char* class_name() const override { return "string"; };

private:
    char* value_ = nullptr;
};

}  // namespace eccodes::expression

grib_expression* new_string_expression(grib_context*, const char*);
