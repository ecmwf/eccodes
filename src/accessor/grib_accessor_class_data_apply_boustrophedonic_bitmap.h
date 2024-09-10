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

#include "grib_accessor_class_gen.h"

class grib_accessor_data_apply_boustrophedonic_bitmap_t : public grib_accessor_gen_t
{
public:
    grib_accessor_data_apply_boustrophedonic_bitmap_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"data_apply_boustrophedonic_bitmap"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;

private:
    const char* coded_values_;
    const char* bitmap_;
    const char* missing_value_;
    const char* binary_scale_factor_;
    const char* numberOfRows_;
    const char* numberOfColumns_;
    const char* numberOfPoints_;
};
