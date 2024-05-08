
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_g1monthlydate_h
#define eccodes_accessor_g1monthlydate_h

#include "grib_accessor_class_long.h"

class grib_accessor_g1monthlydate_t : public grib_accessor_long_t
{
public:
    /* Members defined in g1monthlydate */
    const char* date;
};

class grib_accessor_class_g1monthlydate_t : public grib_accessor_class_long_t
{
public:
    grib_accessor_class_g1monthlydate_t(const char* name) : grib_accessor_class_long_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_g1monthlydate_t{}; }
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_g1monthlydate_h */
