
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_apply_bitmap_h
#define eccodes_accessor_data_apply_bitmap_h

#include "grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_data_apply_bitmap_t : public grib_accessor_gen_t
{
public:
    /* Members defined in data_apply_bitmap */
    const char*  coded_values;
    const char*  bitmap;
    const char*  missing_value;
    const char*  number_of_data_points;
    const char*  number_of_values;
    const char*  binary_scale_factor;
};

class grib_accessor_class_data_apply_bitmap_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_data_apply_bitmap_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_apply_bitmap_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int compare(grib_accessor*, grib_accessor*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;

private:
    template <typename T> int unpack(grib_accessor*, T*, size_t*);
};


template <typename T>
int grib_accessor_class_data_apply_bitmap_t::unpack(grib_accessor* a, T* val, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_accessor_data_apply_bitmap_t* self = (grib_accessor_data_apply_bitmap_t*)a;

    size_t i             = 0;
    size_t j             = 0;
    size_t n_vals        = 0;
    long nn              = 0;
    int err              = 0;
    size_t coded_n_vals  = 0;
    T* coded_vals   = NULL;
    double missing_value = 0;

    err    = a->value_count(&nn);    n_vals = nn;
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
                     __func__,
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
                                 a->name, __func__, coded_n_vals, n_vals);

                return GRIB_ARRAY_TOO_SMALL;
            }
        }
    }

    *len = n_vals;

    grib_context_free(a->context, coded_vals);
    return err;
}

#endif /* eccodes_accessor_data_apply_bitmap_h */
