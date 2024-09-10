
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

#include "grib_accessor_class_double.h"

class grib_accessor_abstract_vector_t : public grib_accessor_double_t
{
public:
    grib_accessor_abstract_vector_t() :
        grib_accessor_double_t() {}
    static inline const AccessorType accessor_type{"abstract_vector"};
    const AccessorType& getClassName() const override { return accessor_type; }

public:
    double* v_;
    int number_of_elements_;
};
