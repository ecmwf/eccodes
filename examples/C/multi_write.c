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
 * C Implementation: multi_write
 *
 * Description: How to encode grib2 messages containing multiple fields
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "grib_api.h"

void usage(const char* prog) {
    printf("usage: %s in.grib out.grib\n",prog);
    exit(1);
}

int main(int argc, char** argv)
{
    int err = 0;
    FILE* in = NULL;
    FILE* of = NULL;
    long edition, step;
    char* filename=NULL;
    char* ofilename=NULL;
    grib_handle *h = NULL;
    grib_multi_handle *mh=NULL;
    const int start_section = 4; /* Grib2 Product Definition Section */

    if (argc < 3) usage(argv[0]);
    filename=argv[1];
    ofilename=argv[2];

    /* open input file */
    in = fopen(filename,"r");
    if(!in) {
        fprintf(stderr, "ERROR: unable to open input file %s\n",filename);
        return 1;
    }

    /* new grib handle from input file */
    h = grib_handle_new_from_file(0,in,&err);
    GRIB_CHECK(err,0);
    GRIB_CHECK(grib_get_long(h,"edition",&edition),0);
    if (edition != 2) {
        fprintf(stderr, "ERROR: Input grib must be edition 2 for multi fields\n");
        exit(1);
    }

    /* create a new empty multi field handle */
    mh=grib_multi_handle_new(0);
    if (!mh) {
        fprintf(stderr,"ERROR: Unable to create multi field handle\n");
        exit(1);
    }

    for (step=12;step<=120;step+=12) {
        /* set step */
        grib_set_long(h,"step",step);
        /* append h to mh repeating from section 4 */
        /* i.e. starting from section 4 all the sections to the end of the message will be copied */
        grib_multi_handle_append(h, start_section, mh);
    }

    /* open output file */
    of=fopen(ofilename,"w");
    if(!of) {
        fprintf(stderr, "ERROR: unable to open output file %s\n",ofilename);
        exit(1);
    }

    /* write multi fields handle to output file */
    grib_multi_handle_write(mh,of);
    fclose(of);

    /* release memory */
    grib_handle_delete(h);
    grib_multi_handle_delete(mh);

    fclose(in);
    return 0;
}
