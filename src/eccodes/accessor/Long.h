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

class Long : public Gen
{
public:
    Long() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    void init(const long len, grib_arguments* arg) override;
    long get_native_type() override;
    int pack_missing() override;
    int pack_string(const char*, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    int compare(grib_accessor*) override;
private:
    static inline const AccessorType accessor_type_{"long"};
};

}  // namespace eccodes::accessor
