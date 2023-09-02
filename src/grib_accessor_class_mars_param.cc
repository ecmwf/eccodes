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
   SUPER      = grib_accessor_class_ascii
   IMPLEMENTS = pack_string;unpack_string
   IMPLEMENTS = init; string_length
   MEMBERS=  const char* paramId
   MEMBERS=  const char* table
   MEMBERS=  const char* param
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_mars_param
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in ascii */
    /* Members defined in mars_param */
    const char* paramId;
    const char* table;
    const char* param;
} grib_accessor_mars_param;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_mars_param = {
    &grib_accessor_class_ascii,                      /* super */
    "mars_param",                      /* name */
    sizeof(grib_accessor_mars_param),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    &string_length,              /* get length of string */
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
    0,              /* unpack_double */
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


grib_accessor_class* grib_accessor_class_mars_param = &_grib_accessor_class_mars_param;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    int n                          = 0;
    grib_accessor_mars_param* self = (grib_accessor_mars_param*)a;
    self->paramId                  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->table                    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->param                    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

// For an alternative implementation of pack_string and unpack_string, see
//   src/deprecated/grib_accessor_class_mars_param.cc
//
static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_mars_param* self = (grib_accessor_mars_param*)a;
    long param                     = 0;
    long table                     = 0;
    int ret                        = 0;

    if (self->table != NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a), self->table, &table)) != GRIB_SUCCESS)
        return ret;
    if (self->param != NULL && (ret = grib_get_long_internal(grib_handle_of_accessor(a), self->param, &param)) != GRIB_SUCCESS)
        return ret;

    /*if (table==200) table=128;*/
    snprintf(val, 32, "%ld.%ld", param, table);
    *len = strlen(val) + 1;

    return GRIB_SUCCESS;
}

static size_t string_length(grib_accessor* a)
{
    return 7;
}
