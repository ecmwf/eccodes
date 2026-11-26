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
 * Description: simple packing, decoding of all values and of single values must agree, also with the
 *              new simple packing (ECCODES_USE_SP=1):
 *              - for every bitsPerValue 1..63 the values must be decoded within one quantisation step
 *                of the packed values (59, 61, 62 and 63 bits lost the high bits of the values), and
 *                codes_get_double_element(s) must give exactly the values of codes_get_double_array
 *                (the element access did not use the new simple packing)
 *              - an empty data section with bitsPerValue > 0 decodes as a constant field of the
 *                reference value, also element by element (the element access read beyond the
 *                message)
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "eccodes.h"

static int failures = 0;

static void check(int ok, const char* what, const char* sample, long bpv, size_t i)
{
    if (!ok && ++failures <= 20)
        fprintf(stderr, "FAIL: %s (%s, bitsPerValue %ld, index %zu)\n", what, sample, bpv, i);
}

static void allBitsPerValue(const char* sample)
{
    for (long bpv = 1; bpv <= 63; ++bpv) {
        codes_handle* h = codes_grib_handle_new_from_samples(NULL, sample);
        size_t n = 0;
        CODES_CHECK(codes_get_size(h, "values", &n), 0);
        double* values = (double*)malloc(n * sizeof(double));
        double* decoded = (double*)malloc(n * sizeof(double));
        unsigned long long x = 12345;
        for (size_t i = 0; i < n; ++i) {
            x = x * 6364136223846793005ULL + 1442695040888963407ULL;
            values[i] = (double)(x >> 11) / 9007199254740992.0; /* [0, 1) */
        }
        CODES_CHECK(codes_set_long(h, "bitsPerValue", bpv), 0);
        CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);
        long bsf = 0, dsf = 0;
        CODES_CHECK(codes_get_long(h, "binaryScaleFactor", &bsf), 0);
        CODES_CHECK(codes_get_long(h, "decimalScaleFactor", &dsf), 0);
        const double step = ldexp(1.0, (int)bsf) / pow(10.0, (double)dsf);

        size_t len = n;
        CODES_CHECK(codes_get_double_array(h, "values", decoded, &len), 0);
        for (size_t i = 0; i < n; ++i) {
            check(fabs(decoded[i] - values[i]) <= step, "decoded value differs by more than one step", sample, bpv, i);
            double v = 0;
            CODES_CHECK(codes_get_double_element(h, "values", i, &v), 0);
            check(v == decoded[i], "codes_get_double_element differs from codes_get_double_array", sample, bpv, i);
        }
        int idx[4] = { 0, (int)(n / 3), (int)(n / 2), (int)(n - 1) };
        double v4[4];
        CODES_CHECK(codes_get_double_elements(h, "values", idx, 4, v4), 0);
        for (int k = 0; k < 4; ++k)
            check(v4[k] == decoded[idx[k]], "codes_get_double_elements differs from codes_get_double_array", sample, bpv, (size_t)idx[k]);
        free(values);
        free(decoded);
        codes_handle_delete(h);
    }
}

static void emptyDataSection(const char* sample)
{
    codes_handle* h = codes_grib_handle_new_from_samples(NULL, sample);
    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
    long before = 0, after = 0, bpv = 0;
    double ref = 0;
    CODES_CHECK(codes_get_long(h, "offsetBeforeData", &before), 0);
    CODES_CHECK(codes_get_long(h, "offsetAfterData", &after), 0);
    CODES_CHECK(codes_get_long(h, "bitsPerValue", &bpv), 0);
    CODES_CHECK(codes_get_double(h, "referenceValue", &ref), 0);
    check(before == after && bpv == 16, "the message should have an empty data section with bitsPerValue 16", sample, bpv, 0);
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    for (size_t i = 0; i < n; ++i) {
        double v = 0;
        CODES_CHECK(codes_get_double_element(h, "values", i, &v), 0);
        check(v == ref, "element of an empty data section is not the reference value", sample, bpv, i);
    }
    codes_handle_delete(h);
}

int main(void)
{
    allBitsPerValue("GRIB2");
    allBitsPerValue("GRIB1");
    emptyDataSection("GRIB2");
    emptyDataSection("GRIB1");
    if (failures) {
        fprintf(stderr, "%d failures\n", failures);
        return 1;
    }
    printf("OK\n");
    return 0;
}
