#pragma once

#include <cmath>
#include <stdexcept>
#include <string>

#include "simple_packing/Optimizer.h"
#include "simple_packing/Parameters.h"
#include "simple_packing/Utils.h"


// last = 127 is the "exponent zero" in single precision IEEE floating point numbers
// last = 63 is the "exponent zero" in single precision IBM floating point numbers

class Dec64Optimizer : public Optimizer
{
    using ValueType = double;

public:
    Dec64Optimizer(const long bitsPerValue, long last) : last_(last), bitsPerValue_(bitsPerValue)
    {
        checkRange("bits per value", bitsPerValue, 1, maxEncodingBitsPerValue);
        checkRange("exponent limit", last, 1, 1023);
    }
    ~Dec64Optimizer() = default;

    Parameters getParams(double min, double max)
    {
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

        long decimalScaleFactor = 0;

        // Scale the field by powers of ten until its range fits, so that the binary scale factor
        // stays within +-last
        const ValueType f = pow(2.0, static_cast<double>(bitsPerValue_)) - 1;
        const ValueType minrange = pow(2.0, -last_ + 1) * f;
        const ValueType maxrange = pow(2.0, last_ - 1) * f;
        const ValueType unscaledMin = min;
        const ValueType unscaledMax = max;
        ValueType decimal = 1;
        ValueType range = max - min;

        auto checkRange = [&]() {
            if (!(std::isfinite(range) && range > 0)) {
                throw std::runtime_error("Simple packing: cannot scale the range [" + std::to_string(unscaledMin) + ", " + std::to_string(unscaledMax) + "]");
            }
        };
        checkRange();
        while (range < minrange) {
            decimalScaleFactor++;
            decimal *= 10;
            min = unscaledMin * decimal;
            max = unscaledMax * decimal;
            range = max - min;
            checkRange();
        }
        while (range > maxrange) {
            decimalScaleFactor--;
            decimal /= 10;
            min = unscaledMin * decimal;
            max = unscaledMax * decimal;
            range = max - min;
            checkRange();
        }

        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            std::stringstream ss;
            ss << "unable to find nearest_smaller_value of " << min;
            throw std::runtime_error(ss.str());
        }

        int binaryScaleFactor = bitsPerValue_ - static_cast<int>(std::ceil(std::log2(range))) - 1;

        params.binaryScaleFactor(-binaryScaleFactor);
        params.decimalScaleFactor(decimalScaleFactor);
        params.bitsPerValue(bitsPerValue_);
        return params;
    }

private:
    long last_;
    long bitsPerValue_;
};
