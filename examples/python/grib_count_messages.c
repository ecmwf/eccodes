/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: get
 *
 * Description: how to get values using keys.
 *
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

static void usage(char* prog) {
    printf("usage: %s filename\n",prog);
    exit(1);
}

int main(int argc, char** argv) {
    int err = 0;

    size_t i = 0;
    int nmsg = 0;

    double latitudeOfFirstGridPointInDegrees;
    double longitudeOfFirstGridPointInDegrees;
    double latitudeOfLastGridPointInDegrees;
    double longitudeOfLastGridPointInDegrees;

    double jDirectionIncrementInDegrees;
    double iDirectionIncrementInDegrees;

    long numberOfPointsAlongAParallel;
    long numberOfPointsAlongAMeridian;

    size_t values_len= 0;
    double average;
    double min;
    double max;

    FILE* in = NULL;
    char* filename;
    grib_handle *h = NULL;

    if (argc<2) usage(argv[0]);
    filename=argv[1];

    in = fopen(filename,"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",filename);
        return 1;
    }

    while((h = grib_handle_new_from_file(0,in,&err)) != NULL) {

        GRIB_CHECK(err,0);

        nmsg++;
        printf("processing message number %d\n",nmsg);

        /* get as a long*/
        GRIB_CHECK(grib_get_long(h,"Ni",&numberOfPointsAlongAParallel),0);
        printf("Ni=%ld\n",numberOfPointsAlongAParallel);

        /* get as a long*/
        GRIB_CHECK(grib_get_long(h,"Nj",&numberOfPointsAlongAMeridian),0);
        printf("Nj=%ld\n",numberOfPointsAlongAMeridian);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
        printf("latitudeOfFirstGridPointInDegrees=%g\n",latitudeOfFirstGridPointInDegrees);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"longitudeOfFirstGridPointInDegrees",&longitudeOfFirstGridPointInDegrees),0);
        printf("longitudeOfFirstGridPointInDegrees=%g\n",longitudeOfFirstGridPointInDegrees);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfLastGridPointInDegrees),0);
        printf("latitudeOfLastGridPointInDegrees=%g\n",latitudeOfLastGridPointInDegrees);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"longitudeOfLastGridPointInDegrees",&longitudeOfLastGridPointInDegrees),0);
        printf("longitudeOfLastGridPointInDegrees=%g\n",longitudeOfLastGridPointInDegrees);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
        printf("jDirectionIncrementInDegrees=%g\n",jDirectionIncrementInDegrees);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
        printf("iDirectionIncrementInDegrees=%g\n",iDirectionIncrementInDegrees);

        /* get as a double*/
        GRIB_CHECK(grib_get_double(h,"average",&average),0);
        GRIB_CHECK(grib_get_double(h,"min",&min),0);
        GRIB_CHECK(grib_get_double(h,"max",&max),0);

        /* get the size of the values array*/
        GRIB_CHECK(grib_get_size(h,"values",&values_len),0);

        printf("There are %d, average is %g, min is %g, max is %g\n",(int)values_len,average,min,max);

        for (i=0;i<100;i++) printf("-");
        printf("\n");

        grib_handle_delete(h);

    }

    fclose(in);
    return 0;
}
