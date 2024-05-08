
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_to_double_h
#define eccodes_accessor_to_double_h

#include "grib_accessor_class_gen.h"

class grib_accessor_to_double_t : public grib_accessor_gen_t
{
public:
    /* Members defined in to_double */
    const char* key;
    long start;
    size_t length;
    long scale;
};

class grib_accessor_class_to_double_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_to_double_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_to_double_t{}; }
    int get_native_type(grib_accessor*) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    size_t string_length(grib_accessor*) override;
    long next_offset(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_to_double_h */
