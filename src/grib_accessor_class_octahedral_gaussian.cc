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
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init
   IMPLEMENTS = pack_long
   MEMBERS=const char*                  N
   MEMBERS=const char*                  Ni
   MEMBERS=const char*                  plpresent
   MEMBERS=const char*                  pl
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
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_octahedral_gaussian
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in octahedral_gaussian */
    const char*                  N;
    const char*                  Ni;
    const char*                  plpresent;
    const char*                  pl;
} grib_accessor_octahedral_gaussian;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_octahedral_gaussian = {
    &grib_accessor_class_long,                      /* super */
    "octahedral_gaussian",                      /* name */
    sizeof(grib_accessor_octahedral_gaussian),  /* size */
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


grib_accessor_class* grib_accessor_class_octahedral_gaussian = &_grib_accessor_class_octahedral_gaussian;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_octahedral_gaussian* self = (grib_accessor_octahedral_gaussian*)a;
    int n                                   = 0;
    grib_handle* hand                       = grib_handle_of_accessor(a);

    self->N         = grib_arguments_get_name(hand, c, n++);
    self->Ni        = grib_arguments_get_name(hand, c, n++);
    self->plpresent = grib_arguments_get_name(hand, c, n++);
    self->pl        = grib_arguments_get_name(hand, c, n++);
}

/* Returns 1 (=true) if input pl array is Octahedral, 0 otherwise.
 * Possible cases for the deltas in an octahedral pl array:
 *  +4 .. +4        Top part, above equator
 *  +4 .. 0         Top part, above and including equator
 *  +4.. 0  -4..    Middle part, above, equator and below
 *  0 -4..          Equator and below
 *  -4 ..-4         All below equator
 * Anything else is considered not octahedral
 */
static int is_pl_octahedral(const long pl[], size_t size)
{
    long i;
    long prev_diff = -1;
    for (i = 1; i < size; ++i) {
        const long diff = pl[i] - pl[i - 1];
        if (diff == 0) {
            /* prev must be +4 or undef */
            if (!(prev_diff == +4 || i == 1)) {
                return 0;
            }
        }
        else {
            if (labs(diff) != 4) {
                return 0;
            }
            if (diff == +4) {
                /* prev must be +4 or undef */
                if (!(prev_diff == +4 || i == 1)) {
                    return 0;
                }
            }
            if (diff == -4) {
                /* prev must be 0, -4 or undef */
                if (!(prev_diff == -4 || prev_diff == 0 || i == 1)) {
                    return 0;
                }
            }
        }
        prev_diff = diff;
    }
    return 1; /* it's octahedral */
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_octahedral_gaussian* self = (grib_accessor_octahedral_gaussian*)a;
    int ret                                 = GRIB_SUCCESS;
    long Ni;
    long plpresent    = 0;
    long* pl          = NULL; /* pl array */
    size_t plsize     = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    grib_context* c = a->context;

    if ((ret = grib_get_long_internal(hand, self->Ni, &Ni)) != GRIB_SUCCESS)
        return ret;

    /* If Ni is not missing, then this is a plain gaussian grid and not reduced. */
    /* So it cannot be an octahedral grid */
    if (Ni != GRIB_MISSING_LONG) {
        *val = 0;
        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_long_internal(hand, self->plpresent, &plpresent)) != GRIB_SUCCESS)
        return ret;
    if (!plpresent) {
        *val = 0; /* Not octahedral */
        return GRIB_SUCCESS;
    }

    if ((ret = grib_get_size(hand, self->pl, &plsize)) != GRIB_SUCCESS)
        return ret;
    Assert(plsize); /* pl array must have at least one element */

    pl = (long*)grib_context_malloc_clear(c, sizeof(long) * plsize);
    if (!pl) {
        return GRIB_OUT_OF_MEMORY;
    }
    if ((ret = grib_get_long_array_internal(hand, self->pl, pl, &plsize)) != GRIB_SUCCESS)
        return ret;

    /* pl[0] is guaranteed to exist. Have already asserted previously */
    *val = is_pl_octahedral(pl, plsize);
    grib_context_free(c, pl);

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}
