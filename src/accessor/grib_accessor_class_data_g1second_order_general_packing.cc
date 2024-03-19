
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
#include "grib_accessor_class_data_g1second_order_general_packing.h"
#include "grib_scaling.h"

grib_accessor_class_data_g1second_order_general_packing_t _grib_accessor_class_data_g1second_order_general_packing{"data_g1second_order_general_packing"};
grib_accessor_class* grib_accessor_class_data_g1second_order_general_packing = &_grib_accessor_class_data_g1second_order_general_packing;


void grib_accessor_class_data_g1second_order_general_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_data_simple_packing_t::init(a, v, args);
    grib_accessor_data_g1second_order_general_packing_t* self = (grib_accessor_data_g1second_order_general_packing_t*)a;
    grib_handle* hand                                       = grib_handle_of_accessor(a);

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
    self->groupWidths                     = grib_arguments_get_name(hand, args, self->carg++);
    self->edition                         = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_g1second_order_general_packing_t::value_count(grib_accessor* a, long* numberOfSecondOrderPackedValues){
    int err                                                 = 0;
    grib_accessor_data_g1second_order_general_packing_t* self = (grib_accessor_data_g1second_order_general_packing_t*)a;
    *numberOfSecondOrderPackedValues                        = 0;

    err = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfSecondOrderPackedValues, numberOfSecondOrderPackedValues);

    return err;
}

template <typename T>
int unpack(grib_accessor* a, T* values, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_accessor_data_g1second_order_general_packing_t* self = (grib_accessor_data_g1second_order_general_packing_t*)a;
    int ret                                                 = 0;
    long numberOfGroups, numberOfSecondOrderPackedValues;
    long* groupWidths            = 0;
    long* firstOrderValues       = 0;
    long* X                      = 0;
    long pos                     = 0;
    long widthOfFirstOrderValues = 0;
    unsigned char* buf           = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    long i, n;
    double reference_value;
    long binary_scale_factor;
    long decimal_scale_factor;
    double s, d;
    long* secondaryBitmap;
    long groupLength, j;
    size_t groupWidthsSize;

    buf += a->byte_offset();
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfGroups, &numberOfGroups)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->widthOfFirstOrderValues, &widthOfFirstOrderValues)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(grib_handle_of_accessor(a), self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfSecondOrderPackedValues,
                                      &numberOfSecondOrderPackedValues)) != GRIB_SUCCESS)
        return ret;

    if (*len < (size_t)numberOfSecondOrderPackedValues)
        return GRIB_ARRAY_TOO_SMALL;

    groupWidths     = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfGroups);
    groupWidthsSize = numberOfGroups;
    if ((ret = grib_get_long_array_internal(grib_handle_of_accessor(a), self->groupWidths, groupWidths, &groupWidthsSize)) != GRIB_SUCCESS)
        return ret;

    secondaryBitmap                                  = (long*)grib_context_malloc_clear(a->context, sizeof(long) * (numberOfSecondOrderPackedValues + 1));
    secondaryBitmap[numberOfSecondOrderPackedValues] = 1;
    grib_decode_long_array(buf, &pos, 1, numberOfSecondOrderPackedValues, secondaryBitmap);
    pos = 8 * ((pos + 7) / 8);

    firstOrderValues = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfGroups);
    grib_decode_long_array(buf, &pos, widthOfFirstOrderValues, numberOfGroups, firstOrderValues);
    pos = 8 * ((pos + 7) / 8);

    X = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfSecondOrderPackedValues);

    n           = 0;
    i           = -1;
    groupLength = 0;
    while (n < numberOfSecondOrderPackedValues) {
        if (secondaryBitmap[n]) {
            long* p     = secondaryBitmap + n + 1;
            groupLength = 1;
            while (*p != 1) {
                groupLength++;
                p++;
            }
            i++;
        }
        if (groupWidths[i] > 0) {
            for (j = 0; j < groupLength; j++) {
                X[n] = grib_decode_unsigned_long(buf, &pos, groupWidths[i]);
                X[n] = firstOrderValues[i] + X[n];
                n++;
            }
        }
        else {
            for (j = 0; j < groupLength; j++) {
                X[n] = firstOrderValues[i];
                n++;
            }
        }
    }

    s = codes_power<T>(binary_scale_factor, 2);
    d = codes_power<T>(-decimal_scale_factor, 10);
    for (i = 0; i < numberOfSecondOrderPackedValues; i++) {
        values[i] = (T)(((X[i] * s) + reference_value) * d);
    }

    *len = numberOfSecondOrderPackedValues;
    grib_context_free(a->context, secondaryBitmap);
    grib_context_free(a->context, firstOrderValues);
    grib_context_free(a->context, X);
    grib_context_free(a->context, groupWidths);

    return ret;
}

int grib_accessor_class_data_g1second_order_general_packing_t::unpack_float(grib_accessor* a, float* values, size_t* len){
    return unpack<float>(a, values, len);
}

int grib_accessor_class_data_g1second_order_general_packing_t::unpack_double(grib_accessor* a, double* values, size_t* len){
    return unpack<double>(a, values, len);
}

int grib_accessor_class_data_g1second_order_general_packing_t::pack_double(grib_accessor* a, const double* cval, size_t* len){
    /* return GRIB_NOT_IMPLEMENTED; */
    int err           = 0;
    char type[]       = "grid_second_order";
    size_t size       = strlen(type);
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_set_string(hand, "packingType", type, &size);
    if (err)
        return err;

    return grib_set_double_array(hand, "values", cval, *len);
}
