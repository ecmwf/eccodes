/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api.h"

/*
 * Check that optimizeScaleFactor=1 reduces the error on spherical harmonics encoding. Check that sub-truncation is preserved in IEEE-32 precision.
 * philippe.marguinaud@meteo.fr, 2016/02
 */

#include "grib_sh_values.h" /* array 'values' defined here*/

#define ILCHAM 992
#define MTRONC 30
#define STRONC 10

int main(int argc, char* argv[])
{
    size_t len;
    grib_handle* h;
    double zval[ILCHAM];
    int ioptimizeScaleFactor;
    double zerr[2] = { 0, 0 };
    int i, m, n, k;

    for (ioptimizeScaleFactor = 0; ioptimizeScaleFactor < 2; ioptimizeScaleFactor++) {
        GRIB_CHECK(((h = grib_handle_new_from_samples(NULL, "sh_ml_grib2")) == NULL), 0);

        /* Meteo-France settings */
        GRIB_CHECK(grib_set_long(h, "centre", 85), 0);
        len = strlen("stretched_rotated_sh");
        GRIB_CHECK(grib_set_string(h, "gridType", "stretched_rotated_sh", &len), 0);
        GRIB_CHECK(grib_set_long(h, "pentagonalResolutionParameterJ", MTRONC), 0);
        GRIB_CHECK(grib_set_long(h, "pentagonalResolutionParameterK", MTRONC), 0);
        GRIB_CHECK(grib_set_long(h, "pentagonalResolutionParameterM", MTRONC), 0);

        GRIB_CHECK(grib_set_double(h, "stretchingFactor", 2.40000000000000), 0);
        GRIB_CHECK(grib_set_double(h, "latitudeOfStretchingPoleInDegrees", 46.4688478326275), 0);
        GRIB_CHECK(grib_set_double(h, "longitudeOfStretchingPoleInDegrees", 2.57831007808864), 0);

        GRIB_CHECK(grib_set_long(h, "bitsPerValue", 16), 0);
        len = strlen("spectral_complex");
        GRIB_CHECK(grib_set_string(h, "packingType", "spectral_complex", &len), 0);
        GRIB_CHECK(grib_set_long(h, "optimizeScaleFactor", ioptimizeScaleFactor), 0);

        GRIB_CHECK(grib_set_long(h, "subSetJ", STRONC), 0);
        GRIB_CHECK(grib_set_long(h, "subSetK", STRONC), 0);
        GRIB_CHECK(grib_set_long(h, "subSetM", STRONC), 0);
        GRIB_CHECK(grib_set_long(h, "unpackedSubsetPrecision", 1), 0);

        GRIB_CHECK(grib_set_double_array(h, "values", values, ILCHAM), 0);
        len = ILCHAM;
        GRIB_CHECK(grib_get_double_array(h, "values", zval, &len), 0);

        /* Compare our values */
        for (i = 0; i < ILCHAM; ++i) {
            const double diff = fabs(zval[i] - values[i]);
            if (diff > 1e-5) {
                fprintf(stderr, "Unpacked value different: i=%d values[i]=%g zval[i]=%g\n", i, values[i], zval[i]);
                return 1;
            }
        }

        for (m = 0, k = 0; m < MTRONC + 1; m++) {
            for (n = m; n < MTRONC + 1; k++, n++) {
                /* Check sub-truncation was fully preserved in IEEE-32 */
                if ((m < STRONC + 1) && (n < STRONC + 1) && (((float)zval[2 * k] != (float)values[2 * k]) || ((float)zval[2 * k + 1] != (float)values[2 * k + 1]))) {
                    printf("Unpacked sub-truncation was not fully preserved; coefficients for wave number (m=%d,n=%d) have been modified\n", m, n);
                    return 1;
                }
            }
        }

        for (i = 0; i < ILCHAM; i++) {
            zerr[ioptimizeScaleFactor] += (zval[i] - values[i]) * (zval[i] - values[i]);
        }

        zerr[ioptimizeScaleFactor] = sqrt(zerr[ioptimizeScaleFactor]);

        GRIB_CHECK(grib_handle_delete(h), 0);
    }

    if (zerr[0] < zerr[1]) {
        printf("Packing error too big: optimizeScaleFactor appears to be broken\n");
        return 1;
    }
    printf("OK\n");

    return 0;
}
