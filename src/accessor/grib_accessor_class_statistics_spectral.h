
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

#include "grib_accessor_class_abstract_vector.h"

class grib_accessor_statistics_spectral_t : public grib_accessor_abstract_vector_t
{
public:
    /* Members defined in statistics_spectral */
    const char* values;
    const char* J;
    const char* K;
    const char* M;
    const char* JS;
};

class grib_accessor_class_statistics_spectral_t : public grib_accessor_class_abstract_vector_t
{
public:
    grib_accessor_class_statistics_spectral_t(const char* name) : grib_accessor_class_abstract_vector_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_statistics_spectral_t{}; }
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int compare(grib_accessor*, grib_accessor*) override;
};
