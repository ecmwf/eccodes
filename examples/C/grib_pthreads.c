/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <pthread.h>
#include "grib_api_internal.h"

#include "eccodes.h"
#define NUM_THREADS 4

/* Return 0 if numbers considered equal, otherwise 1 */
static int compare_doubles(double a, double b, double tolerance)
{
    int ret  = 0;
    double d = fabs(a - b);
    if (d > tolerance) {
        ret = 1;
    }
    return ret;
}

static void* process_grib(void* threadID)
{
    const long tid = (long)threadID;
    size_t str_len = 20, i = 0;
    long indicatorOfUnitOfTimeRange = 1, step = 0;
    char mystring[100];
    double* values    = NULL;
    size_t values_len = 0;
    double min = 0, max = 0, avg = 0;
    const double tol      = 1e-6;
    double pv[4]          = { 1, 2, 3, 4 };
    const size_t pvsize   = 4;
    ProductKind prod_kind = 0;

    codes_handle* h = codes_grib_handle_new_from_samples(0, "regular_ll_pl_grib2");
    Assert(h);
    CODES_CHECK(codes_get_product_kind(h, &prod_kind), 0);
    Assert(prod_kind == PRODUCT_GRIB);
    printf("Thread %ld running\n", tid);

    CODES_CHECK(codes_set_long(h, "indicatorOfUnitOfTimeRange", indicatorOfUnitOfTimeRange), 0);
    CODES_CHECK(codes_set_string(h, "indicatorOfUnitOfTimeRange", "s", &str_len), 0);
    CODES_CHECK(codes_set_string(h, "stepUnits", "s", &str_len), 0);
    CODES_CHECK(codes_set_long(h, "endStep", 86400), 0);
    CODES_CHECK(codes_set_long(h, "centre", 80), 0);

    CODES_CHECK(codes_get_long(h, "endStep", &step), 0);
    CODES_CHECK(codes_get_string(h, "indicatorOfUnitOfTimeRange", mystring, &str_len), 0);

    CODES_CHECK(codes_set_long(h, "PVPresent", 1), 0);
    CODES_CHECK(codes_set_double_array(h, "pv", pv, pvsize), 0);

    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);
    values = (double*)malloc(values_len * sizeof(double));
    CODES_CHECK(codes_get_double_array(h, "values", values, &values_len), 0);
    for (i = 0; i < values_len; i++) {
        if (i % 2)
            values[i] *= 0.94;
        else if (i % 3)
            values[i] *= 0.84;
    }
    GRIB_CHECK(grib_set_double_array(h, "values", values, values_len), 0);
    free(values);

    CODES_CHECK(codes_get_double(h, "min", &min), 0);
    CODES_CHECK(codes_get_double(h, "max", &max), 0);
    CODES_CHECK(codes_get_double(h, "avg", &avg), 0);
    printf("Thread %ld: min=%g max=%g avg=%g\n", tid, min, max, avg);
    Assert(compare_doubles(min, 0.84, tol) == 0);
    Assert(compare_doubles(max, 1.00, tol) == 0);
    Assert(compare_doubles(avg, 0.916774, tol) == 0);

    codes_handle_delete(h);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    pthread_t threads[NUM_THREADS];
    int error = 0;
    long i    = 0;
    for (i = 0; i < NUM_THREADS; ++i) {
        printf("Creating thread %ld\n", i);
        error = pthread_create(&threads[i], NULL, process_grib, (void*)i);
        if (error) {
            Assert(0);
            return 1;
        }
    }
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
