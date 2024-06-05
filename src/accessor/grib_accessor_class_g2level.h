
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

class grib_accessor_g2level_t : public grib_accessor_long_t
{
public:
    /* Members defined in g2level */
    const char* type_first;
    const char* scale_first;
    const char* value_first;
    const char* pressure_units;
};

class grib_accessor_class_g2level_t : public grib_accessor_class_long_t
{
public:
    grib_accessor_class_g2level_t(const char* name) : grib_accessor_class_long_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_g2level_t{}; }
    int is_missing(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
