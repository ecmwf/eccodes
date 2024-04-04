
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
#include "grib_accessor_class_data_apply_boustrophedonic.h"

grib_accessor_class_data_apply_boustrophedonic_t _grib_accessor_class_data_apply_boustrophedonic{"data_apply_boustrophedonic"};
grib_accessor_class* grib_accessor_class_data_apply_boustrophedonic = &_grib_accessor_class_data_apply_boustrophedonic;


void grib_accessor_class_data_apply_boustrophedonic_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_gen_t::init(a, v, args);
    int n = 0;
    grib_accessor_data_apply_boustrophedonic_t* self = (grib_accessor_data_apply_boustrophedonic_t*)a;

    self->values          = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfRows    = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfColumns = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->numberOfPoints  = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->pl              = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);

    a->length = 0;
}
void grib_accessor_class_data_apply_boustrophedonic_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_values(dumper, a);
}

int grib_accessor_class_data_apply_boustrophedonic_t::value_count(grib_accessor* a, long* numberOfPoints){
    grib_accessor_data_apply_boustrophedonic_t* self = (grib_accessor_data_apply_boustrophedonic_t*)a;
    int ret = 0;

    *numberOfPoints = 0;
    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfPoints, numberOfPoints);

    return ret;
}

int grib_accessor_class_data_apply_boustrophedonic_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    return unpack<double>(a, val, len);
}

int grib_accessor_class_data_apply_boustrophedonic_t::unpack_float(grib_accessor* a, float* val, size_t* len){
    return unpack<float>(a, val, len);
}

int grib_accessor_class_data_apply_boustrophedonic_t::unpack_double_element(grib_accessor* a, size_t idx, double* val){
    size_t size;
    double* values;
    int err = 0;

    /* GRIB-564: The index idx relates to codedValues NOT values! */
    err = grib_get_size(a->parent->h, "codedValues", &size);
    if (err)
        return err;
    if (idx > size)
        return GRIB_INVALID_NEAREST;

    values = (double*)grib_context_malloc_clear(a->parent->h->context, size * sizeof(double));
    err    = grib_get_double_array(a->parent->h, "codedValues", values, &size);
    if (err) {
        grib_context_free(a->parent->h->context, values);
        return err;
    }
    *val = values[idx];
    grib_context_free(a->parent->h->context, values);
    return GRIB_SUCCESS;
}

int grib_accessor_class_data_apply_boustrophedonic_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array){
    size_t size = 0, i = 0;
    double* values;
    int err = 0;

    /* GRIB-564: The indexes in index_array relate to codedValues NOT values! */
    err = grib_get_size(grib_handle_of_accessor(a), "codedValues", &size);
    if (err)
        return err;

    for (i = 0; i < len; i++) {
        if (index_array[i] > size) return GRIB_INVALID_ARGUMENT;
    }

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(grib_handle_of_accessor(a), "codedValues", values, &size);
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

int grib_accessor_class_data_apply_boustrophedonic_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_data_apply_boustrophedonic_t* self = (grib_accessor_data_apply_boustrophedonic_t*)a;

    size_t plSize     = 0;
    long* pl          = 0;
    double* values    = 0;
    double* pvalues   = 0;
    double* pval      = 0;
    size_t valuesSize = 0;
    long i, j;
    int ret;
    long numberOfPoints, numberOfRows, numberOfColumns;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfPoints, &numberOfPoints);
    if (ret)
        return ret;

    if (*len < numberOfPoints) {
        *len = numberOfPoints;
        return GRIB_ARRAY_TOO_SMALL;
    }

    valuesSize = numberOfPoints;

    values = (double*)grib_context_malloc_clear(a->context, sizeof(double) * numberOfPoints);

    pvalues = values;
    pval    = (double*)val;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfRows, &numberOfRows);
    if (ret)
        return ret;

    ret = grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfColumns, &numberOfColumns);
    if (ret)
        return ret;

    if (grib_get_size(grib_handle_of_accessor(a), self->pl, &plSize) == GRIB_SUCCESS) {
        Assert(plSize == numberOfRows);
        pl  = (long*)grib_context_malloc_clear(a->context, sizeof(long) * plSize);
        ret = grib_get_long_array_internal(grib_handle_of_accessor(a), self->pl, pl, &plSize);
        if (ret)
            return ret;

        for (j = 0; j < numberOfRows; j++) {
            if (j % 2) {
                pvalues += pl[j];
                for (i = 0; i < pl[j]; i++) {
                    *(--pvalues) = *(pval++);
                }
                pvalues += pl[j];
            }
            else {
                for (i = 0; i < pl[j]; i++)
                    *(pvalues++) = *(pval++);
            }
        }

        grib_context_free(a->context, pl);
    }
    else {
        for (j = 0; j < numberOfRows; j++) {
            if (j % 2) {
                pvalues += numberOfColumns;
                for (i = 0; i < numberOfColumns; i++)
                    *(--pvalues) = *(pval++);
                pvalues += numberOfColumns;
            }
            else {
                for (i = 0; i < numberOfColumns; i++)
                    *(pvalues++) = *(pval++);
            }
        }
    }
    ret = grib_set_double_array_internal(grib_handle_of_accessor(a), self->values, values, valuesSize);
    if (ret)
        return ret;

    grib_context_free(a->context, values);

    return GRIB_SUCCESS;
}

int grib_accessor_class_data_apply_boustrophedonic_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_DOUBLE;
}
