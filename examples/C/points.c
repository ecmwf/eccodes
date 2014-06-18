/*
 * Copyright 2005-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: points
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grib_api.h"

void usage(const char* prog) {
    printf("Usage: %s latlon_file grib_orography grib_file grib_file ...\n",prog);
    exit(1);
}

int main(int argc, char** argv)
{
    FILE* fin=0;
    int ret=0;
    char* fname=0;
    float lat,lon;
    double *vlat,*vlon;
    int npoints=0,i=0,n=0;
    grib_handle* h;
    double *outlats,*outlons,*values,*lsm_values,*distances;
    int* indexes;
    long step=0;
    char time[10]={0,};
    char date[10]={0,};
    long parameter=0;
    size_t len=0;
    long iid=0;
    long *id=NULL;

    if (argc < 2) usage(argv[0]);

    fname=argv[1];
    fin=fopen(fname,"r");
    if(!fin) { perror(fname); exit(1); }
    npoints=0;
    while (fscanf(fin,"%ld %g %g",&iid,&lat,&lon) != EOF) npoints++;
    fclose(fin);

    id=(long*)malloc(npoints*sizeof(long));
    if (!id) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(long)));exit(1);}
    vlat=(double*)malloc(npoints*sizeof(double));
    if (!vlat) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    vlon=(double*)malloc(npoints*sizeof(double));
    if (!vlon) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    outlats=(double*)malloc(npoints*sizeof(double));
    if (!outlats) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    outlons=(double*)malloc(npoints*sizeof(double));
    if (!outlons) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    values=(double*)malloc(npoints*sizeof(double));
    if (!values) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    lsm_values=(double*)malloc(npoints*sizeof(double));
    if (!lsm_values) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    distances=(double*)malloc(npoints*sizeof(double));
    if (!distances) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}
    indexes=(int*)malloc(npoints*sizeof(int));
    if (!indexes) {printf("unable to allocate %ld bytes\n",(long)(npoints*sizeof(double)));exit(1);}

    fname=argv[1];
    fin=fopen(fname,"r");
    if(!fin) { perror(fname); exit(1); }
    i=0;
    while (fscanf(fin,"%ld %g %g",&iid,&lat,&lon) != EOF) {
        id[i]=iid;vlat[i]=lat;
        while(lon < 0) lon+=360;
        vlon[i]=lon;
        i++;
    }
    fclose(fin);

    fname=argv[2];
    fin=fopen(fname,"r");
    if(!fin) { perror(fname); exit(1); }
    h=grib_handle_new_from_file(0,fin,&ret);
    if (!h || ret!=GRIB_SUCCESS) {printf(" unable to create handle\n");exit(1);}

    grib_nearest_find_multiple(h,1,vlat,vlon,npoints,
            outlats,outlons,lsm_values,distances,indexes);

    grib_handle_delete(h);

    fclose(fin);

    for (n=3;n<=argc-1;n++) {
        fname=argv[n];
        fin=fopen(fname,"r");
        if(!fin) { perror(fname); exit(1); }
        while ((h=grib_handle_new_from_file(0,fin,&ret))!=NULL) {
            grib_get_double_elements(h,"values",indexes,npoints,values);

            GRIB_CHECK(grib_get_length(h, "date", &len),0);
            grib_get_string(h,"date",date,&len);
            GRIB_CHECK(grib_get_length(h, "time", &len),0);
            grib_get_string(h,"time",time,&len);
            grib_get_long(h,"step",&step);
            grib_get_long(h,"parameter",&parameter);

            printf("# %s %s %ld %ld\n",date,time,step,parameter);
            grib_handle_delete(h);
            for (i=0;i<npoints;i++)
                printf("%ld %g %g %g %g\n",
                        id[i],outlats[i],outlons[i],
                        lsm_values[i],values[i]);
        }

        fclose(fin);
    }

    return ret;
}
