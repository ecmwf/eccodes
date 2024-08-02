
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_number_of_points.h"

grib_accessor_class_number_of_points_t _grib_accessor_class_number_of_points{ "number_of_points" };
grib_accessor_class* grib_accessor_class_number_of_points = &_grib_accessor_class_number_of_points;


void grib_accessor_class_number_of_points_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    int n = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    grib_accessor_number_of_points_t* self = (grib_accessor_number_of_points_t*)a;

    self->ni        = grib_arguments_get_name(hand, c, n++);
    self->nj        = grib_arguments_get_name(hand, c, n++);
    self->plpresent = grib_arguments_get_name(hand, c, n++);
    self->pl        = grib_arguments_get_name(hand, c, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_number_of_points_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_number_of_points_t* self = (grib_accessor_number_of_points_t*)a;

    int ret = GRIB_SUCCESS;
    long ni = 0, nj = 0, plpresent = 0;
    size_t plsize     = 0;
    long* pl          = NULL;
    int i             = 0;
    grib_context* c   = a->context;
    grib_handle* hand = grib_handle_of_accessor(a);

    if ((ret = grib_get_long_internal(hand, self->ni, &ni)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->nj, &nj)) != GRIB_SUCCESS)
        return ret;

    if (self->plpresent &&
        ((ret = grib_get_long_internal(hand, self->plpresent, &plpresent)) != GRIB_SUCCESS))
        return ret;

    if (grib_is_missing(hand, self->nj, &ret) && ret == GRIB_SUCCESS) {
        grib_context_log(c, GRIB_LOG_ERROR, "grib_accessor_class_number_of_points: Key %s cannot be 'missing'!", self->nj);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    if (nj == 0) {
        grib_context_log(c, GRIB_LOG_ERROR, "grib_accessor_class_number_of_points: Key %s cannot be 0!", self->nj);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    if (plpresent) {
        /*reduced*/
        plsize = nj;
        pl     = (long*)grib_context_malloc(c, sizeof(long) * plsize);
        grib_get_long_array_internal(hand, self->pl, pl, &plsize);
        *val = 0;
        for (i = 0; i < plsize; i++)
            *val += pl[i];
        grib_context_free(c, pl);
    }
    else {
        /*regular*/
        *val = ni * nj;
    }

    return ret;
}
