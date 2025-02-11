/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action/action_class_list.h"
#include "action/action_class_if.h"
#include "action/action_class_gen.h"

static void print_names(eccodes::Action* a)
{
    while (a) {
        if (a->op_ && !strcmp(a->op_, "label")) {
            a = a->next_;
            continue;
        }
        if (!strcmp(a->class_name_, "action_class_noop") ||
            !strcmp(a->class_name_, "action_class_when")) {
            a = a->next_;
            continue;
        }

        if (!strcmp(a->class_name_, "action_class_list")) {
            eccodes::action::List* al = dynamic_cast<eccodes::action::List*>(a);
            /*printf("%s\n", a->name_);*/
            print_names(al->block_list_);
        }
        else if (!strcmp(a->class_name_, "action_class_if")) {
            eccodes::action::If* ai = dynamic_cast<eccodes::action::If*>(a);
            print_names(ai->block_false_);
            print_names(ai->block_true_);
        }
        else if (!strcmp(a->class_name_, "action_class_gen")) {
            const char* table = NULL;
            eccodes::action::Gen* ag = dynamic_cast<eccodes::action::Gen*>(a);
            if (strcmp(a->op_,"codetable")==0 || strcmp(a->op_,"codeflag")==0) {
                printf("%s", a->name_);
                table = ag->params_->get_string(NULL, 0);
                if (table) printf("\t%s=%s", a->op_, table);
                printf("\n");
            }
        }
        else {
            /*printf("%s\n", a->name_);*/
        }

        a = a->next_;
    }
}

int main(int argc, char* argv[])
{
    int i    = 0;
    int fail = 0;

    grib_context* c = grib_context_get_default();
    grib_action* a  = NULL;

    for (i = 1; i < argc; i++) {
        if (!(a = grib_parse_file(c, argv[i]))) {
            fail++;
            printf("FAILED\n");
            exit(1);
        }
        else {
            print_names(a);
        }
    }

    return fail;
}
