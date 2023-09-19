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
   MEMBERS = const char* i_scans_negatively
   MEMBERS = const char* j_scans_positively
   MEMBERS = const char* first
   MEMBERS = const char* last
   MEMBERS = const char* axis
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

typedef struct grib_accessor_change_scanning_direction
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in change_scanning_direction */
    const char* values;
    const char* Ni;
    const char* Nj;
    const char* i_scans_negatively;
    const char* j_scans_positively;
    const char* first;
    const char* last;
    const char* axis;
} grib_accessor_change_scanning_direction;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_change_scanning_direction = {
    &grib_accessor_class_gen,                      /* super */
    "change_scanning_direction",                      /* name */
    sizeof(grib_accessor_change_scanning_direction),  /* size */
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


grib_accessor_class* grib_accessor_class_change_scanning_direction = &_grib_accessor_class_change_scanning_direction;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* args)
{
    int n                                         = 0;
    grib_accessor_change_scanning_direction* self = (grib_accessor_change_scanning_direction*)a;

    self->values             = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->Ni                 = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->Nj                 = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->i_scans_negatively = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->j_scans_positively = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->first              = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->last               = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->axis               = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int err = 0;
    long i, j, jr, theEnd, Ni, Nj, k, kp;
    double tmp;
    long iScansNegatively                         = 0;
    long jScansPositively                         = 0;
    double first                                  = 0;
    double last                                   = 0;
    size_t size                                   = 0;
    double* values                                = NULL;
    grib_accessor_change_scanning_direction* self = (grib_accessor_change_scanning_direction*)a;
    grib_context* c                               = a->context;
    grib_handle* h                                = grib_handle_of_accessor(a);
    const char* cclass_name                       = a->cclass->name;

    if (*val == 0)
        return 0;

    /* Make sure Ni / Nj are not missing */
    if (grib_is_missing(h, self->Ni, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing'!", cclass_name, self->Ni);
        return GRIB_WRONG_GRID;
    }
    if (grib_is_missing(h, self->Nj, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing'!", cclass_name, self->Nj);
        return GRIB_WRONG_GRID;
    }

    if ((err = grib_get_long_internal(h, self->Ni, &Ni)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->Nj, &Nj)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(h, self->i_scans_negatively, &iScansNegatively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->j_scans_positively, &jScansPositively)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(h, self->first, &first)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, self->last, &last)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return err;

    if (size > Ni * Nj) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Wrong values size!=Ni*Nj (%ld!=%ld*%ld)", cclass_name, size, Ni, Nj);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    Assert(self->axis);
    Assert(strcmp(self->axis, "x") == 0 || strcmp(self->axis, "y") == 0);

    if (self->axis[0] == 'x') {
        theEnd = Ni / 2;
        for (j = 0; j < Nj; j++) {
            jr = Ni * j;
            for (i = 0; i < theEnd; i++) {
                k          = jr + i;
                kp         = jr + Ni - i - 1;
                tmp        = values[k];
                values[k]  = values[kp];
                values[kp] = tmp;
            }
        }
        iScansNegatively = !iScansNegatively;
        if ((err = grib_set_long_internal(h, self->i_scans_negatively, iScansNegatively)) != GRIB_SUCCESS)
            return err;
    }
    else {
        long kpj;
        theEnd = Nj / 2;
        for (i = 0; i < Ni; i++) {
            kpj = Ni * (Nj - 1);
            for (j = 0; j < theEnd; j++) {
                k          = Ni * j + i;
                kp         = kpj - Ni * j + i;
                tmp        = values[k];
                values[k]  = values[kp];
                values[kp] = tmp;
            }
        }
        jScansPositively = !jScansPositively;
        if ((err = grib_set_long_internal(h, self->j_scans_positively, jScansPositively)) != GRIB_SUCCESS)
            return err;
    }

    if ((err = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_double_internal(h, self->first, last)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_double_internal(h, self->last, first)) != GRIB_SUCCESS)
        return err;

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int unpack_long(grib_accessor* a, long* v, size_t* len)
{
    /* ECC-976: decoding this accessor doesn't make sense so we return a dummy value */
    *v = -1;
    return GRIB_SUCCESS;
}
