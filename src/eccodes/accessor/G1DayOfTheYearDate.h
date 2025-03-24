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

#include "G1Date.h"

namespace eccodes::accessor
{

class G1DayOfTheYearDate : public G1Date
{
public:
    G1DayOfTheYearDate() :
        G1Date() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_string(char*, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
private:
    static inline const AccessorType accessor_type_{"g1day_of_the_year_date"};
};

}  // namespace eccodes::accessor
