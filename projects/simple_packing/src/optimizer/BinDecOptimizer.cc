// SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// MeteoFrance
// Philippe Marguineaud


#include "simple_packing/detail/optimizer/BinDecOptimizer.h"
#include "simple_packing/detail/Utils.h"

#include <cassert>
#include <cstdint>
#include <cmath>
#include <string>
#include <limits>
#include <optional>

namespace sp::detail
{

namespace
{

constexpr double dbl_max = std::numeric_limits<double>::max();

// static_cast<long> of a double beyond LONG_MAX is undefined behaviour (and aborted under
// UBSan): clamp it, the callers only compare against the encoded-value bounds. The values are
// encoded values, never negative.
int64_t toLongClamped(double value)
{
    assert(value >= 0);
    // 2^63 (2^31 where long has 32 bits): the smallest double beyond LONG_MAX
    static const double limit = std::ldexp(1.0, std::numeric_limits<int64_t>::digits);
    if (value >= limit)
        return std::numeric_limits<int64_t>::max();
    return static_cast<int64_t>(value);
}

// The largest decimal exponent that fits in a double (~307)
int32_t vrange()
{
    return static_cast<int32_t>(log(dbl_max) / log(10)) - 1;
}

// The binary scale factor and the largest encoded value for the range 'pa',
// 'knbit' bits per value and the decimal scale factor 'kdec'
struct FactecResult
{
    int64_t binaryScaleFactor;
    int64_t maxEncodedValue;
};

// The caller ensures that pa > DBL_EPSILON and that encoding it does not overflow
FactecResult factec(double pa, int32_t knbit, int64_t kdec)
{
    /* Binary scale factor associated to kdec */
    int64_t ke = floor(log2((pa * sp_power<double>(kdec, 10)) / (sp_power<double>(knbit, 2) - 0.5))) + 1;
    /* Encoded value for pa = max - min       */
    int64_t knutil = toLongClamped(floor(0.5 + pa * sp_power<double>(kdec, 10) * sp_power<double>(-ke, 2)));
    return FactecResult{ ke, knutil };
}

}  // anonymous namespace


Parameters BinDecOptimizer::getParams(const double pmin, const double pmax)
{
    // The optimiser assumes a validated field: finite values, minimum <= maximum.
    // Direct callers must not be able to make the parameter search loop forever or
    // convert NaN to integers.
    check_data_values_range(pmin, pmax);

    constexpr double flt_max = std::numeric_limits<float>::max();

    // grib_handle* gh = grib_handle_of_accessor(a);
    int32_t idecmin = -15;
    int32_t idecmax = 5;
    int64_t inbint;
    double xhuger4, xnbint;
    int64_t inumax = 0;
    int64_t jdec;
    const int32_t RANGE = vrange();
    constexpr double EPSILON = std::numeric_limits<double>::epsilon();
    double pa = pmax - pmin;

    Parameters params;

    if (pa == 0) {
        auto ref = nearest_smaller_value(pmin, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value");
        }
        params.bitsPerValue(0);
        params.decimalScaleFactor(0);
        params.binaryScaleFactor(0);
        params.isConstantField(true);
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

    xhuger4 = flt_max;

    // 2^63 - 1 is not exactly representable as double: do not convert 2^63 to long (nor values
    // beyond the range of a 32-bit long, where long has 32 bits)
    xnbint = sp_power<double>(knbit_, 2) - 1;
    // 2^knbit - 1 in integers: in double it is 2^knbit from 54 bits on, which accepted a largest
    // code of 2^knbit that does not fit
    inbint = knbit_ >= std::numeric_limits<int64_t>::digits ? std::numeric_limits<int64_t>::max() : static_cast<int64_t>((uint64_t{ 1 } << knbit_) - 1);

    /* Test decimal scale factors; keep the most suitable */
    for (jdec = idecmin; jdec <= idecmax; jdec++) {
        /* Fix a problem in GRIBEX */
        if (compat_gribex_)
            if (pa * sp_power<double>(jdec, 10) <= 1.E-12)
                continue;

        // (Legacy ecCodes also skips jdec if 32-bit floats cannot decode pmin * 10^jdec: never
        // the case here, the search only runs for pmin = 0 or |pmin| >= DBL_EPSILON, jdec >= -15)

        /* Check if encoding will not cause an overflow */
        if (fabs(log10(fabs(pa)) + (double)jdec) >= (double)RANGE)
            continue;

        const FactecResult fact = factec(pa, knbit_, jdec);

        /* Check it will be possible to decode the maximum value of the fields using 32bit floats */
        if (compat_32bit_)
            if (pmin * sp_power<double>(jdec, 10) + xnbint * sp_power<double>(fact.binaryScaleFactor, 2) >= xhuger4)
                continue;

        /* GRIB1 demands that the binary scale factor be encoded in a single byte */
        if (compat_gribex_)
            if ((fact.binaryScaleFactor < -126) || (fact.binaryScaleFactor > 127))
                continue;

        if (fact.maxEncodedValue > inumax) {
            inumax = fact.maxEncodedValue;
            params.decimalScaleFactor(jdec);
            params.binaryScaleFactor(fact.binaryScaleFactor);
        }
    }

    if (inumax > 0) {
        double decimal = sp_power<double>(+params.decimalScaleFactor(), 10);
        double divisor = sp_power<double>(-params.binaryScaleFactor(), 2);
        double min = pmin * decimal;
        int64_t vmin, vmax;
        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value");
        }

        // The largest code is checked as pack checks it, in double: clamped to a long, a code of
        // 2^63 would pass as 2^63 - 1
        const double codedMax = (((pmax * decimal) - params.referenceValue()) * divisor) + 0.5;
        const bool maxFits = codedMax < sp_power<double>(knbit_, 2);
        vmax = toLongClamped(codedMax);
        vmin = toLongClamped((((pmin * decimal) - params.referenceValue()) * divisor) + 0.5);

        /* This may happen if pmin*decimal-*ref is too large */
        if ((vmin != 0) || (vmax > inbint) || !maxFits)
            inumax = 0;
    }

    /* If seeking for an optimal decimal scale factor fails, fall back to a basic method */
    if (inumax == 0) {
        int32_t last = compat_gribex_ ? 99 : 127;
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

        // An infinite range from scaling up (a denormal range with values of both signs) cannot
        // be scaled down again: decimal stays infinite
        while (range > maxrange && std::isfinite(decimal)) {
            params.decimalScaleFactor(params.decimalScaleFactor() - 1);
            decimal /= 10;
            min = pmin * decimal;
            max = pmax * decimal;
            range = max - min;
        }

        if (!(std::isfinite(min) && std::isfinite(max))) {
            throw std::runtime_error("Simple packing: cannot scale the range [" + std::to_string(pmin) + ", " + std::to_string(pmax) + "]");
        }

        auto ref = nearest_smaller_value(min, referenceFloatType_);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            throw std::runtime_error("unable to find nearest_smaller_value");
        }

        // The scaling by powers of ten above makes the range large enough for a binary scale
        // factor of at least -last
        params.binaryScaleFactor(get_binary_scale_factor(max, params.referenceValue(), knbit_).value());
    }

    params.bitsPerValue(knbit_);

    return params;
}

}  // namespace sp::detail
