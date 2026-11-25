/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Description: packing a field whose range is subnormal must return, and must behave like a range
 *              that is tiny but normal. grib_get_binary_scale_fact() doubled 1 until the range did
 *              not fit into the coded values any more; for a subnormal range the required binary
 *              scale factor is below -1024, so the doubling overflowed to +inf, and the halving
 *              search that follows can never end (inf / 2 == inf): the call never returned. The
 *              doubling is also done again after the rounding step, which can overflow the same way.
 *              A complex (spectral) packed field reaches that search for any range, because it
 *              scales the values with the decimal scale factor first.
 *              The test must not hang: an alarm makes it fail instead.
 */
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "eccodes.h"

static void alarm_handler(int sig)
{
    static const char msg[] = "FAIL: packing a subnormal range of values did not return\n";
    (void)sig;
    (void)!write(2, msg, sizeof(msg) - 1);
    _exit(1);
}

static int failures = 0;

/* ldexp(1, exp) is subnormal for exp < -1022 and normal (but tiny) for -1022 <= exp < -53 */
typedef struct {
    int err;
    size_t n;
    double* values;
} result;

static result pack(const char* sample, int exp)
{
    size_t i = 0, n = 0;
    result out;
    double* values = NULL;
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, sample);

    if (!h) {
        fprintf(stderr, "Cannot create a handle from sample %s\n", sample);
        exit(1);
    }
    CODES_CHECK(codes_get_size(h, "values", &n), 0);

    values = (double*)malloc(n * sizeof(double));
    for (i = 0; i < n; i++)
        values[i] = ldexp(1.0, exp) * (double)(i % 7);

    out.err = codes_set_double_array(h, "values", values, n);
    out.n = n;
    out.values = (double*)malloc(n * sizeof(double));
    memset(out.values, 0, n * sizeof(double));

    if (out.err == 0) {
        const void* message = NULL;
        size_t size = 0;
        CODES_CHECK(codes_get_message(h, &message, &size), 0);
        codes_handle* decoded = codes_handle_new_from_message_copy(NULL, message, size);
        if (!decoded) {
            fprintf(stderr, "Cannot create a handle from the packed %s message\n", sample);
            exit(1);
        }
        size_t count = n;
        CODES_CHECK(codes_get_double_array(decoded, "values", out.values, &count), 0);
        codes_handle_delete(decoded);
    }
    else {
        /* The handle must still be usable after the error */
        long n_vals = 0;
        if (codes_get_long(h, "numberOfValues", &n_vals) != 0 || n_vals != (long)n) {
            fprintf(stderr, "FAIL %s: the handle is not usable after %s\n", sample, codes_get_error_message(out.err));
            ++failures;
        }
    }

    free(values);
    codes_handle_delete(h);
    return out;
}

static void free_result(result* r)
{
    free(r->values);
    r->values = NULL;
}

/* A subnormal range must be reported the same way as a range that is tiny but normal */
static void run(const char* sample, int all_zeros)
{
    result subnormal = pack(sample, -1070);
    result tiny = pack(sample, -900);
    size_t i = 0, n = subnormal.n;
    double max_abs = 0;

    if (subnormal.err != tiny.err) {
        fprintf(stderr, "FAIL %s: a subnormal range gives %s, a tiny normal range gives %s\n", sample,
                codes_get_error_message(subnormal.err), codes_get_error_message(tiny.err));
        ++failures;
    }

    for (i = 0; i < n; i++)
        if (fabs(subnormal.values[i]) > max_abs)
            max_abs = fabs(subnormal.values[i]);

    /* The complex packing falls back to a field of zeros when the range is too small to code */
    if (all_zeros && (subnormal.err != 0 || max_abs != 0)) {
        fprintf(stderr, "FAIL %s: a subnormal range gives %s and a maximum decoded value of %.17g, expected 0\n",
                sample, codes_get_error_message(subnormal.err), max_abs);
        ++failures;
    }

    printf("%s: %s\n", sample, subnormal.err ? codes_get_error_message(subnormal.err) : (all_zeros ? "zeros" : "packed"));

    free_result(&subnormal);
    free_result(&tiny);
}

int main(void)
{
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }
    alarm(60);
    run("sh_pl_grib1", 1); /* complex packing: too small a range becomes a field of zeros */
    run("GRIB1", 0);       /* simple packing */
    run("GRIB2", 0);       /* simple packing */
    alarm(0);
    return failures ? 1 : 0;
}
