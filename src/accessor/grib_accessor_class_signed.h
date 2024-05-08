
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_signed_h
#define eccodes_accessor_signed_h

#include "grib_accessor_class_long.h"

class grib_accessor_signed_t : public grib_accessor_long_t
{
public:
    /* Members defined in signed */
    grib_arguments* arg;
    int nbytes;
};

class grib_accessor_class_signed_t : public grib_accessor_class_long_t
{
public:
    grib_accessor_class_signed_t(const char* name) : grib_accessor_class_long_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_signed_t{}; }
    int is_missing(grib_accessor*) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    long byte_count(grib_accessor*) override;
    long byte_offset(grib_accessor*) override;
    long next_offset(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    void update_size(grib_accessor*, size_t) override;
};
#endif /* eccodes_accessor_signed_h */
