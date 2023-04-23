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
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = init;dump
   MEMBERS    = const char* owner
   MEMBERS    = int bit_index
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_bit
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in bit */
    const char* owner;
    int bit_index;
} grib_accessor_bit;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_bit = {
    &grib_accessor_class_long,                      /* super */
    "bit",                      /* name */
    sizeof(grib_accessor_bit),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
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


grib_accessor_class* grib_accessor_class_bit = &_grib_accessor_class_bit;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_bit* ac = (grib_accessor_bit*)a;
    a->length             = 0;
    ac->owner             = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 0);
    ac->bit_index         = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 1);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_bit* ac = (grib_accessor_bit*)a;
    int ret               = 0;

    long data = 0;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_bit : unpack_long : Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), ac->owner, &data)) != GRIB_SUCCESS) {
        *len = 0;
        return ret;
    }

    if (data & (1 << ac->bit_index))
        *val = 1;
    else
        *val = 0;

    *len = 1;
    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_bit* ac = (grib_accessor_bit*)a;
    grib_accessor* owner  = NULL;
    unsigned char* mdata  = 0;
    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_bit : pack_long : At least one value to pack for %s", a->name);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    owner = grib_find_accessor(grib_handle_of_accessor(a), ac->owner);

    if (!owner) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_bit : Cannot get the owner %s for computing the bit value of %s ", ac->owner, a->name);
        *len = 0;
        return GRIB_NOT_FOUND;
    }

    mdata = grib_handle_of_accessor(a)->buffer->data;
    mdata += grib_byte_offset(owner);

    /* Note: In the definitions, flagbit numbers go from 7 to 0 (the bit_index), while WMO convention is from 1 to 8 */
    if (a->context->debug) {
        /* Print bit positions from 1 (MSB) */
        fprintf(stderr, "ECCODES DEBUG Setting bit %d in %s to %d\n", 8 - ac->bit_index, owner->name, (*val > 0) );
    }
    grib_set_bit(mdata, 7 - ac->bit_index, *val > 0);

    *len = 1;
    return GRIB_SUCCESS;
}
