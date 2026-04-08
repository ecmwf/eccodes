#pragma once

#include <Optimizer.h>
#include <Parameters.h>
#include <Utils.h>

class DecOptimizer : public Optimizer {
    using ValueType = double;

    public:
        DecOptimizer(const long bitsPerValue, long last) : last_(last), bitsPerValue_(bitsPerValue) {}
        ~DecOptimizer() = default;

        Parameters getParams(double min, double max) {
            Parameters params;
            params.bitsPerValue(bitsPerValue_);

            ValueType decimal                = 1;
            ValueType unscaled_max           = 0;
            ValueType unscaled_min           = 0;
            ValueType f                      = 0;
            ValueType range                  = 0;
            ValueType minrange = 0, maxrange = 0;
            long decimalScaleFactor = 0;

            range        = (max - min);
            unscaled_min = min;
            unscaled_max = max;
            f            = sp_power<double>(bitsPerValue_, 2) - 1;
            minrange     = sp_power<double>(-last_, 2) * f;
            maxrange     = sp_power<double>(last_, 2) * f;

            while (range < minrange) {
                decimalScaleFactor += 1;
                decimal *= 10;
                min   = unscaled_min * decimal;
                max   = unscaled_max * decimal;
                range = (max - min);
            }

            while (range > maxrange) {
                decimalScaleFactor -= 1;
                decimal /= 10;
                min   = unscaled_min * decimal;
                max   = unscaled_max * decimal;
                range = (max - min);
            }

            params.decimalScaleFactor(decimalScaleFactor);

            auto ref = nearest_smaller_value(min);
            if (ref.has_value()) {
                params.referenceValue(ref.value());
            }
            else {
                std::stringstream ss;
                ss << "unable to find nearest_smaller_value of " << min;
                throw std::runtime_error(ss.str());
            }

            int err;
            params.binaryScaleFactor(get_binary_scale_factor(max, params.referenceValue(), params.bitsPerValue(), &err));
            if (err) {
                throw std::runtime_error("unable to compute binary_scale_factor");
            }
            return params;
        }

    private:
        long last_;
        long bitsPerValue_;
};
