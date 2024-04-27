
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_sh_unpacked_h
#define eccodes_accessor_data_sh_unpacked_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_data_simple_packing.h"

class grib_accessor_data_sh_unpacked_t : public grib_accessor_data_simple_packing_t
{
public:
    /* Members defined in data_sh_unpacked */
    const char*  GRIBEX_sh_bug_present;
    const char*  ieee_floats;
    const char*  laplacianOperatorIsSet;
    const char*  laplacianOperator;
    const char*  sub_j;
    const char*  sub_k;
    const char*  sub_m;
    const char*  pen_j;
    const char*  pen_k;
    const char*  pen_m;
};

class grib_accessor_class_data_sh_unpacked_t : public grib_accessor_class_data_simple_packing_t
{
public:
    grib_accessor_class_data_sh_unpacked_t(const char* name) : grib_accessor_class_data_simple_packing_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_sh_unpacked_t{}; }
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_data_sh_unpacked_h */
