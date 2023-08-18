#include "GribFileTests.h"
#include <iostream>

int main()
{
    std::filesystem::path gribLayoutFile("/home/kev/cpp_demo/grib_layout");
    std::filesystem::path gribFile("/home/kev/cpp_demo/demo.grib");

    std::cout << "Running GRIB file tests...\n\n";

    eccodes::accessor::runTests(gribFile, gribLayoutFile);

    std::cout << "\nFinished running GRIB file tests\n";

}