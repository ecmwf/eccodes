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

#include "Values.h"
#include "grib_scaling.h"

namespace eccodes::accessor
{

class DataCcsdsPacking : public Values
{
public:
    DataCcsdsPacking() :
        Values() {}
    const AccessorType& accessor_type() const override { return accessor_type_; }
    int pack_double(const double* val, size_t* len) override;
    int pack_float(const float* val, size_t* len) override;
    int pack_bytes(const unsigned char*, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;

private:
    const char* number_of_values_ = nullptr;
    const char* reference_value_ = nullptr;
    const char* binary_scale_factor_ = nullptr;
    const char* decimal_scale_factor_ = nullptr;
    const char* optimize_scaling_factor_ = nullptr;
    const char* bits_per_value_ = nullptr;
    const char* number_of_data_points_ = nullptr;
    const char* ccsds_flags_ = nullptr;
    const char* ccsds_block_size_ = nullptr;
    const char* ccsds_rsi_ = nullptr;

    template <typename T>
    int unpack(T* val, size_t* len);

    // The implementations (DataCcsdsPacking.cc selects one):
    // - DataCcsdsPackingLegacy.cc: packing parameters as in legacy ecCodes, coding with libaec
    // - DataCcsdsPackingNew.cc: packing parameters with the simple_packing library, coding with SpAec
    int pack_double_legacy(const double* val, size_t* len);
    template <typename T>
    int pack_new(const T* val, size_t* len);  // floats: not converted (the codes of the converted doubles)
    template <typename T>
    int pack(const T* val, size_t* len);
    template <typename T>
    int unpack_legacy(T* val, size_t* len);
    template <typename T>
    int unpack_new(T* val, size_t* len);

public:
    static constexpr AccessorType accessor_type_{ "data_ccsds_packing" };
};

}  // namespace eccodes::accessor
