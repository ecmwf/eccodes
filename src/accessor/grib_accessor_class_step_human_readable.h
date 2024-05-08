
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_step_human_readable_h
#define eccodes_accessor_step_human_readable_h

#include "grib_accessor_class_gen.h"

class grib_accessor_step_human_readable_t : public grib_accessor_gen_t
{
public:
    /* Members defined in step_human_readable */
    const char* stepUnits;
    const char* step;
};

class grib_accessor_class_step_human_readable_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_step_human_readable_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_step_human_readable_t{}; }
    int get_native_type(grib_accessor*) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_step_human_readable_h */
