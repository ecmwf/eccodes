#pragma once

#include "simple_packing/Parameters.h"
#include "simple_packing/Optimizer.h"
#include "simple_packing/Utils.h"

namespace sp
{


class BinDecOptimizer : public Optimizer
{
public:
    BinDecOptimizer(const int compat_gribex, const int compat_32bit, const int knbit) : compat_gribex_(compat_gribex), compat_32bit_(compat_32bit), knbit_(knbit)
    {
        checkRange("bits per value", knbit, 1, maxEncodingBitsPerValue);
    }
    ~BinDecOptimizer() = default;
    Parameters getParams(double min, double max);

private:
    int compat_gribex_ = 0;
    int compat_32bit_ = 0;
    int knbit_ = 0;
};

}  // namespace sp
