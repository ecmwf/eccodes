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

class StatisticsSpectral : public AbstractVector
{
public:
    StatisticsSpectral() :
        AbstractVector() { class_name_ = "statistics_spectral"; }
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;

private:
    const char* values_ = nullptr;
    const char* J_ = nullptr;
    const char* K_ = nullptr;
    const char* M_ = nullptr;
    const char* JS_ = nullptr;
};

}  // namespace eccodes::accessor
