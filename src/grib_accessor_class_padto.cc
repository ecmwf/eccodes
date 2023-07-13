/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_padding
   IMPLEMENTS = init;preferred_size;dump
   MEMBERS=grib_expression* expression
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static size_t preferred_size(grib_accessor*, int);

typedef struct grib_accessor_padto
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in bytes */
    /* Members defined in padding */
    /* Members defined in padto */
    grib_expression* expression;
} grib_accessor_padto;

extern grib_accessor_class* grib_accessor_class_padding;

static grib_accessor_class _grib_accessor_class_padto = {
    &grib_accessor_class_padding,                      /* super */
    "padto",                      /* name */
    sizeof(grib_accessor_padto),  /* size */
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
    0,                  /* pack_long */
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
    &preferred_size,             /* preferred_size */
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


grib_accessor_class* grib_accessor_class_padto = &_grib_accessor_class_padto;

/* END_CLASS_IMP */


static size_t preferred_size(grib_accessor* a, int from_handle)
{
    grib_accessor_padto* self = (grib_accessor_padto*)a;

    long length = 0;
    long theEnd;

    grib_expression_evaluate_long(grib_handle_of_accessor(a), self->expression, &theEnd);

    length = theEnd - a->offset;

    /* printf("preferred_size: prefered: %ld current:%ld %s %s %ld\n", (long)length,(long)a->length,a->cclass->name,a->name,(long)a->offset); */

    return length > 0 ? length : 0;
}


static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_padto* self = (grib_accessor_padto*)a;

    self->expression = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, 0);
    a->length        = preferred_size(a, 1);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    /*grib_dump_string(dumper,a,NULL);*/
}
