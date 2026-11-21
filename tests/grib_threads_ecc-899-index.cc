/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * ECC-899: Using a GRIB index is not thread safe
 *
 * Several threads read the same index file and retrieve randomly selected
 * messages from the same data file. Each retrieved message must be the
 * selected one.
 */
#include "eccodes.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

static const char* DATA_FILE = "grib_threads_ecc-899-index.grib";
static const char* INDEX_FILE = "grib_threads_ecc-899-index.idx";
static const long NUM_LEVELS = 100;

static std::atomic<long> num_failures{ 0 };

static void fail(const char* what, long level, long got, int err)
{
    fprintf(stderr, "FAILED: %s: level=%ld got=%ld err=%d (%s)\n", what, level, got, err, codes_get_error_message(err));
    num_failures++;
}

static void create_input_files()
{
    int err = 0;
    FILE* out = fopen(DATA_FILE, "wb");
    if (!out) {
        perror(DATA_FILE);
        exit(1);
    }

    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");
    if (!h) {
        fprintf(stderr, "Cannot create handle from sample\n");
        exit(1);
    }
    CODES_CHECK(codes_set_long(h, "typeOfFirstFixedSurface", 100), 0);  // isobaricInhPa
    for (long level = 1; level <= NUM_LEVELS; ++level) {
        const void* buffer = NULL;
        size_t size = 0;
        CODES_CHECK(codes_set_long(h, "level", level), 0);
        CODES_CHECK(codes_get_message(h, &buffer, &size), 0);
        if (fwrite(buffer, 1, size, out) != size) {
            perror(DATA_FILE);
            exit(1);
        }
    }
    codes_handle_delete(h);
    fclose(out);

    codes_index* index = codes_index_new_from_file(NULL, DATA_FILE, "level:l", &err);
    if (!index) {
        fprintf(stderr, "Cannot index %s: %s\n", DATA_FILE, codes_get_error_message(err));
        exit(1);
    }
    CODES_CHECK(codes_index_write(index, INDEX_FILE), 0);
    codes_index_delete(index);
}

static void reader(unsigned int seed, int num_iterations)
{
    for (int i = 0; i < num_iterations; ++i) {
        int err = 0;
        const long level = 1 + (long)(rand_r(&seed) % NUM_LEVELS);

        codes_index* index = codes_index_read(NULL, INDEX_FILE, &err);
        if (!index) {
            fail("codes_index_read", level, -1, err);
            continue;
        }

        size_t num_values = 0;
        err = codes_index_get_size(index, "level", &num_values);
        if (err || num_values != (size_t)NUM_LEVELS) {
            fail("codes_index_get_size", level, (long)num_values, err);
        }

        codes_index_select_long(index, "level", level);
        codes_handle* h = codes_handle_new_from_index(index, &err);
        if (!h) {
            fail("codes_handle_new_from_index", level, -1, err);
        }
        else {
            long got = -1;
            err = codes_get_long(h, "level", &got);
            if (err || got != level)
                fail("wrong message", level, got, err);
            codes_handle_delete(h);
        }
        codes_index_delete(index);
    }
}

static int parse_positive_int(const char* arg, int default_value)
{
    if (!arg) return default_value;
    char* end = NULL;
    const long v = strtol(arg, &end, 10);
    if (*arg == 0 || *end != 0 || v <= 0 || v > 1000000) {
        fprintf(stderr, "Invalid argument '%s'\n", arg);
        exit(1);
    }
    return (int)v;
}

int main(int argc, char** argv)
{
    const int num_threads = parse_positive_int(argc > 1 ? argv[1] : NULL, 8);
    const int num_iterations = parse_positive_int(argc > 2 ? argv[2] : NULL, 500);

    create_input_files();

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
        threads.emplace_back(reader, 1234u + t, num_iterations);
    for (auto& t : threads)
        t.join();

    printf("ECC-899: threads=%d iterations=%d failures=%ld\n", num_threads, num_iterations, num_failures.load());
    if (remove(DATA_FILE) != 0) perror(DATA_FILE);
    if (remove(INDEX_FILE) != 0) perror(INDEX_FILE);
    return num_failures == 0 ? 0 : 1;
}
