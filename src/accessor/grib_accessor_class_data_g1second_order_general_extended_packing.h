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

#include "grib_accessor_class_data_simple_packing.h"

class grib_accessor_data_g1second_order_general_extended_packing_t : public grib_accessor_data_simple_packing_t
{
public:
    grib_accessor_data_g1second_order_general_extended_packing_t() :
        grib_accessor_data_simple_packing_t() {}
    static inline const AccessorType accessor_type{"data_g1second_order_general_extended_packing"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void init(const long, grib_arguments*) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;

private:
    int unpack(double*, float*, size_t*);

private:
    const char* half_byte_;
    const char* packingType_;
    const char* ieee_packing_;
    const char* precision_;
    const char* widthOfFirstOrderValues_;
    const char* firstOrderValues_;
    const char* N1_;
    const char* N2_;
    const char* numberOfGroups_;
    const char* codedNumberOfGroups_;
    const char* numberOfSecondOrderPackedValues_;
    const char* extraValues_;
    const char* groupWidths_;
    const char* widthOfWidths_;
    const char* groupLengths_;
    const char* widthOfLengths_;
    const char* NL_;
    const char* SPD_;
    const char* widthOfSPD_;
    const char* orderOfSPD_;
    const char* numberOfPoints_;
    const char* dataFlag_;
    double* dvalues_;
    float* fvalues_;
    int double_dirty_;
    int float_dirty_;
    size_t size_;
};
