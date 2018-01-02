/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "eccodes.h"
#include <ctype.h>

static void usage(const char* prog) {
    printf("usage: %s in1.grib in2.grib what out.grib\n",prog);
    printf("in1.grib   The grib in whose sections we are interested, i.e. the source of the sections (read-only)\n");
    printf("in2.grib   The input grib (read-only)\n");
    printf("what       The section(s) to copy: p(Product), g(Grid), l(Local), d(Data), b(Bitmap)\n");
    printf("out.grib   The output file\n");
    exit(1);
}

int main ( int argc, char* argv[])
{
    codes_handle *hfrom,*hto,*h;
    FILE *in;
    char *in_name1, *in_name2, *what_str, *out_name;
    int i, err=0, what=0;

    if (argc<5) usage(argv[0]);

    in_name1=argv[1];
    in_name2=argv[2];
    what_str=argv[3];
    out_name=argv[4];

    in=fopen(in_name1,"r");
    if (!in) {
        perror(in_name1);
        exit(1);
    }

    hfrom=codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    CODES_CHECK(err,0);
    fclose(in);

    in=fopen(in_name2,"r");
    if (!in) {
        perror(in_name2);
        exit(1);
    }

    hto=codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    CODES_CHECK(err,0);
    fclose(in);

    /* The sections for the "what" argument are:
     *  CODES_SECTION_PRODUCT
     *  CODES_SECTION_GRID
     *  CODES_SECTION_LOCAL
     *  CODES_SECTION_DATA
     *  CODES_SECTION_BITMAP
     * One can bitwise-OR them to have more than one section copied
     * E.g. what = CODES_SECTION_PRODUCT | CODES_SECTION_LOCAL;
     */
    for(i=0; i<strlen(what_str); ++i) {
        if (what_str[i] == 'p') {
            printf("Copying the PRODUCT section\n");
            what |= CODES_SECTION_PRODUCT;
        }
        else if (what_str[i] == 'g') {
            printf("Copying the GRID section\n");
            what |= CODES_SECTION_GRID;
        }
        else if (what_str[i] == 'l') {
            printf("Copying the LOCAL section\n");
            what |= CODES_SECTION_LOCAL;
        }
        else if (what_str[i] == 'd') {
            printf("Copying the DATA section\n");
            what |= CODES_SECTION_DATA;
        }
        else if (what_str[i] == 'b') {
            printf("Copying the BITMAP section\n");
            what |= CODES_SECTION_BITMAP;
        }
        else if (isspace(what_str[i]) || what_str[i] == ',') {
            /* Ignore spaces and comma separator */
        }
        else {
            fprintf(stderr,"Invalid option: '%c'.  Ignoring.\n",
                    what_str[i]);
        }
    }

    h=codes_grib_util_sections_copy(hfrom,hto,what,&err);
    CODES_CHECK(err,0);

    err=codes_write_message(h,out_name,"w");

    codes_handle_delete(hfrom);
    codes_handle_delete(hto);
    codes_handle_delete(h);

    return err;
}
