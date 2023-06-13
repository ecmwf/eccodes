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
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = value_count;get_native_type
   IMPLEMENTS = pack_string;unpack_string
   IMPLEMENTS = pack_long;unpack_long
   IMPLEMENTS = pack_double;unpack_double
   IMPLEMENTS = init
   MEMBERS= grib_arguments* args
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_suppressed
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in suppressed */
    grib_arguments* args;
} grib_accessor_suppressed;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_suppressed = {
    &grib_accessor_class_long,                      /* super */
    "suppressed",                      /* name */
    sizeof(grib_accessor_suppressed),  /* size */
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
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    0,               /* unpack_float */
    &pack_string,                /* pack_string */
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


grib_accessor_class* grib_accessor_class_suppressed = &_grib_accessor_class_suppressed;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_suppressed* self = (grib_accessor_suppressed*)a;
    self->args                     = c;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

static void log_message(grib_accessor* a)
{
    grib_accessor_suppressed* self = (grib_accessor_suppressed*)a;
    int i                          = 0;
    grib_handle* hand              = grib_handle_of_accessor(a);

    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "key '%s' is unavailable in this version.", a->name);
    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "Please use the following key(s):");
    while (grib_arguments_get_name(hand, self->args, i)) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "\t- %s",
                         grib_arguments_get_name(hand, self->args, i));
        i++;
    }
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    log_message(a);
    return GRIB_NOT_FOUND;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    log_message(a);
    return GRIB_NOT_FOUND;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    log_message(a);
    return GRIB_NOT_FOUND;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    log_message(a);
    return GRIB_NOT_FOUND;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    log_message(a);
    return GRIB_NOT_FOUND;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    log_message(a);
    return GRIB_NOT_FOUND;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}
