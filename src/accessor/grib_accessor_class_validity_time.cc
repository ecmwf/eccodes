
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_validity_time.h"
#include "shared_functions.h"

grib_accessor_class_validity_time_t _grib_accessor_class_validity_time{ "validity_time" };
grib_accessor_class* grib_accessor_class_validity_time = &_grib_accessor_class_validity_time;


void grib_accessor_class_validity_time_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_validity_time_t* self = (grib_accessor_validity_time_t*)a;
    grib_handle* hand                   = grib_handle_of_accessor(a);
    int n                               = 0;

    self->date      = grib_arguments_get_name(hand, c, n++);
    self->time      = grib_arguments_get_name(hand, c, n++);
    self->step      = grib_arguments_get_name(hand, c, n++);
    self->stepUnits = grib_arguments_get_name(hand, c, n++);
    self->hours     = grib_arguments_get_name(hand, c, n++);
    self->minutes   = grib_arguments_get_name(hand, c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_validity_time_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_validity_time_t* self = (grib_accessor_validity_time_t*)a;
    grib_handle* hand                   = grib_handle_of_accessor(a);
    int ret                             = 0;
    long date                           = 0;
    long time                           = 0;
    long step                           = 0;
    long stepUnits                      = 0;
    long hours = 0, minutes = 0, step_mins = 0, tmp, tmp_hrs, tmp_mins;

    if (self->hours) {
        if ((ret = grib_get_long_internal(hand, self->hours, &hours)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_long_internal(hand, self->minutes, &minutes)) != GRIB_SUCCESS)
            return ret;
        *val = hours * 100 + minutes;
        return GRIB_SUCCESS;
    }
    if ((ret = grib_get_long_internal(hand, self->date, &date)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->time, &time)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long(hand, self->step, &step)) != GRIB_SUCCESS) {
        if ((ret = grib_get_long_internal(hand, "endStep", &step)) != GRIB_SUCCESS) {
            return ret; /* See ECC-817 */
        }
    }

    /* Seconds will always be zero. So convert to minutes */
    if (self->stepUnits) {
        if ((ret = grib_get_long_internal(hand, self->stepUnits, &stepUnits)) != GRIB_SUCCESS)
            return ret;
        step_mins = convert_to_minutes(step, stepUnits);
    }

    minutes  = time % 100;
    hours    = time / 100;
    tmp      = minutes + step_mins; /* add the step to our minutes */
    tmp_hrs  = tmp / 60;            /* how many hours and mins is that? */
    tmp_mins = tmp % 60;
    hours += tmp_hrs; /* increment hours */
    if (hours > 0) {
        hours = hours % 24; /* wrap round if >= 24 */
    }
    else {
        /* GRIB-29: Negative forecast time */
        while (hours < 0) {
            hours += 24;
        }
    }
    time = hours * 100 + tmp_mins;

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    *val = time;

    return GRIB_SUCCESS;
}

int grib_accessor_class_validity_time_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    int err      = 0;
    long v       = 0;
    size_t lsize = 1, lmin = 5;

    err = unpack_long(a, &v, &lsize);
    if (err) return err;

    if (*len < lmin) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, lmin, *len);
        *len = lmin;
        return GRIB_BUFFER_TOO_SMALL;
    }

    snprintf(val, 64, "%04ld", v);

    len[0] = lmin;
    return GRIB_SUCCESS;
}
