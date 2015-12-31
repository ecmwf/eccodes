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
 * C Implementation: bufr_read_scatterometer
 *
 * Description: how to read data for a given beam from scatterometer BUFR messages.
 *
 */

/* 
 * Please note that scatterometer data can be encoded in various ways in BUFR. Therefore the code
 * below might not work directly for other types of messages than the one used in the
 * example. It is advised to use bufr_dump to understand the structure of the messages.
 */

#include "eccodes.h"

int main(int argc,char* argv[])
{
    FILE* in = NULL;

    /* Message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    double *lat=NULL, *lon=NULL, *bscatter=NULL;
    long numObs=0;
    size_t len=0;
    int i, err=0;
    int cnt=0;
    const char* infile = "../../data/bufr/asca_139.bufr";
    char key_name[128];

    in=fopen(infile,"r");
    if (!in) {
        printf("ERROR: unable to open file %s\n", infile);
        return 1;
    }

    /* Loop over the messages in the bufr file */
    while ((h = codes_handle_new_from_file(NULL,in,PRODUCT_BUFR,&err)) != NULL || err != CODES_SUCCESS)
    {
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }

        printf("message: %d\n",cnt);

        /* We need to instruct ecCodes to expand the descriptors
         * i.e. unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);
        
        /* The BUFR file contains a single message with 2016 subsets in a compressed form.
         * It means each subset has exactly the same structure: they store one location with
         * several beams and one backscatter value in each beam. 
         * 
         * To print the backScatter values for beamIdentifier=2 from all the subsets 
         * we will simply access the key by condition (see below) */
        
        /* Get the total number of subsets. */
        CODES_CHECK(codes_get_long(h,"numberOfSubsets",&numObs),0);

        printf("Number of values: %ld\n",numObs);
        
        /* Allocate memory for the values to be read. Each
         * parameter must have the same number of values. */
        lat = (double*)malloc(numObs*sizeof(double));
        lon = (double*)malloc(numObs*sizeof(double));
        bscatter = (double*)malloc(numObs*sizeof(double));

        /* Get latitude */
        sprintf(key_name,"latitude");

        /* Check the size (including all the subsets) */
        CODES_CHECK(codes_get_size(h,key_name,&len),0);
        if(len != numObs)
        {
            printf("inconsistent number of %s values found!\n",key_name);
            return 1;
        }

        /* Get the values (from all the subsets) */
        CODES_CHECK(codes_get_double_array(h,key_name,lat,&len),0);

        /* Get longitude */
        sprintf(key_name,"longitude");

        /* Check the size (including all the subsets) */
        CODES_CHECK(codes_get_size(h,key_name,&len),0);
        if(len != numObs)
        {
            printf("inconsistent number of %s values found!\n",key_name);
            return 1;
        }

        /* Get the values (from all the subsets) */
        CODES_CHECK(codes_get_double_array(h,key_name,lon,&len),0);

        /* Get backScatter for beam two. We use an access by condition for this key. */
        sprintf(key_name,"/beamIdentifier=2/backscatter");

        /* Check the size (including all the subsets) */
        CODES_CHECK(codes_get_size(h,key_name,&len),0);
        if(len != numObs)
        {
            printf("inconsistent number of %s values found!\n",key_name);
            return 1;
        }

        /* Get the values (from all the subsets) */
        CODES_CHECK(codes_get_double_array(h,key_name,bscatter,&len),0);

        /* Print the values */
        printf("pixel   lat    lon     backscatter    \n");
        printf("-------------------------------\n");

        for(i=0; i < numObs; i++)
        {
            printf("%4d %.3f %.3f %.3f \n",
                    i+1,lat[i],lon[i],bscatter[i]);
        }

        /* Delete handle */
        codes_handle_delete(h);

        /* Release memory */
        free(lat);
        free(lon);
        free(bscatter);

        cnt++;
    }

    fclose(in);
    return 0;
}
