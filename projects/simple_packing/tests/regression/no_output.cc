// The library must not print anything: errors are reported with exceptions. It wrote
// diagnostics to stdout/stderr, which corrupts the output of the tools that use it.

#include <cstdio>
#include <iostream>
#include <limits>
#include <memory>
#include <unistd.h>
#include <vector>

#include "simple_packing/optimizer/DecOptimizer.h"
#include "simple_packing/simple/Binary.h"

int main()
{
    // Redirect stdout and stderr to a file
    std::FILE* capture = std::tmpfile();
    (void)std::fflush(stdout);
    (void)std::fflush(stderr);
    const int savedOut = dup(1), savedErr = dup(2);
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
    const long size = std::ftell(capture);

    if (size != 0) {
        std::cerr << "FAIL: the library printed " << size << " bytes\n";
        return 1;
    }
    std::cout << "OK\n";
    return 0;
}
