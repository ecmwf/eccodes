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

class DataApplyBitmap : public Gen
{
public:
    DataApplyBitmap() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;

private:
    const char* coded_values_ = nullptr;
    const char* bitmap_ = nullptr;
    const char* missing_value_ = nullptr;
    const char* number_of_data_points_ = nullptr;
    const char* number_of_values_ = nullptr;
    const char* binary_scale_factor_ = nullptr;

    template <typename T> int unpack(T* val, size_t* len);
    static inline const AccessorType accessor_type_{"data_apply_bitmap"};
};

}  // namespace eccodes::accessor
