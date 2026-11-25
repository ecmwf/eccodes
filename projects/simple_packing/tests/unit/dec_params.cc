#include <iostream>
#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "simple_packing/optimizer/Dec64Optimizer.h"
#include "simple_packing/optimizer/DecOptimizer.h"

using namespace sp;


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


void compare_dec_optimizers(size_t /*bpv*/, int last)
{
    std::vector<std::pair<double, double>> minmaxx = {
        { 0.0, 1.0 },
    };

    for (const auto& [min, max] : minmaxx) {
        std::cout << "Min: " << min << ", Max: " << max << std::endl;
        for (size_t bpv = 1; bpv <= 63; ++bpv) {
            compare_dec_optimizers_single(bpv, min, max, last);
        }
        std::cout << "----------------------------------------\n";
    }
}


int main(int argc, char* argv[])
{
    size_t bpv = 0;
    double min = 0;
    double max = 0;

    for (int i = 1; i < argc; ++i) {
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

    if (bpv != 0) {
        compare_dec_optimizers_single(bpv, min, max, last);
        return 0;
    }
    else {
        compare_dec_optimizers(bpv, last);
    }

    return 0;
}
