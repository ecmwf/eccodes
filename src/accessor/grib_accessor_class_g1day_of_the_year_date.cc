
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
#include "grib_accessor_class_g1day_of_the_year_date.h"

grib_accessor_class_g1day_of_the_year_date_t _grib_accessor_class_g1day_of_the_year_date{"g1day_of_the_year_date"};
grib_accessor_class* grib_accessor_class_g1day_of_the_year_date = &_grib_accessor_class_g1day_of_the_year_date;



void grib_accessor_class_g1day_of_the_year_date_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_g1date_t::init(a, l, c);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_class_g1day_of_the_year_date_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_g1day_of_the_year_date_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    /* special clim case where each mont have 30 days.. to comply with mars*/
    grib_accessor_g1day_of_the_year_date_t* self = (grib_accessor_g1day_of_the_year_date_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    char tmp[1024];

    long year    = 0;
    long century = 0;
    long month   = 0;
    long day     = 0;
    long fullyear = 0;
    long fake_day_of_year = 0;

    grib_get_long_internal(hand, self->century, &century);
    grib_get_long_internal(hand, self->day, &day);
    grib_get_long_internal(hand, self->month, &month);
    grib_get_long_internal(hand, self->year, &year);

    fullyear = ((century - 1) * 100 + year);
    fake_day_of_year = ((month - 1) * 30) + day;
    snprintf(tmp, sizeof(tmp), "%04ld-%03ld", fullyear, fake_day_of_year);

    size_t l = strlen(tmp) + 1;
    if (*len < l) {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    *len = l;
    strcpy(val, tmp);

    return GRIB_SUCCESS;
}
