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
#include "string_util.h"

static const size_t MIN_NUM_COLUMNS = 8;
static const size_t NUM_DESCRIPTOR_DIGITS = 6; /* FXY */

#define NUMBER(a) (sizeof(a) / sizeof(a[0]))
static const char* allowed_types[] = { "long", "double", "table", "flag", "string" };

static int check_descriptor_type(const char* atype)
{
    size_t i = 0, numTypes = NUMBER(allowed_types);
    for (i = 0; i < numTypes; ++i) {
        if (strcmp(atype, allowed_types[i]) == 0) {
            return GRIB_SUCCESS;
        }
    }
    return GRIB_INVALID_ARGUMENT;
}

int main(int argc, char** argv)
{
    char* filename  = NULL;
    FILE* fp        = NULL;
    char line[1024] = {0,};
    size_t i = 0, line_number = 0;
    long lValue     = 0;
    std::string str_code; /* descriptor */
    std::string str_key;
    std::string str_type;
    std::string str_scale;
    std::string str_ref;
    std::string str_width;
    std::string str_units;
    bufr_descriptor v;
    const size_t maxlen_keyName = sizeof(v.shortName);
    const size_t maxlen_units   = sizeof(v.units);
    List list;

    ECCODES_ASSERT(argc == 2);

    filename = argv[1];
    fp       = fopen(filename, "r");
    ECCODES_ASSERT(fp);

    while (fgets(line, sizeof(line) - 1, fp)) {
        ++line_number;
        ECCODES_ASSERT(strlen(line) > 0);
        if (line[0] == '#') continue; /* Ignore first line with column titles */
        list = string_split(line, "|");
        if (list.empty()) {
            fprintf(stderr, "Error on line %zu: string_split failed!\n", line_number);
            return 1;
        }
        if (list.size() < MIN_NUM_COLUMNS) {
            fprintf(stderr, "Error on line %zu: Number of columns (=%zu) < required miniumum (=%zu)!\n",
                    line_number, i, MIN_NUM_COLUMNS);
            return 1;
        }
        str_code = list[0];
        if (string_to_long(str_code.c_str(), &lValue, 1) != GRIB_SUCCESS) {
            fprintf(stderr, "Error on line %zu: descriptor code '%s' (column 1) is not numeric.\n",
                    line_number, str_code.c_str());
            return 1;
        }
        if (str_code.size() != NUM_DESCRIPTOR_DIGITS) {
            fprintf(stderr, "Error on line %zu: descriptor code '%s' (column 1) is not %zu digits.\n",
                    line_number, str_code.c_str(), NUM_DESCRIPTOR_DIGITS);
            return 1;
        }
        str_key  = list[1];
        str_type = list[2];
        if (check_descriptor_type(str_type.c_str()) != GRIB_SUCCESS) {
            fprintf(stderr, "Error on line %zu: descriptor key type '%s' (column 3) is not valid.\n",
                    line_number, str_type.c_str());
            fprintf(stderr, "Please choose one of:\n");
            for (i = 0; i < NUMBER(allowed_types); ++i) {
                fprintf(stderr, "\t%s\n", allowed_types[i]);
            }
            return 1;
        }
        if (str_key.size() >= maxlen_keyName) {
            fprintf(stderr, "Error on line %zu: descriptor key name '%s' (column 2) exceeds %zu characters.\n",
                    line_number, str_key.c_str(), maxlen_keyName);
            return 1;
        }
        str_units = list[4];
        if (str_units.size() >= maxlen_units) {
            fprintf(stderr, "Error on line %zu: descriptor units '%s' (column 5) exceeds %zu characters.\n",
                    line_number, str_units.c_str(), maxlen_units);
            return 1;
        }
        str_scale = list[5];
        str_ref   = list[6];
        str_width = list[7];
        if (string_to_long(str_scale.c_str(), &lValue, 1) != GRIB_SUCCESS) {
            fprintf(stderr, "Error on line %zu: descriptor scale '%s' (column 6) is not numeric.\n",
                    line_number, str_scale.c_str());
            return 1;
        }
        if (string_to_long(str_ref.c_str(), &lValue, 1) != GRIB_SUCCESS) {
            fprintf(stderr, "Error on line %zu: descriptor reference '%s' (column 7) is not numeric.\n",
                    line_number, str_ref.c_str());
            return 1;
        }
        // The final width column can have spaces etc at the end. So turn off strict mode
        if (string_to_long(str_width.c_str(), &lValue, /*strict=*/0) != GRIB_SUCCESS) {
            fprintf(stderr, "Error on line %zu: descriptor width '%s' (column 8) is not numeric.\n",
                    line_number, str_width.c_str());
            return 1;
        }
    }

    fclose(fp);

    grib_context_delete(grib_context_get_default());
    return 0;
}
