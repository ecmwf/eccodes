/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_g1second_order_row_by_row_packing.h"

grib_accessor_class_data_g1second_order_row_by_row_packing_t _grib_accessor_class_data_g1second_order_row_by_row_packing{ "data_g1second_order_row_by_row_packing" };
grib_accessor_class* grib_accessor_class_data_g1second_order_row_by_row_packing = &_grib_accessor_class_data_g1second_order_row_by_row_packing;


void grib_accessor_class_data_g1second_order_row_by_row_packing_t::init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_class_data_simple_packing_t::init(a, v, args);
    grib_accessor_data_g1second_order_row_by_row_packing_t* self = (grib_accessor_data_g1second_order_row_by_row_packing_t*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    self->half_byte                       = grib_arguments_get_name(gh, args, self->carg++);
    self->packingType                     = grib_arguments_get_name(gh, args, self->carg++);
    self->ieee_packing                    = grib_arguments_get_name(gh, args, self->carg++);
    self->precision                       = grib_arguments_get_name(gh, args, self->carg++);
    self->widthOfFirstOrderValues         = grib_arguments_get_name(gh, args, self->carg++);
    self->N1                              = grib_arguments_get_name(gh, args, self->carg++);
    self->N2                              = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfGroups                  = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfSecondOrderPackedValues = grib_arguments_get_name(gh, args, self->carg++);
    self->extraValues                     = grib_arguments_get_name(gh, args, self->carg++);
    self->Ni                              = grib_arguments_get_name(gh, args, self->carg++);
    self->Nj                              = grib_arguments_get_name(gh, args, self->carg++);
    self->pl                              = grib_arguments_get_name(gh, args, self->carg++);
    self->jPointsAreConsecutive           = grib_arguments_get_name(gh, args, self->carg++);
    self->groupWidths                     = grib_arguments_get_name(gh, args, self->carg++);
    self->bitmap                          = grib_arguments_get_name(gh, args, self->carg++);
    self->edition                         = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_g1second_order_row_by_row_packing_t* self = (grib_accessor_data_g1second_order_row_by_row_packing_t*)a;

    grib_handle* gh = grib_handle_of_accessor(a);
    long n = 0, i = 0;
    long numberOfRows = 0;
    long jPointsAreConsecutive = 0;
    long Ni = 0, Nj = 0;
    int bitmapPresent = 0;
    size_t plSize     = 0;
    long* pl          = 0;
    int ret           = 0;
    grib_context* c   = a->context;

    if (self->bitmap)
        bitmapPresent = 1;
    if ((ret = grib_get_long_internal(gh, self->jPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->Ni, &Ni)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->Nj, &Nj)) != GRIB_SUCCESS)
        return ret;
    if (jPointsAreConsecutive) {
        numberOfRows = Ni;
    }
    else {
        numberOfRows = Nj;
    }

    plSize = 0;
    ret    = grib_get_size(gh, self->pl, &plSize);
    if (ret == GRIB_SUCCESS) {
        pl = (long*)grib_context_malloc_clear(a->context, sizeof(long) * plSize);
        if ((ret = grib_get_long_array(gh, self->pl, pl, &plSize)) != GRIB_SUCCESS)
            return ret;
    }
    ret = 0;

    n = 0;
    if (bitmapPresent) {
        long *bitmap, *pbitmap;
        size_t numberOfPoints = 0;

        if (plSize && pl) {
            for (i = 0; i < numberOfRows; i++)
                numberOfPoints += pl[i];
            grib_context_free(c, pl);
        }
        else {
            numberOfPoints = Ni * Nj;
        }
        bitmap  = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfPoints);
        pbitmap = bitmap;
        grib_get_long_array(gh, self->bitmap, bitmap, &numberOfPoints);
        for (i = 0; i < numberOfPoints; i++)
            n += *(bitmap++);

        grib_context_free(a->context, pbitmap);
    }
    else {
        if (plSize) {
            if (numberOfRows && !pl) return GRIB_INTERNAL_ERROR;
            for (i = 0; i < numberOfRows; i++)
                n += pl[i];
            grib_context_free(c, pl);
        }
        else {
            n = Ni * Nj;
        }
    }

    *count = n;
    return ret;
}

template <typename T>
static int unpack_real(grib_accessor* a, T* values, size_t* len)
{
    grib_accessor_data_g1second_order_row_by_row_packing_t* self = (grib_accessor_data_g1second_order_row_by_row_packing_t*)a;

    grib_handle* gh = grib_handle_of_accessor(a);
    int ret         = 0;
    long numberOfGroups, numberOfSecondOrderPackedValues;
    long* groupWidths      = 0;
    long* firstOrderValues = 0;
    long* X                = 0;
    long numberOfRows, numberOfColumns;
    long* numbersPerRow;
    long pos                     = 0;
    long widthOfFirstOrderValues = 0;
    long jPointsAreConsecutive;
    unsigned char* buf = (unsigned char*)gh->buffer->data;
    long k, i, j, n, Ni, Nj;
    double reference_value;
    long binary_scale_factor;
    long decimal_scale_factor;
    double s, d;
    size_t groupWidthsSize = 0;
    int bitmapPresent      = 0;
    size_t plSize          = 0;
    long* pl               = 0;

    buf += a->byte_offset();
    if ((ret = grib_get_long_internal(gh, self->numberOfGroups, &numberOfGroups)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(gh, self->jPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;

    if (self->bitmap)
        bitmapPresent = 1;
    ret = grib_get_size(gh, self->pl, &plSize);
    if (ret == GRIB_SUCCESS) {
        pl = (long*)grib_context_malloc_clear(a->context, sizeof(long) * plSize);
        if ((ret = grib_get_long_array(gh, self->pl, pl, &plSize)) != GRIB_SUCCESS)
            return ret;
    }

    if ((ret = grib_get_long_internal(gh, self->Ni, &Ni)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->Nj, &Nj)) != GRIB_SUCCESS)
        return ret;
    if (jPointsAreConsecutive) {
        numberOfRows    = Ni;
        numberOfColumns = Nj;
    }
    else {
        numberOfRows    = Nj;
        numberOfColumns = Ni;
    }

    numbersPerRow = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfRows);
    if (!numbersPerRow)
        return GRIB_OUT_OF_MEMORY;
    if (bitmapPresent) {
        long *bitmap, *pbitmap;
        size_t numberOfPoints = Ni * Nj;

        if (plSize && pl) {
            numberOfPoints = 0;
            for (i = 0; i < numberOfRows; i++)
                numberOfPoints += pl[i];
        }
        bitmap  = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfPoints);
        pbitmap = bitmap;
        grib_get_long_array(gh, self->bitmap, bitmap, &numberOfPoints);
        if (plSize && pl) {
            for (i = 0; i < numberOfRows; i++) {
                for (j = 0; j < pl[i]; j++) {
                    numbersPerRow[i] += *(bitmap++);
                }
            }
        }
        else {
            for (i = 0; i < numberOfRows; i++) {
                numbersPerRow[i] = 0;
                for (j = 0; j < Ni; j++) {
                    numbersPerRow[i] += *(bitmap++);
                }
            }
        }

        grib_context_free(a->context, pbitmap);
    }
    else {
        if (plSize && pl) {
            for (i = 0; i < numberOfRows; i++)
                numbersPerRow[i] = pl[i];
        }
        else {
            for (i = 0; i < numberOfRows; i++)
                numbersPerRow[i] = numberOfColumns;
        }
    }

    if ((ret = grib_get_long_internal(gh, self->widthOfFirstOrderValues, &widthOfFirstOrderValues)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(gh, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(gh, self->numberOfSecondOrderPackedValues,
                                      &numberOfSecondOrderPackedValues)) != GRIB_SUCCESS)
        return ret;

    groupWidths     = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfGroups);
    groupWidthsSize = numberOfGroups;
    if ((ret = grib_get_long_array_internal(gh, self->groupWidths, groupWidths, &groupWidthsSize)) != GRIB_SUCCESS)
        return ret;

    firstOrderValues = (long*)grib_context_malloc_clear(a->context, sizeof(long) * numberOfGroups);
    grib_decode_long_array(buf, &pos, widthOfFirstOrderValues, numberOfGroups, firstOrderValues);
    pos = 8 * ((pos + 7) / 8);

    n = 0;
    for (i = 0; i < numberOfGroups; i++)
        n += numbersPerRow[i];

    if (*len < (size_t)n)
        return GRIB_ARRAY_TOO_SMALL;

    X = (long*)grib_context_malloc_clear(a->context, sizeof(long) * n);
    n = 0;
    k = 0;
    for (i = 0; i < numberOfGroups; i++) {
        if (groupWidths[i] > 0) {
            for (j = 0; j < numbersPerRow[k]; j++) {
                X[n] = grib_decode_unsigned_long(buf, &pos, groupWidths[i]);
                X[n] += firstOrderValues[i];
                n++;
            }
        }
        else {
            for (j = 0; j < numbersPerRow[k]; j++) {
                X[n] = firstOrderValues[i];
                n++;
            }
        }
        k++;
    }

    s = codes_power<T>(binary_scale_factor, 2);
    d = codes_power<T>(-decimal_scale_factor, 10);
    for (i = 0; i < n; i++) {
        values[i] = (T)(((X[i] * s) + reference_value) * d);
    }
    grib_context_free(a->context, firstOrderValues);
    grib_context_free(a->context, X);
    grib_context_free(a->context, groupWidths);
    if (plSize)
        grib_context_free(a->context, pl);
    if (numbersPerRow)
        grib_context_free(a->context, numbersPerRow);

    return ret;
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::unpack_float(grib_accessor* a, float* values, size_t* len)
{
    return unpack_real<float>(a, values, len);
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::unpack_double(grib_accessor* a, double* values, size_t* len)
{
    return unpack_real<double>(a, values, len);
}

int grib_accessor_class_data_g1second_order_row_by_row_packing_t::pack_double(grib_accessor* a, const double* cval, size_t* len)
{
    int err         = 0;
    grib_handle* gh = grib_handle_of_accessor(a);
    char type[]     = "grid_second_order";
    size_t size     = strlen(type);

    err = grib_set_string(gh, "packingType", type, &size);
    if (err)
        return err;

    return grib_set_double_array(gh, "values", cval, *len);
}
