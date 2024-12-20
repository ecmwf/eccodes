/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_set.h"

grib_action* grib_action_create_set(grib_context* context,
                                    const char* name, grib_expression* expression, int nofail)
{
    char buf[1024];

    eccodes::action::Set* act = new eccodes::action::Set();

    act->op_         = grib_context_strdup_persistent(context, "section");
    act->context_    = context;
    act->expression_ = expression;
    act->name2_       = grib_context_strdup_persistent(context, name);
    act->nofail_     = nofail;

    snprintf(buf, 1024, "set%p", (void*)expression);

    act->name_      = grib_context_strdup_persistent(context, buf);

    return act;
}

namespace eccodes::action
{

int Set::execute(grib_handle* h)
{
    int ret = 0;
    ret     = grib_set_expression(h, name2_, expression_);
    if (nofail_)
        return 0;
    if (ret != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Error while setting key '%s' (%s)",
                         name2_, grib_get_error_message(ret));
    }
    return ret;
}

void Set::dump(FILE* f, int lvl)
{
    int i = 0;
    for (i = 0; i < lvl; i++)
        grib_context_print(context_, f, "     ");
    grib_context_print(context_, f, name2_);
    printf("\n");
}

void Set::destroy(grib_context* context)
{
    grib_context_free_persistent(context, name_);
    expression_->destroy(context);
    delete expression_;
    grib_context_free_persistent(context, name2_);
    grib_context_free_persistent(context, op_);

    Action::destroy(context);
}

}  // namespace eccodes::action
