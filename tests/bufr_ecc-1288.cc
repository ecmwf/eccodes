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

#ifndef HAVE_FSEEKO
#define fseeko fseek
#endif

const char* KEY = "encryptedShipOrMobileLandStationIdentifier";

int main(int argc, char* argv[])
{
    FILE* in = NULL;
    codes_handle* h = NULL;
    int err = 0;
    size_t i = 0, slen = 0;
    char buf[1024] = {0,};
    char** strArray    = NULL; /* array of strings */

    const char* infile = argv[1];
    const char* DEFS_PATH_LOCAL = argv[2];

    assert(infile);
    in = fopen(infile, "rb");
    assert(in);

    h = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err);
    assert(h && !err);
    printf("Phase 1: unpacking...\n");
    codes_set_long(h, "unpack", 1); /* An error should be issued */
    assert ( !codes_is_defined(h, KEY) );
    codes_handle_delete(h);

    printf("Wipe the cache and extend the definitions path...\n");
    snprintf(buf, 1024, "%s:%s", DEFS_PATH_LOCAL, codes_definition_path(NULL));
    codes_context_delete(NULL);
    codes_context_set_definitions_path(NULL, buf);
    printf("ECCODES_DEFINITION_PATH is now = |%s|\n", buf);

    /* Now rewind and decode the BUFR again */
    fseeko(in, 0, SEEK_SET);
    h = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err); assert(h); assert(!err);
    printf("Phase 2: unpacking...\n");
    err = codes_set_long(h, "unpack", 1);

    CODES_CHECK(codes_get_size(h, KEY, &slen), 0);
    assert( slen == 40 );
    strArray = (char**)malloc(slen * sizeof(char*));
    CODES_CHECK(codes_get_string_array(h, KEY, strArray, &slen), 0);
    assert( slen == 40 );
    assert( strcmp(strArray[0], "ABCDEFHIJ0123456789ABCDEFGHIJ0123456789ABCD ") == 0 );
    codes_handle_delete(h);

    for (i = 0; i < slen; ++i) free(strArray[i]);
    free(strArray);

    fclose(in);
    return 0;
}
