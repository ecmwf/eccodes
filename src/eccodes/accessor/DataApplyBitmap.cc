/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataApplyBitmap.h"

eccodes::accessor::DataApplyBitmap _grib_accessor_data_apply_bitmap;
eccodes::Accessor* grib_accessor_data_apply_bitmap = &_grib_accessor_data_apply_bitmap;

namespace eccodes::accessor
{

void DataApplyBitmap::init(const long v, grib_arguments* args)
{
    Gen::init(v, args);
    int n = 0;
    grib_handle* hand = get_enclosing_handle();

    coded_values_          = args->get_name(hand, n++);
    bitmap_                = args->get_name(hand, n++);
    missing_value_         = args->get_name(hand, n++);
    binary_scale_factor_   = args->get_name(hand, n++);
    number_of_data_points_ = args->get_name(hand, n++); // can be NULL
    number_of_values_      = args->get_name(hand, n++); // can be NULL

    length_ = 0;
}
void DataApplyBitmap::dump(eccodes::Dumper* dumper)
{
    dumper->dump_values(this);
}

int DataApplyBitmap::value_count(long* count)
{
    size_t len = 0;
    int ret = GRIB_SUCCESS;
    grib_handle* hand = get_enclosing_handle();

    if (grib_find_accessor(hand, bitmap_))
        ret = grib_get_size(hand, bitmap_, &len);
    else
        ret = grib_get_size(hand, coded_values_, &len);

    *count = len;

    return ret;
}

int DataApplyBitmap::unpack_double_element(size_t idx, double* val)
{
    grib_handle* gh = get_enclosing_handle();
    size_t i = 0, cidx = 0;
    double missing_value = 0;
    double* bvals        = NULL;
    size_t n_vals        = 0;
    long nn              = 0;

    int err = value_count(&nn);
    n_vals  = nn;
    if (err)
        return err;

    if (!grib_find_accessor(gh, bitmap_))
        return grib_get_double_element_internal(gh, coded_values_, idx, val);

    if ((err = grib_get_double_internal(gh, missing_value_, &missing_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_element_internal(gh, bitmap_, idx, val)) != GRIB_SUCCESS)
        return err;

    if (*val == 0) {
        *val = missing_value;
        return GRIB_SUCCESS;
    }

    bvals = (double*)grib_context_malloc(context_, n_vals * sizeof(double));
    if (bvals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, bitmap_, bvals, &n_vals)) != GRIB_SUCCESS)
        return err;

    cidx = 0;
    for (i = 0; i < idx; i++) {
        cidx += bvals[i];
    }

    grib_context_free(context_, bvals);

    return grib_get_double_element_internal(gh, coded_values_, cidx, val);
}

int DataApplyBitmap::unpack_double_element_set(const size_t* index_array, size_t len, double* val_array)
{
    grib_handle* gh = get_enclosing_handle();
    int err = 0, all_missing = 1;
    size_t cidx          = 0;    /* index into the coded_values array */
    size_t* cidx_array   = NULL; /* array of indexes into the coded_values */
    double* cval_array   = NULL; /* array of values of the coded_values */
    double missing_value = 0;
    double* bvals        = NULL;
    size_t n_vals = 0, i = 0, j = 0, idx = 0, count_1s = 0, ci = 0;
    long nn = 0;

    err    = value_count(&nn);
    n_vals = nn;
    if (err) return err;

    if (!grib_find_accessor(gh, bitmap_))
        return grib_get_double_element_set_internal(gh, coded_values_, index_array, len, val_array);

    if ((err = grib_get_double_internal(gh, missing_value_, &missing_value)) != GRIB_SUCCESS)
        return err;

    err = grib_get_double_element_set_internal(gh, bitmap_, index_array, len, val_array);
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

    bvals = (double*)grib_context_malloc(context_, n_vals * sizeof(double));
    if (!bvals) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, bitmap_, bvals, &n_vals)) != GRIB_SUCCESS)
        return err;

    cidx_array = (size_t*)grib_context_malloc(context_, count_1s * sizeof(size_t));
    cval_array = (double*)grib_context_malloc(context_, count_1s * sizeof(double));

    ci = 0;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 1) {
            idx  = index_array[i];
            cidx = 0;
            for (j = 0; j < idx; j++) {
                cidx += bvals[j];
            }
            ECCODES_ASSERT(ci < count_1s);
            cidx_array[ci++] = cidx;
        }
    }
    err = grib_get_double_element_set_internal(gh, coded_values_, cidx_array, count_1s, cval_array);
    if (err) return err;

    /* Transfer from cval_array to our result val_array */
    ci = 0;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 1) {
            val_array[i] = cval_array[ci++];
        }
    }

    grib_context_free(context_, bvals);
    grib_context_free(context_, cidx_array);
    grib_context_free(context_, cval_array);

    return GRIB_SUCCESS;
}

int DataApplyBitmap::pack_double(const double* val, size_t* len)
{
    int err              = 0;
    size_t bmaplen       = *len;
    long coded_n_vals    = 0;
    double* coded_vals   = NULL;
    long j               = 0;
    double missing_value = 0;
    grib_handle* hand    = get_enclosing_handle();
    grib_context* ctxt   = context_;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if (!grib_find_accessor(hand, bitmap_)) {
        /*printf("SETTING TOTAL number_of_data_points %s %ld\n",number_of_data_points_ ,*len);*/
        if (number_of_data_points_)
            grib_set_long_internal(hand, number_of_data_points_, *len);

        err = grib_set_double_array_internal(hand, coded_values_, val, *len);
        return err;
    }

    if ((err = grib_get_double_internal(hand, missing_value_, &missing_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_double_array_internal(hand, bitmap_, val, bmaplen)) != GRIB_SUCCESS)
        return err;

    coded_n_vals = *len;

    if (coded_n_vals < 1) {
        err = grib_set_double_array_internal(hand, coded_values_, NULL, 0);
        return err;
    }

    coded_vals = (double*)grib_context_malloc_clear(ctxt, coded_n_vals * sizeof(double));
    if (!coded_vals)
        return GRIB_OUT_OF_MEMORY;

    for (size_t i = 0; i < *len; i++) {
        if (val[i] != missing_value) {
            coded_vals[j++] = val[i];
        }
    }

    err = grib_set_double_array_internal(hand, coded_values_, coded_vals, j);
    grib_context_free(ctxt, coded_vals);
    if (j == 0) {
        if (number_of_values_)
            err = grib_set_long_internal(hand, number_of_values_, 0);
        if (binary_scale_factor_)
            err = grib_set_long_internal(hand, binary_scale_factor_, 0);
    }

    return err;
}

template <typename T>
int DataApplyBitmap::unpack(T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");

    size_t i             = 0;
    size_t j             = 0;
    size_t n_vals        = 0;
    long nn              = 0;
    size_t coded_n_vals  = 0;
    T* coded_vals        = NULL;
    double missing_value = 0;
    grib_handle* hand    = get_enclosing_handle();

    int err = value_count(&nn);
    n_vals  = nn;
    if (err)
        return err;

    if (!grib_find_accessor(hand, bitmap_)) {
        // No bitmap
        return grib_get_array<T>(hand, coded_values_, val, len);
    }

    // There is a bitmap
    if ((err = grib_get_size(hand, coded_values_, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(hand, missing_value_, &missing_value)) != GRIB_SUCCESS)
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

    if ((err = grib_get_array_internal<T>(hand, bitmap_, val, &n_vals)) != GRIB_SUCCESS)
        return err;

    // ECC-2033
    if (coded_n_vals == n_vals && number_of_data_points_) {
        long n_data_points = 0;
        err = grib_get_long(hand, number_of_data_points_, &n_data_points);
        if (!err && n_data_points == coded_n_vals) {
            long n_missing = 0;
            if (grib_get_long(hand, "numberOfMissing", &n_missing) == GRIB_SUCCESS && n_missing > 0) {
                grib_context_log(context_, GRIB_LOG_ERROR, "Bitmap info inconsistent: %s=%ld numberOfCodedValues=%ld numberOfMissing=%ld",
                                 number_of_data_points_, n_data_points, coded_n_vals, n_missing);
            }
        }
    }

    coded_vals = (T*)grib_context_malloc(context_, coded_n_vals * sizeof(T));
    if (coded_vals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_array<T>(hand, coded_values_, coded_vals, &coded_n_vals)) != GRIB_SUCCESS) {
        grib_context_free(context_, coded_vals);
        return err;
    }

    grib_context_log(context_, GRIB_LOG_DEBUG,
                     "grib_accessor_data_apply_bitmap: %s : creating %s, %d values",
                     __func__, name_, n_vals);

    for (i = 0; i < n_vals; i++) {
        if (val[i] == 0) {
            val[i] = missing_value;
        }
        else {
            val[i] = coded_vals[j++];
            if (j > coded_n_vals) {
                grib_context_free(context_, coded_vals);
                grib_context_log(context_, GRIB_LOG_ERROR,
                                 "grib_accessor_data_apply_bitmap [%s]:"
                                 " %s :  number of coded values does not match bitmap %ld %ld",
                                 name_, __func__, coded_n_vals, n_vals);

                return GRIB_ARRAY_TOO_SMALL;
            }
        }
    }

    *len = n_vals;

    grib_context_free(context_, coded_vals);
    return err;
}

int DataApplyBitmap::unpack_double(double* val, size_t* len)
{
    return unpack<double>(val, len);
}

int DataApplyBitmap::unpack_float(float* val, size_t* len)
{
    return unpack<float>(val, len);
}

long DataApplyBitmap::get_native_type()
{
    // grib_accessor_data_apply_bitmap_t* self =  (DataApplyBitmap*)a;
    // return GetNativeype(grib_find_accessor(get_enclosing_handle(),coded_values_ ));
    return GRIB_TYPE_DOUBLE;
}

int DataApplyBitmap::compare(grib_accessor* b)
{
    int retval   = GRIB_SUCCESS;
    double* aval = 0;
    double* bval = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = value_count(&count);
    if (err)
        return err;
    alen = count;

    err = b->value_count(&count);
    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (double*)grib_context_malloc(context_, alen * sizeof(double));
    bval = (double*)grib_context_malloc(b->context_, blen * sizeof(double));

    unpack_double(aval, &alen);
    b->unpack_double(bval, &blen);
    retval = GRIB_SUCCESS;
    for (size_t i = 0; i < alen && retval == GRIB_SUCCESS; ++i) {
        if (aval[i] != bval[i]) retval = GRIB_DOUBLE_VALUE_MISMATCH;
    }

    grib_context_free(context_, aval);
    grib_context_free(b->context_, bval);

    return retval;
}

}  // namespace eccodes::accessor
