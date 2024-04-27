
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_section_padding_h
#define eccodes_accessor_section_padding_h

#include "grib_api_internal.h"
#include "grib_accessor_class_padding.h"

class grib_accessor_section_padding_t : public grib_accessor_padding_t
{
public:
    /* Members defined in section_padding */
    int preserve;
};

class grib_accessor_class_section_padding_t : public grib_accessor_class_padding_t
{
public:
    grib_accessor_class_section_padding_t(const char* name) : grib_accessor_class_padding_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_section_padding_t{}; }
    void init(grib_accessor*, const long, grib_arguments*) override;
    size_t preferred_size(grib_accessor*, int) override;
};
#endif /* eccodes_accessor_section_padding_h */