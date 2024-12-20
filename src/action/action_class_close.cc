/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "action_class_close.h"

grib_action* grib_action_create_close(grib_context* context, const char* filename)
{
    char buf[1024];
    eccodes::action::Close* act = new eccodes::action::Close();

    act->op_       = grib_context_strdup_persistent(context, "section");
    act->context_  = context;
    act->filename_ = grib_context_strdup_persistent(context, filename);

    snprintf(buf, 1024, "close_%p", (void*)act->filename_);

    act->name_     = grib_context_strdup_persistent(context, buf);

    return act;
}

namespace eccodes::action
{

int Close::execute(grib_handle* h)
{
    char filename[2048] = {0, };
    size_t len = sizeof(filename);

    int err = grib_get_string(h, filename_, filename, &len);
    /* fprintf(stderr,"++++ name %s\n",filename); */
    if (err)
        return err;
    /* grib_file_close(filename,1,&err); */
    grib_file* file = grib_get_file(filename, &err);
    if (err)
        return err;
    if (file)
        grib_file_pool_delete_file(file);

    return GRIB_SUCCESS;
}

void Close::destroy(grib_context* context)
{
    grib_context_free_persistent(context, filename_);
    grib_context_free_persistent(context, name_);
    grib_context_free_persistent(context, op_);

    Action::destroy(context);
}

}  // namespace eccodes::action
