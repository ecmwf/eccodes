
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_step_human_readable.h"

grib_accessor_class_step_human_readable_t _grib_accessor_class_step_human_readable{ "step_human_readable" };
grib_accessor_class* grib_accessor_class_step_human_readable = &_grib_accessor_class_step_human_readable;


void grib_accessor_class_step_human_readable_t::init(grib_accessor* a, const long len, grib_arguments* params)
{
    grib_accessor_class_gen_t::init(a, len, params);
    grib_accessor_step_human_readable_t* self = (grib_accessor_step_human_readable_t*)a;
    int n = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    self->stepUnits = grib_arguments_get_name(h, params, n++);
    self->step      = grib_arguments_get_name(h, params, n++);
    a->length       = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_step_human_readable_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static int get_step_human_readable(grib_handle* h, char* result, size_t* length)
{
    int err     = 0;
    size_t slen = 2;
    long step, hour, minute, second;

    /* Change units to seconds (highest resolution)
     * before computing the step value
     */
    if ((err = grib_set_string(h, "stepUnits", "s", &slen)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long(h, "step", &step)) != GRIB_SUCCESS)
        return err;

    hour   = step / 3600;
    minute = step / 60 % 60;
    second = step % 60;
    /* sprintf(result, "%ld:%ld:%ld", hour, minute, second); */

    if (second) {
        snprintf(result, 1024, "%ldh %ldm %lds", hour, minute, second);
    }
    else {
        if (minute)
            snprintf(result, 1024, "%ldh %ldm", hour, minute);
        else
            snprintf(result, 1024, "%ldh", hour);
    }

    *length = strlen(result);
    return GRIB_SUCCESS;
}

int grib_accessor_class_step_human_readable_t::unpack_string(grib_accessor* a, char* buffer, size_t* len)
{
    grib_accessor_step_human_readable_t* self = (grib_accessor_step_human_readable_t*)a;

    grib_handle* h = grib_handle_of_accessor(a);
    long stepUnits = 0;
    int err        = 0;

    /* Save the current value of stepUnits */
    err = grib_get_long_internal(h, self->stepUnits, &stepUnits);
    if (err) return err;

    /* This will change stepUnits to seconds for its calculation */
    err = get_step_human_readable(h, buffer, len);

    /* Restore stepUnits */
    grib_set_long(h, self->stepUnits, stepUnits);
    return err;
}
