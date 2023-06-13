/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/****************************************
 * Enrico Fucile
 *************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init
   MEMBERS = const char* ni
   MEMBERS = const char* nj
   MEMBERS = const char* plpresent
   MEMBERS = const char* pl
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_number_of_points
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in number_of_points */
    const char* ni;
    const char* nj;
    const char* plpresent;
    const char* pl;
} grib_accessor_number_of_points;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_number_of_points = {
    &grib_accessor_class_long,                      /* super */
    "number_of_points",                      /* name */
    sizeof(grib_accessor_number_of_points),  /* size */
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
    0,                  /* pack_long */
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


grib_accessor_class* grib_accessor_class_number_of_points = &_grib_accessor_class_number_of_points;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    int n                                = 0;
    grib_accessor_number_of_points* self = (grib_accessor_number_of_points*)a;
    self->ni                             = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->nj                             = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->plpresent                      = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->pl                             = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    int ret = GRIB_SUCCESS;
    long ni = 0, nj = 0, plpresent = 0;
    size_t plsize = 0;
    long* pl;
    int i;
    grib_accessor_number_of_points* self = (grib_accessor_number_of_points*)a;
    grib_context* c                      = a->context;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->ni, &ni)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->nj, &nj)) != GRIB_SUCCESS)
        return ret;

    if (self->plpresent &&
        ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->plpresent, &plpresent)) != GRIB_SUCCESS))
        return ret;

    if (nj == 0)
        return GRIB_GEOCALCULUS_PROBLEM;

    if (plpresent) {
        /*reduced*/
        plsize = nj;
        pl     = (long*)grib_context_malloc(c, sizeof(long) * plsize);
        grib_get_long_array_internal(grib_handle_of_accessor(a), self->pl, pl, &plsize);
        *val = 0;
        for (i = 0; i < plsize; i++)
            *val += pl[i];
        grib_context_free(c, pl);
    }
    else {
        /*regular*/
        *val = ni * nj;
    }

    return ret;
}
