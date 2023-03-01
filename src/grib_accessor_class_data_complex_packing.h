#pragma once

#include "grib_api_internal_cpp.h"
#include <type_traits>

typedef unsigned long (*encode_float_proc)(double);
typedef double (*decode_float_proc)(unsigned long);

typedef struct grib_accessor_data_complex_packing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in values */
    int  carg;
    const char* seclen;
    const char* offsetdata;
    const char* offsetsection;
    int dirty;
    /* Members defined in data_simple_packing */
    int edition;
    const char*  units_factor;
    const char*  units_bias;
    const char*  changing_precision;
    const char*  number_of_values;
    const char*  bits_per_value;
    const char*  reference_value;
    const char*  binary_scale_factor;
    const char*  decimal_scale_factor;
    const char*  optimize_scaling_factor;
    /* Members defined in data_complex_packing */
    const char*  GRIBEX_sh_bug_present;
    const char*  ieee_floats;
    const char*  laplacianOperatorIsSet;
    const char*  laplacianOperator;
    const char*  sub_j;
    const char*  sub_k;
    const char*  sub_m;
    const char*  pen_j;
    const char*  pen_k;
    const char*  pen_m;
} grib_accessor_data_complex_packing;


template <typename T>
class GribAccessorClassDataComplexPacking {
public:
    static int unpack(grib_accessor* a, T* val, size_t* len);
};

// ECC-1467: FloatDataValues
template <typename T>
int GribAccessorClassDataComplexPacking<T>::unpack(grib_accessor* a, T* val, size_t* len)
{
    grib_accessor_data_complex_packing* self = (grib_accessor_data_complex_packing*)a;
    grib_handle* gh                          = grib_handle_of_accessor(a);

    size_t i       = 0;
    int ret        = GRIB_SUCCESS;
    long hcount    = 0;
    long lcount    = 0;
    long hpos      = 0;
    long lup       = 0;
    long mmax      = 0;
    long n_vals    = 0;
    T* scals  = NULL;
    T*pscals = NULL, *pval = NULL;

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

    T operat = 0;
    int bytes;
    int err = 0;

    decode_float_proc decode_float = NULL;

    err = grib_value_count(a, &n_vals);
    if (err)
        return err;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((ret = grib_get_long_internal(gh, self->offsetdata, &offsetdata)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(gh, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(gh, self->GRIBEX_sh_bug_present, &GRIBEX_sh_bug_present)) != GRIB_SUCCESS)
        return ret;

    /* ECC-774: don't use grib_get_long_internal */
    if ((ret = grib_get_long(gh, self->ieee_floats, &ieee_floats)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_double_internal(gh, self->laplacianOperator, &laplacianOperator)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->sub_j, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->sub_k, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->sub_m, &sub_m)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->pen_j, &pen_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->pen_k, &pen_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(gh, self->pen_m, &pen_m)) != GRIB_SUCCESS)
        return ret;

    self->dirty = 0;

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

    buf = (unsigned char*)gh->buffer->data;

    maxv = pen_j + 1;

    buf += grib_byte_offset(a);
    hres = buf;
    lres = buf;

    if (pen_j == sub_j) {
        n_vals = (pen_j + 1) * (pen_j + 2);
        d      = grib_power(-decimal_scale_factor, 10);
        grib_ieee_decode_array<T>(a->context, buf, n_vals, bytes, val);
        if (d) {
            for (i = 0; i < n_vals; i++)
                val[i] *= d;
        }
        return 0;
    }


    packed_offset = grib_byte_offset(a) + bytes * (sub_k + 1) * (sub_k + 2);

    lpos = 8 * (packed_offset - offsetdata);

    s = grib_power(binary_scale_factor, 2);
    d = grib_power(-decimal_scale_factor, 10);

    scals = (T*)grib_context_malloc(a->context, maxv * sizeof(T));
    Assert(scals);

    scals[0] = 0;
    for (i = 1; i < maxv; i++) {
        operat = pow(i * (i + 1), laplacianOperator);
        if (operat != 0)
            scals[i] = (1.0 / operat);
        else {
            grib_context_log(a->context, GRIB_LOG_WARNING,
                             "COMPLEX_PACKING : problem with operator div by zero at index %d of %d \n",
                             i, maxv);
            scals[i] = 0;
        }
    }

    /*
    printf("UNPACKING LAPLACE=%.20f\n",laplacianOperator);
    printf("packed offset=%ld\n",packed_offset);
    for(i=0;i<maxv;i++)
        printf("scals[%d]=%g\n",i,scals[i]);*/

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

        pscals = scals + lup;
        pval   = val + i;
#if FAST_BIG_ENDIAN
        grib_decode_double_array_complex(lres,
                                         &lpos, bits_per_value,
                                         reference_value, s, pscals, (maxv - hcount) * 2, pval);
        i += (maxv - hcount) * 2;
#else
        (void)pscals; /* suppress gcc warning */
        (void)pval;   /* suppress gcc warning */
        for (lcount = hcount; lcount < maxv; lcount++) {
            val[i++] = d * (T)((grib_decode_unsigned_long(lres, &lpos, bits_per_value) * s) + reference_value) * scals[lup];
            val[i++] = d * (T)((grib_decode_unsigned_long(lres, &lpos, bits_per_value) * s) + reference_value) * scals[lup];
            /* These values should always be zero, but as they are packed,
               it is necessary to force them back to zero */
            if (mmax == 0)
                val[i - 1] = 0;
            lup++;
        }
#endif

        maxv--;
        hcount = 0;
        mmax++;
    }

    Assert(*len >= i);
    *len = i;

    grib_context_free(a->context, scals);

    return ret;
}
