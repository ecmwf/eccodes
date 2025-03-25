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

class RdbTimeGuessDate : public Long
{
public:
    RdbTimeGuessDate() :
        Long() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* typicalYear_ = nullptr;
    const char* typicalMonth_ = nullptr;
    const char* typicalDay_ = nullptr;
    const char* rdbDay_ = nullptr;
    long yearOrMonth_ = 0;

public:
    static inline const AccessorType accessor_type_{"rdbtime_guess_date"};
};

}  // namespace eccodes::accessor
