
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

class grib_accessor_section_t : public grib_accessor_gen_t
{
public:
    grib_accessor_section_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"section"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    long byte_count() override;
    long next_offset() override;
    void destroy(grib_context*) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;
    void update_size(size_t) override;
    grib_section* sub_section() override;
    grib_accessor* next(grib_accessor*, int) override;
};
