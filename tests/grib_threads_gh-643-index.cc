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
 * GitHub issue #643: Concurrent index creation and retrieval race on file_pool.current
 *
 * One thread retrieves messages from an existing index while other threads
 * repeatedly create and delete indexes of other (distinct) data files.
 * The reader must always get its own messages and the builders must
 * always succeed.
 */
#include "eccodes.h"

#include <atomic>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

static const long NUM_LEVELS = 4;

static std::atomic<long> num_failures{ 0 };
static std::atomic<bool> builders_done{ false };

// Very long file names (paths) widen the race window of the unlocked filename
// comparison in grib_file_open() (the file_pool.current fast path) and make the
// failure reproducible
static const int DIR_DEPTH = 8;

static std::string dir_name(int depth)
{
    std::string dir = ".";
    for (int d = 0; d < depth; ++d)
        dir += "/" + std::string(240, 'a' + d);
    return dir;
}

static std::string file_name(int i)
{
    return dir_name(DIR_DEPTH) + "/grib_threads_gh-643-index." + std::to_string(i) + ".grib";
}

static void make_dirs()
{
    for (int d = 1; d <= DIR_DEPTH; ++d) {
        if (mkdir(dir_name(d).c_str(), 0755) != 0 && errno != EEXIST) {
            perror(dir_name(d).c_str());
            exit(1);
        }
    }
}

static void remove_dirs()
{
    for (int d = DIR_DEPTH; d >= 1; --d) {
        if (rmdir(dir_name(d).c_str()) != 0)
            perror(dir_name(d).c_str());
    }
}

static void create_data_file(const std::string& filename, long first_level)
{
    FILE* out = fopen(filename.c_str(), "wb");
    if (!out) {
        perror(filename.c_str());
        exit(1);
    }

    codes_handle* h = codes_grib_handle_new_from_samples(NULL, "GRIB2");
    if (!h) {
        fprintf(stderr, "Cannot create handle from sample\n");
        exit(1);
    }
    CODES_CHECK(codes_set_long(h, "typeOfFirstFixedSurface", 100), 0);  // isobaricInhPa
    for (long level = first_level; level < first_level + NUM_LEVELS; ++level) {
        const void* buffer = NULL;
        size_t size = 0;
        CODES_CHECK(codes_set_long(h, "level", level), 0);
        CODES_CHECK(codes_get_message(h, &buffer, &size), 0);
        if (fwrite(buffer, 1, size, out) != size) {
            perror(filename.c_str());
            exit(1);
        }
    }
    codes_handle_delete(h);
    fclose(out);
}

// Reader: retrieves messages from an index of file 0 (levels 1 .. NUM_LEVELS)
static void reader(codes_index* index)
{
    unsigned int seed = 42;
    long count = 0;
    while (!builders_done || count < 1000) {
        int err = 0;
        const long level = 1 + (long)(rand_r(&seed) % NUM_LEVELS);
        codes_index_select_long(index, "level", level);
        codes_handle* h = codes_handle_new_from_index(index, &err);
        if (!h) {
            fprintf(stderr, "FAILED: reader: level=%ld err=%d (%s)\n", level, err, codes_get_error_message(err));
            num_failures++;
        }
        else {
            long got = -1;
            err = codes_get_long(h, "level", &got);
            if (err || got != level) {
                fprintf(stderr, "FAILED: reader: wrong message: level=%ld got=%ld\n", level, got);
                num_failures++;
            }
            codes_handle_delete(h);
        }
        ++count;
    }
}

// Builder: repeatedly indexes its own file (file 'id')
static void builder(int id, int num_iterations)
{
    const std::string filename = file_name(id);
    for (int i = 0; i < num_iterations; ++i) {
        int err = 0;
        codes_index* index = codes_index_new_from_file(NULL, filename.c_str(), "level:l", &err);
        if (!index) {
            fprintf(stderr, "FAILED: builder %d: %s\n", id, codes_get_error_message(err));
            num_failures++;
            continue;
        }
        size_t num_values = 0;
        err = codes_index_get_size(index, "level", &num_values);
        if (err || num_values != (size_t)NUM_LEVELS) {
            fprintf(stderr, "FAILED: builder %d: index has %zu levels\n", id, num_values);
            num_failures++;
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
    const int num_builders = parse_positive_int(argc > 1 ? argv[1] : NULL, 4);
    const int num_iterations = parse_positive_int(argc > 2 ? argv[2] : NULL, 2000);
    int err = 0;

    make_dirs();
    for (int i = 0; i <= num_builders; ++i)
        create_data_file(file_name(i), 1 + i * NUM_LEVELS);

    codes_index* index = codes_index_new_from_file(NULL, file_name(0).c_str(), "level:l", &err);
    if (!index) {
        fprintf(stderr, "Cannot index %s: %s\n", file_name(0).c_str(), codes_get_error_message(err));
        return 1;
    }

    std::thread reader_thread(reader, index);
    std::vector<std::thread> builders;
    for (int i = 1; i <= num_builders; ++i)
        builders.emplace_back(builder, i, num_iterations);
    for (auto& t : builders)
        t.join();
    builders_done = true;
    reader_thread.join();

    codes_index_delete(index);

    printf("GitHub #643: builders=%d iterations=%d failures=%ld\n", num_builders, num_iterations, num_failures.load());
    for (int i = 0; i <= num_builders; ++i)
        if (remove(file_name(i).c_str()) != 0)
            perror(file_name(i).c_str());
    remove_dirs();
    return num_failures == 0 ? 0 : 1;
}
