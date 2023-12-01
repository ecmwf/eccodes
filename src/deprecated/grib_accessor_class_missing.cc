/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *   Enrico Fucile
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = unpack_bytes;pack_bytes
   IMPLEMENTS = init;dump;destroy
   IMPLEMENTS = get_native_type;sub_section
   IMPLEMENTS = next_offset;value_count;byte_offset;byte_count
   IMPLEMENTS = notify_change;pack_expression
   IMPLEMENTS  = update_size
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static grib_section* sub_section(grib_accessor* a);
static int get_native_type(grib_accessor*);
static int pack_bytes(grib_accessor*, const unsigned char*, size_t* len);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int pack_expression(grib_accessor*, grib_expression*);
static int unpack_bytes(grib_accessor*, unsigned char*, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static int notify_change(grib_accessor*, grib_accessor*);
static void update_size(grib_accessor*, size_t);

typedef struct grib_accessor_missing
{
    grib_accessor att;
    /* Members defined in missing */
} grib_accessor_missing;


static grib_accessor_class _grib_accessor_class_missing = {
    0,                      /* super */
    "missing",                      /* name */
    sizeof(grib_accessor_missing),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* destroy */
    &dump,                       /* dump */
    &next_offset,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    &byte_count,                 /* get number of bytes */
    &byte_offset,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    &sub_section,                /* get sub_section */
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
    &pack_bytes,                 /* pack_bytes */
    &unpack_bytes,               /* unpack_bytes */
    &pack_expression,            /* pack_expression */
    &notify_change,              /* notify_change */
    &update_size,                /* update_size */
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


grib_accessor_class* grib_accessor_class_missing = &_grib_accessor_class_missing;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* param)
{
    /* a->flags = GRIB_FLAG_READONLY; */
    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    /* grib_dump_bytes(dumper,a,NULL); */
}

static long next_offset(grib_accessor* a)
{
    return a->offset;
}

static int value_count(grib_accessor* a, long* c)
{
    *c = 0;
    return 0;
}

static long byte_count(grib_accessor* a)
{
    return 0;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_UNDEFINED;
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static int unpack_bytes(grib_accessor* a, unsigned char* val, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int unpack_long(grib_accessor* a, long* v, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int unpack_double(grib_accessor* a, double* v, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int unpack_string(grib_accessor* a, char* v, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int pack_expression(grib_accessor* a, grib_expression* e)
{
    return GRIB_NOT_FOUND;
}

static int pack_long(grib_accessor* a, const long* v, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int pack_double(grib_accessor* a, const double* v, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int pack_string(grib_accessor* a, const char* v, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static int pack_bytes(grib_accessor* a, const unsigned char* val, size_t* len)
{
    return GRIB_NOT_FOUND;
}

static void destroy(grib_context* ct, grib_accessor* a)
{
}

static grib_section* sub_section(grib_accessor* a)
{
    return NULL;
}

static int notify_change(grib_accessor* self, grib_accessor* observed)
{
    return GRIB_NOT_FOUND;
}

static void update_size(grib_accessor* a, size_t s)
{
}
