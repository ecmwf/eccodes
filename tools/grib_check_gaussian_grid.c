/*
 * Copyright 2005-2019 ECMWF.
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
#include <sys/stat.h>
#include "eccodes.h"

#define STR_EQUAL(s1, s2) (strcmp((s1), (s2)) == 0)

int exit_on_error = 1; /* By default exit if any check fails */
int verbose = 0; /* By default quiet unless errors */
int error_count = 0;

int DBL_EQUAL(double d1, double d2, double tolerance)
{
    return fabs(d1-d2) <= tolerance;
}

static void usage(const char* prog)
{
    printf("Usage: %s [-f] [-v] grib_file grib_file ...\n\n",prog);
    printf("Check geometry of GRIB fields with a Gaussian Grid.\n");
    printf("(The grid is assumed to be GLOBAL)\n\n");
    printf("Options:\n");
    printf("  -f  Do not exit on first error\n");
    printf("  -v  Verbose\n");
    printf("\n");
    exit(1);
}

/* Print an error message and optionally die */
void error(const char* filename, int msg_num, const char* fmt, ...)
{
    char buf[1024] = {0,};
    va_list list;
    va_start(list,fmt);
    if(verbose)
        sprintf(buf, " Error: %s", fmt); /* indent a bit */
    else
        sprintf(buf, "Error: %s #%d: %s", filename, msg_num, fmt);
    vfprintf(stderr, buf, list);
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

static int is_regular_file(const char* path)
{
    struct stat s;
    int stat_val = stat(path,&s);
    if (stat_val != 0) return 0; /*error doing stat*/
    return S_ISREG( s.st_mode );
}

int process_file(const char* filename)
{
    int err = 0, msg_num = 0;
    codes_handle *h = NULL;
    FILE* in = NULL;
    
    if (!is_regular_file(filename)) {
        if(verbose) printf(" WARNING: '%s' not a regular file! Ignoring\n", filename);
        return GRIB_IO_PROBLEM;
    }

    in = fopen(filename, "r");
    if(!in) {
        fprintf(stderr, "ERROR: unable to open input file %s\n",filename);
        exit(1);
    }

    if(verbose) printf("Checking file %s\n", filename);

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
        CODES_CHECK(codes_get_long(h,"edition",&edition),0);
        if(verbose) printf(" Processing GRIB message #%d (edition=%ld)\n", msg_num, edition);

        len = 32;
        CODES_CHECK(codes_get_string(h,"gridType",gridType,&len),0);
        is_regular = STR_EQUAL(gridType, "regular_gg");
        is_reduced = STR_EQUAL(gridType, "reduced_gg");
        grid_ok = is_regular || is_reduced;
        if( !grid_ok ) {
            /*error("ERROR: gridType should be Reduced or Regular Gaussian Grid!\n");*/
            if(verbose) printf(" WARNING: gridType should be Reduced or Regular Gaussian Grid! Ignoring\n");
            codes_handle_delete(h);
            continue;
        }

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
            error(filename, msg_num, "N should be > 0\n", N);
        }
        if ( Nj != 2*N ) {
            error(filename, msg_num, "Nj is %ld but should be 2*N (%ld)\n", Nj, 2*N);
        }

        if (lon1 != 0) {
            error(filename, msg_num, "latitudeOfFirstGridPointInDegrees=%f but should be 0\n", lon1);
        }
        expected_lon2 = 360.0 - 360.0/(4*N);

        /* Check first and last latitudes */
        if (lat1 != -lat2) {
            error(filename, msg_num, "First latitude must be = last latitude but opposite in sign: lat1=%f, lat2=%f\n",
                    lat1, lat2);
        }
        /* Note: grib_get_gaussian_latitudes() assumes the 'lats' array has 2N elements! */
        /*       So do not allocate Nj */
        lats = (double*)malloc(sizeof(double)* 2*N);
        CODES_CHECK(codes_get_gaussian_latitudes(N,lats), 0);

        if (!DBL_EQUAL(lats[0], lat1, angular_tolerance)) {
            error(filename, msg_num, "latitudeOfFirstGridPointInDegrees=%f but should be %f\n", lat1, lats[0]);
        }
        if (!DBL_EQUAL(lats[Nj-1], lat2, angular_tolerance)) {
            error(filename, msg_num, "latitudeOfLastGridPointInDegrees=%f but should be %f\n", lat2, lats[Nj-1]);
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
                error(filename, msg_num, "For a reduced gaussian grid Ni should be missing\n");
            }
            if (!is_missing_Di) {
                error(filename, msg_num, "For a reduced gaussian grid iDirectionIncrement should be missing\n");
            }

            CODES_CHECK(codes_get_size(h, "pl", &pl_len),0);
            assert(pl_len>0);
            if (pl_len != 2*N) {
                error(filename, msg_num, "Length of pl array is %ld but should be 2*N (%ld)\n", pl_len, 2*N);
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
                    error(filename, msg_num, "pl array is not symmetric: pl[%ld]=%ld, pl[%ld]=%ld\n",
                          i, pl_start,  pl_len-1-i, pl_end);
                }
            }

            /* Check sum of pl array and total number of points */
            for(i=0; i<pl_len; ++i) {
                pl_sum += pl[i];
                if (pl[i] > max_pl) max_pl = pl[i];
            }
            if (pl_sum != numberOfDataPoints) {
                error(filename, msg_num, "Sum of pl array %ld does not match numberOfDataPoints %ld\n", pl_sum, numberOfDataPoints);
            }
            CODES_CHECK(codes_get_long(h,"isOctahedral",&is_octahedral),0);
            if (is_octahedral) {
                if(verbose) printf("  This is an Octahedral Gaussian grid\n");
                expected_lon2 = 360.0 - 360.0/max_pl;
            }
            free(pl);
        }

        if (fabs(lon2 - expected_lon2) > angular_tolerance) {
            error(filename, msg_num, "longitudeOfLastGridPointInDegrees=%f but should be %f\n", lon2, expected_lon2);
        }

        CODES_CHECK(codes_get_size(h, "values", &sizeOfValuesArray),0);
        if (sizeOfValuesArray != numberOfDataPoints) {
            error(filename, msg_num, "Number of data points %d different from size of values array %d\n",
                  numberOfDataPoints, sizeOfValuesArray);
        }

        free(lats);
        codes_handle_delete(h);
    }
    fclose(in);
    if(verbose) printf("\n");
    return GRIB_SUCCESS;
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
            exit_on_error = 0;
        }
        else if (STR_EQUAL(arg, "-v"))
        {
            if (argc < 3) {
                usage(argv[0]);
                return 1;
            }
            verbose = 1;
        }
        else
        {
            /* We have a GRIB file */
            process_file(arg);
        }
    }

    if(verbose) printf("###############\n");
    if (error_count == 0)
    {
        if(verbose) printf("ALL OK\n");
    }
    else
    {
        printf("Error count: %d\n", error_count);
        return 1;
    }

    return 0;
}
