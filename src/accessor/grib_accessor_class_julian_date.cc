
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_julian_date.h"

grib_accessor_class_julian_date_t _grib_accessor_class_julian_date{"julian_date"};
grib_accessor_class* grib_accessor_class_julian_date = &_grib_accessor_class_julian_date;



void grib_accessor_class_julian_date_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_double_t::init(a, l, c);
    grib_accessor_julian_date_t* self = (grib_accessor_julian_date_t*)a;
    int n                           = 0;
    grib_handle* h                  = grib_handle_of_accessor(a);

    self->year  = grib_arguments_get_name(h, c, n++);
    self->month = grib_arguments_get_name(h, c, n++);

    self->day = grib_arguments_get_name(h, c, n++);
    if (self->day == NULL) {
        self->hour   = 0;
        self->minute = 0;
        self->second = 0;
        self->ymd    = self->year;
        self->hms    = self->month;
        self->year   = 0;
        self->month  = 0;
    }
    else {
        self->ymd    = 0;
        self->hms    = 0;
        self->hour   = grib_arguments_get_name(h, c, n++);
        self->minute = grib_arguments_get_name(h, c, n++);
        self->second = grib_arguments_get_name(h, c, n++);
    }
    self->sep[0] = ' ';
    self->sep[1] = 0;
    self->sep[2] = 0;
    self->sep[3] = 0;
    self->sep[4] = 0;

    a->length = 0;
}

void grib_accessor_class_julian_date_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_julian_date_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    int ret = 0;
    long hour, minute, second;
    long year, month, day, ymd, hms;
    grib_handle* h                  = grib_handle_of_accessor(a);
    grib_accessor_julian_date_t* self = (grib_accessor_julian_date_t*)a;

    if (self->ymd == NULL) {
        ret = grib_get_long(h, self->year, &year);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->month, &month);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->day, &day);
        if (ret != GRIB_SUCCESS)
            return ret;

        ret = grib_get_long(h, self->hour, &hour);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->minute, &minute);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->second, &second);
        if (ret != GRIB_SUCCESS)
            return ret;
    }
    else {
        ret = grib_get_long(h, self->ymd, &ymd);
        if (ret != GRIB_SUCCESS)
            return ret;
        year = ymd / 10000;
        ymd %= 10000;
        month = ymd / 100;
        ymd %= 100;
        day = ymd;

        ret = grib_get_long(h, self->hms, &hms);
        if (ret != GRIB_SUCCESS)
            return ret;
        hour = hms / 10000;
        hms %= 10000;
        minute = hms / 100;
        hms %= 100;
        second = hms;
    }

    ret = grib_datetime_to_julian(year, month, day, hour, minute, second, val);

    return ret;
}

int grib_accessor_class_julian_date_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_julian_date_t* self = (grib_accessor_julian_date_t*)a;
    int ret                         = 0;
    long hour                       = 0;
    long minute                     = 0;
    long second                     = 0;
    long ymd = 0, hms = 0;
    long year, month, day;
    grib_handle* h = grib_handle_of_accessor(a);

    ret = grib_julian_to_datetime(*val, &year, &month, &day, &hour, &minute, &second);
    if (ret != 0)
        return ret;

    if (self->ymd == NULL) {
        ret = grib_set_long(h, self->year, year);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->month, month);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->day, day);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->hour, hour);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->minute, minute);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->second, second);
        if (ret != 0)
            return ret;
    }
    else {
        ymd = year * 10000 + month * 100 + day;
        ret = grib_set_long(h, self->ymd, ymd);
        if (ret != 0)
            return ret;

        hms = hour * 10000 + minute * 100 + second;
        ret = grib_set_long(h, self->hms, hms);
        if (ret != 0)
            return ret;
    }

    return ret;
}

int grib_accessor_class_julian_date_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    int ret = 0;
    long hour, minute, second;
    long year, month, day, ymd, hms;
    grib_accessor_julian_date_t* self = (grib_accessor_julian_date_t*)a;
    char* sep                       = self->sep;
    grib_handle* h                  = grib_handle_of_accessor(a);

    if (*len < 15)
        return GRIB_BUFFER_TOO_SMALL;

    if (self->ymd == NULL) {
        ret = grib_get_long(h, self->year, &year);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->month, &month);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->day, &day);
        if (ret != GRIB_SUCCESS)
            return ret;

        ret = grib_get_long(h, self->hour, &hour);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->minute, &minute);
        if (ret != GRIB_SUCCESS)
            return ret;
        ret = grib_get_long(h, self->second, &second);
        if (ret != GRIB_SUCCESS)
            return ret;
    }
    else {
        ret = grib_get_long(h, self->ymd, &ymd);
        if (ret != GRIB_SUCCESS)
            return ret;
        year = ymd / 10000;
        ymd %= 10000;
        month = ymd / 100;
        ymd %= 100;
        day = ymd;

        ret = grib_get_long(h, self->hms, &hms);
        if (ret != GRIB_SUCCESS)
            return ret;
        hour = hms / 10000;
        hms %= 10000;
        minute = hms / 100;
        hms %= 100;
        second = hms;
    }

    if (sep[1] != 0 && sep[2] != 0 && sep[3] != 0 && sep[4] != 0) {
        snprintf(val, 1024, "%04ld%c%02ld%c%02ld%c%02ld%c%02ld%c%02ld", year, sep[0], month, sep[1], day, sep[2], hour, sep[3], minute, sep[4], second);
    }
    else if (sep[0] != 0) {
        snprintf(val, 1024, "%04ld%02ld%02ld%c%02ld%02ld%02ld", year, month, day, sep[0], hour, minute, second);
    }
    else {
        snprintf(val, 1024, "%04ld%02ld%02ld%02ld%02ld%02ld", year, month, day, hour, minute, second);
    }
    *len = strlen(val)+1;
    return ret;
}

int grib_accessor_class_julian_date_t::pack_string(grib_accessor* a, const char* val, size_t* len){
    int ret = 0;
    long hour, minute, second;
    long year, month, day, ymd, hms;
    grib_accessor_julian_date_t* self = (grib_accessor_julian_date_t*)a;
    char* sep                       = self->sep;
    grib_handle* h                  = grib_handle_of_accessor(a);

    ret = sscanf(val, "%04ld%c%02ld%c%02ld%c%02ld%c%02ld%c%02ld", &year, &sep[0], &month, &sep[1], &day, &sep[2], &hour, &sep[3], &minute, &sep[4], &second);
    if (ret != 11) {
        if (strlen(val) == 15) {
            ret = sscanf(val, "%04ld%02ld%02ld%c%02ld%02ld%02ld", &year, &month, &day, &sep[0], &hour, &minute, &second);
            if (ret != 7) {
                grib_context_log(h->context, GRIB_LOG_ERROR, " Wrong date time format. Please use \"YYYY-MM-DD hh:mm:ss\"");
                return GRIB_INVALID_KEY_VALUE;
            }
            sep[1] = 0;
            sep[2] = 0;
            sep[3] = 0;
            sep[4] = 0;
        }
        else {
            ret = sscanf(val, "%04ld%02ld%02ld%02ld%02ld%02ld", &year, &month, &day, &hour, &minute, &second);
            if (ret != 6) {
                grib_context_log(h->context, GRIB_LOG_ERROR, " Wrong date time format. Please use \"YYYY-MM-DD hh:mm:ss\"");
                return GRIB_INVALID_KEY_VALUE;
            }
            sep[0] = 0;
            sep[1] = 0;
            sep[2] = 0;
            sep[3] = 0;
            sep[4] = 0;
        }
    }

    if (self->ymd == NULL) {
        ret = grib_set_long(h, self->year, year);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->month, month);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->day, day);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->hour, hour);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->minute, minute);
        if (ret != 0)
            return ret;
        ret = grib_set_long(h, self->second, second);
        if (ret != 0)
            return ret;
    }
    else {
        ymd = year * 10000 + month * 100 + day;
        ret = grib_set_long(h, self->ymd, ymd);
        if (ret != 0)
            return ret;

        hms = hour * 10000 + minute * 100 + second;
        ret = grib_set_long(h, self->hms, hms);
        if (ret != 0)
            return ret;
    }

    return ret;
}

int grib_accessor_class_julian_date_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_context_log(a->context, GRIB_LOG_ERROR, " Cannot unpack %s as long", a->name);
    return GRIB_NOT_IMPLEMENTED;
}
int grib_accessor_class_julian_date_t::pack_long(grib_accessor* a, const long* v, size_t* len){
    grib_context_log(a->context, GRIB_LOG_ERROR, " Cannot pack %s as long", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_julian_date_t::pack_expression(grib_accessor* a, grib_expression* e){
    size_t len        = 1;
    long lval         = 0;
    double dval       = 0;
    const char* cval  = NULL;
    int ret           = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    switch (grib_expression_native_type(hand, e)) {
        case GRIB_TYPE_LONG: {
            len = 1;
            ret = grib_expression_evaluate_long(hand, e, &lval);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to set %s as long", a->name);
                return ret;
            }
            /*if (hand->context->debug)
                    printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %ld\n", a->name,lval);*/
            return a->pack_long(&lval, &len);        }

        case GRIB_TYPE_DOUBLE: {
            len = 1;
            ret = grib_expression_evaluate_double(hand, e, &dval);
            /*if (hand->context->debug)
                    printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %g\n", a->name, dval);*/
            return a->pack_double(&dval, &len);        }

        case GRIB_TYPE_STRING: {
            char tmp[1024];
            len  = sizeof(tmp);
            cval = grib_expression_evaluate_string(hand, e, tmp, &len, &ret);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to set %s as string", a->name);
                return ret;
            }
            len = strlen(cval);
            /*if (hand->context->debug)
                    printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %s\n", a->name, cval);*/
            return a->pack_string(cval, &len);        }
    }

    return GRIB_NOT_IMPLEMENTED;
}
