
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

#include "grib_accessor_class_gen.h"

class grib_accessor_abstract_long_vector_t : public grib_accessor_gen_t
{
public:
    /* Members defined in abstract_long_vector */
    long* v;
    long pack_index;
    int number_of_elements;
};

class grib_accessor_class_abstract_long_vector_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_abstract_long_vector_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_abstract_long_vector_t{}; }
};
