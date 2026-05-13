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

class Lookup : public Long
{
public:
    Lookup() :
        Long() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    long byte_count() override;
    long byte_offset() override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    void post_init() override;
    int notify_change(grib_accessor*) override;

private:
    long llength_ = 0;
    long loffset_ = 0;
    grib_expression* real_name_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"lookup"};
};

}  // namespace eccodes::accessor
