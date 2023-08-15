/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

int main(int argc, char** argv)
{
    FILE* in                 = NULL;
    int err                  = 0;
    grib_handle* h_full      = NULL;
    grib_keys_iterator* iter = NULL;
    const char* name         = NULL;
    const void* msg1         = NULL;
    const char* infilename   = NULL;
    size_t size1             = 0;
    grib_handle* h_partial   = 0;

    Assert(argc == 2);
    infilename = argv[1];

    printf("Processing file %s\n", infilename);
    in = fopen(infilename, "rb");
    Assert(in);

    h_full = grib_handle_new_from_file(0, in, &err);
    Assert(h_full);
    Assert(!err);

    GRIB_CHECK(grib_get_message_headers(h_full, &msg1, &size1), 0);

    h_partial = grib_handle_new_from_partial_message(h_full->context, msg1, size1);
    Assert(h_partial);

    iter = grib_keys_iterator_new(h_partial, GRIB_KEYS_ITERATOR_SKIP_COMPUTED, NULL);
    Assert(iter);

    while (grib_keys_iterator_next(iter)) {
        name = grib_keys_iterator_get_name(iter);
        printf("Header key=%s\n", name);
    }

    grib_keys_iterator_delete(iter);
    grib_handle_delete(h_partial);
    grib_handle_delete(h_full);
    fclose(in);
    return 0;
}
