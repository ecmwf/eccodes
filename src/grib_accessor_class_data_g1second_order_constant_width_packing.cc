/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_scaling.h"
#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double;unpack_float
   IMPLEMENTS = unpack_double_element;unpack_double_element_set
   IMPLEMENTS = value_count
   MEMBERS=const char* half_byte
   MEMBERS=const char* packingType
   MEMBERS=const char* ieee_packing
   MEMBERS=const char* precision
   MEMBERS=const char* widthOfFirstOrderValues
   MEMBERS=const char* N1
   MEMBERS=const char* N2
   MEMBERS=const char* numberOfGroups
   MEMBERS=const char* numberOfSecondOrderPackedValues
   MEMBERS=const char* extraValues
   MEMBERS=const char* pl
   MEMBERS=const char* Ni
   MEMBERS=const char* Nj
   MEMBERS=const char* jPointsAreConsecutive
   MEMBERS=const char* bitmap
   MEMBERS=const char* groupWidth

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_float(grib_accessor*, float* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
static int unpack_double_element(grib_accessor*, size_t i, double* val);
static int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array);

typedef struct grib_accessor_data_g1second_order_constant_width_packing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in values */
    int  carg;
    const char* seclen;
    const char* offsetdata;
    const char* offsetsection;
    int dirty;
    /* Members defined in data_simple_packing */
    int edition;
    const char*  units_factor;
    const char*  units_bias;
    const char*  changing_precision;
    const char*  number_of_values;
    const char*  bits_per_value;
    const char*  reference_value;
    const char*  binary_scale_factor;
    const char*  decimal_scale_factor;
    const char*  optimize_scaling_factor;
    /* Members defined in data_g1second_order_constant_width_packing */
    const char* half_byte;
    const char* packingType;
    const char* ieee_packing;
    const char* precision;
    const char* widthOfFirstOrderValues;
    const char* N1;
    const char* N2;
    const char* numberOfGroups;
    const char* numberOfSecondOrderPackedValues;
    const char* extraValues;
    const char* pl;
    const char* Ni;
    const char* Nj;
    const char* jPointsAreConsecutive;
    const char* bitmap;
    const char* groupWidth;
} grib_accessor_data_g1second_order_constant_width_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_g1second_order_constant_width_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super */
    "data_g1second_order_constant_width_packing",                      /* name */
    sizeof(grib_accessor_data_g1second_order_constant_width_packing),  /* size */
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
    0,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    &unpack_float,               /* unpack_float */
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
    &unpack_double_element,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    &unpack_double_element_set,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_g1second_order_constant_width_packing = &_grib_accessor_class_data_g1second_order_constant_width_packing;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_g1second_order_constant_width_packing* self = (grib_accessor_data_g1second_order_constant_width_packing*)a;
    grib_handle* hand                    = grib_handle_of_accessor(a);

    self->half_byte                       = grib_arguments_get_name(hand, args, self->carg++);
    self->packingType                     = grib_arguments_get_name(hand, args, self->carg++);
    self->ieee_packing                    = grib_arguments_get_name(hand, args, self->carg++);
    self->precision                       = grib_arguments_get_name(hand, args, self->carg++);
    self->widthOfFirstOrderValues         = grib_arguments_get_name(hand, args, self->carg++);
    self->N1                              = grib_arguments_get_name(hand, args, self->carg++);
    self->N2                              = grib_arguments_get_name(hand, args, self->carg++);
    self->numberOfGroups                  = grib_arguments_get_name(hand, args, self->carg++);
    self->numberOfSecondOrderPackedValues = grib_arguments_get_name(hand, args, self->carg++);
    self->extraValues                     = grib_arguments_get_name(hand, args, self->carg++);
    self->Ni                              = grib_arguments_get_name(hand, args, self->carg++);
    self->Nj                              = grib_arguments_get_name(hand, args, self->carg++);
    self->pl                              = grib_arguments_get_name(hand, args, self->carg++);
    self->jPointsAreConsecutive           = grib_arguments_get_name(hand, args, self->carg++);
    self->bitmap                          = grib_arguments_get_name(hand, args, self->carg++);
    self->groupWidth                      = grib_arguments_get_name(hand, args, self->carg++);
    self->edition                         = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a, long* numberOfSecondOrderPackedValues)
{
    int err                                                        = 0;
    grib_accessor_data_g1second_order_constant_width_packing* self = (grib_accessor_data_g1second_order_constant_width_packing*)a;
    *numberOfSecondOrderPackedValues                               = 0;

    err = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfSecondOrderPackedValues, numberOfSecondOrderPackedValues);

    return err;
}

static int unpack_float(grib_accessor*, float* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_double(grib_accessor* a, double* values, size_t* len)
{
    grib_accessor_data_g1second_order_constant_width_packing* self = (grib_accessor_data_g1second_order_constant_width_packing*)a;
    int ret                                                        = 0;
    long numberOfGroups, numberOfSecondOrderPackedValues;
    long groupWidth              = 0;
    long* firstOrderValues       = 0;
    long* X                      = 0;
    long numberPerRow            = 0;
    long pos                     = 0;
    long widthOfFirstOrderValues = 0;
    long jPointsAreConsecutive;
    unsigned char* buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    long i, n;
    double reference_value;
    long binary_scale_factor;
    long decimal_scale_factor;
    double s, d;
    long* secondaryBitmap;
    grib_handle* hand = grib_handle_of_accessor(a);

    buf += grib_byte_offset(a);

    if ((ret = grib_get_long_internal(hand, self->numberOfGroups, &numberOfGroups)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->jPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;

    if (jPointsAreConsecutive) {
        if ((ret = grib_get_long_internal(hand, self->Ni, &numberPerRow)) != GRIB_SUCCESS)
            return ret;
    }
    else {
        if ((ret = grib_get_long_internal(hand, self->Nj, &numberPerRow)) != GRIB_SUCCESS)
            return ret;
    }

    if ((ret = grib_get_long_internal(hand, self->widthOfFirstOrderValues, &widthOfFirstOrderValues)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(hand, self->numberOfSecondOrderPackedValues,
                                      &numberOfSecondOrderPackedValues)) != GRIB_SUCCESS)
        return ret;

    if (*len < numberOfSecondOrderPackedValues)
        return GRIB_ARRAY_TOO_SMALL;

    if ((ret = grib_get_long_internal(hand, self->groupWidth, &groupWidth)) != GRIB_SUCCESS)
        return ret;

    secondaryBitmap = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfSecondOrderPackedValues);
    grib_decode_long_array(buf, &pos, 1, numberOfSecondOrderPackedValues, secondaryBitmap);
    pos = 8 * ((pos + 7) / 8);

    firstOrderValues = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfGroups);
    grib_decode_long_array(buf, &pos, widthOfFirstOrderValues, numberOfGroups, firstOrderValues);
    pos = 8 * ((pos + 7) / 8);

    X = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfSecondOrderPackedValues);

    if (groupWidth > 0) {
        grib_decode_long_array(buf, &pos, groupWidth, numberOfSecondOrderPackedValues, X);
        n = 0;
        i = -1;
        while (n < numberOfSecondOrderPackedValues) {
            i += secondaryBitmap[n];
            X[n] = firstOrderValues[i] + X[n];
            n++;
        }
    }
    else {
        n = 0;
        i = -1;
        while (n < numberOfSecondOrderPackedValues) {
            i += secondaryBitmap[n];
            X[n] = firstOrderValues[i];
            n++;
        }
    }

    /*{
        long extrabits = 16 * ( (pos + 15 ) / 16) - pos;
        printf("XXXXXXX extrabits=%ld pos=%ld\n",extrabits,pos);
    }*/

    s = codes_power<double>(binary_scale_factor, 2);
    d = codes_power<double>(-decimal_scale_factor, 10);
    for (i = 0; i < numberOfSecondOrderPackedValues; i++) {
        values[i] = (double)(((X[i] * s) + reference_value) * d);
    }

    *len = numberOfSecondOrderPackedValues;
    grib_context_free(a->context, secondaryBitmap);
    grib_context_free(a->context, firstOrderValues);
    grib_context_free(a->context, X);

    return ret;
}

static int pack_double(grib_accessor* a, const double* cval, size_t* len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "constant width packing not implemented");
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    grib_handle* hand = grib_handle_of_accessor(a);
    size_t size = 0;
    double* values = NULL;
    int err = 0;

    /* TODO: This should be 'codedValues' not 'values'
       but GRIB1 version of this packing does not have that key!! */
    err = grib_get_size(hand, "values", &size);
    if (err)
        return err;
    if (idx > size)
        return GRIB_INVALID_ARGUMENT;

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(hand, "values", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    *val = values[idx];
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}

static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    grib_handle* hand = grib_handle_of_accessor(a);
    size_t size = 0, i = 0;
    double* values = NULL;
    int err = 0;

    /* TODO: This should be 'codedValues' not 'values'
       but GRIB1 version of this packing does not have that key!! */
    err = grib_get_size(hand, "values", &size);
    if (err) return err;

    for (i = 0; i < len; i++) {
        if (index_array[i] > size) return GRIB_INVALID_ARGUMENT;
    }

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(hand, "values", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    for (i = 0; i < len; i++) {
        val_array[i] = values[index_array[i]];
    }
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}
