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

#include "grib_accessor_class_long.h"

class grib_accessor_g2levelist_t : public grib_accessor_long_t
{
public:
    grib_accessor_g2levelist_t() :
        grib_accessor_long_t() { class_name_ = "g2levelist"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_g2levelist_t{}; }
    int is_missing() override;
    int pack_double(const double* val, size_t* len) override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private: //functions
    //grib_accessor* get_level_accessor();
    bool exists();

private: //data
    const char* levelAccessorName_ = nullptr;
    const char* pressure_units_ = nullptr;
    const char* levtype_        = nullptr;
    const char* typeOfLevel_    = nullptr;

    //grib_accessor* levelAccessor_ = nullptr;
};
