/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_modify.h"

grib_action* grib_action_create_modify(grib_context* context, const char* name, long flags)
{
    eccodes::action::Modify* act = new eccodes::action::Modify();

    act->op_        = grib_context_strdup_persistent(context, "section");
    act->context_   = context;
    act->flags_     = flags;
    act->name_      = grib_context_strdup_persistent(context, name);
    act->type_name_ = grib_context_strdup_persistent(context, "flags");

    return act;
}

namespace eccodes::action
{

int Modify::create_accessor(grib_section* p, grib_loader* h)
{
    grib_accessor* ga = NULL;

    ga = grib_find_accessor(p->h, name_);

    if (ga) {
        ga->flags_ = flags_;
    }
    else {
        grib_context_log(context_, GRIB_LOG_ERROR, "action_class_modify: %s: No accessor named %s to modify",
                         __func__, name_);
        return GRIB_INTERNAL_ERROR;
    }
    return GRIB_SUCCESS;
}

void Modify::destroy(grib_context* context)
{
    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, type_name_);
    grib_context_free_persistent(context, op_);

    Action::destroy(context);
}


}  // namespace eccodes::action
