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

#define MAX_VAL_LEN 1024

int main(int argc, char* argv[])
{
    int err = 0;

    char* input_filename = argv[1];
    assert(input_filename);
    FILE* f = fopen(input_filename, "rb");
    assert(f);
    codes_handle* h = codes_handle_new_from_file(NULL, f, PRODUCT_BUFR, &err);
    assert(h);
    assert(!err);

    codes_bufr_keys_iterator* kiter = codes_bufr_keys_iterator_new(h, CODES_KEYS_ITERATOR_ALL_KEYS);
    assert(kiter);

    int num_messages = 0;
    codes_bufr_header* header_array = NULL;
    const int strict_mode = 1;
    codes_context* c = codes_context_get_default();
    err = codes_bufr_extract_headers_malloc(c, input_filename, &header_array, &num_messages, strict_mode);

    int count = 0;
    while (codes_bufr_keys_iterator_next(kiter)) {
        size_t vlen = MAX_VAL_LEN;
        char value[MAX_VAL_LEN];
        const char* kname = codes_bufr_keys_iterator_get_name(kiter);
        if (strcmp(kname, "unexpandedDescriptors")==0 ||
            strcmp(kname, "bufrHeaderCentre")==0      ||
            strncmp(kname, "correction", 10)==0       ||
            strcmp(kname, "rdbtimeTime")==0) {
            continue;
        }
        memset(value, 0, vlen);
        CODES_CHECK(codes_get_string(h, kname, value, &vlen), kname);
        //printf("%s = %s\t", kname, value);

        // fast
        size_t vlen2 = 0;
        char value2[512] = {0,};
        err = codes_bufr_header_get_string(&header_array[0], kname, value2, &vlen2);
        if (err) {
            fprintf(stderr, "ERROR: Key=%s: codes_bufr_header_get_string failed\n", kname);
            return 1;
        }
        assert(!err);
        assert(vlen > 0);
        //printf("%s\n", value2);
        if (strcmp(value, value2) != 0) {
            fprintf(stderr, "ERROR: Key=%s value1=%s but value2=%s\n", kname, value, value2);
            return 1;
        }
        ++count;
    }

    free(header_array);
    codes_bufr_keys_iterator_delete(kiter);
    codes_handle_delete(h);
    codes_context_delete(codes_context_get_default());
    printf("All OK. Num keys compared = %d\n", count);

    return 0;
}
