
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

#include "grib_accessor_class_double.h"

class grib_accessor_scale_values_t : public grib_accessor_double_t
{
public:
    grib_accessor_scale_values_t() :
        grib_accessor_double_t() {}
    static inline const AccessorType accessor_type{"scale_values"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* values_;
    const char* missingValue_;
};
