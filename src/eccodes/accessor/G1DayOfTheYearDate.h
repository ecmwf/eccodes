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

#include "G1date.h"

namespace eccodes::accessor
{

class G1DayOfTheYearDate : public G1Date
{
public:
    G1DayOfTheYearDate() :
        G1Date() { class_name_ = "g1day_of_the_year_date"; }
    grib_accessor* create_empty_accessor() override { return new G1DayOfTheYearDate{}; }
    int unpack_string(char*, size_t* len) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
};

}  // namespace eccodes::accessor
