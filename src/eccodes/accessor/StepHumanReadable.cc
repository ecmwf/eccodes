/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "StepHumanReadable.h"

eccodes::accessor::StepHumanReadable _grib_accessor_step_human_readable;
eccodes::Accessor* grib_accessor_step_human_readable = &_grib_accessor_step_human_readable;

namespace eccodes::accessor
{

void StepHumanReadable::init(const long len, grib_arguments* params)
{
    Gen::init(len, params);
    int n          = 0;
    grib_handle* h = get_enclosing_handle();

    stepUnits_ = params->get_name(h, n++);
    step_      = params->get_name(h, n++);
    length_    = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

long StepHumanReadable::get_native_type()
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

int StepHumanReadable::unpack_string(char* buffer, size_t* len)
{
    grib_handle* h = get_enclosing_handle();
    long stepUnits = 0;
    int err        = 0;

    /* Save the current value of stepUnits */
    err = grib_get_long_internal(h, stepUnits_, &stepUnits);
    if (err) return err;

    /* This will change stepUnits to seconds for its calculation */
    err = get_step_human_readable(h, buffer, len);

    /* Restore stepUnits */
    grib_set_long(h, stepUnits_, stepUnits);
    return err;
}

}  // namespace eccodes::accessor
