/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 *
 * Description: Check the geometry of a global GRIB field
 *              which has a Gaussian Grid (reduced or regular)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "eccodes.h"

#define STR_EQUAL(s1, s2) (strcmp((s1), (s2)) == 0)

int exit_on_error = 1; /* By default exit if any check fails */
int error_count = 0;

int DBL_EQUAL(double d1, double d2, double tolerance)
{
    return fabs(d1-d2) <= tolerance;
}

void usage(const char* prog)
{
    printf("usage: %s [-f] grib_file grib_file ...\n\n",prog);
    printf("Check geometry of GRIB fields with a Gaussian Grid.\n");
    printf("(The grid is assumed to be global)\n\n");
    printf("Options:\n");
    printf("-f  Do not exit on first error\n");
    printf("\n");
    exit(1);
}

/* Print an error message and die */
void error(const char* fmt, ...)
{
    va_list list;
    va_start(list,fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);

    ++error_count;
    if (exit_on_error) {
        exit(1);
    }
}

double get_precision(long edition)
{
    if (edition == 1) return 1.0/1000.0;    /* milli degrees */
    if (edition == 2) return 1.0/1000000.0; /* micro degrees */
    assert(!"Invalid edition");
    return 0.0;
}

int process_file(const char* filename)
{
    int err = 0, msg_num = 0;
    codes_handle *h = NULL;

    FILE* in = fopen(filename, "r");
    if(!in) {
        error("ERROR: unable to open input file %s\n",filename);
    }

    printf("Checking file %s\n", filename);

    while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL )
    {
        int is_reduced = 0, is_regular = 0, grid_ok = 0;
        long edition = 0, N = 0, Nj = 0, numberOfDataPoints;
        size_t len = 0, sizeOfValuesArray = 0;
        double *lats = NULL;
        long *pl = NULL;
        char gridType[128] = {0,};
        double angular_tolerance, lat1, lon1, lat2, lon2, expected_lon2;
        double iDirectionIncrementInDegrees;

        if (err != CODES_SUCCESS) CODES_CHECK(err,0);
        ++msg_num;
        printf("\tProcessing GRIB message #%d\n", msg_num);

        len = 32;
        CODES_CHECK(codes_get_string(h,"gridType",gridType,&len),0);
        is_regular = STR_EQUAL(gridType, "regular_gg");
        is_reduced = STR_EQUAL(gridType, "reduced_gg");
        grid_ok = is_regular || is_reduced;
        if( !grid_ok ) {
            /*error("ERROR: gridType should be Reduced or Regular Gaussian Grid!\n");*/
            printf("\tWARNING: gridType should be Reduced or Regular Gaussian Grid! Ignoring\n");
            codes_handle_delete(h);
            continue;
        }

        CODES_CHECK(codes_get_long(h,"edition",&edition),0);
        CODES_CHECK(codes_get_long(h,"N",&N),0);
        CODES_CHECK(codes_get_long(h,"Nj",&Nj),0);
        CODES_CHECK(codes_get_long(h,"numberOfDataPoints",&numberOfDataPoints),0);
        CODES_CHECK(codes_get_double(h,"latitudeOfFirstGridPointInDegrees", &lat1),0);
        CODES_CHECK(codes_get_double(h,"longitudeOfFirstGridPointInDegrees",&lon1),0);
        CODES_CHECK(codes_get_double(h,"latitudeOfLastGridPointInDegrees",  &lat2),0);
        CODES_CHECK(codes_get_double(h,"longitudeOfLastGridPointInDegrees", &lon2),0);
        CODES_CHECK(codes_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);

        angular_tolerance = get_precision(edition);

        if (N <= 0) {
            error("ERROR: N should be > 0!\n", N);
        }
        if ( Nj != 2*N ) {
            error("ERROR: Nj is %ld but should be 2*N (%ld)!\n", Nj, 2*N);
        }

        if (lon1 != 0) {
            error("ERROR: latitudeOfFirstGridPointInDegrees=%f but should be 0!\n", lon1);
        }
        expected_lon2 = 360.0 - 360.0/(4*N);

        /* Check first and last latitudes */
        if (lat1 != -lat2) {
            error("First latitude must be = last latitude but opposite in sign: lat1=%f, lat2=%f\n",
                    lat1, lat2);
        }
        /* Note: grib_get_gaussian_latitudes() assumes the 'lats' array has 2N elements! */
        /*       So do not allocate Nj */
        lats = (double*)malloc(sizeof(double)* 2*N);
        CODES_CHECK(codes_get_gaussian_latitudes(N,lats), 0);

        if (!DBL_EQUAL(lats[0], lat1, angular_tolerance)) {
            error("First latitude %f must be %f\n", lat1, lats[0]);
        }
        if (!DBL_EQUAL(lats[Nj-1], lat2, angular_tolerance)) {
            error("Last latitude %f must be %f\n", lat2, lats[Nj-1]);
        }

        if (is_reduced) {
            int pl_sum = 0, max_pl = 0, is_missing_Ni = 0, is_missing_Di = 0;
            size_t i = 0, pl_len = 0;
            long is_octahedral = 0;
            is_missing_Ni = codes_is_missing(h, "Ni", &err);
            assert(err == CODES_SUCCESS);
            is_missing_Di = codes_is_missing(h, "iDirectionIncrement", &err);
            assert(err == CODES_SUCCESS);
            if (!is_missing_Ni) {
                error("ERROR: For a reduced gaussian grid Ni should be missing!\n");
            }
            if (!is_missing_Di) {
                error("ERROR: For a reduced gaussian grid iDirectionIncrement should be missing!\n");
            }

            CODES_CHECK(codes_get_size(h, "pl", &pl_len),0);
            assert(pl_len>0);
            if (pl_len != 2*N) {
                error("ERROR: Length of pl array is %ld but should be 2*N (%ld)!\n", pl_len, 2*N);
            }
            pl = (long*)malloc(pl_len*sizeof(long));
            assert(pl);
            CODES_CHECK(codes_get_long_array(h, "pl", pl, &pl_len),0);
            max_pl = pl[0];

            /* Check pl is symmetric */
            for(i=0; i<pl_len/2; ++i) {
                const long pl_start = pl[i];
                const long pl_end = pl[pl_len-1-i];
                if ( pl_start != pl_end ) {
                    error("ERROR: pl array is not symmetric: pl[%ld]=%ld, pl[%ld]=%ld!\n",
                            i, pl_start,  pl_len-1-i, pl_end);
                }
            }

            /* Check sum of pl array and total number of points */
            for(i=0; i<pl_len; ++i) {
                pl_sum += pl[i];
                if (pl[i] > max_pl) max_pl = pl[i];
            }
            if (pl_sum != numberOfDataPoints) {
                error("ERROR: Sum of pl array %ld does not match numberOfDataPoints %ld!\n", pl_sum, numberOfDataPoints);
            }
            CODES_CHECK(codes_get_long(h,"isOctahedral",&is_octahedral),0);
            if (is_octahedral) {
                printf("\tThis is an Octahedral Gaussian grid!\n");
                expected_lon2 = 360.0 - 360.0/max_pl;
            }
            free(pl);
        }

        if (fabs(lon2 - expected_lon2) > angular_tolerance) {
            error("ERROR: longitudeOfLastGridPointInDegrees=%f but should be %f!\n", lon2, expected_lon2);
        }

        CODES_CHECK(codes_get_size(h, "values", &sizeOfValuesArray),0);
        if (sizeOfValuesArray != numberOfDataPoints) {
            error("Number of data points %d different from size of values array %d\n",
                  numberOfDataPoints, sizeOfValuesArray);
        }

        free(lats);
        codes_handle_delete(h);
    }
    fclose(in);
    printf("\n");
    return 0;
}

int main(int argc, char** argv)
{
    int i = 0;

    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    for(i=1; i<argc; ++i)
    {
        const char* arg = argv[i];
        if (STR_EQUAL(arg, "-f"))
        {
            if (argc < 3) {
                usage(argv[0]);
                return 1;
            }
            /* Process switches */
            exit_on_error = 0;
        }
        else
        {
            /* We have a grib file */
            process_file(arg);
        }
    }

    printf("###############\n");
    if (error_count == 0)
    {
        printf("ALL OK\n");
    }
    else
    {
        printf("Error count: %d\n", error_count);
    }

    return 0;
}
