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

typedef enum ResourceType
{
    UNKNOWN,
    SAMPLE,
    DEFINITION
} ResourceType;

static void usage(const char* prog)
{
    printf("Usage: %s [-s | -d] resource_path out_file\n", prog);
    printf("       -s Extract a resource from the samples directory\n");
    printf("       -d Extract a resource from the definitions directory\n");
    printf("\n");
    printf("       E.g., %s -s GRIB2.tmpl sample.grib2\n", prog);
    printf("       E.g., %s -d grib2/boot.def grib2_boot.txt\n", prog);

    char v[32] = {0,};
    snprintf(v, sizeof(v), "%d.%d.%d", ECCODES_MAJOR_VERSION, ECCODES_MINOR_VERSION, ECCODES_REVISION_VERSION);
    printf("\nTo access all the definitions/samples for ecCodes version %s:\n", v);
    printf("\tgit clone --depth=1 git@github.com:ecmwf/eccodes -b %s eccodes_v%s\n", v, v);
    printf("\tls eccodes_v%s/definitions\n", v);
    printf("\tls eccodes_v%s/samples\n", v);
    exit(1);
}

#define SIZE (1024 * 1024)
int main(int argc, char* argv[])
{
    char* resource_path        = NULL;
    const char* resource_name  = NULL;
    ResourceType resource_type = UNKNOWN;
    char* full_path            = NULL;
    const char* out_file       = NULL;
    const char* option         = NULL;
    grib_context* c            = grib_context_get_default();
    FILE* fin                  = NULL;
    FILE* fout                 = NULL;
    char buffer[SIZE]          = {0,};
    size_t bytes = 0;

    if (argc != 4) usage(argv[0]);

    option        = argv[1];
    resource_path = argv[2];
    out_file      = argv[3];

    if (strcmp(option, "-s") == 0) {
        resource_type = SAMPLE;
        resource_name = "sample";
    }
    else if (strcmp(option, "-d") == 0) {
        resource_type = DEFINITION;
        resource_name = "definition";
    }
    else {
        fprintf(stderr, "Error: Invalid option: Specify either -s or -d\n");
        return 1;
    }

    if (resource_type == SAMPLE) {
        full_path = get_external_sample_path(c, resource_path);
    }
    else if (resource_type == DEFINITION) {
        full_path = grib_context_full_defs_path(c, resource_path);
    }
    if (!full_path) {
        fprintf(stderr, "Error: Failed to access %s file: '%s'\n", resource_name, resource_path);
        return 1;
    }

    fout = fopen(out_file, "wb");
    if (!fout) {
        fprintf(stderr, "Error: Failed to open output file '%s'\n", out_file);
        perror(out_file);
        return 1;
    }
    fin = codes_fopen(full_path, "r");
    if (!fin) {
        fprintf(stderr, "Error: Failed to open resource '%s'\n", full_path);
        fclose(fout);
        return 1;
    }

    /* write resource bytes to fout */
    int read_count = 0;
    while (0 < (bytes = fread(buffer, 1, sizeof(buffer), fin))) {
        if (fwrite(buffer, 1, bytes, fout) != bytes) {
            fprintf(stderr, "Error: Failed to write out bytes\n");
            return 1;
        }
        ++read_count;
    }

    if (fclose(fin) != 0) {
        fprintf(stderr, "Error: Call to fclose failed (input)\n");
        return 1;
    }
    if (fclose(fout) != 0) {
        fprintf(stderr, "Error: Call to fclose failed (output)\n");
        return 1;
    }

    grib_context_free(c, full_path);

    if (read_count == 0) {
        fprintf(stderr, "Error: Number of bytes read == 0. Please check the path refers to a regular %s file.\n",
                        (resource_type == SAMPLE) ? "sample" : "definition");
    } else {
        printf("Resource exported to file '%s'.\n", out_file);
    }

    return 0;
}
