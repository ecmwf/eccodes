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
 * Description: decoding a CCSDS message whose data section is truncated must fail. libaec does
 *              not report truncated input, and the values that were not decoded silently came
 *              out as the reference value.
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

int main(void)
{
    const size_t n = 5000;
    double values[5000];
    size_t len = 10, i;
    long off7 = 0, len7 = 0;
    const void* msg = NULL;
    size_t msglen = 0;
    int failures = 0;

    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");
    for (i = 0; i < n; i++)
        values[i] = (double)((i * 37) % 1000);
    CODES_CHECK(codes_set_long(h, "Ni", n), 0);
    CODES_CHECK(codes_set_long(h, "Nj", 1), 0);
    CODES_CHECK(codes_set_string(h, "packingType", "grid_ccsds", &len), 0);
    CODES_CHECK(codes_set_long(h, "bitsPerValue", 16), 0);
    CODES_CHECK(codes_set_double_array(h, "values", values, n), 0);
    CODES_CHECK(codes_get_message(h, &msg, &msglen), 0);
    CODES_CHECK(codes_get_long(h, "offsetSection7", &off7), 0);
    CODES_CHECK(codes_get_long(h, "section7Length", &len7), 0);

    {
        /* Keep 1 byte and half of the compressed data */
        const long keeps[] = { 1, (len7 - 5) / 2 };
        for (int k = 0; k < 2; k++) {
            const size_t newlen = off7 + 5 + keeps[k] + 4;
            unsigned char* t = (unsigned char*)malloc(newlen);
            memcpy(t, msg, off7 + 5 + keeps[k]);
            memcpy(t + off7 + 5 + keeps[k], "7777", 4);
            put_be(t + 8, newlen, 8);          /* total length (section 0) */
            put_be(t + off7, 5 + keeps[k], 4); /* section 7 length */

            codes_handle* t_h = codes_handle_new_from_message(NULL, t, newlen);
            double* out = (double*)malloc(n * sizeof(double));
            size_t nout = n;
            int err = t_h ? codes_get_double_array(t_h, "values", out, &nout) : GRIB_INVALID_MESSAGE;
            if (err == GRIB_SUCCESS) {
                fprintf(stderr, "FAIL: data section truncated to %ld bytes decoded without error (value[%zu]=%g, packed %g)\n",
                        keeps[k], n - 1, out[n - 1], values[n - 1]);
                failures++;
            }
            else {
                printf("data section truncated to %ld bytes: %s\n", keeps[k], codes_get_error_message(err));
            }
            free(out);
            codes_handle_delete(t_h);
            free(t);
        }
    }
    codes_handle_delete(h);
    return failures ? 1 : 0;
}
