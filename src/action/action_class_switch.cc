/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_switch.h"


grib_action* grib_action_create_switch(grib_context* context,
                                       grib_arguments* args,
                                       grib_case* Case,
                                       grib_action* Default)
{
    char name[1024];
    const size_t nameLen = sizeof(name);

    eccodes::action::Switch* act = new eccodes::action::Switch();

    act->op_      = grib_context_strdup_persistent(context, "section");
    act->context_ = context;
    act->args_    = args;
    act->Case_    = Case;
    act->Default_ = Default;

    snprintf(name, nameLen, "_switch%p", (void*)act);

    act->name_ = grib_context_strdup_persistent(context, name);

    return act;
}

grib_case* grib_case_new(grib_context* c, grib_arguments* values, grib_action* action)
{
    grib_case* Case = (grib_case*)grib_context_malloc_clear_persistent(c, sizeof(grib_case));

    Case->values = values;
    Case->action = action;

    return Case;
}

namespace eccodes::action
{

int Switch::execute(grib_handle* h)
{
    grib_case* c         = Case_;
    grib_action* next    = Default_;
    grib_arguments* args = args_;
    grib_arguments* values;
    grib_expression* e;      // The expression in the switch statement
    grib_expression* value;  // The value in each 'case'
    int ret     = 0;
    long lres   = 0;
    double dres = 0;
    long lval   = 0;
    double dval = 0;
    int type    = 0;
    int ok      = 0;
    const char* cval;
    const char* cres;
    char buf[80];
    char tmp[80];
    size_t len  = sizeof(buf);
    size_t size = sizeof(tmp);
    int err     = 0;

    Assert(args);

    while (c) {
        e      = args->expression_;
        values = c->values;
        value  = values->expression_;
        ok     = 0;
        while (e && value) {
            if (!strcmp(value->class_name(), "true"))
                ok = 1;
            else {
                type = value->native_type(h);

                switch (type) {
                    case GRIB_TYPE_LONG:
                        ok = (value->evaluate_long(h, &lres) == GRIB_SUCCESS) &&
                             (e->evaluate_long(h, &lval) == GRIB_SUCCESS) &&
                             (lval == lres);
                        break;

                    case GRIB_TYPE_DOUBLE:
                        ok = (value->evaluate_double(h, &dres) == GRIB_SUCCESS) &&
                             (e->evaluate_double(h, &dval) == GRIB_SUCCESS) &&
                             (dval == dres);
                        break;

                    case GRIB_TYPE_STRING:
                        len  = sizeof(buf);
                        size = sizeof(tmp);
                        ok   = ((cres = e->evaluate_string(h, buf, &len, &err)) != NULL) &&
                             (err == 0) &&
                             ((cval = value->evaluate_string(h, tmp, &size, &err)) != NULL) &&
                             (err == 0) &&
                             ((strcmp(buf, cval) == 0) || (strcmp(cval, "*") == 0));
                        break;

                    default:
                        /* TODO: */
                        break;
                }
            }
            if (!ok)
                break;

            args = args->next_;
            if (args)
                e = args->expression_;
            else
                e = NULL;

            values = values->next_;
            if (values)
                value = values->expression_;
            else
                value = NULL;
        }

        if (ok) {
            next = c->action;
            break;
        }

        c = c->next;
    }

    if (!next)
        return GRIB_SWITCH_NO_MATCH;

    while (next) {
        ret = next->execute(h);
        if (ret != GRIB_SUCCESS)
            return ret;
        next = next->next_;
    }

    return GRIB_SUCCESS;
}

void Switch::destroy(grib_context* context)
{
    grib_case* t = Case_;

    while (t) {
        grib_case* nt = t->next;
        t->action->destroy(context);
        delete t->action;
        grib_arguments_free(context, t->values);
        grib_context_free(context, t);
        t = nt;
    }

    Default_->destroy(context);
    delete Default_;

    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, op_);

    Section::destroy(context);
}

}  // namespace eccodes::action
