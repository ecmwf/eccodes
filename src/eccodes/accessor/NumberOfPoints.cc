/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "NumberOfPoints.h"

eccodes::accessor::NumberOfPoints _grib_accessor_number_of_points;
eccodes::Accessor* grib_accessor_number_of_points = &_grib_accessor_number_of_points;

namespace eccodes::accessor
{

void NumberOfPoints::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n             = 0;
    grib_handle* hand = get_enclosing_handle();

    ni_        = c->get_name(hand, n++);
    nj_        = c->get_name(hand, n++);
    plpresent_ = c->get_name(hand, n++);
    pl_        = c->get_name(hand, n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int NumberOfPoints::unpack_long(long* val, size_t* len)
{
    int ret = GRIB_SUCCESS;
    long ni = 0, nj = 0, plpresent = 0;
    size_t plsize     = 0;
    long* pl          = NULL;
    grib_context* c   = context_;
    grib_handle* hand = get_enclosing_handle();

    if ((ret = grib_get_long_internal(hand, ni_, &ni)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, nj_, &nj)) != GRIB_SUCCESS)
        return ret;

    if (plpresent_ &&
        ((ret = grib_get_long_internal(hand, plpresent_, &plpresent)) != GRIB_SUCCESS))
        return ret;

    if (grib_is_missing(hand, nj_, &ret) && ret == GRIB_SUCCESS) {
        grib_context_log(c, GRIB_LOG_ERROR, "grib_accessor_number_of_points: Key %s cannot be 'missing'!", nj_);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    if (nj == 0) {
        grib_context_log(c, GRIB_LOG_ERROR, "grib_accessor_number_of_points: Key %s cannot be 0!", nj_);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    if (plpresent) {
        /*reduced*/
        plsize = nj;
        pl     = (long*)grib_context_malloc(c, sizeof(long) * plsize);
        grib_get_long_array_internal(hand, pl_, pl, &plsize);
        *val = 0;
        for (size_t i = 0; i < plsize; i++)
            *val += pl[i];
        grib_context_free(c, pl);
    }
    else {
        /*regular*/
        *val = ni * nj;
    }

    return ret;
}

}  // namespace eccodes::accessor
