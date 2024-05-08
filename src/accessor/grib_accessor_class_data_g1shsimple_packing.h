
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

#include "grib_accessor_class_data_shsimple_packing.h"

class grib_accessor_data_g1shsimple_packing_t : public grib_accessor_data_shsimple_packing_t
{
public:
    /* Members defined in data_g1shsimple_packing */
};

class grib_accessor_class_data_g1shsimple_packing_t : public grib_accessor_class_data_shsimple_packing_t
{
public:
    grib_accessor_class_data_g1shsimple_packing_t(const char* name) : grib_accessor_class_data_shsimple_packing_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_g1shsimple_packing_t{}; }
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
};
