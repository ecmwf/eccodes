
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_apply_boustrophedonic_bitmap_h
#define eccodes_accessor_data_apply_boustrophedonic_bitmap_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_data_apply_boustrophedonic_bitmap_t : public grib_accessor_gen_t
{
public:
    /* Members defined in data_apply_boustrophedonic_bitmap */
    const char*  coded_values;
    const char*  bitmap;
    const char*  missing_value;
    const char*  binary_scale_factor;
    const char*  numberOfRows;
    const char*  numberOfColumns;
    const char*  numberOfPoints;
};

class grib_accessor_class_data_apply_boustrophedonic_bitmap_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_data_apply_boustrophedonic_bitmap_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_apply_boustrophedonic_bitmap_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;
};
#endif /* eccodes_accessor_data_apply_boustrophedonic_bitmap_h */
