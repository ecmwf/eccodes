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
#include <eccodes.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <cmath>

typedef std::numeric_limits<double> dbl;
typedef std::numeric_limits<float> flt;

int main(int argc, char** argv)
{
    size_t values_len = 10;
    std::default_random_engine re;
    std::uniform_real_distribution<double> unif(flt::min(), flt::min() * 10);

    codes_handle* handle      = codes_grib_handle_new_from_samples(0, "reduced_gg_pl_128_grib2");
    double* values            = new double[values_len];
    double* grid_ccsds_values = new double[values_len];

    // Initialize with small random values
    for (size_t i = 0; i < values_len; ++i) {
        values[i] = unif(re);
    }

    // Test grid_ccsds
    std::string packing_type = "grid_ccsds";
    size_t size              = packing_type.size();
    CODES_CHECK(codes_set_double_array(handle, "values", values, values_len), 0);
    CODES_CHECK(codes_set_string(handle, "packingType", packing_type.c_str(), &size), 0);
    CODES_CHECK(codes_get_double_array(handle, "values", grid_ccsds_values, &values_len), 0);

    // Test buffers
    double tolerance = 0.000001;
    for (size_t i = 0; i < values_len; ++i) {
        if (!((grid_ccsds_values[i] < (values[i] * (1 + tolerance))) &&
              grid_ccsds_values[i] > (values[i] / (1 + tolerance)))) {
            std::cout.precision(dbl::max_digits10);
            std::cout << "Test failed: " << grid_ccsds_values[i] << " != " << values[i] << std::endl;
            Assert(0);
        }
    }

    codes_handle_delete(handle);

    delete[] values;
    delete[] grid_ccsds_values;
    return 0;
}
