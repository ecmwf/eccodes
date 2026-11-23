#pragma once

#include "simple_packing/Parameters.h"
#include "simple_packing/Optimizer.h"
#include "simple_packing/Utils.h"

class BPVOptimizer : public Optimizer
{
public:
    BPVOptimizer(int decimalScaleFactor) : decimalScaleFactor_(decimalScaleFactor)
    {
        checkRange("decimal scale factor", decimalScaleFactor, -maxScaleFactor, maxScaleFactor);
    }

    Parameters getParams(double min, double max)
    {
        /* decimal_scale_factor is given, binary_scale_factor=0 and params.bits_per_value is computed */

        Parameters params;

        auto decimal = sp_power<double>(decimalScaleFactor_, 10);
        min *= decimal;
        max *= decimal;

        /* params.bits_per_value=(long)ceil(log((double)(imax-imin+1))/log(2.0)); */
        /* See GRIB-540 for why we use ceil */
        auto bpv = number_of_bits((unsigned long)ceil(fabs(max - min)));
        if (bpv.has_value()) {
            params.bitsPerValue(bpv.value());
        }
        else {
            throw std::runtime_error("Range of values too large. Try a smaller value for decimal precision");
        }

        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value");
        }
        params.binaryScaleFactor(0);
        params.decimalScaleFactor(decimalScaleFactor_);

        return params;
    }

private:
    int decimalScaleFactor_ = 0;
};
