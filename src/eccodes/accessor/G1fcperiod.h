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

#include "G1stepRange.h"

namespace eccodes::accessor
{

class G1fcperiod : public G1stepRange
{
public:
    G1fcperiod() :
        G1stepRange() { class_name_ = "g1fcperiod"; }
    grib_accessor* create_empty_accessor() override { return new G1fcperiod{}; }
    int unpack_string(char*, size_t* len) override;
};

}  // namespace eccodes::accessor
