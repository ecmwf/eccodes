#include "grib_api_internal.h"
#include <eccodes.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>

typedef std::numeric_limits<double> dbl;

typedef struct Bounds {
    double lower;
    double upper;
} Bounds;

int main(int argc, char** argv) {
    std::vector<Bounds> bounds;
    //bounds.push_back({1e+100, 1e+99});  [> fails <]
    bounds.push_back({1e+10, 1e+9});
    bounds.push_back({1e+1, 1e+0});
    bounds.push_back({1e-0, 1e-1});
    bounds.push_back({1e-1, 1e-2});
    bounds.push_back({1e-10, 1e-11});
    bounds.push_back({1e-180, 1e-181});

    std::vector<size_t> values_lens = {
        1, 
        10, 
        100, 
        100000, 
        10000000,
    };

    int err;
    std::default_random_engine re;
    size_t grid_simple_values_len = 0;
    size_t grid_ccsds_values_len = 0;
    std::string packing_type = "";
    size_t size = packing_type.size();

    for (const auto in_values_len: values_lens) {
        for (const auto bound: bounds) {
            Assert(bound.upper <= bound.lower);
            std::cout << "Testing: " << in_values_len << " " << bound.lower << " " << bound.upper << std::endl;
            std::uniform_real_distribution<double> unif(bound.lower, bound.upper);

            codes_handle* handle = codes_grib_handle_new_from_samples(0, "reduced_gg_pl_128_grib2");
            double* in_values = new double[in_values_len];
            double* grid_simple_values = new double[in_values_len];
            double* grid_ccsds_values = new double[in_values_len];

            grid_simple_values_len = in_values_len;
            grid_ccsds_values_len = in_values_len;

            /* Initialize with random values */
            for (size_t i = 0; i < in_values_len; ++i) {
                in_values[i] = unif(re);
            }

            /* Convert original values to quantized values, by grid_simple */
            packing_type = "grid_simple";
            size = packing_type.size();
            CODES_CHECK(codes_set_string(handle, "packingType", packing_type.c_str(), &size), 0);
            CODES_CHECK(codes_set_long(handle, "numberOfValues", in_values_len), 0);
            CODES_CHECK(codes_set_double_array(handle, "values", in_values, in_values_len), 0);
            CODES_CHECK(codes_get_double_array(handle, "values", grid_simple_values, &grid_simple_values_len), 0);
            Assert(in_values_len == grid_simple_values_len);

            /* Test grid_ccsds */
            packing_type = "grid_ccsds";
            size = packing_type.size();
            CODES_CHECK(codes_set_string(handle, "packingType", packing_type.c_str(), &size), 0);
            if ((err = codes_set_double_array(handle, "values", grid_simple_values, grid_simple_values_len)) != 0) {
                Assert(!"CCSDS encoding failed");
            }

            if ((err = codes_get_double_array(handle, "values", grid_ccsds_values, &grid_ccsds_values_len)) != 0) {
                Assert(!"CCSDS decoding failed");
            }
            Assert(grid_ccsds_values_len == grid_simple_values_len);

            /* Compare grid_ccsds and grid_simple buffers */
            for (size_t i; i < grid_ccsds_values_len; ++i) {
                if (grid_ccsds_values[i] != grid_simple_values[i]) {
                    std::cout.precision(dbl::max_digits10);
                    std::cout << "Test failed: " << grid_ccsds_values[i] << " != " <<  grid_simple_values[i] << std::endl;
                    Assert(0);
                }
            }

            codes_handle_delete(handle);

            delete[] in_values;
            delete[] grid_simple_values;
            delete[] grid_ccsds_values;
        }
    }
    return 0;
}

