// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: the version of the header (SIMPLE_PACKING_VERSION_*, sp::version()) is the version of the
// CMake project (PROJECT_VERSION), and the main header provides it.
#include <cstring>
#include <iostream>
#include <string>

#include "simple_packing/simple_packing.h"

static_assert(SIMPLE_PACKING_VERSION_MAJOR >= 0 && SIMPLE_PACKING_VERSION_MINOR >= 0 && SIMPLE_PACKING_VERSION_PATCH >= 0, "version numbers");
static_assert(sp::version()[0] != '\0', "version() is a constant expression");

int main()
{
    int failures = 0;
    const std::string fromNumbers = std::to_string(SIMPLE_PACKING_VERSION_MAJOR) + "." + std::to_string(SIMPLE_PACKING_VERSION_MINOR) + "." +
                                    std::to_string(SIMPLE_PACKING_VERSION_PATCH);
    if (fromNumbers != SIMPLE_PACKING_VERSION) {
        std::cerr << "FAIL: SIMPLE_PACKING_VERSION " << SIMPLE_PACKING_VERSION << " differs from the numbers " << fromNumbers << "\n";
        ++failures;
    }
    if (std::strcmp(sp::version(), SIMPLE_PACKING_VERSION) != 0) {
        std::cerr << "FAIL: sp::version() " << sp::version() << " differs from SIMPLE_PACKING_VERSION " << SIMPLE_PACKING_VERSION << "\n";
        ++failures;
    }
    if (std::string(SIMPLE_PACKING_VERSION) != SIMPLE_PACKING_PROJECT_VERSION) {
        std::cerr << "FAIL: SIMPLE_PACKING_VERSION " << SIMPLE_PACKING_VERSION << " differs from the CMake project version " << SIMPLE_PACKING_PROJECT_VERSION << "\n";
        ++failures;
    }
    if (failures != 0) {
        return 1;
    }
    std::cout << "OK simple_packing " << sp::version() << "\n";
    return 0;
}
