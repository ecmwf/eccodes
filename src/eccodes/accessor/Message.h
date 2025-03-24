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

#include "Bytes.h"

namespace eccodes::accessor
{

class Message : public Bytes
{
public:
    Message() :
        Bytes() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;
    void resize(size_t) override;
private:
    static inline const AccessorType accessor_type_{"message"};
};

}  // namespace eccodes::accessor
