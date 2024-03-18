
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_bits_h
#define eccodes_accessor_bits_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_bits_t : public grib_accessor_gen_t
{
public:
    /* Members defined in bits */
    const char*    argument;
    long    start;
    long    len;
    double referenceValue;
    double referenceValuePresent;
    double scale;
};

class grib_accessor_class_bits_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_bits_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_bits_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int unpack_bytes(grib_accessor*, unsigned char*, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    long byte_count(grib_accessor*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_bits_h */
