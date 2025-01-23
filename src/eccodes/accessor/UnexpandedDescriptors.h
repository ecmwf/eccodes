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

#include "Long.h"

namespace eccodes::accessor
{

class grib_accessor_unexpanded_descriptors_t : public grib_accessor_long_t
{
public:
    grib_accessor_unexpanded_descriptors_t() :
        grib_accessor_long_t() { class_name_ = "unexpanded_descriptors"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_unexpanded_descriptors_t{}; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    long byte_offset() override;
    long next_offset() override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;

private:
    grib_accessor* unexpandedDescriptorsEncoded_ = nullptr;
    const char* createNewData_ = nullptr;
};

}  // namespace eccodes::accessor
