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

class G1NumberOfCodedValuesShComplex : public Long
{
public:
    G1NumberOfCodedValuesShComplex() :
        Long() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* numberOfValues_ = nullptr;
    const char* bitsPerValue_ = nullptr;
    const char* offsetBeforeData_ = nullptr;
    const char* offsetAfterData_ = nullptr;
    const char* unusedBits_ = nullptr;
    const char* JS_ = nullptr;
    const char* KS_ = nullptr;
    const char* MS_ = nullptr;
    static inline const AccessorType accessor_type_{"g1number_of_coded_values_sh_complex"};
};

}  // namespace eccodes::accessor
