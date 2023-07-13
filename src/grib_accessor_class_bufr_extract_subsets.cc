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
   IMPLEMENTS = init; get_native_type
   IMPLEMENTS = pack_long;
   MEMBERS    = const char* numericValues
   MEMBERS    = const char* pack
   MEMBERS    = grib_accessor* numericValuesAccessor
   MEMBERS    = grib_accessor* packAccessor
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
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_bufr_extract_subsets
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in bufr_extract_subsets */
    const char* numericValues;
    const char* pack;
    grib_accessor* numericValuesAccessor;
    grib_accessor* packAccessor;
} grib_accessor_bufr_extract_subsets;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_bufr_extract_subsets = {
    &grib_accessor_class_gen,                      /* super */
    "bufr_extract_subsets",                      /* name */
    sizeof(grib_accessor_bufr_extract_subsets),  /* size */
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


grib_accessor_class* grib_accessor_class_bufr_extract_subsets = &_grib_accessor_class_bufr_extract_subsets;

/* END_CLASS_IMP */

static void get_accessors(grib_accessor* a)
{
    grib_accessor_bufr_extract_subsets* self = (grib_accessor_bufr_extract_subsets*)a;
    grib_handle* h                           = grib_handle_of_accessor(a);

    if (self->packAccessor)
        return;
    self->numericValuesAccessor = grib_find_accessor(h, self->numericValues);
    self->packAccessor          = grib_find_accessor(h, self->pack);
}

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    int n                                    = 0;
    grib_accessor_bufr_extract_subsets* self = (grib_accessor_bufr_extract_subsets*)a;

    a->length           = 0;
    self->numericValues = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->pack          = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int err                                  = 0;
    grib_accessor_bufr_extract_subsets* self = (grib_accessor_bufr_extract_subsets*)a;
    size_t l                                 = 1;
    long v[1];

    get_accessors(a);

    v[0] = 1;
    err  = grib_pack_long(self->packAccessor, v, &l);
    if (err) {
        if (err == GRIB_ENCODING_ERROR)
            grib_context_log(a->context, GRIB_LOG_ERROR, "Could not extract subset(s).\n\tHint: Did you forget to set unpack=1?");
        return err;
    }

    return err;
}
