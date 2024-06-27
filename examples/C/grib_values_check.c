/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "eccodes.h"

int main(int argc, char* argv[])
{
    FILE* f;
    int err          = 0;
    char infile[]    = "../../data/reduced_gaussian_model_level.grib1";
    codes_handle* h  = NULL;
    codes_context* c = codes_context_get_default();
    codes_values values[4];
    const int nvalues = sizeof(values)/sizeof(values[0]);
    int i = 0;
    char* name = NULL;

    f = fopen(infile, "rb");
    if (!f) {
        perror(infile);
        return 1;
    }

    h = codes_handle_new_from_file(c, f, PRODUCT_GRIB, &err);
    if (!h) {
        fprintf(stderr, "unable to create handle from file %s\n", infile);
        return 1;
    }
    fclose(f);

    values[i].type       = CODES_TYPE_LONG;
    values[i].name       = "centre";
    values[i].long_value = 98;
    i++;

    values[i].type       = CODES_TYPE_DOUBLE;
    values[i].name       = "bitsPerValue";
    values[i].double_value = 13;
    i++;

    values[i].type       = CODES_TYPE_STRING;
    values[i].name       = "identifier";
    values[i].string_value = "GRIB";
    i++;

    values[i].type       = CODES_TYPE_LONG;
    values[i].name       = "level";
    values[i].long_value = 2; /* Intentionally made different; actual level==1 */

    err = codes_values_check(h, values, nvalues);
    if (err) {
        for (i = 0; i < nvalues; i++) {
            printf("i=%d %s\n",i, values[i].name);
            if (values[i].error == err) name = (char*)values[i].name;
        }
        fprintf(stderr, "Error: \"%s\" - %s\n", name, codes_get_error_message(err));
    }

    values[1].name = "levelll"; /* non-existent key */
    err = codes_values_check(h, values, nvalues);
    if (err) {
        for (i = 0; i < nvalues; i++) {
            if (values[i].error == err) name = (char*)values[i].name;
        }
        fprintf(stderr, "Error: \"%s\" - %s\n", name, codes_get_error_message(err));
    }

    codes_handle_delete(h);

    return 0;
}
