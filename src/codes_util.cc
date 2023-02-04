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

/* Input lon must be in degrees not radians */
/* Not to be used for latitudes as they can be -ve */
double normalise_longitude_in_degrees(double lon)
{
    while (lon < 0)
        lon += 360;
    while (lon > 360)
        lon -= 360;
    return lon;
}


#ifdef ECCODES_ON_WINDOWS
/* Replace C99/Unix rint() for Windows Visual C++ (only before VC++ 2013 versions) */
#if defined _MSC_VER && _MSC_VER < 1800
double rint(double x)
{
    char* buf   = 0;
    int decimal = 0, sign = 0, err = 0;
    double result = 0;
    buf           = (char*)malloc(_CVTBUFSIZE);
    err           = _fcvt_s(buf, _CVTBUFSIZE, x, 0, &decimal, &sign);
    Assert(err == 0);
    result = atof(buf);
    if (sign == 1) {
        result = result * -1;
    }
    free(buf);
    return result;
}
#endif
#endif


char get_dir_separator_char(void)
{
#ifdef ECCODES_ON_WINDOWS
#define DIR_SEPARATOR_CHAR '\\'
#else
#define DIR_SEPARATOR_CHAR '/'
#endif
    return DIR_SEPARATOR_CHAR;
}

/* Return 1 if the filepath is a regular file, 0 otherwise */
int path_is_regular_file(const char* path)
{
    struct stat s;
    int stat_val = stat(path, &s);
    if (stat_val != 0)
        return 0; /*error doing stat*/
    return S_ISREG(s.st_mode); /* 1 if it's a regular file */
}

/* Return 1 if the filepath is a directory, 0 otherwise */
int path_is_directory(const char* path)
{
    struct stat s;
    int stat_val = stat(path, &s);
    if (stat_val == 0) {
        if (S_ISDIR(s.st_mode)) {
            return 1;
        }
    }
    return 0;
}

char* codes_getenv(const char* name)
{
    /* Look for the new ecCodes environment variable names */
    /* if not found, then look for old grib_api ones for backward compatibility */
    char* result = getenv(name);
    if (result == NULL) {
        const char* old_name = name;

        /* Test the most commonly used variables first */
        if (STR_EQ(name, "ECCODES_SAMPLES_PATH"))
            old_name = "GRIB_SAMPLES_PATH";
        else if (STR_EQ(name, "ECCODES_DEFINITION_PATH"))
            old_name = "GRIB_DEFINITION_PATH";
        else if (STR_EQ(name, "ECCODES_DEBUG"))
            old_name = "GRIB_API_DEBUG";

        else if (STR_EQ(name, "ECCODES_FAIL_IF_LOG_MESSAGE"))
            old_name = "GRIB_API_FAIL_IF_LOG_MESSAGE";
        else if (STR_EQ(name, "ECCODES_GRIB_WRITE_ON_FAIL"))
            old_name = "GRIB_API_WRITE_ON_FAIL";
        else if (STR_EQ(name, "ECCODES_GRIB_LARGE_CONSTANT_FIELDS"))
            old_name = "GRIB_API_LARGE_CONSTANT_FIELDS";
        else if (STR_EQ(name, "ECCODES_NO_ABORT"))
            old_name = "GRIB_API_NO_ABORT";
        else if (STR_EQ(name, "ECCODES_GRIBEX_MODE_ON"))
            old_name = "GRIB_GRIBEX_MODE_ON";
        else if (STR_EQ(name, "ECCODES_GRIB_IEEE_PACKING"))
            old_name = "GRIB_IEEE_PACKING";
        else if (STR_EQ(name, "ECCODES_IO_BUFFER_SIZE"))
            old_name = "GRIB_API_IO_BUFFER_SIZE";
        else if (STR_EQ(name, "ECCODES_LOG_STREAM"))
            old_name = "GRIB_API_LOG_STREAM";
        else if (STR_EQ(name, "ECCODES_GRIB_NO_BIG_GROUP_SPLIT"))
            old_name = "GRIB_API_NO_BIG_GROUP_SPLIT";
        else if (STR_EQ(name, "ECCODES_GRIB_NO_SPD"))
            old_name = "GRIB_API_NO_SPD";
        else if (STR_EQ(name, "ECCODES_GRIB_KEEP_MATRIX"))
            old_name = "GRIB_API_KEEP_MATRIX";
        else if (STR_EQ(name, "_ECCODES_ECMWF_TEST_DEFINITION_PATH"))
            old_name = "_GRIB_API_ECMWF_TEST_DEFINITION_PATH";
        else if (STR_EQ(name, "_ECCODES_ECMWF_TEST_SAMPLES_PATH"))
            old_name = "_GRIB_API_ECMWF_TEST_SAMPLES_PATH";
        else if (STR_EQ(name, "ECCODES_GRIB_JPEG"))
            old_name = "GRIB_JPEG";
        else if (STR_EQ(name, "ECCODES_GRIB_DUMP_JPG_FILE"))
            old_name = "GRIB_DUMP_JPG_FILE";
        else if (STR_EQ(name, "ECCODES_PRINT_MISSING"))
            old_name = "GRIB_PRINT_MISSING";

        result = getenv(old_name);
    }
    return result;
}

int codes_check_grib_ieee_packing_value(int value)
{
    grib_context* c = grib_context_get_default();
    if (value != 32 && value != 64) {
        grib_context_log(c, GRIB_LOG_ERROR, "Invalid value for ECCODES_GRIB_IEEE_PACKING: should be 32 or 64");
        return GRIB_INVALID_ARGUMENT;
    }
    return GRIB_SUCCESS;
}

/* Note: To be called in cases where we are WRITING a file (Do not call when reading) */
int codes_flush_sync_close_file(FILE* f)
{
    int err = 0;
    int fd  = 0;
    grib_context* c = grib_context_get_default();
    Assert(f);

    fd = fileno(f);
    if (fd == -1) {
        grib_context_log(c, GRIB_LOG_PERROR, "fileno() error: invalid stream");
        return GRIB_IO_PROBLEM;
    }

#if 0
#ifdef HAVE_FCNTL_H
    /* Heavy handed way of getting the file access mode: only proceed if writing */
    val = fcntl(fd, F_GETFL, 0);
    if (val < 0) {
        grib_context_log(c, GRIB_LOG_PERROR, "Call to fcntl failed");
        return err;
    }
    if ((val & O_ACCMODE) != O_WRONLY) {
        /* File is not being written */
        return GRIB_SUCCESS;
    }
#endif
#endif

    err = fflush(f);
    if (err) {
        grib_context_log(c, GRIB_LOG_PERROR, "Call to fflush failed");
        return err;
    }

#if defined(ECCODES_HAVE_FSYNC)
    err = fsync(fd);
    while (err < 0 && errno == EINTR) {
        err = fsync(fd);
    }
    if (err < 0) {
        grib_context_log(c, GRIB_LOG_PERROR, "Cannot fsync file");
        return GRIB_IO_PROBLEM;
    }
#endif

    err = fclose(f);
    if (err) {
        grib_context_log(c, GRIB_LOG_PERROR, "Call to fclose failed");
        return err;
    }
    return GRIB_SUCCESS;
}
