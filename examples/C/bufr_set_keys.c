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
 * C Implementation: bufr_set_keys
 *
 * Description: how to set different type of keys in BUFR messages.
 *
 */

#include "eccodes.h"

void usage(char* prog) {
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    FILE* in = NULL;
    FILE* out = NULL;

    /* message handle. Required in all the eccodes calls acting on a message.*/
    codes_handle* h=NULL;

    long longVal;
    /*double doubleVal;*/
    int err=0;
    int cnt=0;
    size_t size = 0;
    char* infile = "../../data/bufr/syno_multi.bufr";
    const void *buffer = NULL;

    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    in = fopen(infile,"r");
    out = fopen(argv[1],"w");

    if (!in || !out) {
        perror("ERROR: unable to open files");
        if (out) fclose(out);
        if (in) fclose(in);
        return 1;
    }

    /* loop over the messages in the bufr file */
    while ((h = codes_handle_new_from_file(NULL,in,PRODUCT_BUFR,&err)) != NULL || err != CODES_SUCCESS)
    {
        if (h == NULL) {
            printf("Error: unable to create handle for message %d\n",cnt);
            cnt++;
            continue;
        }

        printf("message: %d\n",cnt);

        /* we need to instruct ecCodes to expand the descriptors 
          i.e. unpack the data values */
        /*CODES_CHECK(codes_set_long(h,"unpack",1),0);*/

        /* This is the place where you may wish to modify the message*/
        /*E.g. we change the centre  */

        /* set bufrHeaderCentre */
        longVal=222;
        CODES_CHECK(codes_set_long(h, "bufrHeaderCentre", longVal),0);
        printf("  set bufrHeaderCentre to: %ld\n",longVal);

        /* check bufrHeaderCentre */
        CODES_CHECK(codes_get_long(h,"bufrHeaderCentre",&longVal),0);
        printf("  bufrHeaderCentre's new value is: %ld\n",longVal);


        /* get the modified message in a buffer */
        CODES_CHECK(codes_get_message(h,&buffer,&size),0);

        /* write the buffer to a file */
        if(fwrite(buffer,1,size,out) != size) {
            perror(argv[0]);
            return 1;
        }

        /* delete handle */
        codes_handle_delete(h);

        cnt++;
    }

    fclose(in);
    return 0;
}
