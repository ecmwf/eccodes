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
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = string_length
   IMPLEMENTS = get_native_type
   IMPLEMENTS = init
   MEMBERS = const char* values
   MEMBERS = const char* packing_type
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_packing_type
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in packing_type */
    const char* values;
    const char* packing_type;
} grib_accessor_packing_type;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_packing_type = {
    &grib_accessor_class_gen,                      /* super */
    "packing_type",                      /* name */
    sizeof(grib_accessor_packing_type),  /* size */
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


grib_accessor_class* grib_accessor_class_packing_type = &_grib_accessor_class_packing_type;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* args)
{
    int n                              = 0;
    grib_accessor_packing_type* self = (grib_accessor_packing_type*)a;
    self->values                       = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->packing_type                 = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

static size_t string_length(grib_accessor* a)
{
    return 1024;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static int pack_string(grib_accessor*a, const char* sval, size_t* len)
{
    grib_accessor_packing_type* self = (grib_accessor_packing_type*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    double* values = NULL;
    grib_context* c = a->context;
    size_t size = 0;
    int err = 0;

    if ((err = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return err;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_string_internal(h, self->packing_type, sval, len)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    grib_context_free(c, values);
    return GRIB_SUCCESS;
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_packing_type* self = (grib_accessor_packing_type*)a;
    grib_handle* h                   = grib_handle_of_accessor(a);

    return grib_get_string(h, self->packing_type, val, len);
}
