
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
    grib_accessor_bitmap_t() :
        grib_accessor_bytes_t() {}
    static inline const AccessorType accessor_type{"bitmap"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    long next_offset() override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;
    // TODO(maee): make members private
public:
    const char* tableReference_;
    const char* missing_value_;
    const char* offsetbsec_;
    const char* sLength_;
};
