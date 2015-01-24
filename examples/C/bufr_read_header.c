/*
 * Copyright 2005-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"

void usage(char* prog) {
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    char* filename;
    FILE* f;
    codes_handle* h=NULL;
    long longVal;
    int count;
    int err=0;
    int header_only=1;

    if (argc!=2) usage(argv[0]);
    filename=argv[1];

    f=fopen(filename,"r");
    if (!f) {
        perror(filename);
        exit(1);
    }

    count=1;
    while((h=bufr_new_from_file(NULL,f,header_only,&err)) != NULL)
    {
        /* Check for errors after reading a message. */
        if (err != CODES_SUCCESS) CODES_CHECK(err,0);

        printf("=======================================\n");
        printf("message no: %d\n",count);

        /*From section 0 */
        CODES_CHECK(codes_get_long(h,"totalLength",&longVal),0);
        printf("\ttotalLength: %ld\n",longVal);

        /*From section 1 */
        CODES_CHECK(codes_get_long(h,"centre",&longVal),0);
        printf("\tcentre: %ld\n",longVal);

        /*From section 2 */
        CODES_CHECK(codes_get_long(h,"rdbtimeDay",&longVal),0);
        printf("\trdbtimeDay: %ld\n",longVal);

        /*From section 3 */
        /*CODES_CHECK(codes_get_long(h,"numberOfSubsets",&longVal),0);
          printf("\tnumberOfSubsets: %d\n",longVal);*/

        count++;
        codes_handle_delete(h);
    }

    fclose(f);
    return 0;
}
