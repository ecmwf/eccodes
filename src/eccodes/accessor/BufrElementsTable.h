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

class BufrElementsTable : public Gen
{
public:
    BufrElementsTable() :
        Gen() { class_name_ = "bufr_elements_table"; }
    grib_accessor* create_empty_accessor() override { return new BufrElementsTable{}; }
    long get_native_type() override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

    bufr_descriptor* get_descriptor(int code, int* err);

private:
    const char* dictionary_ = nullptr;
    const char* masterDir_ = nullptr;
    const char* localDir_ = nullptr;

    grib_trie* load_bufr_elements_table(int* err);
    int bufr_get_from_table(bufr_descriptor* v);
};

}  // namespace eccodes::accessor

int bufr_descriptor_is_marker(bufr_descriptor* d);
