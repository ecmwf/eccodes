
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

class grib_accessor_section_pointer_t : public grib_accessor_gen_t
{
public:
    grib_accessor_section_pointer_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"section_pointer"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int unpack_string(char*, size_t* len) override;
    long byte_count() override;
    long byte_offset() override;
    void init(const long, grib_arguments*) override;

private:
    const char* sectionOffset_;
    const char* sectionLength_;
    long sectionNumber_;
};
