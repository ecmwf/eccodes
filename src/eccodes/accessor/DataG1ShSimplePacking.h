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

#include "DataShSimplePacking.h"

namespace eccodes::accessor
{

class DataG1ShSimplePacking : public DataShSimplePacking
{
public:
    DataG1ShSimplePacking() :
        DataShSimplePacking() { class_name_ = "data_g1shsimple_packing"; }
    int unpack_double(double* val, size_t* len) override;
    int value_count(long*) override;
};

}  // namespace eccodes::accessor
