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
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;
   IMPLEMENTS = value_count
   IMPLEMENTS = init
   MEMBERS =const char* values
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_latlonvalues
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in latlonvalues */
    const char* values;
} grib_accessor_latlonvalues;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_latlonvalues = {
    &grib_accessor_class_double,                      /* super */
    "latlonvalues",                      /* name */
    sizeof(grib_accessor_latlonvalues),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
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
    0,              /* unpack_string */
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


grib_accessor_class* grib_accessor_class_latlonvalues = &_grib_accessor_class_latlonvalues;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_latlonvalues* self = (grib_accessor_latlonvalues*)a;
    int n                            = 0;

    self->values = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_context* c = a->context;
    int err         = 0;
    double* v       = val;
    double lat, lon, value;
    size_t size         = 0;
    long count          = 0;
    grib_iterator* iter = grib_iterator_new(grib_handle_of_accessor(a), 0, &err);
    if (err) {
        if (iter) grib_iterator_delete(iter);
        grib_context_log(c, GRIB_LOG_ERROR, "latlonvalues: Unable to create iterator");
        return err;
    }

    err  = value_count(a, &count);
    if (err) return err;
    size = count;

    if (*len < size) {
        if (iter) grib_iterator_delete(iter);
        return GRIB_ARRAY_TOO_SMALL;
    }

    while (grib_iterator_next(iter, &lat, &lon, &value)) {
        *(v++) = lat;
        *(v++) = lon;
        *(v++) = value;
    }

    grib_iterator_delete(iter);

    *len = size;

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_latlonvalues* self = (grib_accessor_latlonvalues*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);
    int ret = GRIB_SUCCESS;
    size_t size;
    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "latlonvalues: Unable to get size of %s", self->values);
        return ret;
    }

    *count = 3 * size;
    return GRIB_SUCCESS;
}
