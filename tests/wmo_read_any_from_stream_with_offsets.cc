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
#include <stdlib.h>
#include <eccodes.h>
#include <unistd.h>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 20'000'000   /* 20 MB should cover most messages */

// Simulation of a file stream
class FileStream {
    public:
        long stream_proc(void* buffer, long len) {
            if (!file) return -1;
            return fread(buffer, 1, len, file);
        }

        FileStream(const char* filename) {
            file = fopen(filename, "rb");
        }
        ~FileStream() {
            if (file) fclose(file);
        }
        FILE* file = nullptr;
};


std::vector<off_t> test_wmo_read_any_from_stream_with_offset(const char* filename)
{
    // codes_handle* h = NULL;
    std::vector<off_t> offsets;
    FileStream sf(filename);
    size_t msg_prev_end = 0;

    size_t buflen = BUFFER_SIZE;
    void* buffer = calloc(1, buflen);
    off_t rel_offset = 0;

    while (1) {
        int err;
        size_t bufferlen = buflen;

        err = wmo_read_any_from_stream_with_offset(
                reinterpret_cast<void*>(&sf),
                [](void* stream_data, void* buffer, long len) -> long {
                    FileStream* sf = reinterpret_cast<FileStream*>(stream_data);
                    return sf->stream_proc(buffer, len);
                },
                buffer,
                &bufferlen,
                &rel_offset
                );

        if (err == -1) {
            printf("Reached end of file.\n");
            break;
        }

        if (err != 0) {
            fprintf(stderr, "Error reading WMO message: %s\n", codes_get_error_message(err));
            break;
        }

        // h = codes_handle_new_from_message(NULL, buffer, bufferlen);
        // if (!h) {
        //     printf("  Not a GRIB message (or failed to decode) — skipping.\n");
        //     continue;
        // }

        // codes_handle_delete(h);
        offsets.push_back(rel_offset);
    }
    free(buffer);
    return offsets;
}


std::vector<off_t> test_wmo_read_any_from_stream_malloc_with_offset(const char* filename)
{
    // codes_handle* h = NULL;
    std::vector<off_t> offsets;

    FileStream sf(filename);
    size_t msg_prev_end = 0;

    size_t buflen = BUFFER_SIZE;

    while (1) {
        int err;
        size_t bufferlen = buflen;
        off_t rel_offset;
        void* buffer = nullptr;

        buffer = wmo_read_any_from_stream_malloc_with_offset(
                reinterpret_cast<void*>(&sf),
                [](void* stream_data, void* buffer, long len) -> long {
                    FileStream* sf = reinterpret_cast<FileStream*>(stream_data);
                    return sf->stream_proc(buffer, len);
                },
                &bufferlen,
                &rel_offset, // Option 1: return offset relative to previous message
                &err
                );



        if (err == -1) {
            printf("Reached end of file.\n");
            break;
        }

        if (err != 0) {
            fprintf(stderr, "Error reading WMO message: %s\n", codes_get_error_message(err));
            break;
        }

        // h = codes_handle_new_from_message(NULL, buffer, bufferlen);
        // if (!h) {
        //     printf("  Not a GRIB message (or failed to decode) — skipping.\n");
        //     continue;
        // }

        // codes_handle_delete(h);
        
        free(buffer);
        offsets.push_back(rel_offset);
    }
    return offsets;
}

void print_offsets(const std::vector<off_t>& offsets1, const std::vector<off_t>& offsets2)
{
    size_t max_size = offsets1.size() > offsets2.size() ? offsets1.size() : offsets2.size();
    for (size_t i = 0; i < max_size; i++) {
        off_t val1 = (i < offsets1.size()) ? offsets1[i] : -1;
        off_t val2 = (i < offsets2.size()) ? offsets2[i] : -1;
        std::cout << i << ": " << val1 << " | " << val2 << std::endl;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) return 1;

    auto ret1 = test_wmo_read_any_from_stream_with_offset(argv[1]);
    auto ret2 = test_wmo_read_any_from_stream_malloc_with_offset(argv[1]);
    if (ret1.size() != ret2.size()) {
        print_offsets(ret1, ret2);
        printf("Mismatch in number of messages: %zu vs %zu\n", ret1.size(), ret2.size());
        return 1;
    }
    for (size_t i = 0; i < ret1.size(); i++) {
        if (ret1[i] != ret2[i]) {
            print_offsets(ret1, ret2);
            printf("Mismatch in offsets at message %zu: %ld vs %ld\n", i, ret1[i], ret2[i]);
            return 1;
        }
    }

    for (size_t i = 0; i < ret1.size(); i++) {
        std::cout << ret1[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
