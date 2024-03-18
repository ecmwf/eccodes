
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_latlon_increment.h"

grib_accessor_class_latlon_increment_t _grib_accessor_class_latlon_increment{"latlon_increment"};
grib_accessor_class* grib_accessor_class_latlon_increment = &_grib_accessor_class_latlon_increment;


void grib_accessor_class_latlon_increment_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_latlon_increment_t* self = (grib_accessor_latlon_increment_t*)a;
    int n                                = 0;
    grib_handle* hand                    = grib_handle_of_accessor(a);

    self->directionIncrementGiven = grib_arguments_get_name(hand, c, n++);
    self->directionIncrement      = grib_arguments_get_name(hand, c, n++);
    self->scansPositively         = grib_arguments_get_name(hand, c, n++);
    self->first                   = grib_arguments_get_name(hand, c, n++);
    self->last                    = grib_arguments_get_name(hand, c, n++);
    self->numberOfPoints          = grib_arguments_get_name(hand, c, n++);
    self->angleMultiplier         = grib_arguments_get_name(hand, c, n++);
    self->angleDivisor            = grib_arguments_get_name(hand, c, n++);
    self->isLongitude             = grib_arguments_get_long(hand, c, n++);
}

int grib_accessor_class_latlon_increment_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_latlon_increment_t* self = (grib_accessor_latlon_increment_t*)a;
    int ret                              = 0;
    grib_handle* hand                    = grib_handle_of_accessor(a);

    long directionIncrementGiven = 0;
    long directionIncrement      = 0;
    long angleDivisor            = 1;
    long angleMultiplier         = 1;
    double first                 = 0;
    double last                  = 0;
    long numberOfPoints          = 0;
    long scansPositively         = 0;

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    if ((ret = grib_get_long_internal(hand, self->directionIncrementGiven, &directionIncrementGiven)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->scansPositively, &scansPositively)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->directionIncrement, &directionIncrement)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(hand, self->first, &first)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(hand, self->last, &last)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->numberOfPoints, &numberOfPoints)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->angleMultiplier, &angleMultiplier)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->angleDivisor, &angleDivisor)) != GRIB_SUCCESS)
        return ret;

    if (self->isLongitude) {
        if (last < first && scansPositively)
            last += 360;
        /*if (last > first && !scansPositively) first-=360;*/
    }

    if (!directionIncrementGiven && numberOfPoints != GRIB_MISSING_LONG) {
        if (numberOfPoints < 2) {
            /* We cannot compute the increment if we don't have enough points! */
            grib_context_log(a->parent->h->context, GRIB_LOG_ERROR,
                             "Cannot compute lat/lon increments. Not enough points!");
            return GRIB_GEOCALCULUS_PROBLEM;
        }
        if (!scansPositively) { /* scans negatively */
            if (first > last) {
                *val = (first - last) / (numberOfPoints - 1);
            }
            else {
                *val = (first + 360.0 - last) / (numberOfPoints - 1);
            }
        }
        else {
            /* scans positively */
            if (last > first) {
                *val = (last - first) / (numberOfPoints - 1);
            }
            else {
                *val = (last + 360.0 - first) / (numberOfPoints - 1);
            }
        }
    }
    else if (numberOfPoints == GRIB_MISSING_LONG) {
        *val = GRIB_MISSING_DOUBLE;
    }
    else {
        Assert(angleDivisor != 0);
        *val = (double)directionIncrement / angleDivisor * angleMultiplier;
    }

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}

int grib_accessor_class_latlon_increment_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_latlon_increment_t* self = (grib_accessor_latlon_increment_t*)a;
    int ret                              = 0;
    long codedNumberOfPoints             = 0;
    grib_handle* hand                    = grib_handle_of_accessor(a);

    long directionIncrementGiven = 0;
    long directionIncrement      = 0;
    long angleDivisor            = 1;
    long angleMultiplier         = 1;
    double first                 = 0;
    double last                  = 0;
    long numberOfPoints          = 0;
    /* long numberOfPointsInternal = 0; */
    long scansPositively            = 0;
    double directionIncrementDouble = 0;

    ret = grib_get_double_internal(hand, self->first, &first);
    if (ret != GRIB_SUCCESS)
        return ret;

    ret = grib_get_double_internal(hand, self->last, &last);
    if (ret != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->directionIncrementGiven, &directionIncrementGiven)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->numberOfPoints, &numberOfPoints)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->scansPositively, &scansPositively)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->angleMultiplier, &angleMultiplier)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->angleDivisor, &angleDivisor)) != GRIB_SUCCESS)
        return ret;

    if (self->isLongitude) {
        if (last < first && scansPositively)
            last += 360;
        if (last > first && !scansPositively)
            first -= 360;
    }

    if (*val == GRIB_MISSING_DOUBLE) {
        directionIncrement      = GRIB_MISSING_LONG;
        directionIncrementGiven = 1;
        numberOfPoints          = GRIB_MISSING_LONG;
    }
    else {
        /* numberOfPointsInternal = 1+rint(fabs((last-first) / *val)); */

        directionIncrementDouble = rint(*val * (double)angleDivisor / (double)angleMultiplier);

        directionIncrement = (long)directionIncrementDouble;
        if (directionIncrement == 0) {
            directionIncrement      = GRIB_MISSING_LONG;
            directionIncrementGiven = 0;
        }
    }

    //ret = grib_set_long_internal(hand, self->numberOfPoints,numberOfPoints);
    //if(ret) grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannot pack value for %s error %d \n", a->name, self->numberOfPoints, ret);

    grib_get_long_internal(hand, self->numberOfPoints, &codedNumberOfPoints);

    ret = grib_set_long_internal(hand, self->directionIncrement, directionIncrement);
    if (ret)
        return ret;

    ret = grib_set_long_internal(hand, self->directionIncrementGiven, directionIncrementGiven);
    if (ret)
        return ret;

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return GRIB_SUCCESS;
}

int grib_accessor_class_latlon_increment_t::is_missing(grib_accessor* a){
    size_t len = 1;
    double val = 0;

    unpack_double(a, &val, &len);

    return (val == GRIB_MISSING_DOUBLE);
}
