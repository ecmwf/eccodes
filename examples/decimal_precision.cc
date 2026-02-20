
#include <iostream>
#include <vector>
#include <assert.h>
#include "eccodes.h"


int main(int argc, char* argv[])
{
    FILE* in                     = NULL;
    codes_handle* source_handle  = NULL;
    int err                      = 0;

    std::vector<double> values(10);

    for (size_t i = 0; i < values.size(); ++i) {
        values[i] = static_cast<double>(rand()) / RAND_MAX * 10;
    }

    std::cout << "Generated random values:" << std::endl;
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << "values[" << i << "] = " << values[i] << std::endl;
    }

    assert(argc == 2);

    in = fopen(argv[1], "rb");
    if (!in) {
      std::cerr << "Error: unable to open file " << argv[1] << std::endl;
      return 1;
    }

    source_handle = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    if (source_handle == NULL) {
      std::cerr << "Error: unable to create handle from file " << argv[1] << ": " << codes_get_error_message(err) << std::endl;
      fclose(in);
      return 1;
    }

    codes_set_long(source_handle, "decimalPrecision", 3);
    codes_set_double_array(source_handle, "values", values.data(), values.size());
    codes_write_message(source_handle, "output.grib", "wb");

    // get values back to check with diff
    std::vector<double> retrieved_values(values.size());
    size_t retrieved_size = retrieved_values.size();
    codes_get_double_array(source_handle, "values", retrieved_values.data(), &retrieved_size);
    assert(retrieved_size == values.size());
    std::cout << "\nRetrieved values:" << std::endl;
    for (size_t i = 0; i < retrieved_values.size(); ++i) {
        std::cout << "retrieved_values[" << i << "] = " << retrieved_values[i] << " (diff: " << retrieved_values[i] - values[i] << ")" << std::endl;
    }

    codes_handle_delete(source_handle);

    fclose(in);
    return 0;
}
