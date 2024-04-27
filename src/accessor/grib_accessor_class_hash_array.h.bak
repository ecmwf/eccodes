
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_hash_array_h
#define eccodes_accessor_hash_array_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_hash_array_t : public grib_accessor_gen_t
{
public:
    /* Members defined in hash_array */
    char* key;
    grib_hash_array_value* ha;
};

class grib_accessor_class_hash_array_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_hash_array_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_hash_array_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int pack_string(grib_accessor*, const char*, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    size_t string_length(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int compare(grib_accessor*, grib_accessor*) override;
};
#endif /* eccodes_accessor_hash_array_h */
