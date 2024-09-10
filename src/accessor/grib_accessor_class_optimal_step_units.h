
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

#include "grib_accessor_class_gen.h"

class grib_accessor_optimal_step_units_t : public grib_accessor_gen_t
{
public:
    grib_accessor_optimal_step_units_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"optimal_step_units"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int is_missing() override;
    int pack_long(const long* val, size_t* len) override;
    int pack_string(const char*, size_t* len) override;
    int pack_expression(grib_expression*) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* forecast_time_value_;
    const char* forecast_time_unit_;
    const char* time_range_value_;
    const char* time_range_unit_;
    long overwriteStepUnits_;
};
