/*
*
*  Name: nearest
*
*  Description:
*    Get the nearest for all the messages in the input grib file.
*    No land-sea mask applied yet. Print the nearest values to 
*    stdout with high precision.
*
*  Author: Cristian D Codorean
*/

#include <stdio.h>
#include <stdlib.h>

#include "grib_api.h"

void usage(char* name) {
    printf("Usage is: %s lat lon input1 [input2 ...]\n",name);
}

int main(int argc, char** argv) {

    grib_handle* h = NULL;
    FILE* file_handle = NULL;
    int err = 0;
    int i = 0;
    int nfiles = 0;
    int shift = 3;

    /* results */
    double outlat=0;
    double outlon=0;
    double value=0;
    double distance=0;
    int index=0;
    size_t size=1;

    /* program inputs */
    double lat =0,lon = 0;
    char** file_names  = NULL;
    
    /* just some input validation */
    if (argc < 4) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    lat = atof(argv[1]);
    lon = atof(argv[2]);
    nfiles = argc - shift;

    file_names = (char**)malloc(nfiles*sizeof(char*));
    for (i = 0; i < nfiles; i++) {
        file_names[i] = strdup(argv[i+shift]);
    }

    

    for (i = 0; i < nfiles; i++) {

        /* open the input file */
        file_handle = fopen(file_names[i],"r");
        if (!file_handle) {
            printf("Could not open file %s\n",file_names[i]);
            exit(EXIT_FAILURE);
        }

        /* loop over all messages */
        while (h = grib_handle_new_from_file(0,file_handle,&err)) {

            if (err != GRIB_SUCCESS) GRIB_CHECK(err,0);
            
            GRIB_CHECK(grib_nearest_find_multiple(h,0,&lat,&lon,1,&outlat,&outlon,&value,&distance,&index),0);

            /*printf("outlat: %lf outlon: %lf value: %.10lf distance: %lf index: %d\n",
                outlat, outlon, value, distance, index);*/
            
            printf("%.10lf\n",value);

            GRIB_CHECK(grib_handle_delete(h),0);

        }

        fclose(file_handle);
    }

    for (i = 0; i < nfiles; i++)
        free(file_names[i]);

    return 0;
}
