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
#include <math.h>
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
static void init_class(grib_accessor_class*);

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
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    0,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    &is_missing,                 /* grib_pack procedures long */
    0,                  /* grib_pack procedures long */
    0,                /* grib_unpack procedures long */
    &pack_double,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    0,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_from_scale_factor_scaled_value = &_grib_accessor_class_from_scale_factor_scaled_value;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->get_native_type    =    (*(c->super))->get_native_type;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->pack_long    =    (*(c->super))->pack_long;
    c->unpack_long    =    (*(c->super))->unpack_long;
    c->pack_string    =    (*(c->super))->pack_string;
    c->unpack_string    =    (*(c->super))->unpack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    int n                                              = 0;
    grib_handle* hand                                  = grib_handle_of_accessor(a);

    self->scaleFactor = grib_arguments_get_name(hand, c, n++);
    self->scaledValue = grib_arguments_get_name(hand, c, n++); /* Can be scalar or array */

    /* ECC-979: Allow user to encode */
    /* a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY; */
}

static float float_epsilon(void)
{
    float floatEps = 1.0;
    while (1 + floatEps / 2 != 1)
        floatEps /= 2;
    return floatEps;
}

static int is_approximately_equal(double a, double b, double epsilon)
{
    if (a == b)
        return 1;
    if (fabs(a - b) <= epsilon)
        return 1;
    return 0;
}

static double eval_value_factor(int64_t value, int64_t factor)
{
    return (double)value * pow(10.0, -factor);
}

/* Return 0 if no error and set the two outputs 'value' and 'factor'
 * value cannot exceed maximum_value and factor cannot exceed maximum_factor
 */
static int get_scaled_value_and_scale_factor_algorithm1(
    double input, int64_t maximum_value, int64_t maximum_factor,
    int64_t* ret_value, int64_t* ret_factor)
{
    int64_t factor = 0;
    int64_t value = 0;

    factor = floor(log10(maximum_value)) - floor(log10(input < 0 ? -input : input));
    value = (int64_t)round(input * pow(10, factor));
    while ((value % 10 == 0) && (factor > 0)) {
        value /= 10;
        factor--;
    }
    if (value >= maximum_value)
        return GRIB_INTERNAL_ERROR;
    if (factor > maximum_factor)
        return GRIB_INTERNAL_ERROR;

    *ret_factor = factor;
    *ret_value = value;
    return GRIB_SUCCESS;
}

/* Return 0 if no error and set the two outputs 'value' and 'factor'
 * value cannot exceed maximum_value and factor cannot exceed maximum_factor
 */
static int get_scaled_value_and_scale_factor_algorithm2(
    double input, int64_t maximum_value, int64_t maximum_factor,
    int64_t* ret_value, int64_t* ret_factor)
{
    int64_t factor = 0, prev_factor = 0;
    int64_t value = 0, prev_value = 0;
    double exact        = input;
    const float epsilon = float_epsilon();
    int is_negative = 0;

    /* Loop until we find a close enough approximation. Keep the last good values */
    if (exact < 0) {
        is_negative = 1;
        exact *= -1;
    }
    factor = prev_factor = 0;
    value = prev_value = round(exact);
    while (!is_approximately_equal(exact, eval_value_factor(value, factor), epsilon) &&
           value < maximum_value &&
           factor < maximum_factor) {
        value = round(exact * pow(10., ++factor));
        if (value > maximum_value || factor > maximum_factor) {
            /* One or more maxima exceeded. So stop and use the previous values */
            value  = prev_value;
            factor = prev_factor;
            break;
        }
        prev_factor = factor;
        prev_value  = value;
    }

    if (is_negative) {
        value *= -1;
    }

    if (value == 0)
        return GRIB_INTERNAL_ERROR;

    *ret_factor = factor;
    *ret_value = value;

    return GRIB_SUCCESS;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    /* See ECC-979 and ECC-1416 */
    /* Evaluate self->scaleFactor and self->scaledValue from input double '*val' */
    grib_accessor_from_scale_factor_scaled_value* self = (grib_accessor_from_scale_factor_scaled_value*)a;
    grib_handle* hand                                  = grib_handle_of_accessor(a);
    int err                                            = 0;
    int64_t factor = 0;
    int64_t value = 0;
    double exact        = *val; /*the input*/
    int64_t maxval_value, maxval_factor; /*maximum allowable values*/
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
    maxval_value  = (1UL << value_accessor_num_bits) - 2;  /* exclude missing */
    maxval_factor = (1UL << factor_accessor_num_bits) - 2; /* exclude missing */
    if (strcmp(factor_accessor->cclass->name,"signed")==0) {
        maxval_factor = (1UL << (factor_accessor_num_bits - 1)) - 1;
    }

    err = get_scaled_value_and_scale_factor_algorithm1(exact, maxval_value, maxval_factor, &value, &factor);
    if (err) {
        err = get_scaled_value_and_scale_factor_algorithm2(exact, maxval_value, maxval_factor, &value, &factor);
        if (err) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "Failed to compute %s and %s from %g", self->scaleFactor, self->scaledValue, exact);
            return err;
        }
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
            /* ECC-966: If scale factor is missing, print error and treat it as zero (as a fallback) */
            if (grib_is_missing(hand, self->scaleFactor, &err) && err == GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                        "unpack_double for %s: %s is missing! Using zero instead", a->name, self->scaleFactor);
                scaleFactor = 0;
            }
        }

        *val = scaledValue;

        /* The formula is:
         *  real_value = scaled_value / pow(10, scale_factor)
         */
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
