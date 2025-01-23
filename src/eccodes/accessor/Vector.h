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

#include "AbstractVector.h"

namespace eccodes::accessor
{

class grib_accessor_vector_t : public grib_accessor_abstract_vector_t
{
public:
    grib_accessor_vector_t() :
        grib_accessor_abstract_vector_t() { class_name_ = "vector"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_vector_t{}; }
    int unpack_double(double* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* vector_ = nullptr;
    int index_ = 0;
};

}  // namespace eccodes::accessor
