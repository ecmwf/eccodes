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

#include "grib_accessor_class_unsigned.h"
#include <cctype>

class grib_accessor_codetable_t : public grib_accessor_unsigned_t
{
public:
    grib_accessor_codetable_t() :
        grib_accessor_unsigned_t() {}
    static inline const AccessorType accessor_type{"codetable"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int pack_missing() override;
    int pack_string(const char*, size_t* len) override;
    int pack_expression(grib_expression*) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;

public:
    grib_codetable* table_;
    const char* tablename_;
    const char* masterDir_;
    const char* localDir_;
    int table_loaded_;
};
