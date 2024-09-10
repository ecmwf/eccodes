
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

#include "grib_accessor_class_g1step_range.h"

class grib_accessor_g1fcperiod_t : public grib_accessor_g1step_range_t
{
public:
    grib_accessor_g1fcperiod_t() :
        grib_accessor_g1step_range_t() {}
    static inline const AccessorType accessor_type{"g1fcperiod"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int unpack_string(char*, size_t* len) override;
};
