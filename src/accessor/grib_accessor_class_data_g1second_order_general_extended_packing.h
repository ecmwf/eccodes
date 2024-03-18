
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_g1second_order_general_extended_packing_h
#define eccodes_accessor_data_g1second_order_general_extended_packing_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_data_simple_packing.h"

class grib_accessor_data_g1second_order_general_extended_packing_t : public grib_accessor_data_simple_packing_t
{
public:
    /* Members defined in data_g1second_order_general_extended_packing */
    const char* half_byte;
    const char* packingType;
    const char* ieee_packing;
    const char* precision;
    const char* widthOfFirstOrderValues;
    const char* firstOrderValues;
    const char* N1;
    const char* N2;
    const char* numberOfGroups;
    const char* codedNumberOfGroups;
    const char* numberOfSecondOrderPackedValues;
    const char* extraValues;
    const char* groupWidths;
    const char* widthOfWidths;
    const char* groupLengths;
    const char* widthOfLengths;
    const char* NL;
    const char* SPD;
    const char* widthOfSPD;
    const char* orderOfSPD;
    const char* numberOfPoints;
    const char* dataFlag;
    double* dvalues;
    float* fvalues;
    int double_dirty;
    int float_dirty;
    size_t size;
};

class grib_accessor_class_data_g1second_order_general_extended_packing_t : public grib_accessor_class_data_simple_packing_t
{
public:
    grib_accessor_class_data_g1second_order_general_extended_packing_t(const char* name) : grib_accessor_class_data_simple_packing_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_g1second_order_general_extended_packing_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;
};
#endif /* eccodes_accessor_data_g1second_order_general_extended_packing_h */
