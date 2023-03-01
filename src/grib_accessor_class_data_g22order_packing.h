#pragma once

#include "grib_api_internal.h"

typedef struct grib_accessor_data_g22order_packing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in values */
    int  carg;
    const char* seclen;
    const char* offsetdata;
    const char* offsetsection;
    int dirty;
    /* Members defined in data_g22order_packing */
    const char*  numberOfValues;
    const char*  bits_per_value;
    const char*  reference_value;
    const char*  binary_scale_factor;
    const char*  decimal_scale_factor;
    const char*  typeOfOriginalFieldValues;
    const char*  groupSplittingMethodUsed;
    const char*  missingValueManagementUsed;
    const char*  primaryMissingValueSubstitute;
    const char*  secondaryMissingValueSubstitute;
    const char*  numberOfGroupsOfDataValues;
    const char*  referenceForGroupWidths;
    const char*  numberOfBitsUsedForTheGroupWidths;
    const char*  referenceForGroupLengths;
    const char*  lengthIncrementForTheGroupLengths;
    const char*  trueLengthOfLastGroup;
    const char*  numberOfBitsUsedForTheScaledGroupLengths;
    const char*  orderOfSpatialDifferencing;
    const char*  numberOfOctetsExtraDescriptors;
} grib_accessor_data_g22order_packing;

int post_process(grib_context* c, long* vals, long len, long order, long bias, const unsigned long extras[2]);

template <typename T>
class GribAccessorDataG22orderPacking {
public:
    static int unpack(grib_accessor* a, T* val, size_t* len);
};

template <typename T>
int GribAccessorDataG22orderPacking<T>::unpack(grib_accessor* a, T* val, size_t* len)
{
    grib_accessor_data_g22order_packing* self = (grib_accessor_data_g22order_packing*)a;

    size_t i    = 0;
    size_t j    = 0;
    long n_vals = 0;
    long vcount = 0;
    int err     = GRIB_SUCCESS;

    long* sec_val   = NULL;
    grib_handle* gh = grib_handle_of_accessor(a);

    unsigned char* buf        = (unsigned char*)gh->buffer->data;
    unsigned char* buf_ref    = NULL;
    unsigned char* buf_width  = NULL;
    unsigned char* buf_length = NULL;
    unsigned char* buf_vals   = NULL;

    long length_p = 0;
    long ref_p    = 0;
    long width_p  = 0;
    long vals_p   = 0;

    long nvals_per_group     = 0;
    long nbits_per_group_val = 0;
    long group_ref_val       = 0;

    long bits_per_value    = 0;
    T binary_s        = 0;
    T decimal_s       = 0;
    double reference_value = 0;

    long binary_scale_factor;
    long decimal_scale_factor;
    long typeOfOriginalFieldValues;
    long groupSplittingMethodUsed;
    long missingValueManagementUsed;
    long primaryMissingValueSubstitute;
    long secondaryMissingValueSubstitute;
    long numberOfGroupsOfDataValues;
    long referenceForGroupWidths;
    long numberOfBitsUsedForTheGroupWidths;
    long referenceForGroupLengths;
    long lengthIncrementForTheGroupLengths;
    long trueLengthOfLastGroup;
    long numberOfBitsUsedForTheScaledGroupLengths;
    long orderOfSpatialDifferencing;
    long numberOfOctetsExtraDescriptors;
    double missingValue = 0;

    err = grib_value_count(a, &n_vals);
    if (err)
        return err;

    if (*len < (size_t)n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(gh, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->typeOfOriginalFieldValues, &typeOfOriginalFieldValues)) != GRIB_SUCCESS)
        return err;

    /* Don't call grib_get_long_internal to suppress error message being output */
    if ((err = grib_get_long(gh, self->groupSplittingMethodUsed, &groupSplittingMethodUsed)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->missingValueManagementUsed, &missingValueManagementUsed)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->primaryMissingValueSubstitute, &primaryMissingValueSubstitute)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->secondaryMissingValueSubstitute, &secondaryMissingValueSubstitute)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfGroupsOfDataValues, &numberOfGroupsOfDataValues)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->referenceForGroupWidths, &referenceForGroupWidths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfBitsUsedForTheGroupWidths, &numberOfBitsUsedForTheGroupWidths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->referenceForGroupLengths, &referenceForGroupLengths)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->lengthIncrementForTheGroupLengths, &lengthIncrementForTheGroupLengths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->trueLengthOfLastGroup, &trueLengthOfLastGroup)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfBitsUsedForTheScaledGroupLengths, &numberOfBitsUsedForTheScaledGroupLengths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->orderOfSpatialDifferencing, &orderOfSpatialDifferencing)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfOctetsExtraDescriptors, &numberOfOctetsExtraDescriptors)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(gh, "missingValue", &missingValue)) != GRIB_SUCCESS)
        return err;

    self->dirty = 0;

    sec_val = (long*)grib_context_malloc(a->context, (n_vals) * sizeof(long));
    if (!sec_val)
        return GRIB_OUT_OF_MEMORY;
    memset(sec_val, 0, (n_vals) * sizeof(long)); /* See SUP-718 */

    buf_ref = buf + a->offset;

    ref_p = (numberOfGroupsOfDataValues * bits_per_value);

    if (orderOfSpatialDifferencing)
        ref_p += (1 + orderOfSpatialDifferencing) * (numberOfOctetsExtraDescriptors * 8);

    buf_width = buf_ref + (ref_p / 8) + ((ref_p % 8) ? 1 : 0);

    width_p    = (numberOfGroupsOfDataValues * numberOfBitsUsedForTheGroupWidths);
    buf_length = buf_width + (width_p / 8) + ((width_p % 8) ? 1 : 0);

    length_p = (numberOfGroupsOfDataValues * numberOfBitsUsedForTheScaledGroupLengths);
    buf_vals = buf_length + (length_p / 8) + ((length_p % 8) ? 1 : 0);

    length_p = 0;
    ref_p    = orderOfSpatialDifferencing ? (orderOfSpatialDifferencing + 1) * (numberOfOctetsExtraDescriptors * 8) : 0;
    width_p  = 0;
    vals_p   = 0;
    vcount   = 0;

    for (i = 0; i < numberOfGroupsOfDataValues; i++) {
        group_ref_val       = grib_decode_unsigned_long(buf_ref, &ref_p, bits_per_value);
        nvals_per_group     = grib_decode_unsigned_long(buf_length, &length_p, numberOfBitsUsedForTheScaledGroupLengths);
        nbits_per_group_val = grib_decode_unsigned_long(buf_width, &width_p, numberOfBitsUsedForTheGroupWidths);

        nvals_per_group *= lengthIncrementForTheGroupLengths;
        nvals_per_group += referenceForGroupLengths;
        nbits_per_group_val += referenceForGroupWidths;

        if (i == numberOfGroupsOfDataValues - 1)
            nvals_per_group = trueLengthOfLastGroup;
        Assert(n_vals >= vcount + nvals_per_group);

        /*grib_decode_long_array(buf_vals, &vals_p, nbits_per_group_val, nvals_per_group,
                               &sec_val[vcount]); */
        if (missingValueManagementUsed == 0) {
            /* No explicit missing values included within data values */
            for (j = 0; j < nvals_per_group; j++) {
                DebugAssertAccess(sec_val, (long)(vcount + j), n_vals);
                sec_val[vcount + j] = group_ref_val + grib_decode_unsigned_long(buf_vals, &vals_p, nbits_per_group_val);
                /*printf("sec_val[%ld]=%ld\n", vcount+j, sec_val[vcount+j]);*/
            }
        }
        else if (missingValueManagementUsed == 1) {
            /* Primary missing values included within data values */
            long maxn = 0; /* (1 << bits_per_value) - 1; */
            for (j = 0; j < nvals_per_group; j++) {
                if (nbits_per_group_val == 0) {
                    maxn = (1 << bits_per_value) - 1;
                    if (group_ref_val == maxn) {
                        sec_val[vcount + j] = LONG_MAX; /* missing value */
                    }
                    else {
                        long temp           = grib_decode_unsigned_long(buf_vals, &vals_p, nbits_per_group_val);
                        sec_val[vcount + j] = group_ref_val + temp;
                    }
                }
                else {
                    long temp = grib_decode_unsigned_long(buf_vals, &vals_p, nbits_per_group_val);
                    maxn      = (1 << nbits_per_group_val) - 1;
                    if (temp == maxn) {
                        sec_val[vcount + j] = LONG_MAX; /* missing value */
                    }
                    else {
                        sec_val[vcount + j] = group_ref_val + temp;
                    }
                }
            }
        }
        else if (missingValueManagementUsed == 2) {
            /* Primary and secondary missing values included within data values */
            long maxn  = (1 << bits_per_value) - 1;
            long maxn2 = 0; /* maxn - 1; */
            for (j = 0; j < nvals_per_group; j++) {
                if (nbits_per_group_val == 0) {
                    maxn2 = maxn - 1;
                    if (group_ref_val == maxn || group_ref_val == maxn2) {
                        sec_val[vcount + j] = LONG_MAX; /* missing value */
                    }
                    else {
                        long temp           = grib_decode_unsigned_long(buf_vals, &vals_p, nbits_per_group_val);
                        sec_val[vcount + j] = group_ref_val + temp;
                    }
                }
                else {
                    long temp = grib_decode_unsigned_long(buf_vals, &vals_p, nbits_per_group_val);
                    maxn      = (1 << nbits_per_group_val) - 1;
                    maxn2     = maxn - 1;
                    if (temp == maxn || temp == maxn2) {
                        sec_val[vcount + j] = LONG_MAX; /* missing value */
                    }
                    else {
                        sec_val[vcount + j] = group_ref_val + temp;
                    }
                }
            }
        }

        vcount += nvals_per_group;
    }

    if (orderOfSpatialDifferencing) {
        long bias               = 0;
        unsigned long extras[2] = {0,};
        ref_p = 0;

        /* For Complex packing, order == 0 */
        /* For Complex packing and spatial differencing, order == 1 or 2 (code table 5.6) */
        if (orderOfSpatialDifferencing != 1 && orderOfSpatialDifferencing != 2) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "Unsupported order of spatial differencing %ld", orderOfSpatialDifferencing);
            return GRIB_INTERNAL_ERROR;
        }

        for (i = 0; i < orderOfSpatialDifferencing; i++) {
            extras[i] = grib_decode_unsigned_long(buf_ref, &ref_p, numberOfOctetsExtraDescriptors * 8);
        }

        bias = grib_decode_signed_longb(buf_ref, &ref_p, numberOfOctetsExtraDescriptors * 8);

        post_process(a->context, sec_val, n_vals, orderOfSpatialDifferencing, bias, extras);
        /*de_spatial_difference (a->context, sec_val, n_vals, orderOfSpatialDifferencing, bias);*/
    }

    binary_s  = grib_power(binary_scale_factor, 2);
    decimal_s = grib_power(-decimal_scale_factor, 10);

    for (i = 0; i < n_vals; i++) {
        if (sec_val[i] == LONG_MAX) {
            val[i] = missingValue;
        }
        else {
            val[i] = (double)((((double)sec_val[i]) * binary_s) + reference_value) * decimal_s;
        }
    }

    grib_context_free(a->context, sec_val);
    return err;
}

