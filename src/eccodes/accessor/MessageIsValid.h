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

class MessageIsValid : public Long
{
public:
    MessageIsValid() :
        Long() { class_name_ = "message_is_valid"; }
    grib_accessor* create_empty_accessor() override { return new MessageIsValid{}; }
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    int check_7777();
    int check_sections();
    int check_date();
    int check_spectral();
    int check_grid_and_packing_type();
    int check_field_values();
    int check_grid_pl_array();
    int check_geoiterator();
    int check_surface_keys();
    int check_steps();
    int check_section_numbers(const int* sec_nums, size_t N);
    int check_namespace_keys();
    int check_parameter();
    int check_number_of_missing();

private:
    const char* product_ = nullptr;
    grib_handle* handle_ = nullptr;
    long edition_ = 0;
};

}  // namespace eccodes::accessor
