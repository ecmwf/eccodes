
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_g22order_packing_h
#define eccodes_accessor_data_g22order_packing_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_values.h"

class grib_accessor_data_g22order_packing_t : public grib_accessor_values_t
{
public:
    /* Members defined in data_g22order_packing */
    const char*  numberOfValues;
    const char*  bits_per_value;
    const char*  reference_value;
    const char*  binary_scale_factor;
    const char*  decimal_scale_factor;
    const char*  optimize_scale_factor;
    const char*  typeOfOriginalFieldValues;
    const char*  groupSplittingMethodUsed;
    const char*  missingValueManagementUsed;
    const char*  primaryMissingValueSubstitute;
    const char*  secondaryMissingValueSubstitute;
    const char*  numberOfGroupsOfDataValues;
    const char*  referenceForGroupWidths;
    const char*  numberOfBitsUsedForTheGroupWidths;
    const char*  referenceForGroupLengths;
    const char*  lengthIncrementForTheGroupLengths;
    const char*  trueLengthOfLastGroup;
    const char*  numberOfBitsUsedForTheScaledGroupLengths;
    const char*  orderOfSpatialDifferencing;
    const char*  numberOfOctetsExtraDescriptors;
};

class grib_accessor_class_data_g22order_packing_t : public grib_accessor_class_values_t
{
public:
    grib_accessor_class_data_g22order_packing_t(const char* name) : grib_accessor_class_values_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_g22order_packing_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;
};
#endif /* eccodes_accessor_data_g22order_packing_h */
