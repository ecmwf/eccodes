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

#include "grib_accessor_class_gen.h"

class grib_accessor_values_t : public grib_accessor_gen_t
{
public:
    grib_accessor_values_t() :
        grib_accessor_gen_t() { class_name_ = "values"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_values_t{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    long byte_count() override;
    long byte_offset() override;
    long next_offset() override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;
    int compare(grib_accessor*) override;

public:
    int carg_;
    const char* seclen_;
    const char* offsetdata_;
    const char* offsetsection_;
    int dirty_;
};
