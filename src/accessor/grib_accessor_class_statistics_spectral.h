
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
    grib_accessor_statistics_spectral_t() :
        grib_accessor_abstract_vector_t() {}
    static inline const AccessorType accessor_type{"statistics_spectral"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;

private:
    const char* values_;
    const char* J_;
    const char* K_;
    const char* M_;
    const char* JS_;
};
