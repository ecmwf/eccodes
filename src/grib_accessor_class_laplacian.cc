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
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;pack_double;is_missing
   IMPLEMENTS = pack_long
   IMPLEMENTS = init
   MEMBERS=const char*    value
   MEMBERS=const char*    multiplier
   MEMBERS=const char*    divisor
   MEMBERS=const char*    truncating
   MEMBERS=const char*    computeLaplacianOperator
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int is_missing(grib_accessor*);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_laplacian
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in laplacian */
    const char*    value;
    const char*    multiplier;
    const char*    divisor;
    const char*    truncating;
    const char*    computeLaplacianOperator;
} grib_accessor_laplacian;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_laplacian = {
    &grib_accessor_class_double,                      /* super */
    "laplacian",                      /* name */
    sizeof(grib_accessor_laplacian),  /* size */
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
    &is_missing,                 /* is_missing */
    &pack_long,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
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


grib_accessor_class* grib_accessor_class_laplacian = &_grib_accessor_class_laplacian;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_laplacian* self = (grib_accessor_laplacian*)a;
    int n                         = 0;

    self->value                    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->multiplier               = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->divisor                  = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->truncating               = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->computeLaplacianOperator = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_laplacian* self = (grib_accessor_laplacian*)a;
    int ret                       = 0;
    long value                    = 0;
    long multiplier               = 0;
    long divisor                  = 0;

    if (*len < 1) {
        ret = GRIB_ARRAY_TOO_SMALL;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Accessor %s cannot gather value for %s and/or %s error %d",
                         a->name, self->multiplier, self->divisor, ret);
        return ret;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->divisor, &divisor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->multiplier, &multiplier)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->value, &value)) != GRIB_SUCCESS)
        return ret;

    if (value == GRIB_MISSING_LONG)
        *val = GRIB_MISSING_DOUBLE;
    else
        *val = ((double)(value * multiplier)) / divisor;
    /*printf("unpack_double: divisor=%ld multiplier=%ld long_value=%ld scaled_value=%.30f\n",(double)divisor,(double)multiplier,value,*val);*/

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    const double dval = (double)*val;
    return pack_double(a, &dval, len);
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_laplacian* self = (grib_accessor_laplacian*)a;
    int ret                       = 0;

    long value      = 0;
    long divisor    = 0;
    long multiplier = 0;
    long truncating = 0;
    double x;

    grib_set_long(grib_handle_of_accessor(a), self->computeLaplacianOperator, 0);

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->divisor, &divisor);
    if (ret != GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannot gather value for %s error %d", a->name, self->divisor, ret);
        return ret;
    }
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->multiplier, &multiplier);
    if (ret != GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannot gather value for %s error %d", a->name, self->divisor, ret);
        return ret;
    }
    if (self->truncating) {
        ret = grib_get_long_internal(grib_handle_of_accessor(a), self->truncating, &truncating);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannot gather value for %s error %d", a->name, self->truncating, ret);
            return ret;
        }
    }

    if (multiplier == 0) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannot divide by a zero multiplier %s error %d", a->name, self->multiplier, ret);
        return GRIB_ENCODING_ERROR;
    }

    x = *val * (double)divisor / (double)multiplier;
    if (*val == GRIB_MISSING_DOUBLE)
        value = GRIB_MISSING_LONG;
    else if (truncating) {
        value = (long)x;
    }
    else {
        value = x > 0 ? (long)(x + 0.5) : (long)(x - 0.5);
    }

    ret = grib_set_long_internal(grib_handle_of_accessor(a), self->value, value);
    if (ret)
        grib_context_log(a->context, GRIB_LOG_ERROR, "Accessor %s cannot pack value for %s error %d", a->name, self->value, ret);

    if (ret == GRIB_SUCCESS)
        *len = 1;

    return ret;
}

static int is_missing(grib_accessor* a)
{
    grib_accessor_laplacian* self = (grib_accessor_laplacian*)a;
    grib_accessor* av             = grib_find_accessor(grib_handle_of_accessor(a), self->value);

    if (!av)
        return GRIB_NOT_FOUND;
    return grib_is_missing_internal(av);

    //     int ret=0;
    //     long value=0;
    //     if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->value,
    //             &value) )   != GRIB_SUCCESS){
    //         grib_context_log(a->context, GRIB_LOG_ERROR,
    //                 "Accessor %s cannot gather value for %s error %d", a->name,
    //                 self->value, ret);
    //         return 0;
    //     }
    //     return (value == GRIB_MISSING_LONG);
}
