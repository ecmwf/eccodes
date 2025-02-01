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

class grib_accessor_message_is_valid_t : public grib_accessor_long_t
{
public:
    grib_accessor_message_is_valid_t() :
        grib_accessor_long_t() { class_name_ = "message_is_valid"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_message_is_valid_t{}; }
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    int check_field_values(grib_handle* h);
    int check_grid_pl_array(grib_handle* h);
    int check_geoiterator(grib_handle* h);
    int check_7777(grib_handle* h);
    int check_surface_keys(grib_handle* h);
    int check_steps(grib_handle* h);
    int check_section_numbers(grib_handle* h, const int* sec_nums, size_t N);
    int check_namespace_keys(grib_handle* h);
    int check_sections(grib_handle* h);

private:
    const char* product_ = nullptr;
    long edition_ = 0;
};
