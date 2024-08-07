/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_g1verificationdate.h"

grib_accessor_class_g1verificationdate_t _grib_accessor_class_g1verificationdate{ "g1verificationdate" };
grib_accessor_class* grib_accessor_class_g1verificationdate = &_grib_accessor_class_g1verificationdate;


void grib_accessor_class_g1verificationdate_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_g1verificationdate_t* self = (grib_accessor_g1verificationdate_t*)a;
    int n = 0;

    self->date = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->time = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->step = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int grib_accessor_class_g1verificationdate_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g1verificationdate_t* self = (grib_accessor_g1verificationdate_t*)a;

    int ret    = 0;
    long date  = 0;
    long time  = 0;
    long cdate = 0;
    long step  = 0;
    long vtime = 0;
    long vdate = 0;
    long vd    = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->date, &date)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->time, &time)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->step, &step)) != GRIB_SUCCESS)
        return ret;

    time /= 100;

    cdate = (long)grib_date_to_julian(date);
    vtime = cdate * 24 + time + step;
    vd    = vtime / 24;
    vdate = grib_julian_to_date(vd);

    //printf("\n********\n date %d, time %d, step %d, vdate: %d, cdate %d, vd %d\n********\n", date, time, step, vdate, cdate, vd);

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    *val = vdate;

    // fprintf(stdout,"\n********\n %d cdate %d vd %d\n********\n", vdate, cdate, step);
    return GRIB_SUCCESS;
}
