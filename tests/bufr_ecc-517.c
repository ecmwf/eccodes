/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"
#undef NDEBUG
#include <assert.h>

int main(int argc, char** argv)
{
    size_t size            = 0;
    const void* buffer     = NULL;
    FILE* fout             = NULL;
    codes_handle* h        = NULL;
    long* ivalues          = NULL;
    const char* sampleName = "BUFR3_local";
    char* outfilename      = NULL;

    assert(argc == 2);
    outfilename = argv[1];

    h = codes_bufr_handle_new_from_samples(NULL, sampleName);
    assert(h);

    ivalues = (long*)malloc(1 * sizeof(long));
    if (!ivalues) {
        fprintf(stderr, "Failed to allocate memory (ivalues).\n");
        return 1;
    }
    size       = 1;
    ivalues[0] = 1;
    CODES_CHECK(codes_set_long_array(h, "inputDelayedDescriptorReplicationFactor", ivalues, size), 0);

    free(ivalues);
    ivalues    = (long*)malloc(1 * sizeof(long));
    size       = 1;
    ivalues[0] = 486;
    CODES_CHECK(codes_set_long_array(h, "inputExtendedDelayedDescriptorReplicationFactor", ivalues, size), 0);

    CODES_CHECK(codes_set_long(h, "edition", 3), 0);
    CODES_CHECK(codes_set_long(h, "masterTableNumber", 0), 0);
    CODES_CHECK(codes_set_long(h, "bufrHeaderSubCentre", 0), 0);
    CODES_CHECK(codes_set_long(h, "bufrHeaderCentre", 98), 0);
    CODES_CHECK(codes_set_long(h, "updateSequenceNumber", 0), 0);
    CODES_CHECK(codes_set_long(h, "dataCategory", 2), 0);
    CODES_CHECK(codes_set_long(h, "dataSubCategory", 109), 0);
    CODES_CHECK(codes_set_long(h, "masterTablesVersionNumber", 13), 0);
    CODES_CHECK(codes_set_long(h, "localTablesVersionNumber", 1), 0);
    CODES_CHECK(codes_set_long(h, "typicalYearOfCentury", 17), 0);
    CODES_CHECK(codes_set_long(h, "typicalMonth", 8), 0);
    CODES_CHECK(codes_set_long(h, "typicalDay", 6), 0);
    CODES_CHECK(codes_set_long(h, "typicalHour", 11), 0);
    CODES_CHECK(codes_set_long(h, "typicalMinute", 15), 0);
    CODES_CHECK(codes_set_long(h, "rdbType", 5), 0);
    CODES_CHECK(codes_set_long(h, "oldSubtype", 109), 0);
    CODES_CHECK(codes_set_long(h, "rdbtimeDay", 7), 0);
    CODES_CHECK(codes_set_long(h, "rdbtimeHour", 11), 0);
    CODES_CHECK(codes_set_long(h, "rdbtimeMinute", 13), 0);
    CODES_CHECK(codes_set_long(h, "rdbtimeSecond", 46), 0);
    CODES_CHECK(codes_set_long(h, "rectimeDay", 7), 0);
    CODES_CHECK(codes_set_long(h, "rectimeHour", 11), 0);
    CODES_CHECK(codes_set_long(h, "rectimeMinute", 13), 0);
    CODES_CHECK(codes_set_long(h, "rectimeSecond", 46), 0);
    CODES_CHECK(codes_set_long(h, "correction1", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction1Part", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction2", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction2Part", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction3", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction3Part", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction4", 0), 0);
    CODES_CHECK(codes_set_long(h, "correction4Part", 0), 0);
    CODES_CHECK(codes_set_long(h, "qualityControl", 70), 0);
    CODES_CHECK(codes_set_long(h, "newSubtype", 0), 0);
    CODES_CHECK(codes_set_long(h, "numberOfSubsets", 1), 0);
    CODES_CHECK(codes_set_double(h, "localLatitude", 5.450296999999999770e+01), 0);
    CODES_CHECK(codes_set_double(h, "localLongitude", -6.343119999999999870e+00), 0);
    CODES_CHECK(codes_set_long(h, "observedData", 1), 0);
    CODES_CHECK(codes_set_long(h, "compressedData", 0), 0);

    free(ivalues);
    ivalues = (long*)malloc(9 * sizeof(long));
    if (!ivalues) {
        fprintf(stderr, "Failed to allocate memory (ivalues).\n");
        return 1;
    }
    size       = 4;
    ivalues[0] = 309052;
    ivalues[1] = 5001;
    ivalues[2] = 6001;
    ivalues[3] = 7001;
    CODES_CHECK(codes_set_long_array(h, "unexpandedDescriptors", ivalues, size), 0);

    /* Encode the keys back in the data section */
    CODES_CHECK(codes_set_long(h, "pack", 1), 0);

    fout = fopen(outfilename, "wb");
    if (!fout) {
        fprintf(stderr, "Failed to open (create) output file.\n");
        free(ivalues);
        return 1;
    }
    CODES_CHECK(codes_get_message(h, &buffer, &size), 0);
    CODES_CHECK(codes_check_message_header(buffer, size, PRODUCT_BUFR), 0);
    CODES_CHECK(codes_check_message_footer(buffer, size, PRODUCT_BUFR), 0);
    if (fwrite(buffer, 1, size, fout) != size) {
        fclose(fout);
        fprintf(stderr, "Failed to write data.\n");
        free(ivalues);
        return 1;
    }

    fclose(fout);
    codes_handle_delete(h);
    free(ivalues);

    return 0;
}
