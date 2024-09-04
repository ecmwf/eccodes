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

grib_accessor_class_gds_not_present_bitmap_t _grib_accessor_class_gds_not_present_bitmap{ "gds_not_present_bitmap" };
grib_accessor_class* grib_accessor_class_gds_not_present_bitmap = &_grib_accessor_class_gds_not_present_bitmap;


void grib_accessor_class_gds_not_present_bitmap_t::init(grib_accessor* a, const long v, grib_arguments* args)
{
    Assert(!"grib_accessor_class_gds_not_present_bitmap_t::init should not be called");
    // grib_accessor_class_gen_t::init(a, v, args);
    // int n = 0;
    // grib_accessor_gds_not_present_bitmap_t* self = (grib_accessor_gds_not_present_bitmap_t*)a;
    // grib_handle* hand = grib_handle_of_accessor(a);

    // self->missing_value           = grib_arguments_get_name(hand, args, n++);
    // self->number_of_values        = grib_arguments_get_name(hand, args, n++);
    // self->number_of_points        = grib_arguments_get_name(hand, args, n++);
    // self->latitude_of_first_point = grib_arguments_get_name(hand, args, n++);
    // self->ni                      = grib_arguments_get_name(hand, args, n++);
    // a->length                     = 0;
}

int grib_accessor_class_gds_not_present_bitmap_t::value_count(grib_accessor* a, long* number_of_points)
{
    return GRIB_NOT_IMPLEMENTED;
    // grib_accessor_gds_not_present_bitmap_t* self = (grib_accessor_gds_not_present_bitmap_t*)a;
    // *number_of_points                            = 0;
    // return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_points, number_of_points);
}

int grib_accessor_class_gds_not_present_bitmap_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;

    // grib_accessor_gds_not_present_bitmap_t* self = (grib_accessor_gds_not_present_bitmap_t*)a;
    // grib_handle* hand                            = grib_handle_of_accessor(a);

    // long number_of_points = 0, number_of_values = 0, ni = 0;
    // long latitude_of_first_point = 0;
    // size_t i                     = 0;
    // size_t n_vals                = 0;
    // long nn                      = 0;
    // long missing_value;

    // double* coded_vals = NULL;
    // int err            = a->value_count(&nn);
    // n_vals             = nn;
    // if (err)
    //     return err;

    // if ((err = grib_get_long(hand, self->number_of_points, &number_of_points)) != GRIB_SUCCESS)
    //     return err;

    // if ((err = grib_get_long(hand, self->number_of_values, &number_of_values)) != GRIB_SUCCESS)
    //     return err;

    // if ((err = grib_get_long(hand, self->latitude_of_first_point, &latitude_of_first_point)) != GRIB_SUCCESS)
    //     return err;

    // if ((err = grib_get_long(hand, self->missing_value, &missing_value)) != GRIB_SUCCESS)
    //     return err;

    // if ((err = grib_get_long(hand, self->ni, &ni)) != GRIB_SUCCESS)
    //     return err;

    // if (*len < number_of_points) {
    //     *len = n_vals;
    //     return GRIB_ARRAY_TOO_SMALL;
    // }

    // if (number_of_values > 0) {
    //     coded_vals = (double*)grib_context_malloc(a->context, number_of_values * sizeof(double));

    //     if (coded_vals == NULL)
    //         return GRIB_OUT_OF_MEMORY;
    // }

    // if (latitude_of_first_point == 0) {
    //     for (i = 0; i < number_of_values; i++)
    //         val[i] = 1;
    //     for (i = number_of_values; i < number_of_points; i++)
    //         val[i] = 0;
    // }
    // else {
    //     for (i = 0; i < ni - 1; i++)
    //         val[i] = 0;
    //     for (i = ni - 1; i < number_of_points; i++)
    //         val[i] = 1;
    // }

    // *len = number_of_points;

    // grib_context_free(a->context, coded_vals);
    // return err;
}

int grib_accessor_class_gds_not_present_bitmap_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    // See deprecated/grib_accessor_class_gds_not_present_bitmap.cc for
    // a possible implementation
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gds_not_present_bitmap_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}
