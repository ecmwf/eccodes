// MeteoFrance
// Philippe Marguineaud


#include "BinDecOptimizer.h"
#include "LibEccodes.h"
#include <cmath>
#include <limits>
#include <cstdlib>


constexpr double dbl_min = std::numeric_limits<double>::min();
constexpr double dbl_max = std::numeric_limits<double>::max();

// constexpr double epsilon()
// {
//     double e = 1.;
//     while (1. != (1. + e)) {
//         e /= 2;
//     }
//     return e;
// }

int vrange() {
    return (int)(log(dbl_max) / log(10)) - 1;
}


void factec(int* krep, const double pa, const int knbit, const long kdec, const int range, long* ke, int* knutil)
{
    *krep = 0;
    *ke = 0;
    *knutil = 0;

    if (pa < dbl_min) {
        *knutil = 1;
        return;
    }

    if ((fabs(log10(fabs(pa)) + (double)kdec) >= range)) {
        *krep = 1;
        return;
    }

    /* Binary scale factor associated to kdec */
    *ke = floor(log2((pa * grib_power<double>(kdec, 10)) / (grib_power<double>(knbit, 2) - 0.5))) + 1;
    /* Encoded value for pa = max - min       */
    *knutil = floor(0.5 + pa * grib_power<double>(kdec, 10) * grib_power<double>(-*ke, 2));
}


Parameters BinDecOptimizer::getParams(const double pmin, const double pmax)
{
    constexpr double flt_min = std::numeric_limits<float>::min();
    constexpr double flt_max = std::numeric_limits<float>::max();

    //grib_handle* gh = grib_handle_of_accessor(a);
    int idecmin     = -15;
    int idecmax     = 5;
    long inbint;
    double xtinyr4, xhuger4, xnbint;
    int inumax, inutil;
    long jdec, ie;
    int irep;
    int RANGE      = vrange();
    constexpr double EPSILON = std::numeric_limits<double>::epsilon();
    double pa      = pmax - pmin;

    Parameters params;

    if (pa == 0) {
        params.bitsPerValue(0);
        params.decimalScaleFactor(0);
        params.binaryScaleFactor(0);
        params.referenceValue(0);
        return params;
    }

    inumax = 0;

    if (fabs(pa) <= EPSILON) {
        // *kdec   = 0;
        params.decimalScaleFactor(0);
        idecmin = 1;
        idecmax = 0;
    }
    else if (pmin != 0. && fabs(pmin) < EPSILON) {
        // *kdec   = 0;
        params.decimalScaleFactor(0);
        idecmin = 1;
        idecmax = 0;
    }

    xtinyr4 = flt_min;
    xhuger4 = flt_max;

    inbint = grib_power<double>(knbit_, 2) - 1;
    xnbint = (double)inbint;

    /* Test decimal scale factors; keep the most suitable */
    for (jdec = idecmin; jdec <= idecmax; jdec++) {
        /* Fix a problem in GRIBEX */
        if (compat_gribex_)
            if (pa * grib_power<double>(jdec, 10) <= 1.E-12)
                continue;

        /* Check it will be possible to decode reference value with 32bit floats */
        if (compat_32bit_)
            if (fabs(pmin) > dbl_min)
                if (log10(fabs(pmin)) + (double)jdec <= log10(xtinyr4))
                    continue;

        /* Check if encoding will not cause an overflow */
        if (fabs(log10(fabs(pa)) + (double)jdec) >= (double)RANGE)
            continue;

        factec(&irep, pa, knbit_, jdec, RANGE, &ie, &inutil);

        if (irep != 0)
            continue;

        /* Check it will be possible to decode the maximum value of the fields using 32bit floats */
        if (compat_32bit_)
            if (pmin * grib_power<double>(jdec, 10) + xnbint * grib_power<double>(ie, 2) >= xhuger4)
                continue;

        /* GRIB1 demands that the binary scale factor be encoded in a single byte */
        if (compat_gribex_)
            if ((ie < -126) || (ie > 127))
                continue;

        if (inutil > inumax) {
            inumax = inutil;
            // *kdec  = jdec;
            // *kbin  = ie;
            params.decimalScaleFactor(jdec);
            params.binaryScaleFactor(ie);
        }
    }

    if (inumax > 0) {
        // double decimal = grib_power<double>(+*kdec, 10);
        // double divisor = grib_power<double>(-*kbin, 2);
        double decimal = grib_power<double>(+params.decimalScaleFactor(), 10);
        double divisor = grib_power<double>(-params.binaryScaleFactor(), 2);
        double min     = pmin * decimal;
        double max     = pmax * decimal;
        long vmin, vmax;
        // if (grib_get_nearest_smaller_value(min, ref) != LIB_ECCODES_SUCCESS) {
        auto ref = nearest_smaller_value(min);
        if (!ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            //grib_context_log(gh->context, LIB_ECCODES_LOG_ERROR,
            //                 "unable to find nearest_smaller_value of %g for %s", min, reference_value);
            // return LIB_ECCODES_INTERNAL_ERROR;
            throw std::runtime_error("unable to find nearest_smaller_value");
        }

        // vmax = (((pmax * decimal) - *ref) * divisor) + 0.5;
        // vmin = (((pmin * decimal) - *ref) * divisor) + 0.5;
        vmax = static_cast<long>((((pmax * decimal) - params.referenceValue()) * divisor) + 0.5);
        vmin = static_cast<long>((((pmin * decimal) - params.referenceValue()) * divisor) + 0.5);

        /* This may happen if pmin*decimal-*ref is too large */
        if ((vmin != 0) || (vmax > inbint))
            inumax = 0;
    }

    /* If seeking for an optimal decimal scale factor fails, fall back to a basic method */
    if (inumax == 0) {
        int last   = compat_gribex_ ? 99 : 127;
        double min = pmin, max = pmax;
        double range    = max - min;
        double f        = grib_power<double>(knbit_, 2) - 1;
        double minrange = grib_power<double>(-last, 2) * f;
        double maxrange = grib_power<double>(+last, 2) * f;
        double decimal  = 1;
        int err;

        // *kdec = 0;
        params.decimalScaleFactor(0);

        while (range < minrange) {
            // *kdec += 1;
            params.decimalScaleFactor(params.decimalScaleFactor() + 1);
            decimal *= 10;
            min   = pmin * decimal;
            max   = pmax * decimal;
            range = max - min;
        }

        while (range > maxrange) {
            // *kdec -= 1;
            params.decimalScaleFactor(params.decimalScaleFactor() - 1);
            decimal /= 10;
            min   = pmin * decimal;
            max   = pmax * decimal;
            range = max - min;
        }

        // if (grib_get_nearest_smaller_value(min, ref) != LIB_ECCODES_SUCCESS) {
        auto ref = nearest_smaller_value(min);
        if (ref.has_value()) {
            params.referenceValue(ref.value());
        }
        else {
            //grib_context_log(gh->context, LIB_ECCODES_LOG_ERROR,
            //"unable to find nearest_smaller_value of %g for %s", min, reference_value);
            // return LIB_ECCODES_INTERNAL_ERROR;
            throw std::runtime_error("unable to find nearest_smaller_value");
        }

        // *kbin = grib_get_binary_scale_fact(max, *ref, knbit, &err);
        params.binaryScaleFactor(grib_get_binary_scale_fact(max, params.referenceValue(), knbit_, &err));

        if (err == LIB_ECCODES_UNDERFLOW) {
            // *kbin = 0;
            // *kdec = 0;
            // *ref  = 0;
            params.binaryScaleFactor(0);
            params.decimalScaleFactor(0);
            params.referenceValue(0.);
        }
    }

    params.bitsPerValue(knbit_);

    return params;
}
