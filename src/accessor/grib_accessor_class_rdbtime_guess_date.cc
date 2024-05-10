
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_rdbtime_guess_date.h"

grib_accessor_class_rdbtime_guess_date_t _grib_accessor_class_rdbtime_guess_date{"rdbtime_guess_date"};
grib_accessor_class* grib_accessor_class_rdbtime_guess_date = &_grib_accessor_class_rdbtime_guess_date;


void grib_accessor_class_rdbtime_guess_date_t::init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_class_long_t::init(a, l, c);
    grib_accessor_rdbtime_guess_date_t* self = (grib_accessor_rdbtime_guess_date_t*)a;
    int n = 0;

    self->typicalYear  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->typicalMonth = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->typicalDay   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->rdbDay       = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->yearOrMonth  = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);

    /* a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY; */
}

int grib_accessor_class_rdbtime_guess_date_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_rdbtime_guess_date_t* self = (grib_accessor_rdbtime_guess_date_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    long typicalYear, typicalMonth, typicalDay, rdbDay;
    long rdbYear, rdbMonth;

    int ret = grib_get_long(h, self->typicalYear, &typicalYear);
    if (ret)
        return ret;
    ret = grib_get_long(h, self->typicalMonth, &typicalMonth);
    if (ret)
        return ret;
    ret = grib_get_long(h, self->typicalDay, &typicalDay);
    if (ret)
        return ret;
    ret = grib_get_long(h, self->rdbDay, &rdbDay);
    if (ret)
        return ret;

    if (rdbDay < typicalDay) {
        if (typicalDay == 31 && typicalMonth == 12) {
            rdbYear  = typicalYear + 1;
            rdbMonth = 1;
        }
        else {
            rdbYear  = typicalYear;
            rdbMonth = typicalMonth + 1;
        }
    }
    else {
        rdbYear  = typicalYear;
        rdbMonth = typicalMonth;
    }

    *val = self->yearOrMonth == 1 ? rdbYear : rdbMonth;
    *len = 1;

    return GRIB_SUCCESS;
}

int grib_accessor_class_rdbtime_guess_date_t::pack_long(grib_accessor* a, const long* v, size_t* len)
{
    /* do nothing*/
    return GRIB_SUCCESS;
}
