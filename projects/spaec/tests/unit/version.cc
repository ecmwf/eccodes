// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Unit test: the version of the header (SPAEC_VERSION_*, spaec::version()) is the version of the
// CMake project (PROJECT_VERSION), and the main header provides it.
#include <cstring>
#include <iostream>
#include <string>

#include "spaec/spaec.h"

static_assert(SPAEC_VERSION_MAJOR >= 0 && SPAEC_VERSION_MINOR >= 0 && SPAEC_VERSION_PATCH >= 0, "version numbers");
static_assert(spaec::version()[0] != '\0', "version() is a constant expression");

int main()
{
    int failures = 0;
    const std::string fromNumbers = std::to_string(SPAEC_VERSION_MAJOR) + "." + std::to_string(SPAEC_VERSION_MINOR) + "." +
                                    std::to_string(SPAEC_VERSION_PATCH);
    if (fromNumbers != SPAEC_VERSION) {
        std::cerr << "FAIL: SPAEC_VERSION " << SPAEC_VERSION << " differs from the numbers " << fromNumbers << "\n";
        ++failures;
    }
    if (std::strcmp(spaec::version(), SPAEC_VERSION) != 0) {
        std::cerr << "FAIL: spaec::version() " << spaec::version() << " differs from SPAEC_VERSION " << SPAEC_VERSION << "\n";
        ++failures;
    }
    if (std::string(SPAEC_VERSION) != SPAEC_PROJECT_VERSION) {
        std::cerr << "FAIL: SPAEC_VERSION " << SPAEC_VERSION << " differs from the CMake project version " << SPAEC_PROJECT_VERSION << "\n";
        ++failures;
    }
    if (failures != 0) {
        return 1;
    }
    std::cout << "OK spaec " << spaec::version() << "\n";
    return 0;
}
