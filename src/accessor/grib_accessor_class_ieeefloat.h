
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

#include "grib_accessor_class_double.h"

class grib_accessor_ieeefloat_t : public grib_accessor_double_t
{
public:
    grib_arguments* arg;
};

class grib_accessor_class_ieeefloat_t : public grib_accessor_class_double_t
{
public:
    grib_accessor_class_ieeefloat_t(const char* name) : grib_accessor_class_double_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_ieeefloat_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    void update_size(grib_accessor*, size_t) override;
    int nearest_smaller_value(grib_accessor* a, double val, double* nearest) override;
};
