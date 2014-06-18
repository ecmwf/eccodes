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
 * C Implementation: clone
 *
 * Description: How to create a new GRIB message by cloning
 *              an existing message.
 *
 */
#include <stdio.h>
#include "grib_api.h"

void usage(char *app)
{
    fprintf(stderr,"Usage is: %s input_file ouput_file\n", app);
}

int main(int argc, char *argv[])
{
    FILE *in = NULL;
    FILE *out = NULL;
    grib_handle *source_handle = NULL;
    const void *buffer = NULL;
    size_t size = 0;
    int err = 0;

    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    in = fopen(argv[1],"r");
    out = fopen(argv[2],"w");

    if (!in || !out) {
        perror("ERROR: unable to open files");
        fclose(out);
        fclose(in);
        return 1;
    }

    /* loop over the messages in the source grib and clone them */
    while ((source_handle = grib_handle_new_from_file(0,in,&err))!=NULL)
    {
        grib_handle *clone_handle = grib_handle_clone(source_handle);

        if (clone_handle == NULL) {
            perror("ERROR: could not clone field");
            return 1;
        }

        /* This is the place where you may wish to modify the clone */
        /* E.g.
           GRIB_CHECK(grib_set_long(clone_handle, "centre", 250),0);
           etc...
         */

        /* get the coded message in a buffer */
        GRIB_CHECK(grib_get_message(clone_handle,&buffer,&size),0);
        /* write the buffer to a file */
        if(fwrite(buffer,1,size,out) != size) {
            perror(argv[1]);
            return 1;
        }
        grib_handle_delete(clone_handle);
        grib_handle_delete(source_handle);
    }

    fclose(out);
    fclose(in);

    return 0;
}
