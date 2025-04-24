/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1VerificationDate.h"

eccodes::accessor::G1VerificationDate _grib_accessor_g1verificationdate;
eccodes::Accessor* grib_accessor_g1verificationdate = &_grib_accessor_g1verificationdate;

namespace eccodes::accessor
{

void G1VerificationDate::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n = 0;

    date_ = c->get_name(get_enclosing_handle(), n++);
    time_ = c->get_name(get_enclosing_handle(), n++);
    step_ = c->get_name(get_enclosing_handle(), n++);

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int G1VerificationDate::unpack_long(long* val, size_t* len)
{
    int ret    = 0;
    long date  = 0;
    long time  = 0;
    long cdate = 0;
    long step  = 0;
    long vtime = 0;
    long vdate = 0;
    long vd    = 0;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), date_, &date)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), time_, &time)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), step_, &step)) != GRIB_SUCCESS)
        return ret;

    time /= 100;

    cdate = (long)grib_date_to_julian(date);
    vtime = cdate * 24 + time + step;
    vd    = vtime / 24;
    vdate = grib_julian_to_date(vd);

    // printf("\n********\n date %d, time %d, step %d, vdate: %d, cdate %d, vd %d\n********\n", date, time, step, vdate, cdate, vd);

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    *val = vdate;

    // fprintf(stdout,"\n********\n %d cdate %d vd %d\n********\n", vdate, cdate, step);
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
