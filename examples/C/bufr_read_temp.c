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
 * C Implementation: bufr_read_temp
 *
 * Description: how to read temperature significant levels from TEMP BUFR messages.
 *
 */
 
/* 
 * Please note that TEMP reports can be encoded in various ways in BUFR. Therefore the code
 * below might not work directly for other types of TEMP messages than the one used in the
 * example. It is advised to use bufr_dump to understand the structure of the messages.
 */

#include "eccodes.h"

int main(int argc,char* argv[])
{
    FILE* in = NULL;

    /* Message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    double *sigt_pres=NULL, *sigt_geo=NULL, *sigt_t=NULL;
    double *sigt_td=NULL;
    size_t sigt_len=0, len=0, i=0;
    int err=0;
    int cnt=0;
    const char* infile = "../../data/bufr/temp_101.bufr";
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
           i.e. unpack the data values */
        CODES_CHECK(codes_set_long(h,"unpack",1),0);

        /* In what follows we rely on the fact that for
         * temperature significant levels the value of key
         * verticalSoundingSignificance is 4 (see flag table 8001 for details).
         *
         * In our BUFR message verticalSoundingSignificance is always followed by
         *    geopotential, airTemperature, dewpointTemperature,
         *    windDirection, windSpeed and pressure. 
         * So in order to access any of these keys we need to use the
         * condition: verticalSoundingSignificance=4.
         */

        /* Get the number of the temperature significant levels.*/

        /* We find out the number of temperature significant levels by
         * counting how many pressure values we have on these levels.*/

        sprintf(key_name,"/verticalSoundingSignificance=4/pressure");
        CODES_CHECK(codes_get_size(h,key_name,&sigt_len),0);

        printf("Number of T significant levels: %ld\n",sigt_len);

        /* Allocate memory for the values to be read. Each
         * parameter must have the same number of values. */
        sigt_pres = (double*)malloc(sigt_len*sizeof(double));
        sigt_geo = (double*)malloc(sigt_len*sizeof(double));
        sigt_t = (double*)malloc(sigt_len*sizeof(double));
        sigt_td = (double*)malloc(sigt_len*sizeof(double));

        /* Get pressure */
        sprintf(key_name,"/verticalSoundingSignificance=4/pressure");

        /* Get the values */
        len=sigt_len;
        CODES_CHECK(codes_get_double_array(h,key_name,sigt_pres,&len),0);

        /* Get geopotential */
        sprintf(key_name,"/verticalSoundingSignificance=4/nonCoordinateGeopotential");

        /* Check the size*/
        CODES_CHECK(codes_get_size(h,key_name,&len),0);
        if(len != sigt_len)
        {
            printf("inconsistent number of geopotential values found!\n");
            return 1;
        }

        /* Get the values */
        CODES_CHECK(codes_get_double_array(h,key_name,sigt_geo,&len),0);

        /* Get temperature */
        sprintf(key_name,"/verticalSoundingSignificance=4/airTemperature");

        /* Check the size*/
        if(len != sigt_len)
        {
            printf("inconsistent number of temperature values found!\n");
            return 1;
        }

        /* Get the values */
        CODES_CHECK(codes_get_double_array(h,key_name,sigt_t,&len),0);

        /* Get dew point */
        sprintf(key_name,"/verticalSoundingSignificance=4/dewpointTemperature");

        /* Check the size*/
        if(len != sigt_len)
        {
            printf("inconsistent number of dewpoint temperature values found!\n");
            return 1;
        }

        /* Get the values */
        CODES_CHECK(codes_get_double_array(h,key_name,sigt_td,&len),0);

        /* Print the values */
        printf("lev  pres    geo    t    td\n");
        printf("-------------------------------\n");

        for(i=0; i < sigt_len; i++)
        {
            printf("%3ld %6.0f %6.0f %.1f %.1f\n",
                    i+1,sigt_pres[i],sigt_geo[i],sigt_t[i],sigt_td[i]);
        }

        /* Delete handle */
        codes_handle_delete(h);

        /* Release memory */
        free(sigt_pres);
        free(sigt_geo);
        free(sigt_t);
        free(sigt_td);

        cnt++;
    }

    fclose(in);
    return 0;
}
