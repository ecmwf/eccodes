/*
 * SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cmath>
#include <stdexcept>
#include <string>

#include "simple_packing/detail/Optimizer.h"
#include "simple_packing/detail/Parameters.h"
#include "simple_packing/detail/Utils.h"

namespace sp::detail
{

// The decimal scale factor and the bits per value are given, the binary scale factor is computed
// (ecCodes CCSDS packing with binaryScaleFactor = 0 and decimalScaleFactor != 0): the field is
// scaled by 10^D, the reference value is the nearest float not above the scaled minimum, and the
// binary scale factor the smallest that fits the scaled range into bitsPerValue bits
class BinOptimizer : public Optimizer
{
public:
    BinOptimizer(int decimalScaleFactor, long bitsPerValue) : decimalScaleFactor_(decimalScaleFactor), bitsPerValue_(bitsPerValue)
    {
        checkRange("decimal scale factor", decimalScaleFactor, -maxScaleFactor, maxScaleFactor);
        checkRange("bits per value", bitsPerValue, 1, maxEncodingBitsPerValue);
    }

    Parameters getParams(double min, double max)
    {
        check_data_values_range(min, max);

        Parameters params;
        if (min == max) {
            auto ref = nearest_smaller_value(min, referenceFloatType_);
            if (!ref) {
                throw std::runtime_error("unable to find nearest_smaller_value");
            }
            params.referenceValue(*ref);
            params.bitsPerValue(0);
            params.binaryScaleFactor(0);
            params.decimalScaleFactor(0);
            params.isConstantField(true);
            return params;
        }

        const double decimal = sp_power<double>(decimalScaleFactor_, 10);
        const double scaledMin = min * decimal;
        const double scaledMax = max * decimal;
        if (!(std::isfinite(scaledMin) && std::isfinite(scaledMax))) {
            throw std::runtime_error("Simple packing: cannot scale the range [" + std::to_string(min) + ", " + std::to_string(max) + "] by 10^" +
                                     std::to_string(decimalScaleFactor_));
        }

        auto ref = nearest_smaller_value(scaledMin, referenceFloatType_);
        if (!ref) {
            throw std::runtime_error("unable to find nearest_smaller_value of " + std::to_string(scaledMin));
        }
        auto scale = get_binary_scale_factor(scaledMax, *ref, bitsPerValue_);
        if (!scale) {
            throw std::runtime_error("unable to compute binary_scale_factor");
        }

        params.bitsPerValue(bitsPerValue_);
        params.decimalScaleFactor(decimalScaleFactor_);
        params.referenceValue(*ref);
        params.binaryScaleFactor(*scale);
        return params;
    }

private:
    int decimalScaleFactor_;
    long bitsPerValue_;
};

}  // namespace sp::detail
