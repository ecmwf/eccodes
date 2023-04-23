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
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = unpack_long;pack_long; value_count
   IMPLEMENTS = init
   MEMBERS= const char*      value
   MEMBERS= const char*      factor
   MEMBERS= const char*      divisor
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
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_times
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
    /* Members defined in times */
    const char*      value;
    const char*      factor;
    const char*      divisor;
} grib_accessor_times;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_times = {
    &grib_accessor_class_unsigned,                      /* super */
    "times",                      /* name */
    sizeof(grib_accessor_times),  /* size */
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


grib_accessor_class* grib_accessor_class_times = &_grib_accessor_class_times;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_times* self = (grib_accessor_times*)a;
    int n                     = 0;

    self->value   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->factor  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->divisor = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    a->length     = 0;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_times* self = (grib_accessor_times*)a;
    int ret                   = 0;
    long factor               = 0;
    long divisor              = 1;
    int* err                  = &ret;
    long value                = 0;

    if (*len < 1)
        return GRIB_ARRAY_TOO_SMALL;

    if (grib_is_missing(grib_handle_of_accessor(a), self->value, err) != 0) {
        *val = GRIB_MISSING_LONG;
        return GRIB_SUCCESS;
    }
    if (ret)
        return ret;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->factor, &factor);
    if (ret)
        return ret;
    if (self->divisor)
        ret = grib_get_long_internal(grib_handle_of_accessor(a), self->divisor, &divisor);
    if (ret)
        return ret;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->value, &value);
    if (ret)
        return ret;
    /* printf("factor=%ld divisor=%ld value=%ld\n",factor,divisor,value); */

    *val = ((double)value * (double)factor) / (double)divisor;

    *len = 1;

    return ret;
}


static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_times* self = (grib_accessor_times*)a;
    int ret                   = 0;
    long value                = 0;
    long factor, v, divisor = 1;

    if (*val == GRIB_MISSING_LONG)
        return grib_set_missing(grib_handle_of_accessor(a), self->value);

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->factor, &factor);
    if (ret)
        return ret;
    if (self->divisor)
        ret = grib_get_long_internal(grib_handle_of_accessor(a), self->divisor, &divisor);
    if (ret)
        return ret;

    /*Assert((*val%self->factor)==0);*/
    v = *val * divisor;
    if ((v % factor) == 0) {
        value = v / factor;
    }
    else {
        value = v > 0 ? ((double)v) / factor + 0.5 : ((double)v) / factor - 0.5;
        /* grib_context_log(a->context,GRIB_LOG_WARNING,"%s/%ld = %ld/%ld = %ld. Rounding to convert key.",a->name,self->factor,*val,self->factor,value); */
    }

    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->value, value);
    if (ret)
        return ret;

    *len = 1;

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}
