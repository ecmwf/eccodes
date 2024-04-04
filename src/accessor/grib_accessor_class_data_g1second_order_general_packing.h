
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_g1second_order_general_packing_h
#define eccodes_accessor_data_g1second_order_general_packing_h

#include "grib_accessor_class_data_simple_packing.h"
#include "grib_scaling.h"

class grib_accessor_data_g1second_order_general_packing_t : public grib_accessor_data_simple_packing_t
{
public:
    /* Members defined in data_g1second_order_general_packing */
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
    const char* groupWidths;
};

class grib_accessor_class_data_g1second_order_general_packing_t : public grib_accessor_class_data_simple_packing_t
{
public:
    grib_accessor_class_data_g1second_order_general_packing_t(const char* name) : grib_accessor_class_data_simple_packing_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_g1second_order_general_packing_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;

private:
    template <typename T> int unpack(grib_accessor*, T*, size_t*);
};


template <typename T>
int grib_accessor_class_data_g1second_order_general_packing_t::unpack(grib_accessor* a, T* values, size_t* len){
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

#endif /* eccodes_accessor_data_g1second_order_general_packing_h */
