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

class grib_accessor_unsigned_t : public grib_accessor_long_t
{
public:
    long nbytes;
    grib_arguments* arg;
};


class grib_accessor_class_unsigned_t : public grib_accessor_class_long_t
{
public:
    grib_accessor_class_unsigned_t(const char* name) : grib_accessor_class_long_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_unsigned_t{}; }
    void init(grib_accessor* a, const long len, grib_arguments* arg) override;
    void dump(grib_accessor* a, grib_dumper* dumper) override;
    int unpack_long(grib_accessor* a, long* val, size_t* len) override;
    int pack_long(grib_accessor* a, const long* val, size_t* len) override;
    long byte_count(grib_accessor* a) override;
    int value_count(grib_accessor* a, long* len) override;
    long byte_offset(grib_accessor* a) override;
    void update_size(grib_accessor* a, size_t s) override;
    long next_offset(grib_accessor* a) override;
    int is_missing(grib_accessor* a) override;
    void destroy(grib_context* context, grib_accessor* a) override;
};
