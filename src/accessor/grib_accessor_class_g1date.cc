
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
#include "grib_accessor_class_g1date.h"

grib_accessor_class_g1date_t _grib_accessor_class_g1date{"g1date"};
grib_accessor_class* grib_accessor_class_g1date = &_grib_accessor_class_g1date;


void grib_accessor_class_g1date_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_g1date_t* self = (grib_accessor_g1date_t*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);
    int n                      = 0;

    self->century = grib_arguments_get_name(hand, c, n++);
    self->year    = grib_arguments_get_name(hand, c, n++);
    self->month   = grib_arguments_get_name(hand, c, n++);
    self->day     = grib_arguments_get_name(hand, c, n++);
}

int grib_accessor_class_g1date_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_g1date_t* self = (grib_accessor_g1date_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);

    int ret = 0;
    long year = 0, century = 0, month = 0, day = 0;

    if ((ret = grib_get_long_internal(hand, self->century, &century)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->day, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->month, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->year, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    *val = ((century - 1) * 100 + year) * 10000 + month * 100 + day;

    if (year == 255 && day == 255 && month >= 1 && month <= 12) {
        *val = month;
    }

    if (year == 255 && day != 255 && month >= 1 && month <= 12) {
        *val = month * 100 + day;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_g1date_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_g1date_t* self = (grib_accessor_g1date_t*)a;
    grib_handle* hand = grib_handle_of_accessor(a);

    int ret = 0;
    long v = val[0];
    long year = 0, century = 0, month = 0, day = 0;

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    long d = grib_julian_to_date(grib_date_to_julian(v));
    if (v != d) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_g1date_t: pack_long invalid date %ld, changed to %ld", v, d);
        return GRIB_ENCODING_ERROR;
    }

    century = v / 1000000;
    v %= 1000000;
    year = v / 10000;
    v %= 10000;
    month = v / 100;
    v %= 100;
    day = v;

    if (year == 0)
        year = 100;
    else
        century++;

    if ((ret = grib_set_long_internal(hand, self->century, century)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->day, day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->month, month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_set_long_internal(hand, self->year, year)) != GRIB_SUCCESS)
        return ret;

    return GRIB_SUCCESS;
}

static const char* months[] = {
    "jan",
    "feb",
    "mar",
    "apr",
    "may",
    "jun",
    "jul",
    "aug",
    "sep",
    "oct",
    "nov",
    "dec",
};

int grib_accessor_class_g1date_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    grib_accessor_g1date_t* self = (grib_accessor_g1date_t*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);

    int ret = 0;
    char tmp[1024];
    long year = 0, century = 0, month = 0, day = 0;

    if ((ret = grib_get_long_internal(hand, self->century, &century)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->day, &day)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->month, &month)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->year, &year)) != GRIB_SUCCESS)
        return ret;

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    if (year == 255 && day == 255 && month >= 1 && month <= 12) {
        strcpy(tmp, months[month - 1]);
    }
    else if (year == 255 && month >= 1 && month <= 12) {
        snprintf(tmp, sizeof(tmp), "%s-%02ld", months[month - 1], day);
    }
    else {
        long x = ((century - 1) * 100 + year) * 10000 + month * 100 + day;
        snprintf(tmp, sizeof(tmp), "%ld", x);
    }

    size_t l = strlen(tmp) + 1;
    if (*len < l) {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    *len = l;
    strcpy(val, tmp);

    return GRIB_SUCCESS;
}

int grib_accessor_class_g1date_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}
