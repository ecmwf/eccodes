#pragma once

#include <cmath>
#include <stdexcept>
#include <string>

#include "simple_packing/Optimizer.h"
#include "simple_packing/Parameters.h"
#include "simple_packing/Utils.h"

class DecOptimizer : public Optimizer
{
    using ValueType = double;

public:
    DecOptimizer(const long bitsPerValue, long last) : last_(last), bitsPerValue_(bitsPerValue)
    {
        checkRange("bits per value", bitsPerValue, 1, maxEncodingBitsPerValue);
        checkRange("exponent limit", last, 1, 1023);
    }
    ~DecOptimizer() = default;

    Parameters getParams(double min, double max)
    {
        // The optimiser assumes a validated field: finite values, minimum <= maximum.
        // Direct callers must not be able to make the parameter search loop forever
        // (it scales the field by powers of ten, which never converges for a negative range).
        check_data_values_range(min, max);

        Parameters params;

        if (min == max) {
            auto ref = nearest_smaller_value(min, referenceFloatType_);
            if (ref.has_value()) {
                params.referenceValue(ref.value());
            }
            else {
                throw std::runtime_error("unable to find nearest_smaller_value");
            }
            params.bitsPerValue(0);
            params.binaryScaleFactor(0);
            params.decimalScaleFactor(0);
            params.isConstantField(true);
            return params;
        }

        params.bitsPerValue(bitsPerValue_);

        ValueType decimal = 1;
        ValueType unscaled_max = 0;
        ValueType unscaled_min = 0;
        ValueType f = 0;
        ValueType range = 0;
        ValueType minrange = 0, maxrange = 0;
        long decimalScaleFactor = 0;

        range = (max - min);
        unscaled_min = min;
        unscaled_max = max;
        f = sp_power<double>(bitsPerValue_, 2) - 1;
        minrange = sp_power<double>(-last_, 2) * f;
        maxrange = sp_power<double>(last_, 2) * f;

        while (range < minrange) {
            decimalScaleFactor += 1;
            decimal *= 10;
            min = unscaled_min * decimal;
            max = unscaled_max * decimal;
            range = (max - min);
        }

        // An infinite range from scaling up (a denormal range with values of both signs) cannot
        // be scaled down again: decimal stays infinite (the check below reports it)
        while (range > maxrange && std::isfinite(decimal)) {
            decimalScaleFactor -= 1;
            decimal /= 10;
            min = unscaled_min * decimal;
            max = unscaled_max * decimal;
            range = (max - min);
        }

        // Scaling by 10^decimalScaleFactor must keep the field within the range of a double,
        // otherwise the reference value and the packed values cannot be computed
        if (!(std::isfinite(min) && std::isfinite(max))) {
            throw std::runtime_error("Simple packing: cannot scale the range [" + std::to_string(unscaled_min) +
                                     ", " + std::to_string(unscaled_max) + "]");
        }

        params.decimalScaleFactor(decimalScaleFactor);

        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value of " + std::to_string(min));
        }

        auto scale = get_binary_scale_factor(max, params.referenceValue(), params.bitsPerValue());
        if (!scale) {
            throw std::runtime_error("unable to compute binary_scale_factor");
        }
        params.binaryScaleFactor(*scale);
        return params;
    }

private:
    long last_;
    long bitsPerValue_;
};
