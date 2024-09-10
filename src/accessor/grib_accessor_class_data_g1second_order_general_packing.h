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
#include "grib_scaling.h"

class grib_accessor_data_g1second_order_general_packing_t : public grib_accessor_data_simple_packing_t
{
public:
    grib_accessor_data_g1second_order_general_packing_t() :
        grib_accessor_data_simple_packing_t() {}
    static inline const AccessorType accessor_type{"data_g1second_order_general_packing"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

public:
    const char* half_byte_;
    const char* packingType_;
    const char* ieee_packing_;
    const char* precision_;
    const char* widthOfFirstOrderValues_;
    const char* N1_;
    const char* N2_;
    const char* numberOfGroups_;
    const char* numberOfSecondOrderPackedValues_;
    const char* extraValues_;
    const char* pl_;
    const char* Ni_;
    const char* Nj_;
    const char* jPointsAreConsecutive_;
    const char* bitmap_;
    const char* groupWidths_;
};
