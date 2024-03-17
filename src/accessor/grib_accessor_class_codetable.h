/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*****************************************
 *  Enrico Fucile
 ****************************************/

#pragma once

#include "../grib_api_internal.h"
#include "grib_accessor_class_unsigned.h"
#include <cctype>


class grib_accessor_codetable_t : public grib_accessor_unsigned_t
{
public:
    const char* tablename;
    const char* masterDir;
    const char* localDir;
    grib_codetable* table;
    int table_loaded;
};

class grib_accessor_class_codetable_t : public grib_accessor_class_unsigned_t
{
public:
    grib_accessor* create_empty_accessor() override { return new grib_accessor_codetable_t{}; }
    grib_accessor_class_codetable_t(const char* name) : grib_accessor_class_unsigned_t(name) {}
    int get_native_type(grib_accessor*) override;
    int pack_missing(grib_accessor*) override;
    int pack_string(grib_accessor*, const char*, size_t* len) override;
    int pack_expression(grib_accessor*, grib_expression*) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
