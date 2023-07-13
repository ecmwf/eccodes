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

// Input lon must be in degrees not radians
// Not to be used for latitudes as they can be -ve
double normalise_longitude_in_degrees(double lon)
{
    while (lon < 0)
        lon += 360;
    while (lon > 360)
        lon -= 360;
    return lon;
}


#ifdef ECCODES_ON_WINDOWS
// Replace C99/Unix rint() for Windows Visual C++ (only before VC++ 2013 versions)
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

// Return 1 if the filepath is a regular file, 0 otherwise
int path_is_regular_file(const char* path)
{
    struct stat s;
    int stat_val = stat(path, &s);
    if (stat_val != 0)
        return 0; // error doing stat
    return S_ISREG(s.st_mode); // 1 if it's a regular file
}

// Return 1 if the filepath is a directory, 0 otherwise
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
    // Look for the new ecCodes environment variable names
    // if not found, then look for old grib_api ones for backward compatibility
    char* result = getenv(name);
    if (result == NULL) {
        const char* old_name = name;

        // Test the most commonly used variables first
        if (STR_EQUAL(name, "ECCODES_SAMPLES_PATH"))
            old_name = "GRIB_SAMPLES_PATH";
        else if (STR_EQUAL(name, "ECCODES_DEFINITION_PATH"))
            old_name = "GRIB_DEFINITION_PATH";
        else if (STR_EQUAL(name, "ECCODES_DEBUG"))
            old_name = "GRIB_API_DEBUG";

        else if (STR_EQUAL(name, "ECCODES_FAIL_IF_LOG_MESSAGE"))
            old_name = "GRIB_API_FAIL_IF_LOG_MESSAGE";
        else if (STR_EQUAL(name, "ECCODES_GRIB_WRITE_ON_FAIL"))
            old_name = "GRIB_API_WRITE_ON_FAIL";
        else if (STR_EQUAL(name, "ECCODES_GRIB_LARGE_CONSTANT_FIELDS"))
            old_name = "GRIB_API_LARGE_CONSTANT_FIELDS";
        else if (STR_EQUAL(name, "ECCODES_NO_ABORT"))
            old_name = "GRIB_API_NO_ABORT";
        else if (STR_EQUAL(name, "ECCODES_GRIBEX_MODE_ON"))
            old_name = "GRIB_GRIBEX_MODE_ON";
        else if (STR_EQUAL(name, "ECCODES_GRIB_IEEE_PACKING"))
            old_name = "GRIB_IEEE_PACKING";
        else if (STR_EQUAL(name, "ECCODES_IO_BUFFER_SIZE"))
            old_name = "GRIB_API_IO_BUFFER_SIZE";
        else if (STR_EQUAL(name, "ECCODES_LOG_STREAM"))
            old_name = "GRIB_API_LOG_STREAM";
        else if (STR_EQUAL(name, "ECCODES_GRIB_NO_BIG_GROUP_SPLIT"))
            old_name = "GRIB_API_NO_BIG_GROUP_SPLIT";
        else if (STR_EQUAL(name, "ECCODES_GRIB_NO_SPD"))
            old_name = "GRIB_API_NO_SPD";
        else if (STR_EQUAL(name, "ECCODES_GRIB_KEEP_MATRIX"))
            old_name = "GRIB_API_KEEP_MATRIX";
        else if (STR_EQUAL(name, "_ECCODES_ECMWF_TEST_DEFINITION_PATH"))
            old_name = "_GRIB_API_ECMWF_TEST_DEFINITION_PATH";
        else if (STR_EQUAL(name, "_ECCODES_ECMWF_TEST_SAMPLES_PATH"))
            old_name = "_GRIB_API_ECMWF_TEST_SAMPLES_PATH";
        else if (STR_EQUAL(name, "ECCODES_GRIB_JPEG"))
            old_name = "GRIB_JPEG";
        else if (STR_EQUAL(name, "ECCODES_GRIB_DUMP_JPG_FILE"))
            old_name = "GRIB_DUMP_JPG_FILE";
        else if (STR_EQUAL(name, "ECCODES_PRINT_MISSING"))
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

// Note: To be called in cases where we are WRITING a file (Do not call when reading)
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

#ifdef HAVE_FCNTL_H
    // Heavy handed way of getting the file access mode: only proceed if writing
    //val = fcntl(fd, F_GETFL, 0);
    //if (val < 0) {
    //    grib_context_log(c, GRIB_LOG_PERROR, "Call to fcntl failed");
    //    return err;
    //}
    //if ((val & O_ACCMODE) != O_WRONLY) {
    //    // File is not being written
    //    return GRIB_SUCCESS;
    //}
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

// Return 1 if input date is valid. Otherwise 0
int is_date_valid(long year, long month, long day, long hour, long minute, double second)
{
    // Convert input date to Julian number
    double result = 0; // Julian number in units of days
    long year1, month1, day1, hour1, minute1, lSecond1;

    // For validating the date/time, we specify seconds as an integer
    long lSecond = (long)second;
    grib_datetime_to_julian(year, month, day, hour, minute, lSecond, &result);

    // Check conversion worked by going other way
    grib_julian_to_datetime(result, &year1, &month1, &day1, &hour1, &minute1, &lSecond1);
    if (year1 != year || month1 != month || day1 != day || minute1 != minute || lSecond1 != lSecond) {
        return 0; // bad date
    }

    return 1;
}

static float float_epsilon(void)
{
    float floatEps = 1.0;
    while (1 + floatEps / 2 != 1)
        floatEps /= 2;
    return floatEps;
}

static int is_approximately_equal(double a, double b, double epsilon)
{
    if (a == b)
        return 1;
    if (fabs(a - b) <= epsilon)
        return 1;
    return 0;
}

static double eval_value_factor(int64_t value, int64_t factor)
{
    return (double)value * pow(10.0, -factor);
}

static int compute_scaled_value_and_scale_factor_algorithm1(
    double input, int64_t maximum_value, int64_t maximum_factor,
    int64_t* ret_value, int64_t* ret_factor)
{
    int64_t factor = 0;
    int64_t value = 0;

    if (input == 0) {
        *ret_factor = *ret_value = 0;
        return GRIB_SUCCESS;
    }

    factor = floor(log10(maximum_value)) - floor(log10(input < 0 ? -input : input));
    value = (int64_t)round(input * pow(10, factor));
    while ((value % 10 == 0) && (factor > 0)) {
        value /= 10;
        factor--;
    }
    if (value >= maximum_value)
        return GRIB_INTERNAL_ERROR;
    if (factor > maximum_factor)
        return GRIB_INTERNAL_ERROR;

    *ret_factor = factor;
    *ret_value = value;
    return GRIB_SUCCESS;
}

static int compute_scaled_value_and_scale_factor_algorithm2(
    double input, int64_t maximum_value, int64_t maximum_factor,
    int64_t* ret_value, int64_t* ret_factor)
{
    int64_t factor = 0, prev_factor = 0;
    int64_t value = 0, prev_value = 0;
    double exact        = input;
    const float epsilon = float_epsilon();
    int is_negative = 0;
    // Loop until we find a close enough approximation. Keep the last good values
    if (exact < 0) {
        is_negative = 1;
        exact *= -1;
    }
    factor = prev_factor = 0;
    value = prev_value = round(exact);
    while (!is_approximately_equal(exact, eval_value_factor(value, factor), epsilon) &&
           value < maximum_value &&
           factor < maximum_factor) {
        value = round(exact * pow(10., ++factor));
        if (value > maximum_value || factor > maximum_factor) {
            // One or more maxima exceeded. So stop and use the previous values
            value  = prev_value;
            factor = prev_factor;
            break;
        }
        prev_factor = factor;
        prev_value  = value;
    }

    if (is_negative) {
        value *= -1;
    }

    if (value == 0)
        return GRIB_INTERNAL_ERROR;

    *ret_factor = factor;
    *ret_value = value;

    return GRIB_SUCCESS;
}

// Return GRIB_SUCCESS if no error and set the two outputs 'ret_value' and 'ret_factor'.
// ret_value cannot exceed scaled_value_max and ret_factor cannot exceed scale_factor_max
int compute_scaled_value_and_scale_factor(
    double input, int64_t scaled_value_max, int64_t scale_factor_max,
    int64_t* ret_value, int64_t* ret_factor)
{
    int err = compute_scaled_value_and_scale_factor_algorithm1(input, scaled_value_max, scale_factor_max, ret_value, ret_factor);
    if (err) {
        err = compute_scaled_value_and_scale_factor_algorithm2(input, scaled_value_max, scale_factor_max, ret_value, ret_factor);
    }
    return err;
}
