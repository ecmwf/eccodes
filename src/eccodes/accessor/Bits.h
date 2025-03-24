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

class Bits : public Gen
{
public:
    Bits() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_bytes(unsigned char*, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    long byte_count() override;
    void init(const long, grib_arguments*) override;

private:
    const char* argument_ = nullptr;
    long start_ = 0;
    long len_ = 0;
    double referenceValue_ = 0.;
    double referenceValuePresent_ = 0.;
    double scale_ = 0.;
    static inline const AccessorType accessor_type_{"bits"};
};

}  // namespace eccodes::accessor
