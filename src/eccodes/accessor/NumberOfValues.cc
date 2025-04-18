/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "NumberOfValues.h"

eccodes::accessor::NumberOfValues _grib_accessor_number_of_values;
eccodes::Accessor* grib_accessor_number_of_values = &_grib_accessor_number_of_values;

namespace eccodes::accessor
{

void NumberOfValues::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n             = 0;
    grib_handle* hand = get_enclosing_handle();

    values_              = c->get_name(hand, n++);
    bitsPerValue_        = c->get_name(hand, n++);
    numberOfPoints_      = c->get_name(hand, n++);
    bitmapPresent_       = c->get_name(hand, n++);
    bitmap_              = c->get_name(hand, n++);
    numberOfCodedValues_ = c->get_name(hand, n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;

    length_ = 0;
}

int NumberOfValues::unpack_long(long* val, size_t* len)
{
    int ret      = GRIB_SUCCESS;
    long npoints = 0, bitmap_present = 0;
    size_t size = 0;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), numberOfPoints_, &npoints)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), bitmapPresent_, &bitmap_present)) != GRIB_SUCCESS)
        return ret;

    if (bitmap_present) {
        double* bitmap;
        size   = npoints;
        bitmap = (double*)grib_context_malloc(context_, sizeof(double) * size);
        if ((ret = grib_get_double_array_internal(get_enclosing_handle(), bitmap_, bitmap, &size)) != GRIB_SUCCESS) {
            grib_context_free(context_, bitmap);
            return ret;
        }
        *val = 0;
        for (size_t i = 0; i < size; i++) {
            if (bitmap[i] != 0) {
                (*val)++;
            }
        }

        grib_context_free(context_, bitmap);
    }
    else {
        *val = npoints;
    }

    return ret;
}

}  // namespace eccodes::accessor
