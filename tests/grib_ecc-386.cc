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
 * Description: Reads a GRIB message from file, measures time taken
 */


#include "grib_api_internal.h"

#include <chrono>
#include <iostream>
using namespace std::chrono;
using std::cout, std::cerr, std::endl;

int main(int argc, char** argv)
{
    int err = 0;
    size_t values_len = 0;
    double* values = NULL;
    #ifdef DEBUG
    const auto duration_max = std::chrono::milliseconds(1500);
    #else
    const auto duration_max = std::chrono::milliseconds(600);
    #endif
    const int num_repetitions = 100;

    if (argc < 2) return 1;

    FILE* in = fopen(argv[1], "rb");
    ECCODES_ASSERT(in);

    // create new handle
    err = 0;
    grib_handle* h = grib_handle_new_from_file(0, in, &err);
    ECCODES_ASSERT(h);

    // get the size of the values array
    GRIB_CHECK(grib_get_size(h, "values", &values_len), 0);

    values = (double*)malloc(values_len * sizeof(double));

    // get data values and time it
    auto start = high_resolution_clock::now();
    for (int i = 0; i < num_repetitions; i++) {
        GRIB_CHECK(grib_get_double_array(h, "values", values, &values_len), 0);
    }
    auto stop = high_resolution_clock::now();
    auto duration_actual = duration_cast<milliseconds>(stop - start);

    if (duration_actual > duration_max) {
        cerr << "ERROR: Decoding took longer than expected! actual time=" << duration_actual.count() <<
                " ms, expected " << duration_max.count() << " ms" << endl;
        return 1;
    }
    cout << "Test passed. Actual decode time  = " << duration_actual.count() << " ms" <<endl;
    cout << "Test passed. Maximum decode time = " << duration_max.count() << " ms" <<endl;
    free(values);
    grib_handle_delete(h);
    fclose(in);
    return 0;
}
