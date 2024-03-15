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
#include <cmath>
#ifdef ECCODES_ON_WINDOWS
 #include <stdint.h>
#endif

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;pack_double;is_missing
   IMPLEMENTS = value_count
   IMPLEMENTS = init
   MEMBERS=const char*    scaleFactor
   MEMBERS=const char*    scaledValue
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
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_from_scale_factor_scaled_value
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in from_scale_factor_scaled_value */
    const char*    scaleFactor;
    const char*    scaledValue;
} grib_accessor_from_scale_factor_scaled_value;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_from_scale_factor_scaled_value = {
    &grib_accessor_class_double,                      /* super */
    "from_scale_factor_scaled_value",                      /* name */
    sizeof(grib_accessor_from_scale_factor_scaled_value),  /* size */
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
    &is_missing,                 /* is_missing */
    0,                  /* pack_long */
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


grib_accessor_class* grib_accessor_class_from_scale_factor_scaled_value = &_grib_accessor_class_from_scale_factor_scaled_value;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    int n                                              = 0;
    grib_handle* hand                                  = grib_handle_of_accessor(a);

    self->scaleFactor = grib_arguments_get_name(hand, c, n++);
    self->scaledValue = grib_arguments_get_name(hand, c, n++); // Can be scalar or array

    // ECC-979: Allow user to encode
    // a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    // See ECC-979 and ECC-1416
    // Evaluate self->scaleFactor and self->scaledValue from input double '*val'
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    grib_handle* hand                                  = grib_handle_of_accessor(a);
    int err                                            = 0;
    int64_t factor = 0;
    int64_t value = 0;
    double exact        = *val; // the input
    int64_t maxval_value, maxval_factor; // maximum allowable values
    int value_accessor_num_bits = 0, factor_accessor_num_bits = 0;
    grib_accessor *factor_accessor, *value_accessor;

    if (exact == 0) {
        if ((err = grib_set_long_internal(hand, self->scaleFactor, 0)) != GRIB_SUCCESS)
            return err;
        if ((err = grib_set_long_internal(hand, self->scaledValue, 0)) != GRIB_SUCCESS)
            return err;
        return GRIB_SUCCESS;
    }

    if (exact == GRIB_MISSING_DOUBLE) {
        if ((err = grib_set_missing(hand, self->scaleFactor)) != GRIB_SUCCESS)
            return err;
        if ((err = grib_set_missing(hand, self->scaledValue)) != GRIB_SUCCESS)
            return err;
        return GRIB_SUCCESS;
    }

    factor_accessor = grib_find_accessor(hand, self->scaleFactor);
    value_accessor  = grib_find_accessor(hand, self->scaledValue);
    if (!factor_accessor || !value_accessor) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Could not access keys %s and %s", self->scaleFactor, self->scaledValue);
        return GRIB_ENCODING_ERROR;
    }
    value_accessor_num_bits  = value_accessor->length * 8;
    factor_accessor_num_bits = factor_accessor->length * 8;
    maxval_value  = (1UL << value_accessor_num_bits) - 2;  // exclude missing
    maxval_factor = (1UL << factor_accessor_num_bits) - 2; // exclude missing
    if (strcmp(factor_accessor->cclass->name,"signed")==0) {
        maxval_factor = (1UL << (factor_accessor_num_bits - 1)) - 1;
    }

    err = compute_scaled_value_and_scale_factor(exact, maxval_value, maxval_factor, &value, &factor);
    if (err) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Failed to compute %s and %s from %g", self->scaleFactor, self->scaledValue, exact);
        return err;
    }

    if ((err = grib_set_long_internal(hand, self->scaleFactor, factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(hand, self->scaledValue, value)) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    int err = 0;
    long scaleFactor = 0, scaledValue = 0;
    grib_handle* hand = grib_handle_of_accessor(a);
    grib_context* c = a->context;
    size_t vsize = 0;

    if ((err = grib_get_long_internal(hand, self->scaleFactor, &scaleFactor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_size(hand, self->scaledValue, &vsize)) != GRIB_SUCCESS)
        return err;

    if (vsize == 1) {
        if ((err = grib_get_long_internal(hand, self->scaledValue, &scaledValue)) != GRIB_SUCCESS)
            return err;

        if (grib_is_missing(hand, self->scaledValue, &err) && err == GRIB_SUCCESS) {
            *val = GRIB_MISSING_DOUBLE;
            *len = 1;
            return GRIB_SUCCESS;
        } else {
            // ECC-966: If scale factor is missing, print error and treat it as zero (as a fallback)
            if (grib_is_missing(hand, self->scaleFactor, &err) && err == GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                        "unpack_double for %s: %s is missing! Using zero instead", a->name, self->scaleFactor);
                scaleFactor = 0;
            }
        }

        *val = scaledValue;

        // The formula is:
        //  real_value = scaled_value / pow(10, scale_factor)
        //
        while (scaleFactor < 0) {
            *val *= 10;
            scaleFactor++;
        }
        while (scaleFactor > 0) {
            *val /= 10;
            scaleFactor--;
        }

        if (err == GRIB_SUCCESS)
            *len = 1;
    } else {
        size_t i;
        long* lvalues = (long*)grib_context_malloc(c, vsize * sizeof(long));
        if (!lvalues)
            return GRIB_OUT_OF_MEMORY;
        if ((err = grib_get_long_array_internal(hand, self->scaledValue, lvalues, &vsize)) != GRIB_SUCCESS) {
            grib_context_free(c, lvalues);
            return err;
        }
        for (i = 0; i < vsize; i++) {
            long sf = scaleFactor;
            val[i] = lvalues[i];
            while (sf < 0) {
                val[i] *= 10;
                sf++;
            }
            while (sf > 0) {
                val[i] /= 10;
                sf--;
            }
        }
        *len = vsize;
        grib_context_free(c, lvalues);
    }

    return err;
}

static int is_missing(grib_accessor* a)
{
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    grib_handle* hand = grib_handle_of_accessor(a);
    int err = 0;
    long scaleFactor = 0, scaledValue = 0;

    if ((err = grib_get_long_internal(hand, self->scaleFactor, &scaleFactor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(hand, self->scaledValue, &scaledValue)) != GRIB_SUCCESS)
        return err;

    return ((scaleFactor == GRIB_MISSING_LONG) || (scaledValue == GRIB_MISSING_LONG));
}

static int value_count(grib_accessor* a, long* len)
{
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    int err = 0;
    grib_handle* hand = grib_handle_of_accessor(a);
    size_t vsize;

    if ((err = grib_get_size(hand, self->scaledValue, &vsize)) != GRIB_SUCCESS)
        return err;
    *len = (long)vsize;
    return GRIB_SUCCESS;
}
