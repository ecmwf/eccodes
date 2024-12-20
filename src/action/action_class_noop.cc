/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_noop.h"

grib_action* grib_action_create_noop(grib_context* context, const char* fname)
{
    char buf[1024];

    eccodes::action::Noop* act = new eccodes::action::Noop();

    act->op_      = grib_context_strdup_persistent(context, "section");
    act->context_ = context;

    snprintf(buf, sizeof(buf), "_noop%p", (void*)act);

    act->name_    = grib_context_strdup_persistent(context, buf);

    return act;
}

namespace eccodes::action
{

void Noop::destroy(grib_context* context)
{
    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, op_);

    Action::destroy(context);
}

int Noop::execute(grib_handle* h)
{
    return 0;
}

}  // namespace eccodes::action
