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
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init
   IMPLEMENTS = dump
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count
   IMPLEMENTS = byte_offset
   IMPLEMENTS = get_native_type
   IMPLEMENTS = update_size
   IMPLEMENTS = compare
   IMPLEMENTS = pack_long
   MEMBERS= int  carg
   MEMBERS= const char* seclen
   MEMBERS= const char* offsetdata
   MEMBERS= const char* offsetsection
   MEMBERS= int dirty
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
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);
static void update_size(grib_accessor*, size_t);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_values
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in values */
    int  carg;
    const char* seclen;
    const char* offsetdata;
    const char* offsetsection;
    int dirty;
} grib_accessor_values;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_values = {
    &grib_accessor_class_gen,                      /* super */
    "values",                      /* name */
    sizeof(grib_accessor_values),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    &next_offset,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    &byte_count,                 /* get number of bytes */
    &byte_offset,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    0,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    &update_size,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    &compare,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_values = &_grib_accessor_class_values;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */
static long init_length(grib_accessor* a)
{
    grib_accessor_values* self = (grib_accessor_values*)a;
    int ret                    = 0;

    long seclen        = 0;
    long offsetsection = 0;
    long offsetdata    = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->seclen, &seclen)))
        return ret;

    if (seclen == 0) {
        /* printf("init_length seclen=0\n"); */
        return 0;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetsection, &offsetsection)))
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetdata, &offsetdata)))
        return ret;

    /* When reparsing */
    if (offsetdata < offsetsection) {
        /* printf("init_length offsetdata < offsetsection=0\n"); */
        Assert(grib_handle_of_accessor(a)->loader);
        return 0;
    }

    return seclen - (offsetdata - offsetsection);
}

static void init(grib_accessor* a, const long v, grib_arguments* params)
{
    grib_accessor_values* self = (grib_accessor_values*)a;
    self->carg                 = 0;

    self->seclen        = grib_arguments_get_name(grib_handle_of_accessor(a), params, self->carg++);
    self->offsetdata    = grib_arguments_get_name(grib_handle_of_accessor(a), params, self->carg++);
    self->offsetsection = grib_arguments_get_name(grib_handle_of_accessor(a), params, self->carg++);
    self->dirty         = 1;

    a->length = init_length(a);
    /* Assert(a->length>=0); */
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_DOUBLE;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper, a);
}

static long byte_count(grib_accessor* a)
{
    grib_context_log(a->context, GRIB_LOG_DEBUG, "byte_count of %s = %ld", a->name, a->length);
    return a->length;
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static long next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}

static void update_size(grib_accessor* a, size_t s)
{
    grib_context_log(a->context, GRIB_LOG_DEBUG, "updating size of %s old %ld new %ld", a->name, a->length, s);
    a->length = s;
    Assert(a->length >= 0);
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval   = 0;
    double* aval = 0;
    double* bval = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = grib_value_count(a, &count);
    if (err)
        return err;
    alen = count;

    err = grib_value_count(b, &count);
    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (double*)grib_context_malloc(a->context, alen * sizeof(double));
    bval = (double*)grib_context_malloc(b->context, blen * sizeof(double));

    grib_unpack_double(a, aval, &alen);
    grib_unpack_double(b, bval, &blen);

    retval = GRIB_SUCCESS;
    while (alen != 0) {
        if (*bval != *aval)
            retval = GRIB_DOUBLE_VALUE_MISMATCH;
        alen--;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int ret                    = 0;
    grib_accessor_values* self = (grib_accessor_values*)a;
    int i;
    double* dval = (double*)grib_context_malloc(a->context, *len * sizeof(double));

    for (i = 0; i < *len; i++)
        dval[i] = (double)val[i];

    ret = grib_pack_double(a, dval, len);

    grib_context_free(a->context, dval);

    self->dirty = 1;

    return ret;
}
