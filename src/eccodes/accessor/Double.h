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

#include "Gen.h"

namespace eccodes::accessor
{

class Double : public Gen
{
public:
    Double() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    long get_native_type() override;
    int pack_missing() override;
    int unpack_string(char*, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    int compare(grib_accessor*) override;
private:

public:
    static inline const AccessorType accessor_type_{"double"};
};

}  // namespace eccodes::accessor
