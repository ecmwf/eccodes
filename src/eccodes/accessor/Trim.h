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

class Trim : public Ascii
{
public:
    Trim() :
        Ascii() { class_name_ = "trim"; }
    int pack_string(const char*, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    void init(const long, grib_arguments*) override;

private:
    const char* input_ = nullptr;
    int trim_left_ = 0;
    int trim_right_ = 0;
};

}  // namespace eccodes::accessor
