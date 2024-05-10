
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

class grib_accessor_bufr_string_values_t : public grib_accessor_ascii_t
{
public:
    /* Members defined in bufr_string_values */
    const char* dataAccessorName;
    grib_accessor* dataAccessor;
};

class grib_accessor_class_bufr_string_values_t : public grib_accessor_class_ascii_t
{
public:
    grib_accessor_class_bufr_string_values_t(const char* name) : grib_accessor_class_ascii_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_bufr_string_values_t{}; }
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    int unpack_string_array(grib_accessor*, char**, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
