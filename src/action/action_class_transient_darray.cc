/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_transient_darray.h"

grib_action* grib_action_create_transient_darray(grib_context* context, const char* name, grib_darray* darray, int flags)
{
    eccodes::action::TransientDArray* act = new eccodes::action::TransientDArray();

    act->op_      = grib_context_strdup_persistent(context, "transient_darray");
    act->context_ = context;
    act->flags_   = flags;
    act->darray_  = darray;
    act->name2_   = grib_context_strdup_persistent(context, name);
    act->name_    = grib_context_strdup_persistent(context, name);

    return act;
}

namespace eccodes::action
{

int TransientDArray::execute(grib_handle* h)
{
    size_t len       = grib_darray_used_size(darray_);
    grib_accessor* a = NULL;
    grib_section* p  = h->root;

    a = grib_accessor_factory(p, this, len_, params_);
    if (!a)
        return GRIB_INTERNAL_ERROR;

    grib_push_accessor(a, p->block);

    if (a->flags_ & GRIB_ACCESSOR_FLAG_CONSTRAINT)
        grib_dependency_observe_arguments(a, default_value_);

    return a->pack_double(darray_->v, &len);
}

void TransientDArray::dump(FILE* f, int lvl)
{
    int i = 0;
    for (i = 0; i < lvl; i++)
        grib_context_print(context_, f, "     ");
    grib_context_print(context_, f, name2_);
    printf("\n");
}

void TransientDArray::destroy(grib_context* context)
{
    grib_context_free_persistent(context, name2_);
    grib_darray_delete(darray_);

    Gen::destroy(context);
}

}  // namespace eccodes::action
