
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_gds_not_present_bitmap_h
#define eccodes_accessor_gds_not_present_bitmap_h

#include "grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_gds_not_present_bitmap_t : public grib_accessor_gen_t
{
public:
    /* Members defined in gds_not_present_bitmap */
    const char*  missing_value;
    const char*  number_of_values;
    const char*  number_of_points;
    const char*  latitude_of_first_point;
    const char*  ni;
};

class grib_accessor_class_gds_not_present_bitmap_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_gds_not_present_bitmap_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_gds_not_present_bitmap_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_gds_not_present_bitmap_h */
