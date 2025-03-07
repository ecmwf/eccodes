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

#include "Gen.h"

namespace eccodes::action
{

class Variable : public Gen
{
public:
    Variable(grib_context* context, const char* name, const char* op, const long len, grib_arguments* params, grib_arguments* default_value, int flags, const char* name_space);

    int execute(grib_handle* h) override;
};

}  // namespace eccodes::action

grib_action* grib_action_create_variable(grib_context* context, const char* name, const char* op, const long len, grib_arguments* params, grib_arguments* default_value, int flags, const char* name_space);
