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

#include "Double.h"

namespace eccodes::accessor
{

class G2Grid : public Double
{
public:
    G2Grid() :
        Double() { class_name_ = "g2grid"; }
    grib_accessor* create_empty_accessor() override { return new G2Grid{}; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* latitude_first_ = nullptr;
    const char* longitude_first_ = nullptr;
    const char* latitude_last_ = nullptr;
    const char* longitude_last_ = nullptr;
    const char* i_increment_ = nullptr;
    const char* j_increment_ = nullptr;
    const char* basic_angle_ = nullptr;
    const char* sub_division_ = nullptr;
};

}  // namespace eccodes::accessor
