
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

class grib_accessor_validity_time_t : public grib_accessor_long_t
{
public:
    grib_accessor_validity_time_t() :
        grib_accessor_long_t() {}
    static inline const AccessorType accessor_type{"validity_time"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int unpack_long(long* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* date_;
    const char* time_;
    const char* step_;
    const char* stepUnits_;
    const char* hours_;
    const char* minutes_;
};
