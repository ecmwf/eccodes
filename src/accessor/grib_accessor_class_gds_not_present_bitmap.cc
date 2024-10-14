/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_gds_not_present_bitmap.h"

grib_accessor_gds_not_present_bitmap_t _grib_accessor_gds_not_present_bitmap{};
grib_accessor* grib_accessor_gds_not_present_bitmap = &_grib_accessor_gds_not_present_bitmap;

void grib_accessor_gds_not_present_bitmap_t::init(const long v, grib_arguments* args)
{
    grib_accessor_gen_t::init(v, args);
    int n             = 0;
    grib_handle* hand = grib_handle_of_accessor(this);

    missing_value_           = grib_arguments_get_name(hand, args, n++);
    number_of_values_        = grib_arguments_get_name(hand, args, n++);
    number_of_points_        = grib_arguments_get_name(hand, args, n++);
    latitude_of_first_point_ = grib_arguments_get_name(hand, args, n++);
    ni_                      = grib_arguments_get_name(hand, args, n++);
    length_                  = 0;
}

int grib_accessor_gds_not_present_bitmap_t::value_count(long* number_of_points)
{
    *number_of_points = 0;
    return grib_get_long_internal(grib_handle_of_accessor(this), number_of_points_, number_of_points);
}

int grib_accessor_gds_not_present_bitmap_t::unpack_double(double* val, size_t* len)
{
    grib_handle* hand = grib_handle_of_accessor(this);

    long number_of_points = 0, number_of_values = 0, ni = 0;
    long latitude_of_first_point = 0;
    size_t i                     = 0;
    size_t n_vals                = 0;
    long nn                      = 0;
    long missing_value;

    double* coded_vals = NULL;
    int err            = value_count(&nn);
    n_vals             = nn;
    if (err)
        return err;

    if ((err = grib_get_long(hand, number_of_points_, &number_of_points)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long(hand, number_of_values_, &number_of_values)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long(hand, latitude_of_first_point_, &latitude_of_first_point)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long(hand, missing_value_, &missing_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long(hand, ni_, &ni)) != GRIB_SUCCESS)
        return err;

    if (*len < number_of_points) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (number_of_values > 0) {
        coded_vals = (double*)grib_context_malloc(context_, number_of_values * sizeof(double));

        if (coded_vals == NULL)
            return GRIB_OUT_OF_MEMORY;
    }

    if (latitude_of_first_point == 0) {
        for (i = 0; i < number_of_values; i++)
            val[i] = 1;
        for (i = number_of_values; i < number_of_points; i++)
            val[i] = 0;
    }
    else {
        for (i = 0; i < ni - 1; i++)
            val[i] = 0;
        for (i = ni - 1; i < number_of_points; i++)
            val[i] = 1;
    }

    *len = number_of_points;

    grib_context_free(context_, coded_vals);
    return err;
}

int grib_accessor_gds_not_present_bitmap_t::pack_double(const double* val, size_t* len)
{
    // See deprecated/grib_accessor_gds_not_present_bitmap.cc for
    // a possible implementation
    return GRIB_NOT_IMPLEMENTED;
}

long grib_accessor_gds_not_present_bitmap_t::get_native_type()
{
    return GRIB_TYPE_DOUBLE;
}
