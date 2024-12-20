/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_set_darray.h"

grib_action* grib_action_create_set_darray(grib_context* context,
                                           const char* name,
                                           grib_darray* darray)
{
    char buf[1024];

    eccodes::action::SetDArray* act = new eccodes::action::SetDArray();

    act->op_      = grib_context_strdup_persistent(context, "section");
    act->context_ = context;
    act->darray_  = darray;
    act->name2_   = grib_context_strdup_persistent(context, name);

    snprintf(buf, 1024, "set_darray%p", (void*)darray);

    act->name_    = grib_context_strdup_persistent(context, buf);

    return act;
}

namespace eccodes::action
{

int SetDArray::execute(grib_handle* h)
{
    return grib_set_double_array(h, name2_, darray_->v, darray_->n);
}

void SetDArray::dump(FILE* f, int lvl)
{
    int i = 0;
    for (i = 0; i < lvl; i++)
        grib_context_print(context_, f, "     ");
    grib_context_print(context_, f, name2_);
    printf("\n");
}

void SetDArray::destroy(grib_context* context)
{
    grib_context_free_persistent(context, name2_);
    grib_darray_delete(darray_);
    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, op_);

    Action::destroy(context);
}

}  // namespace eccodes::action
