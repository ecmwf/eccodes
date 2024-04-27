
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_abstract_vector_h
#define eccodes_accessor_abstract_vector_h

#include "grib_api_internal.h"
#include "grib_accessor_class_double.h"

class grib_accessor_abstract_vector_t : public grib_accessor_double_t
{
public:
    /* Members defined in abstract_vector */
    double* v;
    int number_of_elements;
};

class grib_accessor_class_abstract_vector_t : public grib_accessor_class_double_t
{
public:
    grib_accessor_class_abstract_vector_t(const char* name) : grib_accessor_class_double_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_abstract_vector_t{}; }
};
#endif /* eccodes_accessor_abstract_vector_h */
