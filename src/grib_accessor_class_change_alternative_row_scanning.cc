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
   IMPLEMENTS = pack_long;
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init;get_native_type
   MEMBERS = const char* values
   MEMBERS = const char* Ni
   MEMBERS = const char* Nj
   MEMBERS = const char* alternativeRowScanning
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
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_change_alternative_row_scanning
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in change_alternative_row_scanning */
    const char* values;
    const char* Ni;
    const char* Nj;
    const char* alternativeRowScanning;
} grib_accessor_change_alternative_row_scanning;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_change_alternative_row_scanning = {
    &grib_accessor_class_gen,                      /* super */
    "change_alternative_row_scanning",                      /* name */
    sizeof(grib_accessor_change_alternative_row_scanning),  /* size */
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


grib_accessor_class* grib_accessor_class_change_alternative_row_scanning = &_grib_accessor_class_change_alternative_row_scanning;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* args)
{
    int n                                               = 0;
    grib_accessor_change_alternative_row_scanning* self = (grib_accessor_change_alternative_row_scanning*)a;

    self->values                 = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->Ni                     = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->Nj                     = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->alternativeRowScanning = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int err = 0;
    grib_accessor_change_alternative_row_scanning* self = (grib_accessor_change_alternative_row_scanning*)a;
    grib_context* c = a->context;
    grib_handle* h  = grib_handle_of_accessor(a);
    long i, j, jr, theEnd, Ni, Nj, k, kp, alternativeRowScanning;
    size_t size = 0;
    double* values = NULL;
    double tmp = 0.0;

    if (*val == 0)
        return 0;

    /* Make sure Ni / Nj are not missing */
    if (grib_is_missing(h, self->Ni, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "change_alternative_row_scanning: Key %s cannot be 'missing'!", self->Ni);
        return GRIB_WRONG_GRID;
    }
    if (grib_is_missing(h, self->Nj, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "change_alternative_row_scanning: Key %s cannot be 'missing'!", self->Nj);
        return GRIB_WRONG_GRID;
    }

    if ((err = grib_get_long_internal(h, self->Ni, &Ni)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->Nj, &Nj)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->alternativeRowScanning, &alternativeRowScanning)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return err;

    if ( size > (size_t)(Ni * Nj) ) {
        grib_context_log(c, GRIB_LOG_ERROR, "change_alternative_row_scanning: wrong values size!=Ni*Nj (%zu!=%ld*%ld)", size, Ni, Nj);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    theEnd = Ni / 2;
    for (j = 0; j < Nj; j++) {
        jr = Ni * j;
        for (i = 0; i < theEnd; i++) {
            if (j % 2 == 1) {
                /* Swap first and last value on every odd row */
                k          = jr + i;
                kp         = jr + Ni - i - 1;
                tmp        = values[k];
                values[k]  = values[kp];
                values[kp] = tmp;
            }
        }
    }
    alternativeRowScanning = !alternativeRowScanning;
    if ((err = grib_set_long_internal(h, self->alternativeRowScanning, alternativeRowScanning)) != GRIB_SUCCESS) {
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

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int unpack_long(grib_accessor* a, long* v, size_t* len)
{
    /* Decoding this accessor doesn't make sense so we return a dummy value */
    *v = -1;
    return GRIB_SUCCESS;
}
