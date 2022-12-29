#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err           = 0;
    float* values    = NULL;
    size_t values_len = 0;
    size_t i = 0, len = 0;

    double average    = 0;

    FILE* in             = NULL;
    const char* filename = "../../data/sample.grib2";
    codes_handle* h      = NULL;

    in = fopen(filename, "rb");
    if (!in) {
        fprintf(stderr, "Error: unable to open file %s\n", filename);
        return 1;
    }

    /* create new handle from the first message in the file*/
    h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    if (h == NULL) {
        fprintf(stderr, "Error: unable to create handle from file %s\n", filename);
        return 1;
    }
    fclose(in);

    /* get the size of the values array*/
    CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

    values = (float*)malloc(values_len * sizeof(float));

    /* get data values*/
    CODES_CHECK(codes_get_float_array(h, "values", values, &values_len), 0);

    average = 0;
    for (i = 0; i < values_len; i++){
        //printf("%f\n",values[i]);
        average += values[i];
    }

    average /= (double)values_len;

    free(values);
    printf("There are %d values, average is %g\n", (int)values_len, average);

    codes_handle_delete(h);

    return 0;
}
