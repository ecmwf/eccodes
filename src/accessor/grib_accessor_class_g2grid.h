
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

#include "grib_accessor_class_double.h"

class grib_accessor_g2grid_t : public grib_accessor_double_t
{
public:
    grib_accessor_g2grid_t() :
        grib_accessor_double_t() { class_name_ = "g2grid"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_g2grid_t{}; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* latitude_first_;
    const char* longitude_first_;
    const char* latitude_last_;
    const char* longitude_last_;
    const char* i_increment_;
    const char* j_increment_;
    const char* basic_angle_;
    const char* sub_division_;
};
