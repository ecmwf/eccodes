
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

#include "grib_accessor_class_bytes.h"

class grib_accessor_bitmap_t : public grib_accessor_bytes_t
{
public:
    /* Members defined in bitmap */
    const char* tableReference;
    const char* missing_value;
    const char* offsetbsec;
    const char* sLength;
};

class grib_accessor_class_bitmap_t : public grib_accessor_class_bytes_t
{
public:
    grib_accessor_class_bitmap_t(const char* name) : grib_accessor_class_bytes_t(name) {}
    //grib_accessor* create_empty_accessor() override { return new grib_accessor_bitmap_t{}; }
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    size_t string_length(grib_accessor*) override;
    long next_offset(grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    void update_size(grib_accessor*, size_t) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;
};
