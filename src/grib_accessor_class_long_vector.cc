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
   SUPER      = grib_accessor_class_abstract_long_vector
   IMPLEMENTS = init
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;get_native_type
   MEMBERS = const char* vector
   MEMBERS = int index
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_long_vector
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in abstract_long_vector */
    long* v;
    long pack_index;
    int number_of_elements;
    /* Members defined in long_vector */
    const char* vector;
    int index;
} grib_accessor_long_vector;

extern grib_accessor_class* grib_accessor_class_abstract_long_vector;

static grib_accessor_class _grib_accessor_class_long_vector = {
    &grib_accessor_class_abstract_long_vector,                      /* super */
    "long_vector",                      /* name */
    sizeof(grib_accessor_long_vector),  /* size */
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
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
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


grib_accessor_class* grib_accessor_class_long_vector = &_grib_accessor_class_long_vector;

/* END_CLASS_IMP */

typedef struct grib_accessor_abstract_long_vector
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in abstract_long_vector */
    long* v;
    long pack_index;
    int number_of_elements;
} grib_accessor_abstract_long_vector;

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_long_vector* self       = (grib_accessor_long_vector*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector* v = NULL;
    int n                                 = 0;

    self->vector = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    va = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v  = (grib_accessor_abstract_long_vector*)va;

    self->index = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);

    /* check self->index on init and never change it */
    Assert(self->index < v->number_of_elements && self->index >= 0);

    a->length = 0;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    size_t size = 0;
    int err = 0;
    long* vector;
    grib_accessor_long_vector* self       = (grib_accessor_long_vector*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector* v = NULL;

    va = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v  = (grib_accessor_abstract_long_vector*)va;

    /*TODO implement the dirty mechanism to avoid to unpack every time */
    err = grib_get_size(grib_handle_of_accessor(a), self->vector, &size);
    if (err) return err;
    DEBUG_ASSERT(size > 0);
    vector = (long*)grib_context_malloc(a->context, sizeof(long) * size);
    err = grib_unpack_long(va, vector, &size);
    grib_context_free(a->context, vector);
    if (err) return err;

    *val = v->v[self->index];

    return GRIB_SUCCESS;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    long lval                             = 0;
    int err                               = 0;
    grib_accessor_long_vector* self       = (grib_accessor_long_vector*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector* v = NULL;
    va                                    = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v                                     = (grib_accessor_abstract_long_vector*)va;

    err = unpack_long(a, &lval, len);

    *val = (double)v->v[self->index];

    return err;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int err                               = 0;
    grib_accessor_long_vector* self       = (grib_accessor_long_vector*)a;
    grib_accessor* va                     = NULL;
    grib_accessor_abstract_long_vector* v = NULL;

    va = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(a), self->vector);
    v  = (grib_accessor_abstract_long_vector*)va;

    v->pack_index = self->index;

    err = grib_pack_long(va, val, len);

    return err;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}
