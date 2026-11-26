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
 * Description: packing values whose reference value does not fit into an IEEE float must fail
 *              with an error, not abort (simple and CCSDS packing). Values that can be packed
 *              must decode within the packing precision.
 */
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "eccodes.h"

int main(void)
{
    const char* packings[] = { "grid_simple", "grid_ccsds" };
    const double sets[][4] = { { 1e300, -1e300, 0, 0 }, { 1e100, 0, 0, 0 }, { -1e39, 1, 2, 3 }, { 3.5e38, 0, 0, 0 }, { 1e38, -1e38, 0, 0 } };
    const long bpvs[] = { 1, 12, 24, 32 };
    int failures = 0;
    for (size_t p = 0; p < 2; p++) {
        for (size_t c = 0; c < sizeof(sets) / sizeof(sets[0]); c++) {
            for (size_t b = 0; b < 4; b++) {
                codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");
                size_t len = strlen(packings[p]), n = 4, i;
                double out[4] = { 0 };
                int err = 0;
                codes_set_long(h, "Ni", 4);
                codes_set_long(h, "Nj", 1);
                err = codes_set_string(h, "packingType", packings[p], &len);
                if (!err) err = codes_set_long(h, "bitsPerValue", bpvs[b]);
                if (!err) err = codes_set_double_array(h, "values", sets[c], 4);
                if (!err) err = codes_get_double_array(h, "values", out, &n);
                if (!err) {
                    /* Packed: the values must come back within the precision of bpv bits over the range */
                    double range = fabs(sets[c][0] - sets[c][1]) + fabs(sets[c][0]) * 1e-6;
                    for (i = 0; i < 4; i++) {
                        if (!(fabs(out[i] - sets[c][i]) <= range)) {
                            fprintf(stderr, "FAIL %s set %zu bpv %ld: value %zu decoded as %g, packed %g\n", packings[p], c,
                                    bpvs[b], i, out[i], sets[c][i]);
                            failures++;
                        }
                    }
                }
                printf("%s set %zu bpv %ld: %s\n", packings[p], c, bpvs[b], err ? codes_get_error_message(err) : "packed");
                codes_handle_delete(h);
            }
        }
    }
    return failures ? 1 : 0;
}
