/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Latitudes.h"

eccodes::AccessorBuilder<eccodes::accessor::Latitudes> _grib_accessor_latitudes_builder{};

namespace eccodes::accessor
{

static int get_distinct(grib_accessor* a, double** val, long* len);
static int compare_doubles(const void* a, const void* b, bool ascending)
{
    const double* arg1 = (double*)a;
    const double* arg2 = (double*)b;
    if (ascending) {
        if (*arg1 < *arg2)
            return -1;  // Smaller values come before larger ones
    }
    else {
        if (*arg1 > *arg2)
            return -1;  // Larger values come before smaller ones
    }
    if (*arg1 == *arg2)
        return 0;
    else
        return 1;
}

static int compare_doubles_ascending(const void* a, const void* b)
{
    return compare_doubles(a, b, true);
}

static int compare_doubles_descending(const void* a, const void* b)
{
    return compare_doubles(a, b, false);
}

void Latitudes::init(const long l, grib_arguments* args)
{
    Double::init(l, args);
    int n = 0;

    values_   = args->get_name(get_enclosing_handle(), n++);
    distinct_ = args->get_long(get_enclosing_handle(), n++);
    save_     = 0;

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int Latitudes::unpack_double(double* val, size_t* len)
{
    int ret             = 0;
    double* v           = val;
    double dummyLon     = 0;
    size_t size         = 0;
    long count          = 0;
    grib_iterator* iter = NULL;

    save_ = 1;
    ret   = value_count(&count);
    if (ret) return ret;
    size = count;
    if (*len < size) {
        // lats_ are computed in value_count so must free
        if (lats_) {
            grib_context_free(context_, lats_);
            lats_ = NULL;
        }
        return GRIB_ARRAY_TOO_SMALL;
    }
    save_ = 0;

    // lats_ are computed in value_count
    if (lats_) {
        *len = size_;
        for (size_t i = 0; i < size; i++)
            val[i] = lats_[i];
        grib_context_free(context_, lats_);
        lats_ = NULL;
        size_ = 0;
        return GRIB_SUCCESS;
    }

    // ECC-1525 Performance: We do not need the values to be decoded
    iter = grib_iterator_new(get_enclosing_handle(), GRIB_GEOITERATOR_NO_VALUES, &ret);
    if (ret != GRIB_SUCCESS) {
        grib_iterator_delete(iter);
        grib_context_log(context_, GRIB_LOG_ERROR, "latitudes: Unable to create iterator");
        return ret;
    }

    while (grib_iterator_next(iter, v++, &dummyLon, NULL)) {}
    grib_iterator_delete(iter);

    *len = size;

    return ret;
}

int Latitudes::value_count(long* len)
{
    grib_handle* h  = get_enclosing_handle();
    double* val     = NULL;
    int ret;
    size_t size;

    *len = 0;
    if ((ret = grib_get_size(h, values_, &size)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "latitudes: Unable to get size of %s", values_);
        return ret;
    }
    *len = size;

    // See ECC-1792
    // Give priority to the Grid Section (rather than the Data Section)
    long numberOfDataPoints = 0;
    if (grib_get_long(h, "numberOfDataPoints", &numberOfDataPoints) == GRIB_SUCCESS) {
        *len = numberOfDataPoints;
    }

    if (distinct_) {
        ret = get_distinct(this, &val, len);
        if (ret != GRIB_SUCCESS)
            return ret;
        if (save_) {
            lats_ = val;
            size_ = *len;
        }
        else {
            grib_context_free(context_, val);
        }
    }

    return ret;
}

static int get_distinct(grib_accessor* a, double** val, long* len)
{
    double* v   = NULL;
    double* v1  = NULL;
    double dummyLon = 0;
    int ret = GRIB_SUCCESS;
    long jScansPositively = 0;  // default: north to south
    size_t size = *len;
    grib_context* c = a->context_;

    // Performance: We do not need the values to be decoded
    grib_iterator* iter = grib_iterator_new(a->get_enclosing_handle(), GRIB_GEOITERATOR_NO_VALUES, &ret);
    if (ret != GRIB_SUCCESS) {
        grib_iterator_delete(iter);
        grib_context_log(c, GRIB_LOG_ERROR, "latitudes: Unable to create iterator");
        return ret;
    }
    v = (double*)grib_context_malloc_clear(c, size * sizeof(double));
    if (!v) {
        grib_context_log(c, GRIB_LOG_ERROR, "latitudes: Error allocating %zu bytes", size * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    *val = v;

    while (grib_iterator_next(iter, v++, &dummyLon, NULL)) {}
    grib_iterator_delete(iter);
    v = *val;

    // See which direction the latitudes are to be scanned
    if ((ret = grib_get_long_internal(a->get_enclosing_handle(), "jScansPositively", &jScansPositively)))
        return ret;
    if (jScansPositively) {
        if (!is_sorted_ascending(v, size)) {
            qsort(v, *len, sizeof(double), &compare_doubles_ascending);  // South to North
        }
    }
    else {
        if (!is_sorted_descending(v, size)) {
            qsort(v, *len, sizeof(double), &compare_doubles_descending);  // North to South
        }
    }

    v1 = (double*)grib_context_malloc_clear(c, size * sizeof(double));
    if (!v1) {
        grib_context_log(c, GRIB_LOG_ERROR, "latitudes: Error allocating %zu bytes", size * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    // Construct a unique set of lats by filtering out duplicates
    double prev  = v[0];
    v1[0] = prev;
    long count = 1;
    for (long i = 1; i < *len; i++) {
        if (v[i] != prev) {
            prev      = v[i];
            v1[count] = prev;  // Value different from previous so store it
            count++;
        }
    }

    grib_context_free(c, v);

    *val = v1;

    *len = count;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
