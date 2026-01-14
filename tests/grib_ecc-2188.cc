/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
// #include <iostream>

#include "grib_api_internal.h"
#include "eccodes.h"

#define SIZE 1024 * 1024 * 20


static void deleter(void* data)
{
    delete[] static_cast<unsigned char*>(data);
}

static int test_message_ownership(const char* filename) // , size_t* previous_end_of_message)
{
    FILE* in = fopen(filename, "r");
    if (!in) return 1;

    while (true) {
        size_t buf_len = SIZE;
        void* buf = new unsigned char[SIZE];

        int err = wmo_read_any_from_file(in, buf, &buf_len);

        if (err == GRIB_END_OF_FILE && buf_len == 0) {
            break;
        }

        if (err != GRIB_SUCCESS) {
            printf("Error reading message at offset: %s\n", codes_get_error_message(err));
            return err;
        }

        codes_handle* h = codes_handle_new_from_message(NULL, buf, buf_len);
        if (!h) {
            printf("Error creating handle at offset\n");
            return 1;
        }

        err = codes_handle_take_message_ownership(h, deleter);

        if (err != GRIB_SUCCESS) {
            printf("Error taking message ownership at offset: %s\n", codes_get_error_message(err));
            codes_handle_delete(h);
            return err;
        }

        codes_handle_delete(h);
        // codes_context_free(NULL, buf); // Don't free the buffer here. Freed by codes_handle_delete
    }

    fclose(in);
    return 0;
}

static long fread_wrapper(void* stream_data, void* buffer, long len)
{
    FILE* f = static_cast<FILE*>(stream_data);
    return fread(buffer, 1, len, f);
}


static int test_codes_handle_from_stream(const char* filename)
{
    FILE* in = fopen(filename, "r");
    codes_handle* h = NULL;
    int err = 0;

    if (!in)
        return 1;

    while ((h = codes_handle_new_from_stream(NULL, reinterpret_cast<void*>(in), fread_wrapper, &err)) != NULL) {
        if (err != GRIB_SUCCESS) {
            printf("Error reading message from stream: %s\n", codes_get_error_message(err));
            return err;
        }

        long paramId = 0;
        err = codes_get_long(h, "paramId", &paramId); // Just a dummy operation
        if (err != GRIB_SUCCESS) {
            printf("Error getting paramId: %s\n", codes_get_error_message(err));
            codes_handle_delete(h);
            return err;
        }

        codes_handle_delete(h);
    }

    fclose(in);
    return 0;
}


int main(int argc, char** argv)
{
    int err = 0;
    FILE* in = NULL;

    if (argc != 2) return 1;

    err = test_message_ownership(argv[1]);
    if (err != 0) return err;

    err = test_codes_handle_from_stream(argv[1]);
    if (err != 0) return err;

    return 0;
}
