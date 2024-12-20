/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_variable.h"

grib_action* grib_action_create_variable(grib_context* context, const char* name, const char* op, const long len, grib_arguments* params, grib_arguments* default_value, int flags, const char* name_space)
{
    eccodes::action::Variable* act = new eccodes::action::Variable();

    act->next_ = NULL;
    act->name_ = grib_context_strdup_persistent(context, name);
    if (name_space)
        act->name_space_ = grib_context_strdup_persistent(context, name_space);
    act->op_            = grib_context_strdup_persistent(context, op);
    act->context_       = context;
    act->flags_         = flags;
    act->len_           = len;
    act->params_        = params;
    act->default_value_ = default_value;

    /* printf("CREATE %s\n",name); */

    return act;
}

namespace eccodes::action
{

int Variable::execute(grib_handle* h)
{
    return grib_create_accessor(h->root, this, NULL);
}

}  // namespace eccodes::action
