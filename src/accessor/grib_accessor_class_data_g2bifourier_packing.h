
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

class grib_accessor_data_g2bifourier_packing_t : public grib_accessor_data_simple_packing_t
{
public:
    /* Members defined in data_g2bifourier_packing */
    const char*  ieee_floats;
    const char*  laplacianOperatorIsSet;
    const char*  laplacianOperator;
    const char*  biFourierTruncationType;
    const char*  sub_i;
    const char*  sub_j;
    const char*  bif_i;
    const char*  bif_j;
    const char*  biFourierSubTruncationType;
    const char*  biFourierDoNotPackAxes;
    const char*  biFourierMakeTemplate;
    const char*  totalNumberOfValuesInUnpackedSubset;
    const char*  numberOfValues;
};

class grib_accessor_class_data_g2bifourier_packing_t : public grib_accessor_class_data_simple_packing_t
{
public:
    grib_accessor_class_data_g2bifourier_packing_t(const char* name) : grib_accessor_class_data_simple_packing_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_g2bifourier_packing_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
