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

class HashArray : public Gen
{
public:
    HashArray() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    int pack_long(const long* val, size_t* len) override;
    int pack_string(const char*, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;

private:
    char* key_ = nullptr;
    grib_hash_array_value* ha_ = nullptr;

    grib_hash_array_value* find_hash_value(int* err);
    static inline const AccessorType accessor_type_{"hash_array"};
};

}  // namespace eccodes::accessor
