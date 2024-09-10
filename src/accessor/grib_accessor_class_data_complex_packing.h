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
#include "grib_ieeefloat.h"

typedef unsigned long (*encode_float_proc)(double);
typedef double (*decode_float_proc)(unsigned long);

class grib_accessor_data_complex_packing_t : public grib_accessor_data_simple_packing_t
{
public:
    grib_accessor_data_complex_packing_t() :
        grib_accessor_data_simple_packing_t() {}
    static inline const AccessorType accessor_type{"data_complex_packing"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

public:
    const char* GRIBEX_sh_bug_present_;
    const char* ieee_floats_;
    const char* laplacianOperatorIsSet_;
    const char* laplacianOperator_;
    const char* sub_j_;
    const char* sub_k_;
    const char* sub_m_;
    const char* pen_j_;
    const char* pen_k_;
    const char* pen_m_;
};
