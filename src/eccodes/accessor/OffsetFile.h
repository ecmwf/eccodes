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

#include "Double.h"

namespace eccodes::accessor
{

class OffsetFile : public Double
{
public:
    OffsetFile() :
        Double() { class_name_ = "offset_file"; }
    int unpack_double(double* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    void init(const long, grib_arguments*) override;
};

}  // namespace eccodes::accessor
