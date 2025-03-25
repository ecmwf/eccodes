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

class G2MarsLabeling : public Gen
{
public:
    G2MarsLabeling() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    int pack_string(const char*, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    int index_ = 0;
    const char* the_class_ = nullptr;
    const char* stream_ = nullptr;
    const char* type_ = nullptr;
    const char* expver_ = nullptr;
    const char* typeOfProcessedData_ = nullptr;
    const char* productDefinitionTemplateNumber_ = nullptr;
    const char* stepType_ = nullptr;
    const char* derivedForecast_ = nullptr;
    const char* typeOfGeneratingProcess_ = nullptr;

    int extra_set(long val);

public:
    static inline const AccessorType accessor_type_{"g2_mars_labeling"};
};

}  // namespace eccodes::accessor
