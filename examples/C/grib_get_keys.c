#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "eccodes.h"

int main(int argc, char** argv)
{
    int err           = 0;
    float* fvalues    = NULL;
    double* dvalues    = NULL;
    size_t values_len = 0;
    size_t i = 0;

    double daverage    = 0;
    float  faverage    = 0;

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

    fvalues = (float*)malloc(values_len * sizeof(float));
    dvalues = (double*)malloc(values_len * sizeof(double));

    /* get data values*/
    CODES_CHECK(codes_get_float_array(h, "values", fvalues, &values_len), 0);
    CODES_CHECK(codes_get_double_array(h, "values", dvalues, &values_len), 0);

    daverage = 0;
    faverage = 0;
    for (i = 0; i < values_len; i++){
        //printf("%f\n",values[i]);
        faverage += fvalues[i];
        daverage += dvalues[i];
    }

    daverage /= (double)values_len;
    faverage /= (double)values_len;

    free(dvalues);
    free(fvalues);
    printf("There are %d values, double average is %g\n", (int)values_len, daverage);
    printf("There are %d values, float average is  %f\n", (int)values_len, faverage);

    codes_handle_delete(h);

    return 0;
}
