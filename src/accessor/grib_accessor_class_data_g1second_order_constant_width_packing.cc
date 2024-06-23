
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_g1second_order_constant_width_packing.h"
#include "grib_scaling.h"

grib_accessor_class_data_g1second_order_constant_width_packing_t _grib_accessor_class_data_g1second_order_constant_width_packing{"data_g1second_order_constant_width_packing"};
grib_accessor_class* grib_accessor_class_data_g1second_order_constant_width_packing = &_grib_accessor_class_data_g1second_order_constant_width_packing;


void grib_accessor_class_data_g1second_order_constant_width_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_data_simple_packing_t::init(a, v, args);
    grib_accessor_data_g1second_order_constant_width_packing_t* self = (grib_accessor_data_g1second_order_constant_width_packing_t*)a;
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

int grib_accessor_class_data_g1second_order_constant_width_packing_t::value_count(grib_accessor* a, long* numberOfSecondOrderPackedValues){
    int err                                                        = 0;
    grib_accessor_data_g1second_order_constant_width_packing_t* self = (grib_accessor_data_g1second_order_constant_width_packing_t*)a;
    *numberOfSecondOrderPackedValues                               = 0;

    err = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfSecondOrderPackedValues, numberOfSecondOrderPackedValues);

    return err;
}

int grib_accessor_class_data_g1second_order_constant_width_packing_t::unpack_float(grib_accessor*, float* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_data_g1second_order_constant_width_packing_t::unpack_double(grib_accessor* a, double* values, size_t* len){
    grib_accessor_data_g1second_order_constant_width_packing_t* self = (grib_accessor_data_g1second_order_constant_width_packing_t*)a;
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

    buf += a->byte_offset();
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
    if (!secondaryBitmap)
        return GRIB_OUT_OF_MEMORY;

    grib_decode_long_array(buf, &pos, 1, numberOfSecondOrderPackedValues, secondaryBitmap);
    pos = 8 * ((pos + 7) / 8);

    firstOrderValues = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfGroups);
    if (!firstOrderValues)
        return GRIB_OUT_OF_MEMORY;

    grib_decode_long_array(buf, &pos, widthOfFirstOrderValues, numberOfGroups, firstOrderValues);
    pos = 8 * ((pos + 7) / 8);

    X = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfSecondOrderPackedValues);
    if (!X)
        return GRIB_OUT_OF_MEMORY;

    if (groupWidth > 0) {
        grib_decode_long_array(buf, &pos, groupWidth, numberOfSecondOrderPackedValues, X);
        n = 0;
        i = -1;
        while (n < numberOfSecondOrderPackedValues) {
            i += secondaryBitmap[n];
            long fovi = 0;
            // ECC-1703
            if ( i >=0 && i < numberOfGroups )
                fovi = firstOrderValues[i];
            X[n] = fovi + X[n];
            n++;
        }
    }
    else {
        n = 0;
        i = -1;
        while (n < numberOfSecondOrderPackedValues) {
            i += secondaryBitmap[n];
            long fovi = 0;
            if ( i >=0 && i < numberOfGroups )
                fovi = firstOrderValues[i];
            X[n] = fovi;
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

int grib_accessor_class_data_g1second_order_constant_width_packing_t::pack_double(grib_accessor* a, const double* cval, size_t* len){
    const char* cclass_name = a->cclass->name;
    grib_context_log(a->context, GRIB_LOG_ERROR, "%s: %s: Not implemented", cclass_name, __func__);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_data_g1second_order_constant_width_packing_t::unpack_double_element(grib_accessor* a, size_t idx, double* val){
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

int grib_accessor_class_data_g1second_order_constant_width_packing_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array){
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
