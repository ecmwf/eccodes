#pragma once

#include <cmath>
#include <stdexcept>

#include "simple_packing/Parameters.h"
#include "simple_packing/Optimizer.h"
#include "simple_packing/Utils.h"

namespace sp
{

class BPVOptimizer : public Optimizer
{
public:
    BPVOptimizer(int decimalScaleFactor) : decimalScaleFactor_(decimalScaleFactor)
    {
        checkRange("decimal scale factor", decimalScaleFactor, -maxScaleFactor, maxScaleFactor);
    }

    Parameters getParams(double min, double max)
    {
        // The optimiser assumes a validated field: finite values, minimum <= maximum.
        // Direct callers must not be able to make the parameter search convert NaN or
        // infinity to integers.
        check_data_values_range(min, max);

        /* decimal_scale_factor is given, binary_scale_factor=0 and params.bits_per_value is computed */

        Parameters params;

        auto decimal = sp_power<double>(decimalScaleFactor_, 10);
        const double unscaledMin = min;
        const double unscaledMax = max;
        min *= decimal;
        max *= decimal;

        // Scaling by 10^decimalScaleFactor must keep the field within the range of a double,
        // otherwise the number of bits per value cannot be computed
        if (!(std::isfinite(min) && std::isfinite(max))) {
            throw std::runtime_error("Simple packing: cannot scale the range [" + std::to_string(unscaledMin) +
                                     ", " + std::to_string(unscaledMax) + "]");
        }

        /* bits_per_value = ceil(log2(fabs(max - min))) (ceil: see GRIB-540) */
        // The ceil() result must fit into an unsigned long, otherwise the conversion is
        // undefined: such a range cannot be counted in bits anyway
        const double scaledRange = ceil(fabs(max - min));
        if (!(scaledRange <= 9.223372036854775808e18)) {
            throw std::runtime_error("Range of values too large. Try a smaller value for decimal precision");
        }
        auto bpv = number_of_bits(static_cast<unsigned long>(scaledRange));
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

}  // namespace sp
