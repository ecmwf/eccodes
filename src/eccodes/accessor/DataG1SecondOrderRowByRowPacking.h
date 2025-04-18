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
#include "grib_scaling.h"

namespace eccodes::accessor
{

class DataG1SecondOrderRowByRowPacking : public DataSimplePacking
{
public:
    DataG1SecondOrderRowByRowPacking() :
        DataSimplePacking() { class_name_ = "data_g1second_order_row_by_row_packing"; }
    grib_accessor* create_empty_accessor() override { return new DataG1SecondOrderRowByRowPacking{}; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* half_byte_ = nullptr;
    const char* packingType_ = nullptr;
    const char* ieee_packing_ = nullptr;
    const char* precision_ = nullptr;
    const char* widthOfFirstOrderValues_ = nullptr;
    const char* N1_ = nullptr;
    const char* N2_ = nullptr;
    const char* numberOfGroups_ = nullptr;
    const char* numberOfSecondOrderPackedValues_ = nullptr;
    const char* extraValues_ = nullptr;
    const char* pl_ = nullptr;
    const char* Ni_ = nullptr;
    const char* Nj_ = nullptr;
    const char* jPointsAreConsecutive_ = nullptr;
    const char* bitmap_ = nullptr;
    const char* groupWidths_ = nullptr;

    template <typename T> int unpack_real(T* values, size_t* len);
};

}  // namespace eccodes::accessor
