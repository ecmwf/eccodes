#pragma once

#include "simple_packing/Parameters.h"
#include "simple_packing/Optimizer.h"
#include "simple_packing/Utils.h"

namespace sp
{

// Chooses the decimal and binary scale factors that use the bits per value best, like ecCodes'
// grib_optimize_decimal_factor (optimizeScaleFactor=1), with one intended difference: legacy
// stores the largest coded value in an int. From 32 bits per value on it always overflows, so
// legacy never finds a best decimal scale factor and falls back to the parameters of
// optimizeScaleFactor=0 (at 31 bits this happens when the value rounds up to 2^31). This class
// uses long and optimises for every number of bits: the decoding error is never larger than with
// legacy on general fields, but the coded messages differ from 31 bits on.
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
