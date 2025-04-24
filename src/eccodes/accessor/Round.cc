/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Round.h"

eccodes::accessor::Round _grib_accessor_round;
eccodes::Accessor* grib_accessor_round = &_grib_accessor_round;

namespace eccodes::accessor
{

void Round::init(const long l, grib_arguments* arg)
{
    Evaluate::init(l, arg);
}

void Round::destroy(grib_context* c)
{
    Evaluate::destroy(c);
}

void Round::dump(eccodes::Dumper* dumper)
{
    Evaluate::dump(dumper);
}

int Round::unpack_double(double* val, size_t* len)
{
    int ret = GRIB_SUCCESS;

    size_t replen             = 0;
    double rounding_precision = 0;
    double rounded            = 0;
    double toround            = 0;

    const char* oval = NULL;
    oval             = arg_->get_name(get_enclosing_handle(), 0);

    if ((ret = grib_get_double_internal(get_enclosing_handle(), oval, &toround)) != 0)
        return ret;

    rounding_precision = arg_->get_long(get_enclosing_handle(), 1);

    rounded = floor(rounding_precision * toround + 0.5) / rounding_precision;

    *len = replen;

    *val = rounded;

    return ret;
}

int Round::unpack_string(char* val, size_t* len)
{
    char result[1024];
    int ret       = GRIB_SUCCESS;
    size_t replen = 1;

    double value = 0;

    ret = unpack_double(&value, &replen);

    snprintf(result, sizeof(result), "%.3f", value);

    replen = strlen(result) + 1;

    if (*len < replen) {
        *len = replen;
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = replen;

    snprintf(val, 1024, "%s", result);

    return ret;
}

}  // namespace eccodes::accessor
