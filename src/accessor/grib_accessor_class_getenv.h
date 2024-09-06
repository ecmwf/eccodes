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

#include "grib_accessor_class_ascii.h"

class grib_accessor_getenv_t : public grib_accessor_ascii_t
{
public:
    const char* envvar;
    char* value;
    const char* default_value;
};

class grib_accessor_class_getenv_t : public grib_accessor_class_ascii_t
{
public:
    grib_accessor_class_getenv_t(const char* envvar) : grib_accessor_class_ascii_t(envvar) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_getenv_t{}; }
    int pack_string(grib_accessor*, const char*, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    size_t string_length(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
