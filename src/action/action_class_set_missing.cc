/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_set_missing.h"

grib_action* grib_action_create_set_missing(grib_context* context, const char* name)
{
    char buf[1024];

    eccodes::action::SetMissing* act = new eccodes::action::SetMissing();

    act->op_      = grib_context_strdup_persistent(context, "set_missing");
    act->context_ = context;
    act->name2_   = grib_context_strdup_persistent(context, name);

    snprintf(buf, sizeof(buf), "set_missing_%s", name);

    act->name_    = grib_context_strdup_persistent(context, buf);

    return act;
}

namespace eccodes::action
{

int SetMissing::execute(grib_handle* h)
{
    return grib_set_missing(h, name2_);
}

void SetMissing::destroy(grib_context* context)
{
    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, name2_);
    grib_context_free_persistent(context, op_);

    Action::destroy(context);
}

}  // namespace eccodes::action
