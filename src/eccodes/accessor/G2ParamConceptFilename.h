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

#include "accessor/Gen.h"

namespace eccodes::accessor
{

class G2ParamConceptFilename : public Gen

{
public:
    G2ParamConceptFilename() : Gen() { class_name_ = "g2_param_concept_filename"; }
    long get_native_type() override;
    int unpack_string(char*, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* basename_                = nullptr;  // str: paramId, shortName, units, name
    const char* MTG2Switch_              = nullptr;  // int: 0 or 1
    const char* tablesVersionMTG2Switch_ = nullptr;  // int: e.g. 33
};

}  // namespace eccodes::accessor
