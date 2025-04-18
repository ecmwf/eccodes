/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2BitmapPresent.h"

eccodes::accessor::G2BitmapPresent _grib_accessor_g2bitmap_present;
eccodes::Accessor* grib_accessor_g2bitmap_present = &_grib_accessor_g2bitmap_present;

namespace eccodes::accessor
{

void G2BitmapPresent::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n            = 0;
    bitmapIndicator_ = c->get_name(get_enclosing_handle(), n++);

    length_ = 0;
}

int G2BitmapPresent::unpack_long(long* val, size_t* len)
{
    int ret              = GRIB_SUCCESS;
    long bitmapIndicator = 0;

    ret = grib_get_long_internal(get_enclosing_handle(), bitmapIndicator_, &bitmapIndicator);
    if (ret) {
        if (ret == GRIB_NOT_FOUND) {
            *val = 0;
            return 0;
        }
        else
            return ret;
    }

    *val = 1;
    if (bitmapIndicator == 255)
        *val = 0;

    *len = 1;
    return ret;
}

int G2BitmapPresent::pack_long(const long* val, size_t* len)
{
    long bitmapIndicator = 0;

    if (*val == 0)
        bitmapIndicator = 255;

    return grib_set_long(get_enclosing_handle(), bitmapIndicator_, bitmapIndicator);
}

}  // namespace eccodes::accessor
