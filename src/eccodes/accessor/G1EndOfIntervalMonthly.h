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

class G1EndOfIntervalMonthly : public AbstractVector
{
public:
    G1EndOfIntervalMonthly() :
        AbstractVector() { class_name_ = "g1end_of_interval_monthly"; }
    grib_accessor* create_empty_accessor() override { return new G1EndOfIntervalMonthly{}; }
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void init(const long, grib_arguments*) override;
    int compare(grib_accessor*) override;

private:
    const char* verifyingMonth_ = nullptr;
};

}  // namespace eccodes::accessor
