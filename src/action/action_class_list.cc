/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_list.h"

grib_action* grib_action_create_list(grib_context* context, const char* name, grib_expression* expression, grib_action* block)
{
    eccodes::action::List* act = new eccodes::action::List();

    act->context_    = context;
    act->next_       = NULL;
    act->name_       = grib_context_strdup_persistent(context, name);
    act->op_         = grib_context_strdup_persistent(context, "section");
    act->expression_ = expression;
    act->block_list_ = block;

    grib_context_log(context, GRIB_LOG_DEBUG, " Action List %s is created  \n", act->name_);
    return act;
}


namespace eccodes::action
{

void List::dump(FILE* f, int lvl)
{
    int i = 0;
    for (i = 0; i < lvl; i++)
        grib_context_print(context_, f, "     ");
    grib_context_print(context_, f, "Loop   %s\n", name_);
    grib_dump_action_branch(f, block_list_, lvl + 1);
}

int List::create_accessor(grib_section* p, grib_loader* h)
{
    grib_accessor* ga = NULL;
    grib_section* gs  = NULL;
    grib_action* la   = NULL;
    grib_action* next = NULL;
    int ret           = 0;
    long val          = 0;

    if ((ret = expression_->evaluate_long(p->h, &val)) != GRIB_SUCCESS) {
        grib_context_log(p->h->context, GRIB_LOG_DEBUG, "List %s creating %ld values: Unable to evaluate long", name_, val);
        return ret;
    }

    grib_context_log(p->h->context, GRIB_LOG_DEBUG, "List %s creating %d values", name_, val);

    ga = grib_accessor_factory(p, this, 0, NULL);
    if (!ga)
        return GRIB_BUFFER_TOO_SMALL;
    gs        = ga->sub_section_;
    ga->loop_ = val;

    grib_push_accessor(ga, p->block);

    la = block_list_;

    gs->branch = la;
    grib_dependency_observe_expression(ga, expression_);

    while (val--) {
        next = la;
        while (next) {
            ret = grib_create_accessor(gs, next, h);
            if (ret != GRIB_SUCCESS)
                return ret;
            next = next->next_;
        }
    }
    return GRIB_SUCCESS;
}


grib_action* List::reparse(grib_accessor* acc, int* doit)
{
    long val = 0;

    int ret = expression_->evaluate_long(grib_handle_of_accessor(acc), &val);
    if (ret != GRIB_SUCCESS) {
        grib_context_log(acc->context_, GRIB_LOG_ERROR,
                         "List %s creating %ld values: Unable to evaluate long", acc->name_, val);
    }

    *doit = (val != acc->loop_);

    return block_list_;
}

void List::destroy(grib_context* context)
{
    grib_action* a = block_list_;

    while (a) {
        grib_action* na = a->next_;
        a->destroy(context);
        delete a;
        a = na;
    }

    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, op_);
    expression_->destroy(context);
    delete expression_;

    Section::destroy(context);
}

}  // namespace eccodes::action
