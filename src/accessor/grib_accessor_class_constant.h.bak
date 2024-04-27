/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


/*************************************
 * Enrico Fucile
 **************************************/

#include "../grib_api_internal.h"
#include "grib_accessor_class_variable.h"

class grib_accessor_constant_t : public grib_accessor_variable_t {};

class grib_accessor_class_constant_t : public grib_accessor_class_variable_t
{
public:
    grib_accessor_class_constant_t(const char* name) : grib_accessor_class_variable_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_constant_t{}; }
    void init(grib_accessor*, const long, grib_arguments*) override;
};
