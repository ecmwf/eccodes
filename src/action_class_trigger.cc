/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

grib_action* grib_action_create_trigger(grib_context* context, grib_arguments* args, grib_action* block)
{
    grib_context_log(context, GRIB_LOG_FATAL, "The 'trigger' statement is deprecated");
    return NULL;
}
