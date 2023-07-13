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
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = init; string_length
   MEMBERS=  const char* input
   MEMBERS=  int trim_left
   MEMBERS=  int trim_right
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

typedef struct grib_accessor_trim
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in ascii */
    /* Members defined in trim */
    const char* input;
    int trim_left;
    int trim_right;
} grib_accessor_trim;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_trim = {
    &grib_accessor_class_ascii,                      /* super */
    "trim",                      /* name */
    sizeof(grib_accessor_trim),  /* size */
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


grib_accessor_class* grib_accessor_class_trim = &_grib_accessor_class_trim;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* arg)
{
    int n                    = 0;
    grib_accessor_trim* self = (grib_accessor_trim*)a;
    grib_handle* h           = grib_handle_of_accessor(a);

    self->input     = grib_arguments_get_name(h, arg, n++);
    self->trim_left = grib_arguments_get_long(h, arg, n++);
    self->trim_right= grib_arguments_get_long(h, arg, n++);
    DEBUG_ASSERT(self->trim_left == 0 || self->trim_left == 1);
    DEBUG_ASSERT(self->trim_right == 0 || self->trim_right == 1);
}

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_trim* self = (grib_accessor_trim*)a;

    int err        = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    char input[256] = {0,};
    size_t size    = sizeof(input) / sizeof(*input);
    char* pInput   = input;

    err = grib_get_string(h, self->input, input, &size);
    if (err) return err;

    string_lrtrim(&pInput, self->trim_left, self->trim_right);
    snprintf(val, 1024, "%s", pInput);
    size = strlen(val);
    *len = size + 1;
    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    char input[256] = {0,};

    size_t inputLen = 256;
    char buf[256]   = {0,};
    char* pBuf = NULL;
    int err;
    grib_handle* h = grib_handle_of_accessor(a);
    grib_accessor_trim* self = (grib_accessor_trim*)a;
    grib_accessor* inputAccesstor = grib_find_accessor(h, self->input);
    if (!inputAccesstor) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor for %s not found", self->input);
        return GRIB_NOT_FOUND;
    }

    if ((err = grib_get_string(h, self->input, input, &inputLen)) != GRIB_SUCCESS)
        return err;

    snprintf(buf, sizeof(buf), "%s", val);
    pBuf = buf;
    string_lrtrim(&pBuf, self->trim_left, self->trim_right);

    return grib_pack_string(inputAccesstor, pBuf, len);
}

static size_t string_length(grib_accessor* a)
{
    return 1024;
}
