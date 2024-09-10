
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

class grib_accessor_number_of_values_t : public grib_accessor_long_t
{
public:
    grib_accessor_number_of_values_t() :
        grib_accessor_long_t() {}
    static inline const AccessorType accessor_type{"number_of_values"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* values_;
    const char* bitsPerValue_;
    const char* numberOfPoints_;
    const char* bitmapPresent_;
    const char* bitmap_;
    const char* numberOfCodedValues_;
};
