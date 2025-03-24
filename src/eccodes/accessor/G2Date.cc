/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2Date.h"

eccodes::accessor::G2Date _grib_accessor_g2date;
eccodes::Accessor* grib_accessor_g2date = &_grib_accessor_g2date;

namespace eccodes::accessor
{

void G2Date::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n = 0;

    year_  = c->get_name(grib_handle_of_accessor(this), n++);
    month_ = c->get_name(grib_handle_of_accessor(this), n++);
    day_   = c->get_name(grib_handle_of_accessor(this), n++);
}

int G2Date::unpack_long(long* val, size_t* len)
{
    int ret    = 0;
    long year  = 0;
    long month = 0;
    long day   = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), day_, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), month_, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), year_, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    val[0] = year * 10000 + month * 100 + day;

    return GRIB_SUCCESS;
}

int G2Date::pack_long(const long* val, size_t* len)
{
    int ret    = GRIB_SUCCESS;
    long v     = val[0];
    long year  = 0;
    long month = 0;
    long day   = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    year = v / 10000;
    v %= 10000;
    month = v / 100;
    v %= 100;
    day = v;

    if (!is_date_valid(year, month, day, 0, 0, 0)) {
        // ECC-1777: For now just a warning. Will later change to an error
        fprintf(stderr, "ECCODES WARNING :  %s:%s: Date is not valid! year=%ld month=%ld day=%ld\n",
                accessor_type().get().c_str(), __func__, year, month, day);
    }

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(this), day_, day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(this), month_, month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(grib_handle_of_accessor(this), year_, year)) != GRIB_SUCCESS)
        return ret;

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
