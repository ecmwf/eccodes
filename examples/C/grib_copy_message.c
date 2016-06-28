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
 * C Implementation: grib_copy_message
 *
 * Description: How to copy a GRIB message in memory
 *
 */
#include <stdio.h>
#include "eccodes.h"
#include <assert.h>

void usage(char *app)
{
    fprintf(stderr,"Usage is: %s input_file ouput_file\n", app);
}

int main(int argc, char *argv[])
{
    FILE *in = NULL;
    codes_handle *source_handle = NULL;
    int err = 0;

    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    in = fopen(argv[1],"r");

    if (!in) {
        perror("ERROR: unable to input file");
        return 1;
    }

    /* loop over the GRIB messages in the source */
    while ((source_handle = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err))!=NULL)
    {
        size_t totalLength = 0, size = 0;
        const void* buffer = NULL;
        codes_handle* new_handle = NULL;

        CODES_CHECK(grib_get_message_size(source_handle,&totalLength),0);
        buffer=(unsigned char*)malloc(totalLength*sizeof(char));

        CODES_CHECK(codes_get_message(source_handle, &buffer, &size),0);
        assert(size == totalLength);

        new_handle = grib_handle_new_from_message(0, buffer, totalLength);

        if (new_handle == NULL) {
            perror("ERROR: could not create GRIB handle from message");
            return 1;
        }
        CODES_CHECK(codes_set_long(new_handle, "hour", 18),0);
        CODES_CHECK(codes_write_message(new_handle, argv[2], "w"),0);

        codes_handle_delete(new_handle);
        codes_handle_delete(source_handle);
    }
    fclose(in);

    return 0;
}
