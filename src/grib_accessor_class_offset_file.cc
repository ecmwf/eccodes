/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double
   IMPLEMENTS = unpack_string
   IMPLEMENTS = init
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_offset_file
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in offset_file */
} grib_accessor_offset_file;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_offset_file = {
    &grib_accessor_class_double,                      /* super */
    "offset_file",                      /* name */
    sizeof(grib_accessor_offset_file),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
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


grib_accessor_class* grib_accessor_class_offset_file = &_grib_accessor_class_offset_file;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->length = 0;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    *val = (double)grib_handle_of_accessor(a)->offset;
    *len = 1;
    return GRIB_SUCCESS;
}

static int unpack_string(grib_accessor* a, char* v, size_t* len)
{
    double val = 0;
    size_t l   = 1;
    char repres[1024] = {0,};
    int err = 0;

    err = grib_unpack_double(a, &val, &l);
    if (err)
        return err;

    snprintf(repres, sizeof(repres), "%.0f", val);

    l = strlen(repres) + 1;
    if (l > *len) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_offset: unpack_string: Buffer too small for %s",
                         a->name);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }
    grib_context_log(a->context, GRIB_LOG_DEBUG, "grib_accessor_offset: Casting double %s to string", a->name);

    *len = l;

    strcpy(v, repres);
    return GRIB_SUCCESS;
}
