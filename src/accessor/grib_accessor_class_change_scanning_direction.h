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

class grib_accessor_change_scanning_direction_t : public grib_accessor_gen_t
{
public:
    grib_accessor_change_scanning_direction_t() :
        grib_accessor_gen_t() { class_name_ = "change_scanning_direction"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_change_scanning_direction_t{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* values_;
    const char* Ni_;
    const char* Nj_;
    const char* i_scans_negatively_;
    const char* j_scans_positively_;
    const char* first_;
    const char* last_;
    const char* axis_;
};
