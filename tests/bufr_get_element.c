/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api_internal.h"
#include "eccodes.h"
#include <assert.h>

static void compare_doubles(const double d1, const double d2, const double epsilon)
{
   assert(fabs(d1 - d2) < epsilon);
}

int main(int argc,char* argv[])
{
    FILE* in = NULL;
    codes_handle* h=NULL;
    size_t values_len=0;
    int elem_index = 0;
    double a_value = 0; /* value at index */
    int err=0;
    long compressed = 0;
    const char* infile = "../data/bufr/mhen_55.bufr";
    const char* key_name = "#5#brightnessTemperature";
    const double tolerance = 1e-6;

    in=fopen(infile,"r");
    assert(in);

    while ((h = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err)) != NULL || err != CODES_SUCCESS)
    {
        assert(h);
        CODES_CHECK(codes_get_long(h, "compressedData", &compressed), 0);
        assert(compressed == 1);

        CODES_CHECK(codes_set_long(h, "unpack", 1),0);

        CODES_CHECK(codes_get_size(h, key_name, &values_len),0);
        assert(values_len == 2070);
        elem_index = values_len - 1; /* last value of key */
        CODES_CHECK(codes_get_double_element(h, key_name, elem_index, &a_value), 0);
        printf("Key = %s, index = %d, value = %.5f\n", key_name, elem_index, a_value);
        compare_doubles(a_value, 237.43, tolerance);

        codes_handle_delete(h);
    }
    fclose(in);
    return 0;
}
