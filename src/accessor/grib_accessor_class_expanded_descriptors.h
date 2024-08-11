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

class grib_accessor_expanded_descriptors_t : public grib_accessor_long_t
{
public:
    grib_accessor_expanded_descriptors_t() :
        grib_accessor_long_t() { class_name_ = "expanded_descriptors"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_expanded_descriptors_t{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string_array(char**, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void init(const long, grib_arguments*) override;

public:
    const char* unexpandedDescriptors_;
    const char* sequence_;
    const char* expandedName_;
    const char* tablesAccessorName_;
    bufr_descriptors_array* expanded_;
    int rank_;
    grib_accessor* expandedAccessor_;
    int do_expand_;
    grib_accessor* tablesAccessor_;
};

int grib_accessor_expanded_descriptors_set_do_expand(grib_accessor* a, long do_expand);
bufr_descriptors_array* grib_accessor_expanded_descriptors_get_expanded(grib_accessor* a, int* err);
