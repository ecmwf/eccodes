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
 * C Implementation: set_pv
 *
 * Description: how to set pv (vertical coordinate parameters)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "grib_api.h"

void usage(const char* prog) {
    fprintf(stderr, "usage: %s in out\n",prog);
    exit(1);
}

int main(int argc, char** argv)
{
    int err = 0;
    long PVPresent=1, NV = 0;
    size_t size=0;
    double pv[4]={1,2,3,4};
    size_t pvsize=4;

    FILE* in = NULL;
    char* infile = NULL;
    FILE* out = NULL;
    char* outfile = NULL;
    grib_handle *h = NULL;
    const void* buffer = NULL;

    if (argc != 3) usage(argv[0]);
    infile = argv[1];
    outfile= argv[2];
    
    in = fopen(infile,"r");
    if(!in) {
        fprintf(stderr, "ERROR: unable to open input file %s\n",infile);
        return 1;
    }

    out = fopen(outfile,"w");
    if(!out) {
        fprintf(stderr, "ERROR: unable to open output file %s\n",outfile);
        fclose(in);
        return 1;
    }

    /* create a new handle from a message in a file */
    h = grib_handle_new_from_file(0,in,&err);
    if (h == NULL) {
        fprintf(stderr, "Error: unable to create handle from file %s\n",infile);
    }

    GRIB_CHECK(grib_set_long(h,"PVPresent",PVPresent),0);

    GRIB_CHECK(grib_set_double_array(h,"pv",pv,pvsize),0);
    
    /* Once we set the pv array, the NV key should be also set */
    GRIB_CHECK(grib_get_long(h,"NV",&NV),0);
    assert( NV == pvsize );

    /* get the coded message in a buffer */
    GRIB_CHECK(grib_get_message(h,&buffer,&size),0);

    /* write the buffer in a file*/
    if(fwrite(buffer,1,size,out) != size)
    {
        perror(argv[1]);
        exit(1);
    }

    grib_handle_delete(h);
    fclose(in);
    fclose(out);

    return 0;
}
