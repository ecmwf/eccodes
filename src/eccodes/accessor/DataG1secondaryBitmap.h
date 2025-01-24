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

#include "DataSecondaryBitmap.h"

namespace eccodes::accessor
{

class DataG1secondaryBitmap : public DataSecondaryBitmap
{
public:
    DataG1secondaryBitmap() :
        DataSecondaryBitmap() { class_name_ = "data_g1secondary_bitmap"; }
    grib_accessor* create_empty_accessor() override { return new DataG1secondaryBitmap{}; }
    int pack_double(const double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* number_of_ones_ = nullptr;
};

}  // namespace eccodes::accessor
