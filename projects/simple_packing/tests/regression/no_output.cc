// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// The library must not print anything: errors are reported with exceptions. It wrote
// diagnostics to stdout/stderr, which corrupts the output of the tools that use it.

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#if defined(_WIN32)
    #include <io.h>  // _dup, _dup2, _fileno
    #define dup    _dup
    #define dup2   _dup2
    #define fileno _fileno
#else
    #include <unistd.h>
#endif
#include <vector>

#include "simple_packing/detail/optimizer/DecOptimizer.h"
#include "simple_packing/detail/simple/Binary.h"

using namespace sp::detail;

int main()
{
    // Redirect stdout and stderr to a file
    std::FILE* capture = std::tmpfile();
    (void)std::fflush(stdout);
    (void)std::fflush(stderr);
    const int32_t savedOut = dup(1), savedErr = dup(2);
    dup2(fileno(capture), 1);
    dup2(fileno(capture), 2);

    for (double v : { 1e39, -1e39, 1e300 }) {
        nearest_smaller_value(v, FloatType::IEEE32);
        nearest_smaller_value(v, FloatType::IBM32);
    }
    const double inf = std::numeric_limits<double>::infinity();
    for (const std::vector<double>& values : { std::vector<double>{ -1e300, 1e300 }, std::vector<double>{ 0.0, 1e39 },
                                               std::vector<double>{ 0.0, inf }, std::vector<double>{ -inf, 1.0 } }) {
        try {
            SimplePackingBinary<double> sp(std::make_unique<DecOptimizer>(16, 127));
            sp.pack(values);
        }
        catch (const std::exception&) {
        }
    }

    (void)std::fflush(stdout);
    std::cout.flush();
    (void)std::fflush(stderr);
    dup2(savedOut, 1);
    dup2(savedErr, 2);
    const int64_t size = std::ftell(capture);

    if (size != 0) {
        std::cerr << "FAIL: the library printed " << size << " bytes\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
