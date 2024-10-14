/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_sh_unpacked.h"
#include "grib_scaling.h"

grib_accessor_data_sh_unpacked_t _grib_accessor_data_sh_unpacked{};
grib_accessor* grib_accessor_data_sh_unpacked = &_grib_accessor_data_sh_unpacked;

typedef unsigned long (*encode_float_proc)(double);
typedef double (*decode_float_proc)(unsigned long);

void grib_accessor_data_sh_unpacked_t::init(const long v, grib_arguments* args)
{
    grib_accessor_data_simple_packing_t::init(v, args);
    grib_handle* hand = grib_handle_of_accessor(this);

    GRIBEX_sh_bug_present_  = grib_arguments_get_name(hand, args, carg_++);
    ieee_floats_            = grib_arguments_get_name(hand, args, carg_++);
    laplacianOperatorIsSet_ = grib_arguments_get_name(hand, args, carg_++);
    laplacianOperator_      = grib_arguments_get_name(hand, args, carg_++);
    sub_j_                  = grib_arguments_get_name(hand, args, carg_++);
    sub_k_                  = grib_arguments_get_name(hand, args, carg_++);
    sub_m_                  = grib_arguments_get_name(hand, args, carg_++);
    pen_j_                  = grib_arguments_get_name(hand, args, carg_++);
    pen_k_                  = grib_arguments_get_name(hand, args, carg_++);
    pen_m_                  = grib_arguments_get_name(hand, args, carg_++);

    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
    length_ = 0;
}

int grib_accessor_data_sh_unpacked_t::value_count(long* count)
{
    int ret = 0;

    long sub_j = 0;
    long sub_k = 0;
    long sub_m = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), sub_j_, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), sub_k_, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), sub_m_, &sub_m)) != GRIB_SUCCESS)
        return ret;

    if (sub_j != sub_k || sub_j != sub_m) {
        grib_context_log(context_, GRIB_LOG_ERROR, "sub_j=%ld, sub_k=%ld, sub_m=%ld\n", sub_j, sub_k, sub_m);
        Assert((sub_j == sub_k) && (sub_j == sub_m));
    }
    *count = (sub_j + 1) * (sub_j + 2);
    return ret;
}

int grib_accessor_data_sh_unpacked_t::unpack_double(double* val, size_t* len)
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

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), offsetdata_, &offsetdata)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), GRIBEX_sh_bug_present_, &GRIBEX_sh_bug_present)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), ieee_floats_, &ieee_floats)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), sub_j_, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), sub_k_, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), sub_m_, &sub_m)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), pen_j_, &pen_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), pen_k_, &pen_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), pen_m_, &pen_m)) != GRIB_SUCCESS)
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

    Assert(sub_j == sub_k);
    Assert(sub_j == sub_m);
    Assert(pen_j == pen_k);
    Assert(pen_j == pen_m);

    buf = (unsigned char*)grib_handle_of_accessor(this)->buffer->data;

    maxv = pen_j + 1;

    buf += offsetdata;
    hres = buf;
    lres = buf;

    packed_offset = offsetdata + bytes * (sub_k + 1) * (sub_k + 2);

    lpos = 8 * (packed_offset - offsetdata);

    s = codes_power<double>(binary_scale_factor, 2);
    d = codes_power<double>(-decimal_scale_factor, 10);

    scals = (double*)grib_context_malloc(context_, maxv * sizeof(double));
    Assert(scals);
    if ((ret = grib_get_double_internal(grib_handle_of_accessor(this), laplacianOperator_, &laplacianOperator)) != GRIB_SUCCESS)
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

    Assert(*len >= i);
    *len = n_vals;

    grib_context_free(context_, scals);

    return ret;
}
