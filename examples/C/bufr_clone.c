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
 * C implementation: bufr_clone
 *
 * Description: how to create new BUFR messages by cloning
 *              an existing message.
 *
 */
#include <stdio.h>
#include "eccodes.h"

void usage(char *app)
{
    fprintf(stderr,"Usage is: %s input_file ouput_file\n", app);
}

int main(int argc, char *argv[])
{
    FILE *in = NULL;
    FILE *out = NULL;

    /* message handle. Required in all the ecCodes calls acting on a message.*/
    codes_handle *source_handle = NULL;

    const void *buffer = NULL;
    size_t size = 0;
    int i, err = 0;

    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    in = fopen(argv[1],"r");
    out = fopen(argv[2],"w");

    /* open input and output */
    if (!in || !out) {
        perror("ERROR: unable to open files");
        if (out) fclose(out);
        if (in) fclose(in);
        return 1;
    }

    /* create a handle for the first message */
    source_handle = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err);

    if (source_handle == NULL) {
        perror("ERROR: could not create handle for message");
        fclose(out);
        fclose(in);
        return 1;
    }

    /* create several clones of this message and alter them
       in different ways */

    for(i=0; i < 3; i++) {

        /* clone the current handle */
        codes_handle *clone_handle = codes_handle_clone(source_handle);

        if (clone_handle == NULL) {
            perror("ERROR: could not clone field");
            return 1;
        }

        /* This is the place where you may wish to modify the clone
           E.g. we change the bufrHeaderCentre */

        CODES_CHECK(codes_set_long(clone_handle, "bufrHeaderCentre", 222),0);

        /* get the coded message in a buffer */
        CODES_CHECK(codes_get_message(clone_handle, &buffer, &size),0);

        /* write the buffer to a file */
        if(fwrite(buffer, 1, size, out) != size) {
            perror("ERROR: could not write message to file");
            return 1;
        }

        /* release the clone's handle */
        codes_handle_delete(clone_handle);
    }

    /* release the source's handle */
    codes_handle_delete(source_handle);

    fclose(out);
    fclose(in);

    return 0;
}
