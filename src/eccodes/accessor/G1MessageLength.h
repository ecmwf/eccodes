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

#include "SectionLength.h"

namespace eccodes::accessor
{

class G1MessageLength : public SectionLength
{
public:
    G1MessageLength() :
        SectionLength() { class_name_ = "g1_message_length"; }
    grib_accessor* create_empty_accessor() override { return new G1MessageLength{}; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* sec4_length_ = nullptr;
};

}  // namespace eccodes::accessor
