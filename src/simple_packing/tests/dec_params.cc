#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <tuple>
#include <numeric>


// #include <optimizer/DecOptimizer.h>
#include <optimizer/BinDecOptimizer.h>
#include <optimizer/BPVOptimizer.h>
#include <optimizer/Dec64Optimizer.h>
#include <optimizer/DecOptimizer.h>


void compare_dec_optimizers_single(size_t bpv, double min, double max, int last)
{
    Dec64Optimizer optimizer64(bpv, last);
    DecOptimizer optimizer(bpv, last);
    auto params1 = optimizer.getParams(min, max);
    std::cout << "bpv: " << bpv
        << " -> bpv: " << params1.bitsPerValue()
        << ", d: " << params1.decimalScaleFactor()
        << ", b: " << params1.binaryScaleFactor()
        << ", r: " << params1.referenceValue()
        << std::endl;
    auto params2 = optimizer64.getParams(min, max);
    std::cout << "bpv: " << bpv
        << " -> bpv: " << params2.bitsPerValue()
        << ", d: " << params2.decimalScaleFactor()
        << ", b: " << params2.binaryScaleFactor()
        << ", r: " << params2.referenceValue()
        << std::endl;
    assert(params1.binaryScaleFactor() == params2.binaryScaleFactor());
}


void compare_dec_optimizers(size_t bpv, int last) {

    std::vector<std::pair<double, double>> minmaxx = {
        // {10.0, 10.0},
        // {0.0, 0.0},
        // {std::numeric_limits<double>::min(), std::numeric_limits<double>::max()},
        // {0, std::numeric_limits<double>::max()},
        // {std::numeric_limits<double>::min(), 0},
        {0.0, 1.0},
        // {0.0, 10.0},
        // {0.0, 100.0},
        // {-0.1, 0.1},
        // {-1.0, 0.0},
        // {-1.0, 1.0},
        // {-10.0, 10.0},
        // {-100.0, 100.0}
    };

    for (const auto& [min, max] : minmaxx) {
        std::cout << "Min: " << min << ", Max: " << max << std::endl;
        for (size_t bpv = 1; bpv <= 63; ++bpv) {
            // compare(bpv, min, max, last);
            compare_dec_optimizers_single(bpv, min, max, last);
        }
        std::cout << "----------------------------------------" << std::endl;
    }
}


void test_dec64_optimizer(size_t bpv, int last) {

    auto mi = std::numeric_limits<double>::min();
    auto ma = std::numeric_limits<double>::max();

    std::vector<std::pair<std::pair<double, double>, Parameters>> minmaxx = {
        {{10.0, 10.0}, Parameters{}},
        {{0.0, 0.0}, Parameters{}},
        {{mi, ma}, Parameters{}},
        {{0, ma}, Parameters{}},
        {{mi, 0}, Parameters{}},
        {{0.0, 1.0}, Parameters{}},
        {{0.0, 10.0}, Parameters{}},
        {{0.0, 100.0}, Parameters{}},
        {{-0.1, 0.1}, Parameters{}},
        {{-1.0, 0.0}, Parameters{}},
        {{-1.0, 1.0}, Parameters{}},
        {{-10.0, 10.0}, Parameters{}},
        {{-100.0, 100.0}, Parameters{}}
    };


    Dec64Optimizer optimizer64(bpv, last);
    for (const auto& [minmax, params] : minmaxx) {
        const auto& [min, max] = minmax;
        auto params2 = optimizer64.getParams(min, max);
        // std::cout << "bpv: " << bpv
        //     << " -> bpv: " << params2.bitsPerValue()
        //     << ", d: " << params2.decimalScaleFactor()
        //     << ", b: " << params2.binaryScaleFactor()
        //     << ", r: " << params2.referenceValue()
        //     << std::endl;
        std::cout
            << ", " << params2.bitsPerValue()
            << ", " << params2.decimalScaleFactor()
            << ", " << params2.binaryScaleFactor()
            << ", " << params2.referenceValue()
            << std::endl;
    }
}


int main(int argc, char* argv[])
{
    // if (argc != 4) {
    //     std::cerr << "Usage: " << argv[0] << " <bits_per_value> <min_value> <max_value>" << std::endl;
    //     return 1;
    // }

    size_t bpv = 0;
    double min = 0;
    double max = 0;

    for (size_t i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--bpv" && i + 1 < argc) {
            bpv = std::stol(argv[++i]);
        }
        else if (arg == "--min" && i + 1 < argc) {
            min = std::stod(argv[++i]);
        }
        else if (arg == "--max" && i + 1 < argc) {
            max = std::stod(argv[++i]);
        }
    }

    auto last = 127;

    // test_dec64_optimizer(bpv, last);

    if (bpv != 0) {
        compare_dec_optimizers_single(bpv, min, max, last);
        return 0;
    }
    else {
        compare_dec_optimizers(bpv, last);
    }

    return 0;
}
