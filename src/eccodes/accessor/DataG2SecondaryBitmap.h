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

class DataG2SecondaryBitmap : public DataSecondaryBitmap
{
public:
    DataG2SecondaryBitmap() :
        DataSecondaryBitmap() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int pack_double(const double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* number_of_values_ = nullptr;
    static inline const AccessorType accessor_type_{"data_g2secondary_bitmap"};
};

}  // namespace eccodes::accessor
