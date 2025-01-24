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

#include "Gen.h"

namespace eccodes::accessor
{

class Variable : public Gen
{
public:
    Variable() :
        Gen() { class_name_ = "variable"; }
    grib_accessor* create_empty_accessor() override { return new Variable{}; }
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    int pack_float(const float* val, size_t* len) override;
    int pack_long(const long* val, size_t* len) override;
    int pack_string(const char*, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    long byte_count() override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;
    grib_accessor* make_clone(grib_section*, int*) override;

    void accessor_variable_set_type(int type);
private:
    double dval_ = 0.;
    float fval_ = 0.f;
    char* cval_ = nullptr;
    char* cname_ = nullptr;
    int type_ = 0;
};

}  // namespace eccodes::accessor
