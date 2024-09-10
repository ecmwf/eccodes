
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

#include "grib_accessor_class_section_length.h"

class grib_accessor_g1_section4_length_t : public grib_accessor_section_length_t
{
public:
    grib_accessor_g1_section4_length_t() :
        grib_accessor_section_length_t() {}
    static inline const AccessorType accessor_type{"g1_section4_length"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* total_length_;
};
