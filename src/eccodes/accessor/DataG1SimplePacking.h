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

class DataG1SimplePacking : public DataSimplePacking
{
public:
    DataG1SimplePacking() :
        DataSimplePacking() { class_name_ = "data_g1simple_packing"; }
    grib_accessor* create_empty_accessor() override { return new DataG1SimplePacking{}; }
    int pack_double(const double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

protected:
    const char* half_byte_ = nullptr;
    const char* packingType_ = nullptr;
    const char* ieee_packing_ = nullptr;
    const char* precision_ = nullptr;
};

}  // namespace eccodes::accessor
