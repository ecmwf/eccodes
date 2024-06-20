
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

#include "grib_accessor_class_abstract_long_vector.h"

class grib_accessor_g1step_range_t : public grib_accessor_abstract_long_vector_t
{
public:
    /* Members defined in g1step_range */
    const char* p1;
    const char* p2;
    const char* timeRangeIndicator;
    const char *unit;
    const char *step_unit;
    const char *stepType;
    const char *patch_fp_precip;
    int error_on_units;
};

class grib_accessor_class_g1step_range_t : public grib_accessor_class_abstract_long_vector_t
{
public:
    grib_accessor_class_g1step_range_t(const char* name) : grib_accessor_class_abstract_long_vector_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_g1step_range_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int pack_string(grib_accessor*, const char*, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    size_t string_length(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
