/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2Lon.h"

eccodes::accessor::G2Lon _grib_accessor_g2lon;
eccodes::Accessor* grib_accessor_g2lon = &_grib_accessor_g2lon;

namespace eccodes::accessor
{

void G2Lon::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    int n = 0;

    longitude_ = c->get_name(get_enclosing_handle(), n++);
}

int G2Lon::unpack_double(double* val, size_t* len)
{
    int ret = 0;
    long longitude;

    if ((ret = grib_get_long(get_enclosing_handle(), longitude_, &longitude)) != GRIB_SUCCESS)
        return ret;

    if (longitude == GRIB_MISSING_LONG) {
        *val = GRIB_MISSING_DOUBLE;
        return GRIB_SUCCESS;
    }

    *val = ((double)longitude) / 1000000.0;

    return GRIB_SUCCESS;
}

int G2Lon::pack_double(const double* val, size_t* len)
{
    long longitude;
    double value = *val;

    if (value == GRIB_MISSING_DOUBLE) {
        longitude = GRIB_MISSING_LONG;
    }
    else {
        if (value < 0)
            value += 360;
        longitude = (long)(value * 1000000);
    }
    return grib_set_long(get_enclosing_handle(), longitude_, longitude);
}

}  // namespace eccodes::accessor
