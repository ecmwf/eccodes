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

#include "Ascii.h"

namespace eccodes::accessor
{

class GtsHeader : public Ascii
{
public:
    GtsHeader() :
        Ascii() { class_name_ = "gts_header"; }
    grib_accessor* create_empty_accessor() override { return new GtsHeader{}; }
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    int gts_offset_ = 0;
    int gts_length_ = 0;
};

}  // namespace eccodes::accessor
