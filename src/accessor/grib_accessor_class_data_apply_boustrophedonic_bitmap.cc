/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_apply_boustrophedonic_bitmap.h"

grib_accessor_class_data_apply_boustrophedonic_bitmap_t _grib_accessor_class_data_apply_boustrophedonic_bitmap{ "data_apply_boustrophedonic_bitmap" };
grib_accessor_class* grib_accessor_class_data_apply_boustrophedonic_bitmap = &_grib_accessor_class_data_apply_boustrophedonic_bitmap;


void grib_accessor_class_data_apply_boustrophedonic_bitmap_t::init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_class_gen_t::init(a, v, args);
    grib_accessor_data_apply_boustrophedonic_bitmap_t* self = (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;
    grib_handle* gh                                         = grib_handle_of_accessor(a);

    int n = 0;
    self->coded_values        = grib_arguments_get_name(gh, args, n++);
    self->bitmap              = grib_arguments_get_name(gh, args, n++);
    self->missing_value       = grib_arguments_get_name(gh, args, n++);
    self->binary_scale_factor = grib_arguments_get_name(gh, args, n++);

    self->numberOfRows    = grib_arguments_get_name(gh, args, n++);
    self->numberOfColumns = grib_arguments_get_name(gh, args, n++);
    self->numberOfPoints  = grib_arguments_get_name(gh, args, n++);

    a->length = 0;
}

void grib_accessor_class_data_apply_boustrophedonic_bitmap_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper, a);
}

int grib_accessor_class_data_apply_boustrophedonic_bitmap_t::value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_apply_boustrophedonic_bitmap_t* self = (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;

    grib_handle* gh = grib_handle_of_accessor(a);
    size_t len      = 0;
    int ret         = 0;

    /* This accessor is for data with a bitmap after all */
    Assert(grib_find_accessor(gh, self->bitmap));

    ret    = grib_get_size(gh, self->bitmap, &len);
    *count = len;
    return ret;
}

int grib_accessor_class_data_apply_boustrophedonic_bitmap_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_data_apply_boustrophedonic_bitmap_t* self = (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;
    grib_handle* gh                                         = grib_handle_of_accessor(a);

    size_t i = 0, j = 0, n_vals = 0, irow = 0;
    long nn              = 0;
    int err              = 0;
    size_t coded_n_vals  = 0;
    double* coded_vals   = NULL;
    double missing_value = 0;
    long numberOfPoints, numberOfRows, numberOfColumns;

    err    = a->value_count(&nn);
    n_vals = nn;
    if (err)
        return err;

    err = grib_get_long_internal(gh, self->numberOfRows, &numberOfRows);
    if (err)
        return err;
    err = grib_get_long_internal(gh, self->numberOfColumns, &numberOfColumns);
    if (err)
        return err;
    err = grib_get_long_internal(gh, self->numberOfPoints, &numberOfPoints);
    if (err)
        return err;
    Assert(nn == numberOfPoints);

    if (!grib_find_accessor(gh, self->bitmap))
        return grib_get_double_array_internal(gh, self->coded_values, val, len);

    if ((err = grib_get_size(gh, self->coded_values, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(gh, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (coded_n_vals == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = missing_value;

        *len = n_vals;
        return GRIB_SUCCESS;
    }

    if ((err = grib_get_double_array_internal(gh, self->bitmap, val, &n_vals)) != GRIB_SUCCESS)
        return err;

    coded_vals = (double*)grib_context_malloc(a->context, coded_n_vals * sizeof(double));
    if (coded_vals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, self->coded_values, coded_vals, &coded_n_vals)) != GRIB_SUCCESS) {
        grib_context_free(a->context, coded_vals);
        return err;
    }

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_class_data_apply_boustrophedonic_bitmap: unpack_double : creating %s, %d values",
                     a->name, n_vals);

    /* Boustrophedonic ordering (See GRIB-472):
     * Values on even rank lines (the initial line scanned having rank 1) are swapped
     */
    for (irow = 0; irow < numberOfRows; ++irow) {
        if (irow % 2) {
            /* Reverse bitmap entries */
            size_t k     = 0;
            size_t start = irow * numberOfColumns;
            size_t end   = start + numberOfColumns - 1;
            size_t mid   = (numberOfColumns - 1) / 2;
            for (k = 0; k < mid; ++k) {
                /* Swap value at either end */
                double temp    = val[start + k];
                val[start + k] = val[end - k];
                val[end - k]   = temp;
            }
        }
    }

    for (i = 0; i < n_vals; i++) {
        if (val[i] == 0) {
            val[i] = missing_value;
        }
        else {
            val[i] = coded_vals[j++];
            if (j > coded_n_vals) {
                grib_context_free(a->context, coded_vals);
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "grib_accessor_class_data_apply_boustrophedonic_bitmap [%s]:"
                                 " unpack_double :  number of coded values does not match bitmap %ld %ld",
                                 a->name, coded_n_vals, n_vals);

                return GRIB_ARRAY_TOO_SMALL;
            }
        }
    }

    *len = n_vals;

    grib_context_free(a->context, coded_vals);
    return err;
}

int grib_accessor_class_data_apply_boustrophedonic_bitmap_t::unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    grib_accessor_data_apply_boustrophedonic_bitmap_t* self = (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;
    grib_handle* gh                                         = grib_handle_of_accessor(a);
    int err = 0, i = 0;
    size_t cidx          = 0;
    double missing_value = 0;
    double* bvals        = NULL;
    size_t n_vals        = 0;
    long nn              = 0;

    err    = a->value_count(&nn);
    n_vals = nn;
    if (err)
        return err;

    if (!grib_find_accessor(gh, self->bitmap))
        return grib_get_double_element_internal(gh, self->coded_values, idx, val);

    if ((err = grib_get_double_internal(gh, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_element_internal(gh, self->bitmap, idx, val)) != GRIB_SUCCESS)
        return err;

    if (*val == 0) {
        *val = missing_value;
        return GRIB_SUCCESS;
    }

    bvals = (double*)grib_context_malloc(a->context, n_vals * sizeof(double));
    if (bvals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, self->bitmap, bvals, &n_vals)) != GRIB_SUCCESS)
        return err;

    cidx = 0;
    for (i = 0; i < idx; i++) {
        cidx += bvals[i];
    }

    grib_context_free(a->context, bvals);

    return grib_get_double_element_internal(gh, self->coded_values, cidx, val);
}

int grib_accessor_class_data_apply_boustrophedonic_bitmap_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    grib_accessor_data_apply_boustrophedonic_bitmap_t* self = (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;
    grib_handle* gh                                         = grib_handle_of_accessor(a);
    int err = 0, all_missing = 1;
    size_t cidx          = 0;    /* index into the coded_values array */
    size_t* cidx_array   = NULL; /* array of indexes into the coded_values */
    double* cval_array   = NULL; /* array of values of the coded_values */
    double missing_value = 0;
    double* bvals        = NULL;
    size_t n_vals = 0, i = 0, j = 0, idx = 0, count_1s = 0, ci = 0;
    long nn = 0;

    err    = a->value_count(&nn);
    n_vals = nn;
    if (err) return err;

    if (!grib_find_accessor(gh, self->bitmap))
        return grib_get_double_element_set_internal(gh, self->coded_values, index_array, len, val_array);

    if ((err = grib_get_double_internal(gh, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    err = grib_get_double_element_set_internal(gh, self->bitmap, index_array, len, val_array);
    if (err) return err;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 0) {
            val_array[i] = missing_value;
        }
        else {
            all_missing = 0;
            count_1s++;
        }
    }

    if (all_missing) {
        return GRIB_SUCCESS;
    }

    /* At this point val_array contains entries which are either missing_value or 1 */
    /* Now we need to dig into the codes values with index array of count_1s */

    bvals = (double*)grib_context_malloc(a->context, n_vals * sizeof(double));
    if (!bvals) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, self->bitmap, bvals, &n_vals)) != GRIB_SUCCESS)
        return err;

    cidx_array = (size_t*)grib_context_malloc(a->context, count_1s * sizeof(size_t));
    cval_array = (double*)grib_context_malloc(a->context, count_1s * sizeof(double));

    ci = 0;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 1) {
            idx  = index_array[i];
            cidx = 0;
            for (j = 0; j < idx; j++) {
                cidx += bvals[j];
            }
            Assert(ci < count_1s);
            cidx_array[ci++] = cidx;
        }
    }
    err = grib_get_double_element_set_internal(gh, self->coded_values, cidx_array, count_1s, cval_array);
    if (err) return err;

    /* Transfer from cval_array to our result val_array */
    ci = 0;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 1) {
            val_array[i] = cval_array[ci++];
        }
    }

    grib_context_free(a->context, bvals);
    grib_context_free(a->context, cidx_array);
    grib_context_free(a->context, cval_array);

    return GRIB_SUCCESS;
}

int grib_accessor_class_data_apply_boustrophedonic_bitmap_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_apply_boustrophedonic_bitmap_t* self = (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;

    grib_handle* gh    = grib_handle_of_accessor(a);
    int err            = 0;
    size_t bmaplen     = *len;
    size_t irow        = 0;
    long coded_n_vals  = 0;
    double* coded_vals = NULL;
    double* values     = 0;
    long i             = 0;
    long j             = 0;
    long numberOfPoints, numberOfRows, numberOfColumns;
    double missing_value = 0;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if (!grib_find_accessor(gh, self->bitmap)) {
        err = grib_set_double_array_internal(gh, self->coded_values, val, *len);
        /*printf("SETTING TOTAL number_of_data_points %s %ld\n",self->number_of_data_points,*len);*/
        /*if(self->number_of_data_points)
            grib_set_long_internal(gh,self->number_of_data_points,*len);*/
        return err;
    }

    if ((err = grib_get_double_internal(gh, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    err = grib_get_long_internal(gh, self->numberOfRows, &numberOfRows);
    if (err)
        return err;
    err = grib_get_long_internal(gh, self->numberOfColumns, &numberOfColumns);
    if (err)
        return err;
    err = grib_get_long_internal(gh, self->numberOfPoints, &numberOfPoints);
    if (err)
        return err;
    Assert(numberOfPoints == bmaplen);

    /* Create a copy of the incoming 'val' array because we're going to change it */
    values = (double*)grib_context_malloc_clear(a->context, sizeof(double) * numberOfPoints);
    if (!values)
        return GRIB_OUT_OF_MEMORY;
    for (i = 0; i < numberOfPoints; ++i) {
        values[i] = val[i];
    }

    /* Boustrophedonic ordering must be applied to the bitmap (See GRIB-472) */
    for (irow = 0; irow < numberOfRows; ++irow) {
        if (irow % 2) {
            size_t k     = 0;
            size_t start = irow * numberOfColumns;
            size_t end   = start + numberOfColumns - 1;
            size_t mid   = (numberOfColumns - 1) / 2;
            for (k = 0; k < mid; ++k) {
                double temp       = values[start + k];
                values[start + k] = values[end - k];
                values[end - k]   = temp;
            }
        }
    }
    /* Now set the bitmap based on the array with the boustrophedonic ordering */
    if ((err = grib_set_double_array_internal(gh, self->bitmap, values, bmaplen)) != GRIB_SUCCESS)
        return err;

    grib_context_free(a->context, values);

    coded_n_vals = *len;

    if (coded_n_vals < 1) {
        err = grib_set_double_array_internal(gh, self->coded_values, NULL, 0);
        return err;
    }

    coded_vals = (double*)grib_context_malloc_clear(a->context, coded_n_vals * sizeof(double));
    if (!coded_vals)
        return GRIB_OUT_OF_MEMORY;

    for (i = 0; i < *len; i++) {
        /* To set the coded values, look at 'val' (the original array) */
        /* NOT 'values' (bitmap) which we swapped about */
        if (val[i] != missing_value) {
            coded_vals[j++] = val[i];
        }
    }

    err = grib_set_double_array_internal(gh, self->coded_values, coded_vals, j);
    if (j == 0) {
        /*if (self->number_of_values)
            err=grib_set_long_internal(gh,self->number_of_values,0);*/
        if (self->binary_scale_factor)
            err = grib_set_long_internal(gh, self->binary_scale_factor, 0);
    }

    grib_context_free(a->context, coded_vals);

    return err;
}

int grib_accessor_class_data_apply_boustrophedonic_bitmap_t::get_native_type(grib_accessor* a)
{
    // grib_accessor_data_apply_boustrophedonic_bitmap_t* self =  (grib_accessor_data_apply_boustrophedonic_bitmap_t*)a;
    // return grib_accessor_get_native_type(grib_find_accessor(grib_handle_of_accessor(a),self->coded_values));

    return GRIB_TYPE_DOUBLE;
}
