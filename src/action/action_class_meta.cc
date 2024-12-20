/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_meta.h"

grib_action* grib_action_create_meta(grib_context* context, const char* name, const char* op,
                                     grib_arguments* params, grib_arguments* default_value, unsigned long flags, const char* name_space)
{
    eccodes::action::Meta* act = new eccodes::action::Meta();

    act->next_ = NULL;
    act->name_ = grib_context_strdup_persistent(context, name);
    act->op_   = grib_context_strdup_persistent(context, op);
    if (name_space)
        act->name_space_ = grib_context_strdup_persistent(context, name_space);
    act->context_       = context;
    act->flags_         = flags;
    act->params_        = params;
    act->default_value_ = default_value;
    act->len_           = 0;

    /* grib_arguments_print(context,a->params,0); printf("\n"); */

    return act;
}

namespace eccodes::action
{

void Meta::dump(FILE* f, int lvl)
{
    int i = 0;
    for (i = 0; i < lvl; i++)
        grib_context_print(context_, f, "     ");
    grib_context_print(context_, f, " meta %s \n", name_);
}

int Meta::execute(grib_handle* h)
{
    return create_accessor(h->root, NULL);
}

}  // namespace eccodes::action
