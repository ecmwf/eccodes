#pragma once

#include <cmath>

#include <Optimizer.h>
#include <Parameters.h>
#include <LibEccodes.h>


// last = 127 is the "exponent zero" in single precision IEEE floating point numbers
// last = 63 is the "exponent zero" in single precision IBM floating point numbers

class Dec64Optimizer : public Optimizer {
    using ValueType = double;

    public:
        Dec64Optimizer(const long bitsPerValue, long last) : last_(last), bitsPerValue_(bitsPerValue) {}
        ~Dec64Optimizer() = default;

        Parameters getParams(double min, double max) {
            Parameters params;

            if (min == max) {
                auto ref = nearest_smaller_value(min);
                if (ref.has_value()){
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

            const ValueType f = pow(2.0, static_cast<double>(bitsPerValue_)) - 1;
            const ValueType minrange = pow(2.0, -last_ + 1) * f;
            const ValueType maxrange = pow(2.0, last_) * f;
            ValueType range = max - min;

            if (range < minrange) {
                decimalScaleFactor = static_cast<long>(std::ceil(std::log10(range)));
                min = min * pow(10.0, decimalScaleFactor);
                max = max * pow(10.0, decimalScaleFactor);
            }

            if (range > maxrange) {
                decimalScaleFactor = static_cast<long>(std::floor(std::log10(range)));
                min = min * pow(10.0, decimalScaleFactor);
                max = max * pow(10.0, decimalScaleFactor);
            }

            range = max - min;

            auto ref = nearest_smaller_value(min);
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
