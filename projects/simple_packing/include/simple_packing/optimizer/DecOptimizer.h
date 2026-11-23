#pragma once

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
        Parameters params;
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

        while (range > maxrange) {
            decimalScaleFactor -= 1;
            decimal /= 10;
            min = unscaled_min * decimal;
            max = unscaled_max * decimal;
            range = (max - min);
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
