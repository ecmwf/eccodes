/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"
#include <assert.h>

static const char* not_found = "not_found";

void print_rdb_key(int has_local, long value)
{
    if (has_local)
        printf("%ld ", value);
    else
        printf("%s ", not_found);
}
void print_rdb_ident(int has_local, const char* value)
{
    if (value == NULL || strlen(value) == 0)
        printf("%s ", not_found);
    else
        printf("%s ", value);
}

int main(int argc, char* argv[])
{
    char *filename, *keys;
    int i, err = 0;
    int num_messages = 0;
    codes_bufr_header* headers = NULL;
    codes_context* c = codes_context_get_default();

    if (argc != 3) return 1;

    keys = argv[1]; /* comma-separated like bufr_ls/bufr_get */
    filename = argv[2];

    err = codes_bufr_extract_headers_malloc(c, filename, &headers, &num_messages);
    if (err) return 1;

    for (i=0; i < num_messages; ++i) {
        codes_bufr_header bh = headers[i];
        /*
         * Mimic the behaviour of bufr_get -f -p keys
         * for testing
         */
        const int has_ecmwf_local = (bh.localSectionPresent == 1 && bh.bufrHeaderCentre == 98);

        if (strstr(keys, "message_offset")) printf("%ld ", bh.message_offset);
        if (strstr(keys, "message_size")) printf("%ld ", bh.message_size);
        if (strstr(keys, "edition")) printf("%ld ", bh.edition);
        if (strstr(keys, "totalLength")) printf("%ld ", bh.totalLength);
        if (strstr(keys, "masterTableNumber")) printf("%ld ", bh.masterTableNumber);
        if (strstr(keys, "bufrHeaderSubCentre")) printf("%ld ", bh.bufrHeaderSubCentre);
        if (strstr(keys, "bufrHeaderCentre")) printf("%ld ", bh.bufrHeaderCentre);
        if (strstr(keys, "updateSequenceNumber")) printf("%ld ", bh.updateSequenceNumber);
        if (strstr(keys, "section1Flags")) printf("%ld ", bh.section1Flags);
        if (strstr(keys, "dataCategory")) printf("%ld ", bh.dataCategory);
        if (strstr(keys, "dataSubCategory")) printf("%ld ", bh.dataSubCategory);
        if (strstr(keys, "masterTablesVersionNumber")) printf("%ld ", bh.masterTablesVersionNumber);
        if (strstr(keys, "localTablesVersionNumber")) printf("%ld ", bh.localTablesVersionNumber);
        if (strstr(keys, "typicalYearOfCentury")) printf("%ld ", bh.typicalYearOfCentury);
        if (strstr(keys, "typicalMonth")) printf("%ld ", bh.typicalMonth);
        if (strstr(keys, "typicalDay")) printf("%ld ", bh.typicalDay);
        if (strstr(keys, "typicalHour")) printf("%ld ", bh.typicalHour);
        if (strstr(keys, "typicalMinute")) printf("%ld ", bh.typicalMinute);
        if (strstr(keys, "internationalDataSubCategory")) printf("%ld ", bh.internationalDataSubCategory);
        if (strstr(keys, "typicalYear")) printf("%ld ", bh.typicalYear);
        if (strstr(keys, "typicalSecond")) printf("%ld ", bh.typicalSecond);
        if (strstr(keys, "localSectionPresent")) printf("%ld ", bh.localSectionPresent);

        if (strstr(keys, "section2Length")) printf("%ld ", bh.section2Length);

        if (strstr(keys, "rdbType"))        print_rdb_key(has_ecmwf_local, bh.rdbType);
        if (strstr(keys, "oldSubtype"))     print_rdb_key(has_ecmwf_local, bh.oldSubtype);
        if (strstr(keys, "ident"))          print_rdb_ident(has_ecmwf_local, bh.ident);
        if (strstr(keys, "localYear"))      print_rdb_key(has_ecmwf_local, bh.localYear);
        if (strstr(keys, "localMonth"))     print_rdb_key(has_ecmwf_local, bh.localMonth);
        if (strstr(keys, "localDay"))       print_rdb_key(has_ecmwf_local, bh.localDay);
        if (strstr(keys, "localHour"))      print_rdb_key(has_ecmwf_local, bh.localHour);
        if (strstr(keys, "localMinute"))    print_rdb_key(has_ecmwf_local, bh.localMinute);
        if (strstr(keys, "localSecond"))    print_rdb_key(has_ecmwf_local, bh.localSecond);
        if (strstr(keys, "rdbtimeDay"))     print_rdb_key(has_ecmwf_local, bh.rdbtimeDay);
        if (strstr(keys, "rdbtimeHour"))    print_rdb_key(has_ecmwf_local, bh.rdbtimeHour);
        if (strstr(keys, "rdbtimeMinute"))  print_rdb_key(has_ecmwf_local, bh.rdbtimeMinute);
        if (strstr(keys, "rdbtimeSecond"))  print_rdb_key(has_ecmwf_local, bh.rdbtimeSecond);
        if (strstr(keys, "rectimeDay"))     print_rdb_key(has_ecmwf_local, bh.rectimeDay);
        if (strstr(keys, "rectimeHour"))    print_rdb_key(has_ecmwf_local, bh.rectimeHour);
        if (strstr(keys, "rectimeMinute"))  print_rdb_key(has_ecmwf_local, bh.rectimeMinute);
        if (strstr(keys, "rectimeSecond"))  print_rdb_key(has_ecmwf_local, bh.rectimeSecond);
        if (strstr(keys, "qualityControl")) print_rdb_key(has_ecmwf_local, bh.qualityControl);
        if (strstr(keys, "newSubtype"))     print_rdb_key(has_ecmwf_local, bh.newSubtype);
        if (strstr(keys, "daLoop"))         print_rdb_key(has_ecmwf_local, bh.daLoop);

        if (strstr(keys, "numberOfSubsets"))    printf("%ld ", bh.numberOfSubsets);
        if (strstr(keys, "observedData"))       printf("%ld ", bh.observedData);
        if (strstr(keys, "compressedData"))     printf("%ld ", bh.compressedData);
        printf("\n");
    }

    free(headers);

    return 0;
}
