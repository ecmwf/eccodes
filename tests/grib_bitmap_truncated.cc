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
 * Description: a bitmap (section 6) shorter than the number of data points must give an error.
 *              The bitmap was read beyond its end (and beyond the end of the message).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eccodes.h"

static void put_be(unsigned char* p, unsigned long v, int n)
{
    for (int i = n - 1; i >= 0; i--, v >>= 8)
        p[i] = (unsigned char)(v & 0xff);
}

static int run(const char* packing)
{
    const size_t n = 100;
    double values[100];
    size_t len = strlen(packing), i;
    long off6 = 0, len6 = 0;
    const void* msg = NULL;
    size_t msglen = 0;
    int failures = 0;

    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");
    for (i = 0; i < n; i++)
        values[i] = (i % 3 == 0) ? 9999 : (double)(i % 17);
    CODES_CHECK(codes_set_long(h, "Ni", n), 0);
    CODES_CHECK(codes_set_long(h, "Nj", 1), 0);
    CODES_CHECK(codes_set_string(h, "packingType", packing, &len), 0);
    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
    CODES_CHECK(codes_set_long(h, "bitmapPresent", 1), 0);
    CODES_CHECK(codes_set_double(h, "missingValue", 9999), 0);
    CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);
    CODES_CHECK(codes_get_message(h, &msg, &msglen), 0);
    CODES_CHECK(codes_get_long(h, "offsetSection6", &off6), 0);
    CODES_CHECK(codes_get_long(h, "section6Length", &len6), 0);

    {
        /* Keep 1 byte of the bitmap: section 6 is 5 + 1 + 1 bytes, the rest of the message follows */
        const long keep = 1, cut = len6 - 6 - keep;
        const size_t newlen = msglen - cut;
        unsigned char* t = (unsigned char*)malloc(newlen);
        double* out = (double*)malloc(n * sizeof(double));
        size_t nout = n;
        int err = 0;
        memcpy(t, msg, off6 + 6 + keep);
        memcpy(t + off6 + 6 + keep, (const unsigned char*)msg + off6 + len6, msglen - off6 - len6);
        put_be(t + 8, newlen, 8);          /* total length (section 0) */
        put_be(t + off6, 6 + keep, 4);     /* section 6 length */

        codes_handle* th = codes_handle_new_from_message(NULL, t, newlen);
        err = th ? codes_get_double_array(th, "values", out, &nout) : GRIB_INVALID_MESSAGE;
        if (err == GRIB_SUCCESS) {
            fprintf(stderr, "FAIL %s: a bitmap of %ld byte for %zu values decoded without error\n", packing, keep, n);
            failures++;
        }
        else {
            printf("%s: bitmap of %ld byte for %zu values: %s\n", packing, keep, n, codes_get_error_message(err));
        }
        if (th) {
            double v = 0;
            err = codes_get_double_element(th, "bitmap", n - 1, &v);
            if (err == GRIB_SUCCESS) {
                fprintf(stderr, "FAIL %s: bitmap element %zu read without error\n", packing, n - 1);
                failures++;
            }
        }
        free(out);
        codes_handle_delete(th);
        free(t);
    }
    codes_handle_delete(h);
    return failures;
}

int main(void)
{
    int failures = run("grid_simple");
    failures += run("grid_ccsds");
    return failures ? 1 : 0;
}
