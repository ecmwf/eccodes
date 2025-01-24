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

class DataG2simplePacking : public DataSimplePacking
{
public:
    DataG2simplePacking() :
        DataSimplePacking() { class_name_ = "data_g2simple_packing"; }
    grib_accessor* create_empty_accessor() override { return new DataG2simplePacking{}; }
    int pack_bytes(const unsigned char*, size_t* len) override;
    int pack_double(const double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
};

}  // namespace eccodes::accessor
