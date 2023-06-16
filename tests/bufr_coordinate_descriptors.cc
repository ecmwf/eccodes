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

// Get the X part of a BUFR descriptor (FXY) and determine if it
// is a coordinate.
// See https://confluence.ecmwf.int/display/UDOC/What+are+coordinate+descriptors+-+ecCodes+BUFR+FAQ
static int is_coord_using_Xcode(const char* X)
{
    if (strcmp(X, "01") == 0) return 1;
    if (strcmp(X, "02") == 0) return 1;
    if (strcmp(X, "04") == 0) return 1;
    if (strcmp(X, "05") == 0) return 1;
    if (strcmp(X, "06") == 0) return 1;
    if (strcmp(X, "07") == 0) return 1;
    if (strcmp(X, "08") == 0) return 1;
    return 0;
}

int main(int argc, char* argv[])
{
    int err         = 0;
    codes_handle* h = NULL;
    char* filename  = NULL;
    FILE* fin       = NULL;
    int is_coord    = 0;

    assert (argc == 2);
    filename = argv[1];

    fin = fopen(filename, "rb");
    assert(fin);
    while ((h = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err)) != NULL || err != CODES_SUCCESS) {
        codes_bufr_keys_iterator* kiter = NULL;
        CODES_CHECK(codes_set_long(h, "unpack", 1), 0);
        kiter = codes_bufr_keys_iterator_new(h, 0);
        assert(kiter);
        while (codes_bufr_keys_iterator_next(kiter)) {
            char* name = codes_bufr_keys_iterator_get_name(kiter);
            if (strcmp(name, "subsetNumber") == 0) continue;
            if (!codes_bufr_key_is_header(h, name, &err) && !err) {
                char name1[256] = {0,};
                char scode[256] = {0,};
                char X[3] = {0,}; // the 'X' part of FXY
                size_t slen     = 256;
                snprintf(name1, 256, "%s->code", name);
                int error = codes_get_string(h, name1, scode, &slen);
                if (!error) {
                    assert(strlen(scode) == 6);
                    X[0] = scode[1];
                    X[1] = scode[2];
                    X[2] = 0;
                    int is_X_coord = is_coord_using_Xcode(X);
                    is_coord = codes_bufr_key_is_coordinate(h, name, &error);
                    assert(!error);
                    if (is_coord != is_X_coord) {
                        fprintf(stderr, "ERROR: %s X=%s   is_coord=%d is_X_coord=%d\n", name, X, is_coord, is_X_coord);
                        return 1;
                    }
                }
            }
            else {
                // header keys cannot be coordinate descriptors
                is_coord = codes_bufr_key_is_coordinate(h, name, &err);
                assert(!is_coord);
            }
        }

        is_coord = codes_bufr_key_is_coordinate(h, "nosuchkey", &err);
        assert(err == CODES_NOT_FOUND);

        codes_bufr_keys_iterator_delete(kiter);
        codes_handle_delete(h);
    }
    fclose(fin);
    return 0;
}
