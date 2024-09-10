
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

class grib_accessor_g2end_step_t : public grib_accessor_long_t
{
public:
    grib_accessor_g2end_step_t() :
        grib_accessor_long_t() {}
    static inline const AccessorType accessor_type{"g2end_step"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    int pack_string(const char*, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;

public:
    const char* start_step_value_;
    const char* step_units_;
    const char* year_;
    const char* month_;
    const char* day_;
    const char* hour_;
    const char* minute_;
    const char* second_;
    const char* year_of_end_of_interval_;
    const char* month_of_end_of_interval_;
    const char* day_of_end_of_interval_;
    const char* hour_of_end_of_interval_;
    const char* minute_of_end_of_interval_;
    const char* second_of_end_of_interval_;
    const char* time_range_unit_;
    const char* time_range_value_;
    const char* typeOfTimeIncrement_;
    const char* numberOfTimeRanges_;
};
