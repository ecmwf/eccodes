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

#include "DataSimplePacking.h"

namespace eccodes::accessor
{

class DataJpeg2000Packing : public DataSimplePacking
{
public:
    DataJpeg2000Packing() :
        DataSimplePacking() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;

private:
    const char* type_of_compression_used_ = nullptr;
    const char* target_compression_ratio_ = nullptr;
    const char* ni_ = nullptr;
    const char* nj_ = nullptr;
    const char* list_defining_points_ = nullptr;
    const char* number_of_data_points_ = nullptr;
    const char* scanning_mode_ = nullptr;
    int jpeg_lib_ = 0;
    const char* dump_jpg_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"data_jpeg2000_packing"};
};

}  // namespace eccodes::accessor
