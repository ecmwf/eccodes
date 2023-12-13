/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/*
 * Test the use of optimizeScaleFactor; on the following field, the packing error
 * is reduced by almost a factor of 2;
 * Test that second order packing gives the same result as simple packing with optimizeScaleFactor=1
 * philippe.marguinaud@meteo.fr, 2016/02
 */
#include "grib_gridded_values.h" // array 'values' defined here

#define NLON 81
#define NLAT 166

static void encode_decode(double* zval, const char* packingType, const int bitsPerValue, const int ioptimizeScaleFactor)
{
    size_t len, slen;
    grib_handle* h;

    printf("encode_decode: packingType=%s bitsPerValue=%d opt=%d\n", packingType, bitsPerValue, ioptimizeScaleFactor);
    GRIB_CHECK(((h = grib_handle_new_from_samples(NULL, "regular_ll_pl_grib2")) == NULL), 0);
    GRIB_CHECK(grib_set_long(h, "Ni", NLON), 0);
    GRIB_CHECK(grib_set_long(h, "Nj", NLAT), 0);
    slen = strlen(packingType);
    GRIB_CHECK(grib_set_string(h, "packingType", packingType, &slen), 0);
    GRIB_CHECK(grib_set_long(h, "bitsPerValue", bitsPerValue), 0);
    GRIB_CHECK(grib_set_long(h, "optimizeScaleFactor", ioptimizeScaleFactor), 0);

    len = NLON * NLAT;
    GRIB_CHECK(grib_set_double_array(h, "values", values, len), 0);
    GRIB_CHECK(grib_get_double_array(h, "values", zval, &len), 0);

    GRIB_CHECK(grib_handle_delete(h), 0);
}

static double calc_error(const double* zval1, const double* zval2)
{
    int i;
    double zerr = 0;

    for (i = 0; i < NLON * NLAT; i++)
        zerr += (zval2[i] - zval1[i]) * (zval2[i] - zval1[i]);

    return sqrt(zerr);
}

int main(int argc, char* argv[])
{
    const int bitsPerValue[11] = { 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
    int ipackingType, ibitsPerValue, ioptimizeScaleFactor;

    for (ibitsPerValue = 0; ibitsPerValue < 11; ibitsPerValue++) {
        for (ipackingType = 0; ipackingType < 2; ipackingType++) {
            double zval_simple[NLAT * NLON];
            double zval_second[NLAT * NLON];
            double zerr;

            encode_decode(zval_simple, "grid_simple", bitsPerValue[ibitsPerValue], 1);
            encode_decode(zval_second, "grid_second_order", bitsPerValue[ibitsPerValue], 1);

            zerr = calc_error(zval_simple, zval_second);

            if (zerr > 0.) {
                fprintf(stderr, "Error: Second order packing does not reproduce simple packing with optimizeScaleFactor=1\n");
                return 1;
            }
        }
    }
    printf(" %30s %30s %30s  %s\n", "bitsPerValue", "error(optimizeScaleFactor=0)", "error(optimizeScaleFactor=1)", "ratio");
    for (ibitsPerValue = 0; ibitsPerValue < 11; ibitsPerValue++) {
        double zerr[2];
        double ratio = 0;
        for (ioptimizeScaleFactor = 0; ioptimizeScaleFactor < 2; ioptimizeScaleFactor++) {
            double zval[NLAT * NLON];
            encode_decode(zval, "grid_simple", bitsPerValue[ibitsPerValue], ioptimizeScaleFactor);
            zerr[ioptimizeScaleFactor] = calc_error(zval, values);
        }

        ratio = zerr[0] / zerr[1];
        printf(" %30d %30.9e %30.9e  %g\n", bitsPerValue[ibitsPerValue], zerr[0], zerr[1], ratio);

        if ((ratio < 1.90) || (ratio > 2.00)) {
            fprintf(stderr, "Error: Packing error too big: optimizeScaleFactor appears to be broken (zerr[0]/zerr[1]=%g)\n", ratio);
            return 1;
        }
    }

    return 0;
}
