// MeteoFrance
// Philippe Marguineaud


#include "simple_packing/optimizer/BinDecOptimizer.h"
#include "simple_packing/Utils.h"

#include <cmath>
#include <limits>
#include <optional>

namespace
{

constexpr double dbl_min = std::numeric_limits<double>::min();
constexpr double dbl_max = std::numeric_limits<double>::max();

// The largest decimal exponent that fits in a double (~307)
int vrange()
{
    return (int)(log(dbl_max) / log(10)) - 1;
}

// The binary scale factor and the largest encoded value for the range 'pa',
// 'knbit' bits per value and the decimal scale factor 'kdec'
struct FactecResult
{
    long binaryScaleFactor;
    long maxEncodedValue;
};

// std::nullopt if encoding 'pa' would overflow
std::optional<FactecResult> factec(double pa, int knbit, long kdec, int range)
{
    if (pa < dbl_min) {
        return FactecResult{ 0, 1 };
    }

    if ((fabs(log10(fabs(pa)) + (double)kdec) >= range)) {
        return std::nullopt;
    }

    /* Binary scale factor associated to kdec */
    long ke = floor(log2((pa * sp_power<double>(kdec, 10)) / (sp_power<double>(knbit, 2) - 0.5))) + 1;
    /* Encoded value for pa = max - min       */
    long knutil = floor(0.5 + pa * sp_power<double>(kdec, 10) * sp_power<double>(-ke, 2));
    return FactecResult{ ke, knutil };
}

}  // anonymous namespace


Parameters BinDecOptimizer::getParams(const double pmin, const double pmax)
{
    constexpr double flt_min = std::numeric_limits<float>::min();
    constexpr double flt_max = std::numeric_limits<float>::max();

    // grib_handle* gh = grib_handle_of_accessor(a);
    int idecmin = -15;
    int idecmax = 5;
    long inbint;
    double xtinyr4, xhuger4, xnbint;
    long inumax = 0;
    long jdec;
    const int RANGE = vrange();
    constexpr double EPSILON = std::numeric_limits<double>::epsilon();
    double pa = pmax - pmin;

    Parameters params;

    if (pa == 0) {
        params.bitsPerValue(0);
        params.decimalScaleFactor(0);
        params.binaryScaleFactor(0);
        params.referenceValue(0);
        return params;
    }

    if (fabs(pa) <= EPSILON) {
        params.decimalScaleFactor(0);
        idecmin = 1;
        idecmax = 0;
    }
    else if (pmin != 0. && fabs(pmin) < EPSILON) {
        params.decimalScaleFactor(0);
        idecmin = 1;
        idecmax = 0;
    }

    xtinyr4 = flt_min;
    xhuger4 = flt_max;

    // 2^63 - 1 is not exactly representable as double: do not convert 2^63 to long
    xnbint = sp_power<double>(knbit_, 2) - 1;
    inbint = knbit_ >= 63 ? std::numeric_limits<long>::max() : static_cast<long>(xnbint);

    /* Test decimal scale factors; keep the most suitable */
    for (jdec = idecmin; jdec <= idecmax; jdec++) {
        /* Fix a problem in GRIBEX */
        if (compat_gribex_)
            if (pa * sp_power<double>(jdec, 10) <= 1.E-12)
                continue;

        /* Check it will be possible to decode reference value with 32bit floats */
        if (compat_32bit_)
            if (fabs(pmin) > dbl_min)
                if (log10(fabs(pmin)) + (double)jdec <= log10(xtinyr4))
                    continue;

        /* Check if encoding will not cause an overflow */
        if (fabs(log10(fabs(pa)) + (double)jdec) >= (double)RANGE)
            continue;

        auto fact = factec(pa, knbit_, jdec, RANGE);

        if (!fact)
            continue;

        /* Check it will be possible to decode the maximum value of the fields using 32bit floats */
        if (compat_32bit_)
            if (pmin * sp_power<double>(jdec, 10) + xnbint * sp_power<double>(fact->binaryScaleFactor, 2) >= xhuger4)
                continue;

        /* GRIB1 demands that the binary scale factor be encoded in a single byte */
        if (compat_gribex_)
            if ((fact->binaryScaleFactor < -126) || (fact->binaryScaleFactor > 127))
                continue;

        if (fact->maxEncodedValue > inumax) {
            inumax = fact->maxEncodedValue;
            params.decimalScaleFactor(jdec);
            params.binaryScaleFactor(fact->binaryScaleFactor);
        }
    }

    if (inumax > 0) {
        double decimal = sp_power<double>(+params.decimalScaleFactor(), 10);
        double divisor = sp_power<double>(-params.binaryScaleFactor(), 2);
        double min = pmin * decimal;
        long vmin, vmax;
        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value");
        }

        vmax = static_cast<long>((((pmax * decimal) - params.referenceValue()) * divisor) + 0.5);
        vmin = static_cast<long>((((pmin * decimal) - params.referenceValue()) * divisor) + 0.5);

        /* This may happen if pmin*decimal-*ref is too large */
        if ((vmin != 0) || (vmax > inbint))
            inumax = 0;
    }

    /* If seeking for an optimal decimal scale factor fails, fall back to a basic method */
    if (inumax == 0) {
        int last = compat_gribex_ ? 99 : 127;
        double min = pmin, max = pmax;
        double range = max - min;
        double f = sp_power<double>(knbit_, 2) - 1;
        double minrange = sp_power<double>(-last, 2) * f;
        double maxrange = sp_power<double>(+last, 2) * f;
        double decimal = 1;

        params.decimalScaleFactor(0);

        while (range < minrange) {
            params.decimalScaleFactor(params.decimalScaleFactor() + 1);
            decimal *= 10;
            min = pmin * decimal;
            max = pmax * decimal;
            range = max - min;
        }

        while (range > maxrange) {
            params.decimalScaleFactor(params.decimalScaleFactor() - 1);
            decimal /= 10;
            min = pmin * decimal;
            max = pmax * decimal;
            range = max - min;
        }

        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value");
        }

        auto scale = get_binary_scale_factor(max, params.referenceValue(), knbit_);
        if (scale) {
            params.binaryScaleFactor(*scale);
        }
        else {
            // The range is too small even with the smallest binary scale factor:
            // fall back to no scaling
            params.binaryScaleFactor(0);
            params.decimalScaleFactor(0);
            params.referenceValue(0.);
        }
    }

    params.bitsPerValue(knbit_);

    return params;
}
