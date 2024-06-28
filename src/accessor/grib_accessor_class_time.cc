
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_time.h"

grib_accessor_class_time_t _grib_accessor_class_time{ "time" };
grib_accessor_class* grib_accessor_class_time = &_grib_accessor_class_time;


void grib_accessor_class_time_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_time_t* self = (grib_accessor_time_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    int n = 0;

    self->hour   = grib_arguments_get_name(hand, c, n++);
    self->minute = grib_arguments_get_name(hand, c, n++);
    self->second = grib_arguments_get_name(hand, c, n++);
}

int grib_accessor_class_time_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    const grib_accessor_time_t* self = (grib_accessor_time_t*)a;

    int ret   = 0;
    long hour = 0, minute = 0, second = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    if ((ret = grib_get_long_internal(hand, self->hour, &hour)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->minute, &minute)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->second, &second)) != GRIB_SUCCESS)
        return ret;

    /* We ignore the 'seconds' in our time calculation! */
    if (second != 0) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Key %s (%s): Truncating time: non-zero seconds(%ld) ignored", a->name, __func__, second);
    }

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    *val = hour * 100 + minute;

    if (hour == 255) {
        *val = 12 * 100;
    }
    if (hour != 255 && minute == 255) {
        *val = hour * 100;
    }
    return GRIB_SUCCESS;
}

int grib_accessor_class_time_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    const grib_accessor_time_t* self = (grib_accessor_time_t*)a;

    int ret = 0;
    long v  = val[0];
    grib_handle* hand = grib_handle_of_accessor(a);
    long hour = 0, minute = 0, second = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    if (!is_time_valid(v)) {
        // ECC-1777: For now just a warning. Will later change to an error
        fprintf(stderr, "ECCODES WARNING :  %s:%s: Time is not valid! hour=%ld min=%ld sec=%ld\n",
                a->cclass->name, __func__, hour, minute, second);
        // return GRIB_ENCODING_ERROR;
    }

    hour   = v / 100;
    minute = v % 100;
    second = 0; /* We ignore the 'seconds' in our time calculation! */

    if ((ret = grib_set_long_internal(hand, self->hour, hour)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->minute, minute)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->second, second)) != GRIB_SUCCESS)
        return ret;

    return GRIB_SUCCESS;
}

int grib_accessor_class_time_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    long v = 0;
    size_t lsize = 1, lmin = 5;

    unpack_long(a, &v, &lsize);

    if (*len < lmin) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         a->cclass->name, a->name, lmin, *len);
        *len = lmin;
        return GRIB_BUFFER_TOO_SMALL;
    }

    snprintf(val, 64, "%04ld", v);

    len[0] = lmin;
    return GRIB_SUCCESS;
}
