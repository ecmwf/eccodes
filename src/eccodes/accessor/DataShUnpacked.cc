/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataShUnpacked.h"
#include "grib_scaling.h"

eccodes::AccessorBuilder<eccodes::accessor::DataShUnpacked> _grib_accessor_data_sh_unpacked_builder{};

namespace eccodes::accessor
{

typedef unsigned long (*encode_float_proc)(double);
typedef double (*decode_float_proc)(unsigned long);

void DataShUnpacked::init(const long v, grib_arguments* args)
{
    DataSimplePacking::init(v, args);
    grib_handle* hand = get_enclosing_handle();

    GRIBEX_sh_bug_present_  = args->get_name(hand, carg_++);
    ieee_floats_            = args->get_name(hand, carg_++);
    laplacianOperatorIsSet_ = args->get_name(hand, carg_++);
    laplacianOperator_      = args->get_name(hand, carg_++);
    sub_j_                  = args->get_name(hand, carg_++);
    sub_k_                  = args->get_name(hand, carg_++);
    sub_m_                  = args->get_name(hand, carg_++);
    pen_j_                  = args->get_name(hand, carg_++);
    pen_k_                  = args->get_name(hand, carg_++);
    pen_m_                  = args->get_name(hand, carg_++);

    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
    length_ = 0;
}

int DataShUnpacked::value_count(long* count)
{
    int ret = 0;

    long sub_j = 0;
    long sub_k = 0;
    long sub_m = 0;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_j_, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_k_, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_m_, &sub_m)) != GRIB_SUCCESS)
        return ret;

    if (sub_j != sub_k || sub_j != sub_m) {
        grib_context_log(context_, GRIB_LOG_ERROR, "sub_j=%ld, sub_k=%ld, sub_m=%ld\n", sub_j, sub_k, sub_m);
        ECCODES_ASSERT((sub_j == sub_k) && (sub_j == sub_m));
    }
    *count = (sub_j + 1) * (sub_j + 2);
    return ret;
}

int DataShUnpacked::unpack_double(double* val, size_t* len)
{
    size_t i      = 0;
    int ret       = GRIB_SUCCESS;
    long hcount   = 0;
    long lcount   = 0;
    long hpos     = 0;
    long lup      = 0;
    long mmax     = 0;
    long n_vals   = 0;
    double* scals = NULL;
    /* double *pscals=NULL; */
    double dummy = 0;

    double s                 = 0;
    double d                 = 0;
    double laplacianOperator = 0;
    unsigned char* buf       = NULL;
    unsigned char* hres      = NULL;
    unsigned char* lres      = NULL;
    unsigned long packed_offset;
    long lpos = 0;

    long maxv                  = 0;
    long GRIBEX_sh_bug_present = 0;
    long ieee_floats           = 0;

    long offsetdata           = 0;
    long bits_per_value       = 0;
    double reference_value    = 0;
    long binary_scale_factor  = 0;
    long decimal_scale_factor = 0;

    long sub_j = 0;
    long sub_k = 0;
    long sub_m = 0;
    long pen_j = 0;
    long pen_k = 0;
    long pen_m = 0;

    double operat = 0;
    int bytes;
    int err = 0;

    decode_float_proc decode_float = NULL;

    n_vals = 0;
    err    = value_count(&n_vals);
    if (err)
        return err;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((ret = grib_get_long_internal(get_enclosing_handle(), offsetdata_, &offsetdata)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), GRIBEX_sh_bug_present_, &GRIBEX_sh_bug_present)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), ieee_floats_, &ieee_floats)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_j_, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_k_, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), sub_m_, &sub_m)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), pen_j_, &pen_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), pen_k_, &pen_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(get_enclosing_handle(), pen_m_, &pen_m)) != GRIB_SUCCESS)
        return ret;

    dirty_ = 0;

    switch (ieee_floats) {
        case 0:
            decode_float = grib_long_to_ibm;
            bytes        = 4;
            break;
        case 1:
            decode_float = grib_long_to_ieee;
            bytes        = 4;
            break;
        case 2:
            decode_float = grib_long_to_ieee64;
            bytes        = 8;
            break;
        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    ECCODES_ASSERT(sub_j == sub_k);
    ECCODES_ASSERT(sub_j == sub_m);
    ECCODES_ASSERT(pen_j == pen_k);
    ECCODES_ASSERT(pen_j == pen_m);

    buf = get_enclosing_handle()->buffer->data;

    maxv = pen_j + 1;

    buf += offsetdata;
    hres = buf;
    lres = buf;

    packed_offset = offsetdata + bytes * (sub_k + 1) * (sub_k + 2);

    lpos = 8 * (packed_offset - offsetdata);

    s = codes_power<double>(binary_scale_factor, 2);
    d = codes_power<double>(-decimal_scale_factor, 10);

    scals = (double*)grib_context_malloc(context_, maxv * sizeof(double));
    ECCODES_ASSERT(scals);
    if ((ret = grib_get_double_internal(get_enclosing_handle(), laplacianOperator_, &laplacianOperator)) != GRIB_SUCCESS)
        return ret;

    scals[0] = 0;
    for (i = 1; i < maxv; i++) {
        operat = pow(i * (i + 1), laplacianOperator);
        if (operat != 0)
            scals[i] = (1.0 / operat);
        else {
            scals[i] = 0;
        }
    }

    i = 0;

    while (maxv > 0) {
        lup = mmax;
        if (sub_k >= 0) {
            for (hcount = 0; hcount < sub_k + 1; hcount++) {
                val[i++] = decode_float(grib_decode_unsigned_long(hres, &hpos, 8 * bytes));
                val[i++] = decode_float(grib_decode_unsigned_long(hres, &hpos, 8 * bytes));

                if (GRIBEX_sh_bug_present && hcount == sub_k) {
                    /*  bug in ecmwf data, last row (K+1)is scaled but should not */
                    val[i - 2] *= scals[lup];
                    val[i - 1] *= scals[lup];
                }
                lup++;
            }
            sub_k--;
        }

        /* pscals=scals+lup; */
        for (lcount = hcount; lcount < maxv; lcount++) {
            dummy = d * (double)((grib_decode_unsigned_long(lres, &lpos, bits_per_value) * s) + reference_value);
            (void)dummy;
            dummy = d * (double)((grib_decode_unsigned_long(lres, &lpos, bits_per_value) * s) + reference_value);
            (void)dummy; /* suppress gcc warning */
            lup++;
        }

        maxv--;
        hcount = 0;
        mmax++;
    }

    ECCODES_ASSERT(*len >= i);
    *len = n_vals;

    grib_context_free(context_, scals);

    return ret;
}

}  // namespace eccodes::accessor
