/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: bufr_copy_data
 *
 * Description: How to copy all the values in the data section that are present in the same
 *              position in the data tree and with the same number of values to the output handle
 *
 */
#include "eccodes.h"

static void usage(const char* progname);

int main(int argc, char *argv[])
{
    FILE* f = NULL;
    codes_handle* h = NULL;
    codes_handle* ho = NULL;
    long size;
    char** keys = NULL;
    size_t nkeys=0,i;
    int err=0;
    char* outfile = NULL;
    const char* sampleName = "BUFR3";
    const long ibitmap[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            1,1,1,1,0};

    const long ud[]={307011,7006,10004,222000,101023,31031,1031,1032,101023,33007,
            225000,236000,101023,31031,1031,1032,8024,101001,225255,225000,
            236000,101023,31031,1031,1032,8024,101001,225255,
            1063,2001,4001,4002,4003,4004,4005,5002,
            6002,7001,7006,11001,11016,11017,11002};

    if (argc != 3) usage(argv[0]);

    outfile=argv[2];

    f = fopen(argv[1],"r");
    if(!f) {
        perror(argv[1]);
        exit(1);
    }
    ho = codes_bufr_handle_new_from_samples(NULL, sampleName);
    if (ho == NULL) {
        fprintf(stderr, "ERROR creating BUFR from %s\n", sampleName);
        fclose(f);
        return 1;
    }

    size = sizeof(ibitmap)/sizeof(ibitmap[0]);
    CODES_CHECK(codes_set_long_array(ho, "inputDataPresentIndicator", ibitmap, size), 0);
    CODES_CHECK(codes_set_long(ho, "bufrHeaderCentre", 98), 0);
    CODES_CHECK(codes_set_long(ho, "updateSequenceNumber", 1), 0);
    CODES_CHECK(codes_set_long(ho, "dataCategory", 0), 0);
    CODES_CHECK(codes_set_long(ho, "dataSubCategory", 140), 0);
    CODES_CHECK(codes_set_long(ho, "masterTablesVersionNumber", 13), 0);
    CODES_CHECK(codes_set_long(ho, "localTablesVersionNumber", 1), 0);
    CODES_CHECK(codes_set_long(ho, "typicalYearOfCentury", 15), 0);
    CODES_CHECK(codes_set_long(ho, "typicalMonth", 5), 0);
    CODES_CHECK(codes_set_long(ho, "typicalDay", 4), 0);
    CODES_CHECK(codes_set_long(ho, "typicalHour", 9), 0);
    CODES_CHECK(codes_set_long(ho, "typicalMinute", 30), 0);
    CODES_CHECK(codes_set_long(ho, "numberOfSubsets", 1), 0);
    CODES_CHECK(codes_set_long(ho, "observedData", 1), 0);
    CODES_CHECK(codes_set_long(ho, "compressedData", 0), 0);

    size = sizeof(ud)/sizeof(ud[0]);
    codes_set_long_array(ho,"unexpandedDescriptors",ud,size);

    while((h = codes_handle_new_from_file(0,f,PRODUCT_BUFR,&err)) != NULL)
    {
        if(!h) {
            printf("ERROR: Unable to create BUFR handle\n");
            return 1;
        }
        /* codes_copy_key(h,ho,"unexpandedDescriptors",0); */
        err = codes_set_long(h,"unpack",1);
        if (err) {
            printf("ERROR: Unable to unpack BUFR message. Quitting\n");
            printf("       %s\n", codes_get_error_message(err));
            return 1;
        }

        /* err=codes_bufr_copy_data(h,ho); */
        keys=codes_bufr_copy_data_return_copied_keys(h,ho,&nkeys,&err);
        for (i=0;i<nkeys;i++) {
            printf("Copied %s\n",keys[i]);
            free(keys[i]);
        }
        printf("Total number of copied keys = %lu\n", nkeys);
        free(keys);

        codes_handle_delete(h);
    }
    fclose(f);
    codes_write_message(ho,outfile,"w");
    codes_handle_delete(ho);

    return err;
}

static void usage(const char* progname)
{
    printf("\nUsage: %s bufr_in bufr_out\n",progname);
    exit(1);
}
