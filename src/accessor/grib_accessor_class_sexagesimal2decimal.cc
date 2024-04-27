
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_sexagesimal2decimal.h"

grib_accessor_class_sexagesimal2decimal_t _grib_accessor_class_sexagesimal2decimal{"sexagesimal2decimal"};
grib_accessor_class* grib_accessor_class_sexagesimal2decimal = &_grib_accessor_class_sexagesimal2decimal;


void grib_accessor_class_sexagesimal2decimal_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_to_double_t::init(a, len, arg);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void grib_accessor_class_sexagesimal2decimal_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_double(dumper, a, NULL);
}

int grib_accessor_class_sexagesimal2decimal_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_DOUBLE;
}

int grib_accessor_class_sexagesimal2decimal_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    int err                                 = 0;
    grib_accessor_sexagesimal2decimal_t* self = (grib_accessor_sexagesimal2decimal_t*)a;
    char buff[512]                          = {0,};
    size_t length = 0;
    size_t size   = 512;
    char* p;
    char* q;
    double dd, mm = 0, ss = 0;
    int dd_sign = 1;

    err = grib_get_string(grib_handle_of_accessor(a), self->key, buff, &size);
    if (err)
        return err;
    q = buff + self->start;
    if (self->length)
        q[length] = 0;
    p = q;

    while (*p != '-' && *p != ':' && *p != ' ' && *p != 0) {
        p++;
    }

    if (*p == 0) {
        return GRIB_WRONG_CONVERSION;
    }
    *p = 0;

    dd = atoi(q);
    p++;
    q = p;
    while (*p != '-' && *p != ':' && *p != ' ' && *p != 'N' && *p != 'S' && *p != 'E' && *p != 'W' && *p != 0) {
        p++;
    }
    switch (*p) {
        case ' ':
        case '-':
        case ':':
            *p = 0;
            mm = atoi(q) / 60.0;
            dd += mm;
            p++;
            q = p;
            break;
        case 'N':
        case 'E':
            *p      = 0;
            dd_sign = 1;
            mm      = atoi(q) / 60.0;
            dd += mm;
            p++;
            q = p;
            break;
        case 'S':
        case 'W':
            *p = 0;
            mm = atoi(q) / 60.0;
            dd += mm;
            dd_sign = -1;
            p++;
            q = p;
            break;
        case 0:
            break;
        default:
            return GRIB_WRONG_CONVERSION;
    }
    if (*p) {
        while (*p != '-' && *p != ':' && *p != ' ' && *p != 'N' && *p != 'S' && *p != 'E' && *p != 'W' && *p != 0) {
            p++;
        }
        switch (*p) {
            case ' ':
            case '-':
            case ':':
                *p = 0;
                ss = atof(q) / 60.0;
                dd += ss;
                break;
            case 'N':
            case 'E':
                *p = 0;
                ss = atof(q) / 60.0;
                dd += ss;
                dd_sign = 1;
                break;
            case 'S':
            case 'W':
                *p = 0;
                ss = atof(q) / 60.0;
                dd += ss;
                dd_sign = -1;
                break;
            case 0:
                break;
            default:
                return GRIB_WRONG_CONVERSION;
        }
    }
    dd *= dd_sign;

    snprintf(buff, sizeof(buff), "%.2f", dd);
    length = strlen(buff);

    if (len[0] < length + 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "unpack_string: Wrong size (%lu) for %s, it contains %ld values",
                len[0], a->name, a->length + 1);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    strcpy(val, buff);

    len[0] = length;
    return GRIB_SUCCESS;
}
