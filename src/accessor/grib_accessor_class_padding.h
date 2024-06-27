
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

#include "grib_accessor_class_bytes.h"

class grib_accessor_padding_t : public grib_accessor_bytes_t
{
public:
    /* Members defined in padding */
};

class grib_accessor_class_padding_t : public grib_accessor_class_bytes_t
{
public:
    grib_accessor_class_padding_t(const char* name) : grib_accessor_class_bytes_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_padding_t{}; }
    size_t string_length(grib_accessor*) override;
    long byte_count(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    void update_size(grib_accessor*, size_t) override;
    void resize(grib_accessor*,size_t) override;
    int compare(grib_accessor*, grib_accessor*) override;
};
