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

#include "Bitmap.h"

namespace eccodes::accessor
{

class G1Bitmap : public Bitmap
{
public:
    G1Bitmap() :
        Bitmap() { class_name_ = "g1bitmap"; }
    grib_accessor* create_empty_accessor() override { return new G1Bitmap{}; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_bytes(unsigned char*, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* unusedBits_ = nullptr;
};

}  // namespace eccodes::accessor
