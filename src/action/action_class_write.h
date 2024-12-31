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

#include "action.h"

namespace eccodes::action
{

class Write : public Action
{
public:
    Write() { class_name_ = "action_class_write"; }

    void destroy(grib_context*) override;
    int execute(grib_handle* h) override;

    char* name2_       = nullptr;
    int append_        = 0;
    int padtomultiple_ = 0;
};

}  // namespace eccodes::action

grib_action* grib_action_create_write(grib_context* context, const char* name, int append, int padtomultiple);
