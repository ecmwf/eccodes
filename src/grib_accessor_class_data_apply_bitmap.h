// ECC-1467
#pragma once

#include "grib_api_internal_cpp.h"
#include <typeinfo>
#include <type_traits>

typedef struct grib_accessor_data_apply_bitmap
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in data_apply_bitmap */
    const char*  coded_values;
    const char*  bitmap;
    const char*  missing_value;
    const char*  number_of_data_points;
    const char*  number_of_values;
    const char*  binary_scale_factor;
} grib_accessor_data_apply_bitmap;

template <typename T>
class GribAccessorDataApplyBitmap {
public:
    static int unpack(grib_accessor* a, T* val, size_t* len);
};

template <typename T>
int GribAccessorDataApplyBitmap<T>::unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;

    size_t i             = 0;
    size_t j             = 0;
    size_t n_vals        = 0;
    long nn              = 0;
    int err              = 0;
    size_t coded_n_vals  = 0;
    T* coded_vals   = NULL;
    double missing_value = 0;

    err    = grib_value_count(a, &nn);
    n_vals = nn;
    if (err)
        return err;

    if (!grib_find_accessor(grib_handle_of_accessor(a), self->bitmap))
        return grib_get_array<T>(grib_handle_of_accessor(a), self->coded_values, val, len);

    if ((err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &missing_value)) != GRIB_SUCCESS)
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

    if ((err = grib_get_array_internal<T>(grib_handle_of_accessor(a), self->bitmap, val, &n_vals)) != GRIB_SUCCESS)
        return err;

    coded_vals = (T*)grib_context_malloc(a->context, coded_n_vals * sizeof(T));
    if (coded_vals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_array<T>(grib_handle_of_accessor(a), self->coded_values, coded_vals, &coded_n_vals)) != GRIB_SUCCESS) {
        grib_context_free(a->context, coded_vals);
        return err;
    }

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_class_data_apply_bitmap: %s : creating %s, %d values",
                     __PRETTY_FUNCTION__,
                     a->name, n_vals);

    for (i = 0; i < n_vals; i++) {
        if (val[i] == 0) {
            val[i] = missing_value;
        }
        else {
            val[i] = coded_vals[j++];
            if (j > coded_n_vals) {
                grib_context_free(a->context, coded_vals);
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "grib_accessor_class_data_apply_bitmap [%s]:"
                                 " %s :  number of coded values does not match bitmap %ld %ld",
                                 a->name, __PRETTY_FUNCTION__, coded_n_vals, n_vals);

                return GRIB_ARRAY_TOO_SMALL;
            }
        }
    }

    *len = n_vals;

    grib_context_free(a->context, coded_vals);
    return err;
}

