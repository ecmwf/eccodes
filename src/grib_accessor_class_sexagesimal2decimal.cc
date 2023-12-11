/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/***********************************************************
 *  Enrico Fucile
 ***********************************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_to_double
   IMPLEMENTS = init; unpack_string
   IMPLEMENTS = dump
   IMPLEMENTS = get_native_type
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_string(grib_accessor*, char*, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_sexagesimal2decimal
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in to_double */
    const char* key;
    long start;
    size_t length;
    long scale;
    /* Members defined in sexagesimal2decimal */
} grib_accessor_sexagesimal2decimal;

extern grib_accessor_class* grib_accessor_class_to_double;

static grib_accessor_class _grib_accessor_class_sexagesimal2decimal = {
    &grib_accessor_class_to_double,                      /* super */
    "sexagesimal2decimal",                      /* name */
    sizeof(grib_accessor_sexagesimal2decimal),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    &unpack_string,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_sexagesimal2decimal = &_grib_accessor_class_sexagesimal2decimal;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_double(dumper, a, NULL);
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    int err                                 = 0;
    grib_accessor_sexagesimal2decimal* self = (grib_accessor_sexagesimal2decimal*)a;
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
