/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_tools.h"

extern char* optarg;
extern int optind;

#ifdef ECCODES_ON_WINDOWS
#include "wingetopt.h"
#endif

static void usage_and_exit(const char* progname)
{
    printf("\nUsage: %s [-v] [-d] [-s]\n", progname);
    exit(1);
}

#define INFO_PRINT_ALL 0
#define INFO_PRINT_VERSION (1 << 0)
#define INFO_PRINT_DEFINITION_PATH (1 << 1)
#define INFO_PRINT_SAMPLES_PATH (1 << 2)

static void print_debug_info(grib_context* context)
{
    int memfs = 0, aec = 0;
#ifdef HAVE_MEMFS
    memfs = 1;
#endif
#ifdef HAVE_AEC
    aec = 1;
#endif
    grib_context_log(context, GRIB_LOG_DEBUG, "Git SHA1=%s", grib_get_git_sha1());
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_JPEG=%d", HAVE_JPEG);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_LIBJASPER=%d", HAVE_LIBJASPER);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_LIBOPENJPEG=%d", HAVE_JPEG);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_LIBPNG=%d", HAVE_LIBPNG);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_AEC=%d", aec);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_ECCODES_THREADS=%d", GRIB_PTHREADS);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_ECCODES_OMP_THREADS=%d", GRIB_OMP_THREADS);
    grib_context_log(context, GRIB_LOG_DEBUG, "HAVE_MEMFS=%d", memfs);
}

int main(int argc, char* argv[])
{
    char* path                = NULL;
    int nfiles                = 0;
    unsigned long print_flags = 0;
    int major                 = ECCODES_MAJOR_VERSION;
    int minor                 = ECCODES_MINOR_VERSION;
    int revision              = ECCODES_REVISION_VERSION;
    grib_context* context     = grib_context_get_default();

    while (1) {
        int c = getopt(argc, argv, "vds");

        if (c == -1)
            break;

        switch (c) {
            case 'v':
                print_flags |= INFO_PRINT_VERSION;
                break;
            case 'd':
                print_flags |= INFO_PRINT_DEFINITION_PATH;
                break;
            case 's':
                print_flags |= INFO_PRINT_SAMPLES_PATH;
                break;
            default:
                usage_and_exit(argv[0]);
        }
    }

    nfiles = argc - optind;
    if (nfiles != 0)
        usage_and_exit(argv[0]);

    if (print_flags == INFO_PRINT_ALL) {
        print_debug_info(context);
        printf("\n");
        printf("%s Version %d.%d.%d",
               grib_get_package_name(), major, minor, revision);

        if (ECCODES_MAJOR_VERSION < 1)
            printf(" PRE-RELEASE");

        printf("\n");
        printf("\n");

        if ((path = getenv("ECCODES_DEFINITION_PATH")) != NULL) {
            printf("Definition files path from environment variable");
            printf(" ECCODES_DEFINITION_PATH=%s\n", path);
            printf("Full definition files path=%s\n", context->grib_definition_files_path);
        }
        else if ((path = getenv("GRIB_DEFINITION_PATH")) != NULL) {
            printf("Definition files path from environment variable");
            printf(" GRIB_DEFINITION_PATH=%s\n", path);
            printf(
                "(This is for backward compatibility. "
                "It is recommended you use ECCODES_DEFINITION_PATH instead!)\n");
            printf("Full definition files path=%s\n", context->grib_definition_files_path);
        }
        else {
            printf("Default definition files path is used: %s\n", context->grib_definition_files_path);
            printf("Definition files path can be changed by setting the ECCODES_DEFINITION_PATH environment variable.\n");
        }

        if((path = getenv("ECCODES_EXTRA_DEFINITION_PATH")) != NULL) {
            printf("Environment variable ECCODES_EXTRA_DEFINITION_PATH=%s\n", path);
        }
        printf("\n");

        if ((path = getenv("ECCODES_SAMPLES_PATH")) != NULL) {
            printf("SAMPLES path from environment variable");
            printf(" ECCODES_SAMPLES_PATH=%s\n", path);
        }
        else if ((path = getenv("GRIB_SAMPLES_PATH")) != NULL) {
            printf("SAMPLES path from environment variable");
            printf(" GRIB_SAMPLES_PATH=%s\n", path);
            printf(
                "(This is for backward compatibility. "
                "It is recommended you use ECCODES_SAMPLES_PATH instead!)\n");
        }
        else {
            printf("Default SAMPLES path is used: %s\n", context->grib_samples_path);
            printf("SAMPLES path can be changed by setting the ECCODES_SAMPLES_PATH environment variable.\n");
        }
        if ((path = getenv("ECCODES_EXTRA_SAMPLES_PATH")) != NULL) {
            printf("Environment variable ECCODES_EXTRA_SAMPLES_PATH=%s\n", path);
        }
        printf("\n");
        return 0;
    }

    if (print_flags & INFO_PRINT_VERSION)
        printf("%d.%d.%d", major, minor, revision);

    if (print_flags & INFO_PRINT_DEFINITION_PATH) {
        if ((path = codes_getenv("ECCODES_DEFINITION_PATH")) != NULL) {
            printf("%s", path);
        }
        else {
            printf("%s", context->grib_definition_files_path);
        }
    }

    if (print_flags & INFO_PRINT_SAMPLES_PATH) {
        if ((path = codes_getenv("ECCODES_SAMPLES_PATH")) != NULL) {
            printf("%s", path);
        }
        else {
            printf("%s", context->grib_samples_path);
        }
    }

    return 0;
}
