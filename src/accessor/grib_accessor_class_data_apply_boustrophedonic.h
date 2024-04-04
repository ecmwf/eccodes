
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_data_apply_boustrophedonic_h
#define eccodes_accessor_data_apply_boustrophedonic_h

#include "../grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_data_apply_boustrophedonic_t : public grib_accessor_gen_t
{
public:
    /* Members defined in data_apply_boustrophedonic */
    const char*  values;
    const char*  numberOfRows;
    const char*  numberOfColumns;
    const char*  numberOfPoints;
    const char*  pl;
};

class grib_accessor_class_data_apply_boustrophedonic_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_data_apply_boustrophedonic_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_data_apply_boustrophedonic_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;

private:
    template <typename T> int unpack(grib_accessor*, T*, size_t*);
};


template <typename T>
int grib_accessor_class_data_apply_boustrophedonic_t::unpack(grib_accessor* a, T* val, size_t* len){
    grib_accessor_data_apply_boustrophedonic_t* self = (grib_accessor_data_apply_boustrophedonic_t*)a;

    size_t plSize     = 0;
    long* pl          = 0;
    double* values    = 0;
    double* pvalues   = 0;
    T* pval           = 0;
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

    ret = grib_get_size(grib_handle_of_accessor(a), self->values, &valuesSize);
    if (ret)
        return ret;

    /* constant field */
    if (valuesSize == 0)
        return 0;

    if (valuesSize != numberOfPoints) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "boustrophedonic ordering error: ( %s=%ld ) != (sizeOf(%s)=%ld)",
                         self->numberOfPoints, numberOfPoints, self->values, (long)valuesSize);
        return GRIB_DECODING_ERROR;
    }

    values = (double*)grib_context_malloc_clear(a->context, sizeof(double) * numberOfPoints);
    ret    = grib_get_double_array_internal(grib_handle_of_accessor(a), self->values, values, &valuesSize);
    if (ret)
        return ret;

    pvalues = values;
    pval    = val;

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
                pval += pl[j];
                for (i = 0; i < pl[j]; i++)
                    *(pval--) = *(pvalues++);
                pval += pl[j];
            }
            else {
                for (i = 0; i < pl[j]; i++)
                    *(pval++) = *(pvalues++);
            }
        }

        grib_context_free(a->context, pl);
    }
    else {
        for (j = 0; j < numberOfRows; j++) {
            if (j % 2) {
                pval += numberOfColumns - 1;
                for (i = 0; i < numberOfColumns; i++)
                    *(pval--) = *(pvalues++);
                pval += numberOfColumns + 1;
            }
            else {
                for (i = 0; i < numberOfColumns; i++)
                    *(pval++) = *(pvalues++);
            }
        }
    }

    grib_context_free(a->context, values);

    return GRIB_SUCCESS;
}


#endif /* eccodes_accessor_data_apply_boustrophedonic_h */
