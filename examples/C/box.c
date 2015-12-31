/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"

int main(int argc, char* argv[])
{
    FILE* f;
    int err=0;
    char infile[]="../../data/reduced_gaussian_model_level.grib1";
    codes_handle *h=NULL;
    codes_box* box;
    codes_points* points;
    codes_context* c=codes_context_get_default();
    double north,west,south,east;
    double* val;
    int i;

    north=60;
    south=10;
    west=-10;
    east=30;

    f=fopen(infile,"r");
    if (!f) {
        perror(infile);
        exit(1);
    }

    h=codes_handle_new_from_file(c, f, PRODUCT_GRIB, &err);
    if (!h) {
        printf("unable to create handle from file %s\n",infile);
        exit(err);
    }

    box=codes_box_new(h,&err);

    if (!box) {
        printf("unable to create box\n");
        exit(err);
    }

    points=codes_box_get_points(box,north,west,south,east,&err);

    val=(double*)malloc(sizeof(double)*points->n);
    codes_points_get_values(h,points,val);

    for (i=0;i<points->n;i++) {
        printf("%d -- %.3f %.3f %ld %g\n",i,
                points->latitudes[i], points->longitudes[i], (long)(points->indexes[i]), val[i]);
    }

    free(val);

    fclose(f);
    return 0;

}
