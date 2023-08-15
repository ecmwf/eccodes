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
#include <iomanip>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <cmath>

typedef std::numeric_limits<double> dbl;
typedef std::numeric_limits<float> flt;

struct Range
{
    double min;
    double max;
};


//
// Test: This test doesn't accept errors after decompression.
//
int main(int argc, char** argv)
{
    std::vector<Range> ranges;
    ranges.push_back({ flt::max() / 10, flt::max() });
    ranges.push_back({ flt::max() / 100, flt::max() / 10 });
    ranges.push_back({ 1e+9, 1e+10 });
    ranges.push_back({ 1e+0, 1e+1 });
    ranges.push_back({ 1e-1, 1e-0 });
    ranges.push_back({ 1e-2, 1e-1 });
    ranges.push_back({ 1e-11, 1e-10 });
    ranges.push_back({ flt::min() * 10, flt::min() * 100 });
    ranges.push_back({ flt::min(), flt::min() * 10 });
    ranges.push_back({ flt::min(), flt::max() });

    std::vector<size_t> values_lens = {
        1,
        10,
        100,
        100000,
    };

    int err;

    std::default_random_engine re;
    size_t in_values_ecc_1612_workaround_len = 0;
    size_t grid_simple_values_len = 0;
    size_t grid_ccsds_values_len  = 0;
    std::string packing_type      = "";
    size_t size                   = 0;

    for (const size_t in_values_len : values_lens) {
        for (const Range range : ranges) {
            Assert(range.min < range.max);
            std::cout << "Testing: "
                << "n_vals : " << std::setw(7) << in_values_len
                << " range(" << std::scientific << std::setw(12) << range.min << ", " << std::setw(12) << range.max << ") "
                << std::endl;
            std::uniform_real_distribution<double>
            unif(range.min, range.max);

            codes_handle* handle       = codes_grib_handle_new_from_samples(0, "reduced_gg_pl_128_grib2");
            double* in_values          = new double[in_values_len];
            double* in_values_ecc_1612_workaround = new double[in_values_len];
            double* grid_simple_values = new double[in_values_len];
            double* grid_ccsds_values  = new double[in_values_len];

            in_values_ecc_1612_workaround_len = in_values_len;
            grid_simple_values_len = in_values_len;
            grid_ccsds_values_len  = in_values_len;

            // Initialize with random values
            for (size_t i = 0; i < in_values_len; ++i) {
                in_values[i] = unif(re);
            }

            // Convert original values to quantized values using grid_simple
            { // Workaround ECC-1612. Can be removed when fixed. Use code below instead.
                CODES_CHECK(codes_set_double_array(handle, "values", in_values, in_values_len), 0);
                CODES_CHECK(codes_get_double_array(handle, "values", in_values_ecc_1612_workaround, &in_values_ecc_1612_workaround_len), 0);
                CODES_CHECK(codes_set_double_array(handle, "values", in_values_ecc_1612_workaround, in_values_ecc_1612_workaround_len), 0);
                CODES_CHECK(codes_get_double_array(handle, "values", grid_simple_values, &grid_simple_values_len), 0);
                //CODES_CHECK(codes_set_double_array(handle, "values", in_values, in_values_len), 0);
                //CODES_CHECK(codes_get_double_array(handle, "values", grid_simple_values, &grid_simple_values_len), 0);
            }

            Assert(in_values_len == grid_simple_values_len);

            // Test grid_ccsds
            packing_type = "grid_ccsds";
            size         = packing_type.size();
            CODES_CHECK(codes_set_string(handle, "packingType", packing_type.c_str(), &size), 0);
            if ((err = codes_set_double_array(handle, "values", grid_simple_values, grid_simple_values_len)) != 0) {
                Assert(!"CCSDS encoding failed");
            }
            if ((err = codes_get_double_array(handle, "values", grid_ccsds_values, &grid_ccsds_values_len)) != 0) {
                Assert(!"CCSDS decoding failed");
            }
            Assert(grid_ccsds_values_len == grid_simple_values_len);

            // Test buffers
            for (size_t i = 0; i < grid_ccsds_values_len; ++i) {
                if (grid_ccsds_values[i] != grid_simple_values[i]) {
                    std::cout.precision(dbl::max_digits10);
                    std::cout << "Test failed: " << grid_ccsds_values[i] << " != " << grid_simple_values[i] << std::endl;
                    Assert(0);
                }
            }

            codes_handle_delete(handle);

            delete[] in_values;
            delete[] in_values_ecc_1612_workaround;
            delete[] grid_simple_values;
            delete[] grid_ccsds_values;
        }
    }
    return 0;
}
