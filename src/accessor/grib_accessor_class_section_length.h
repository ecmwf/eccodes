
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

#include "grib_accessor_class_unsigned.h"

class grib_accessor_section_length_t : public grib_accessor_unsigned_t
{
public:
    grib_accessor_section_length_t() :
        grib_accessor_unsigned_t() {}
    static inline const AccessorType accessor_type{"section_length"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int value_count(long*) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;
};
