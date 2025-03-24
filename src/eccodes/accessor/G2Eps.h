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

#include "Unsigned.h"

namespace eccodes::accessor
{

class G2Eps : public Unsigned
{
public:
    G2Eps() :
        Unsigned() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* productDefinitionTemplateNumber_ = nullptr;
    const char* stream_ = nullptr;
    const char* type_ = nullptr;
    const char* stepType_ = nullptr;
    const char* derivedForecast_ = nullptr;
    static inline const AccessorType accessor_type_{"g2_eps"};
};

}  // namespace eccodes::accessor
