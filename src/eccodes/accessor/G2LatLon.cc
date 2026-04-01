/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2LatLon.h"

eccodes::AccessorBuilder<eccodes::accessor::G2LatLon> _grib_accessor_g2latlon_builder{};

namespace eccodes::accessor
{

void G2LatLon::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    int n = 0;

    grid_  = c->get_name(get_enclosing_handle(), n++);
    index_ = c->get_long(get_enclosing_handle(), n++);
    given_ = c->get_name(get_enclosing_handle(), n++);
}

int G2LatLon::unpack_double(double* val, size_t* len)
{
    int ret = 0;

    long given = 1;
    double grid[6];
    size_t size = 6;

    if (*len < 1) {
        ret = GRIB_ARRAY_TOO_SMALL;
        return ret;
    }

    if (given_)
        if ((ret = grib_get_long_internal(get_enclosing_handle(), given_, &given)) != GRIB_SUCCESS)
            return ret;

    if (!given) {
        *val = GRIB_MISSING_DOUBLE;
        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_double_array_internal(get_enclosing_handle(), grid_, grid, &size)) != GRIB_SUCCESS)
        return ret;

    *val = grid[index_];

    return GRIB_SUCCESS;
}

int G2LatLon::pack_double(const double* val, size_t* len)
{
    int ret = 0;
    double grid[6];
    size_t size       = 6;
    double new_val    = *val;
    grib_handle* hand = get_enclosing_handle();

    if (*len < 1) {
        ret = GRIB_ARRAY_TOO_SMALL;
        return ret;
    }

    if (given_) {
        long given = *val != GRIB_MISSING_DOUBLE;
        if ((ret = grib_set_long_internal(hand, given_, given)) != GRIB_SUCCESS)
            return ret;
    }

    if ((ret = grib_get_double_array_internal(hand, grid_, grid, &size)) != GRIB_SUCCESS)
        return ret;

    /* index 1 is longitudeOfFirstGridPointInDegrees
     * index 3 is longitudeOfLastGridPointInDegrees
     */
    if ((index_ == 1 || index_ == 3)) {
        /* WMO regulation for GRIB edition 2:
         * The longitude values shall be limited to the range 0 to 360 degrees inclusive */
        new_val = normalise_longitude_in_degrees(*val);
        if (hand->context->debug && new_val != *val) {
            fprintf(stderr, "ECCODES DEBUG pack_double g2latlon: normalise longitude %g -> %g\n", *val, new_val);
        }
    }
    grid[index_] = new_val;

    return grib_set_double_array_internal(hand, grid_, grid, size);
}

int G2LatLon::pack_missing()
{
    double missing = GRIB_MISSING_DOUBLE;
    size_t size    = 1;

    if (!given_)
        return GRIB_NOT_IMPLEMENTED;

    return pack_double(&missing, &size);
}

int G2LatLon::is_missing()
{
    long given = 1;

    if (given_)
        grib_get_long_internal(get_enclosing_handle(), given_, &given);

    return !given;
}

}  // namespace eccodes::accessor
