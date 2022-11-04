/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_values
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double_element;unpack_double_element_set
   IMPLEMENTS = value_count
   MEMBERS=const char*  numberOfValues
   MEMBERS=const char*  bits_per_value
   MEMBERS=const char*  reference_value
   MEMBERS=const char*  binary_scale_factor
   MEMBERS=const char*  decimal_scale_factor
   MEMBERS=const char*  typeOfOriginalFieldValues
   MEMBERS=const char*  groupSplittingMethodUsed
   MEMBERS=const char*  missingValueManagementUsed
   MEMBERS=const char*  primaryMissingValueSubstitute
   MEMBERS=const char*  secondaryMissingValueSubstitute
   MEMBERS=const char*  numberOfGroupsOfDataValues
   MEMBERS=const char*  referenceForGroupWidths
   MEMBERS=const char*  numberOfBitsUsedForTheGroupWidths
   MEMBERS=const char*  referenceForGroupLengths
   MEMBERS=const char*  lengthIncrementForTheGroupLengths
   MEMBERS=const char*  trueLengthOfLastGroup
   MEMBERS=const char*  numberOfBitsUsedForTheScaledGroupLengths
   MEMBERS=const char*  orderOfSpatialDifferencing
   MEMBERS=const char*  numberOfOctetsExtraDescriptors

   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);
static int unpack_double_element(grib_accessor*, size_t i, double* val);
static int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array);

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

extern grib_accessor_class* grib_accessor_class_values;

static grib_accessor_class _grib_accessor_class_data_g22order_packing = {
    &grib_accessor_class_values,                      /* super */
    "data_g22order_packing",                      /* name */
    sizeof(grib_accessor_data_g22order_packing),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    0,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    0,                  /* grib_pack procedures long */
    0,                /* grib_unpack procedures long */
    &pack_double,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    0,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    &unpack_double_element,      /* unpack only ith value */
    &unpack_double_element_set,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_g22order_packing = &_grib_accessor_class_data_g22order_packing;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->get_native_type    =    (*(c->super))->get_native_type;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_long    =    (*(c->super))->pack_long;
    c->unpack_long    =    (*(c->super))->unpack_long;
    c->pack_string    =    (*(c->super))->pack_string;
    c->unpack_string    =    (*(c->super))->unpack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_g22order_packing* self = (grib_accessor_data_g22order_packing*)a;
    grib_handle* gh                           = grib_handle_of_accessor(a);

    self->numberOfValues       = grib_arguments_get_name(gh, args, self->carg++);
    self->bits_per_value       = grib_arguments_get_name(gh, args, self->carg++);
    self->reference_value      = grib_arguments_get_name(gh, args, self->carg++);
    self->binary_scale_factor  = grib_arguments_get_name(gh, args, self->carg++);
    self->decimal_scale_factor = grib_arguments_get_name(gh, args, self->carg++);

    self->typeOfOriginalFieldValues                = grib_arguments_get_name(gh, args, self->carg++);
    self->groupSplittingMethodUsed                 = grib_arguments_get_name(gh, args, self->carg++);
    self->missingValueManagementUsed               = grib_arguments_get_name(gh, args, self->carg++);
    self->primaryMissingValueSubstitute            = grib_arguments_get_name(gh, args, self->carg++);
    self->secondaryMissingValueSubstitute          = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfGroupsOfDataValues               = grib_arguments_get_name(gh, args, self->carg++);
    self->referenceForGroupWidths                  = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfBitsUsedForTheGroupWidths        = grib_arguments_get_name(gh, args, self->carg++);
    self->referenceForGroupLengths                 = grib_arguments_get_name(gh, args, self->carg++);
    self->lengthIncrementForTheGroupLengths        = grib_arguments_get_name(gh, args, self->carg++);
    self->trueLengthOfLastGroup                    = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfBitsUsedForTheScaledGroupLengths = grib_arguments_get_name(gh, args, self->carg++);

    self->orderOfSpatialDifferencing     = grib_arguments_get_name(gh, args, self->carg++);
    self->numberOfOctetsExtraDescriptors = grib_arguments_get_name(gh, args, self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

#if 0
static int      reverse_rows (unsigned long* data, long  len, long number_along_parallel){
    long count = 0;
    long i = 0;
    long left = 0;
    long right = number_along_parallel-1;
    long tmp = 0;

    data += number_along_parallel;
    count = number_along_parallel;

    while(count < len){
        left = 0;
        right = number_along_parallel-1;

        for (i = 0;i<number_along_parallel/2;i++){
            tmp = data[left];
            data[left] = data[right];
            data[right] = tmp;

            right--;
            left++;
        }

        data  += number_along_parallel*2;
        count += number_along_parallel*2;
    }

    return 0;
}
#endif

static unsigned long calc_pow_2(unsigned long op)
{
    unsigned long a = 1;
    while (op--)
        a *= 2;
    return a;
}

static int calc_bits_needed(unsigned long spread)
{
    int nbit = 0;
    if (spread == 0)
        return nbit;
    while (spread > 0) {
        spread /= 2;
        nbit++;
    }
    return nbit;
}

static int find_next_group(const unsigned long* vals, size_t len, unsigned long w, unsigned long l, long* nbits, long* groupsize, long* r_val)
{
    unsigned long lmin = 0;
    unsigned long lmax = 0;

    size_t i = 0;

    if (len == 0)
        return GRIB_ARRAY_TOO_SMALL;
    lmin = vals[0];
    lmax = lmin;

    while (i < len) {
        if (vals[i] > lmax)
            lmax = vals[i];
        else if (vals[i] < lmin)
            lmin = vals[i];
        /*Assert((lmax-lmin) >= 0);*/
        *nbits = calc_bits_needed(lmax - lmin);
        *r_val = lmin;
        i++;
        *groupsize = i;

        if (*groupsize > l - 2)
            return GRIB_SUCCESS;
        if (*nbits > w - 2)
            return GRIB_SUCCESS;
    }

    return GRIB_SUCCESS;
}

#if 0
static int spatial_difference (grib_context *c, unsigned long* vals, long  len, long order, long* bias){

    long j = 3;


    long *v = grib_context_malloc(c,(len)*sizeof(long));

    for(j = 0; j< len;j++)
        v[j] = vals[j];

    if (order == 1){
        grib_context_free(c,v);
        return GRIB_NOT_IMPLEMENTED;
        *bias=v[order];
        for(j = order; j< len;j++){
            v[j]  -=  vals [j-1];
            if(*bias>v[j])
                *bias = v[j];
        }
    }

    if (order == 2){
        *bias=v[order];
        for(j = order; j< len;j++){
            v[j]  -=  vals [j-1];
            v[j]  -=  vals [j-1] - vals[j-2];
            if(*bias>v[j])
                *bias = v[j];
        }
    }
    if (order == 3){
        grib_context_free(c,v);
        return GRIB_NOT_IMPLEMENTED;
        *bias=v[order];
        for(j = order; j< len;j++){
            v[j]  -=  vals[j-1];
            v[j]  -=  vals[j-1] - vals[j-2];
            v[j]  -=  vals[j-2] - vals[j-3];
            if(*bias>v[j])
                *bias = v[j];
        }
    }
    for(j = order; j< len;j++){
        Assert(v[j]-*bias >=0);
        vals[j] = v[j]-*bias;
    }

    grib_context_free(c,v);
    return 0;
}
#endif

static int post_process(grib_context* c, long* vals, long len, long order, long bias, const unsigned long extras[2])
{
    unsigned long last, penultimate, j = 0;
    Assert(order > 0);
    Assert(order <= 3);
    if (!vals)
        return GRIB_INTERNAL_ERROR;

    if (order == 1) {
        last = extras[0];
        while (j < len) {
            if (vals[j] == LONG_MAX)
                j++;
            else {
                vals[j++] = extras[0];
                break;
            }
        }
        while (j < len) {
            if (vals[j] == LONG_MAX)
                j++;
            else {
                vals[j] += last + bias;
                last = vals[j++];
            }
        }
    }
    else if (order == 2) {
        penultimate = extras[0];
        last        = extras[1];
        while (j < len) {
            if (vals[j] == LONG_MAX)
                j++;
            else {
                vals[j++] = extras[0];
                break;
            }
        }
        while (j < len) {
            if (vals[j] == LONG_MAX)
                j++;
            else {
                vals[j++] = extras[1];
                break;
            }
        }
        for (; j < len; j++) {
            if (vals[j] != LONG_MAX) {
                vals[j]     = vals[j] + bias + last + last - penultimate;
                penultimate = last;
                last        = vals[j];
            }
        }
    }
    return GRIB_SUCCESS;
}

#if 0
static int de_spatial_difference (grib_context *c, unsigned long* vals, long  len, long order, long bias)
{
    long j = 0;

    long i_origin = 0;
    long i_first_diff = 0;
    long i_second_diff = 0;

    Assert(order > 0);
    Assert(order <= 3);

    i_origin=vals[order-1];

    if (order == 1) {
        for(j = 1; j< len;j++){
            i_origin=i_origin+(vals[j]+bias);
            vals[j]=i_origin;
        }
    }

    if (order == 2){
        i_first_diff=vals[1]-vals[0];
        for(j = 2; j< len;j++){
            i_first_diff=i_first_diff+(vals[j]+bias);
            i_origin=i_origin+i_first_diff;
            vals[j]=i_origin;
        }
    }

    if (order == 3){
        i_first_diff=vals[2]-vals[1];
        i_second_diff=i_first_diff-(vals[1]-vals[0]);
        for(j = 3; j< len;j++){
            i_second_diff=i_second_diff+(vals[j]+bias);
            i_first_diff=i_first_diff+i_second_diff;
            i_origin=i_origin+i_first_diff;
            vals[j]=i_origin;
        }
    }
    return 0;
}
#endif

static int unpack_double(grib_accessor* a, double* val, size_t* len)
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
    double binary_s        = 0;
    double decimal_s       = 0;
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

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_g22order_packing* self = (grib_accessor_data_g22order_packing*)a;
    grib_handle* gh                           = grib_handle_of_accessor(a);

    size_t i      = 0;
    size_t j      = 0;
    size_t n_vals = *len;

    int err = 0;

    unsigned char* buf        = NULL;
    unsigned char* buf_ref    = NULL;
    unsigned char* buf_width  = NULL;
    unsigned char* buf_length = NULL;
    unsigned char* buf_vals   = NULL;

    double d       = 0;
    double divisor = 0;

    unsigned long* sec_val   = NULL;
    unsigned long* group_val = NULL;

    double max;
    double min;

    long length_p = 0;
    long ref_p    = 0;
    long width_p  = 0;
    long vals_p   = 0;

    size_t nv       = 0;
    size_t buf_size = 0;

    long bits_per_value = 0;

    double reference_value = 0;

    long nvals_per_group     = 0;
    long nbits_per_group_val = 0;
    long group_ref_val       = 0;

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

    long maxgrw;
    long maxgrl;
    char packingType[254] = {0,};
    size_t slen = 254;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->typeOfOriginalFieldValues, &typeOfOriginalFieldValues)) != GRIB_SUCCESS)
        return err;

    /*if((err = grib_get_long_internal(gh,self->groupSplittingMethodUsed,&groupSplittingMethodUsed )) != GRIB_SUCCESS) return err;*/
    /* Key groupSplittingMethodUsed uses Code table 5.4 which has two entries:
     *  0 Row by row splitting
     *  1 General group splitting
     * We only support General group splitting
     */
    groupSplittingMethodUsed = 1;

    if ((err = grib_get_long_internal(gh, self->missingValueManagementUsed, &missingValueManagementUsed)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->primaryMissingValueSubstitute, &primaryMissingValueSubstitute)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->secondaryMissingValueSubstitute, &secondaryMissingValueSubstitute)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfBitsUsedForTheGroupWidths, &numberOfBitsUsedForTheGroupWidths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfBitsUsedForTheScaledGroupLengths, &numberOfBitsUsedForTheScaledGroupLengths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->orderOfSpatialDifferencing, &orderOfSpatialDifferencing)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->numberOfOctetsExtraDescriptors, &numberOfOctetsExtraDescriptors)) != GRIB_SUCCESS)
        return err;

    self->dirty = 1;

    referenceForGroupWidths                  = 0;
    referenceForGroupLengths                 = 0;
    lengthIncrementForTheGroupLengths        = 1;
    numberOfBitsUsedForTheGroupWidths        = 4;
    numberOfBitsUsedForTheScaledGroupLengths = 10;

    /*     calculation of integer array   */
    sec_val = (unsigned long*)grib_context_malloc(a->context, (n_vals) * sizeof(long));
    if (!sec_val)
        return GRIB_OUT_OF_MEMORY;

    d = grib_power(decimal_scale_factor, 10);

    max = val[0];
    min = max;
    for (i = 0; i < n_vals; i++) {
        if (val[i] > max)
            max = val[i];
        else if (val[i] < min)
            min = val[i];
    }
    min *= d;
    max *= d;

    if (grib_get_nearest_smaller_value(gh, self->reference_value, min, &reference_value) != GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to find nearest_smaller_value of %g for %s", min, self->reference_value);
        return GRIB_INTERNAL_ERROR;
    }

    binary_scale_factor = grib_get_binary_scale_fact(max, reference_value, bits_per_value, &err);
    if (err != GRIB_SUCCESS)
        return err;
    divisor = grib_power(-binary_scale_factor, 2);

    for (i = 0; i < n_vals; i++)
        sec_val[i] = (unsigned long)((((val[i] * d) - reference_value) * divisor) + 0.5);

    group_val = sec_val;

    maxgrw = calc_pow_2(numberOfBitsUsedForTheGroupWidths);
    maxgrl = calc_pow_2(numberOfBitsUsedForTheScaledGroupLengths);

    numberOfGroupsOfDataValues = 0;
    nv                         = n_vals;
    vals_p                     = 0;

    while (find_next_group(group_val, nv, maxgrw, maxgrl, &nbits_per_group_val, &nvals_per_group, &group_ref_val) == GRIB_SUCCESS) {
        numberOfGroupsOfDataValues++;
        nv -= nvals_per_group;
        group_val += nvals_per_group;
        vals_p += nbits_per_group_val * nvals_per_group;
    }

    trueLengthOfLastGroup = nvals_per_group;

    buf_size = (7 + (numberOfGroupsOfDataValues * bits_per_value)) / 8;
    buf_size += (7 + (numberOfGroupsOfDataValues * numberOfBitsUsedForTheGroupWidths)) / 8;
    buf_size += (7 + (numberOfGroupsOfDataValues * numberOfBitsUsedForTheScaledGroupLengths)) / 8;

    buf_size += (vals_p / 8) + ((vals_p % 8) ? 1 : 0);

    buf = (unsigned char*)grib_context_malloc_clear(a->context, buf_size);

    buf_ref    = buf;
    buf_width  = buf_ref + (7 + (numberOfGroupsOfDataValues * bits_per_value)) / 8;
    buf_length = buf_width + (7 + (numberOfGroupsOfDataValues * numberOfBitsUsedForTheGroupWidths)) / 8;
    buf_vals   = buf_length + (7 + (numberOfGroupsOfDataValues * numberOfBitsUsedForTheScaledGroupLengths)) / 8;

    nv        = n_vals;
    group_val = sec_val;
    vals_p    = 0;
    ref_p     = 0;
    width_p   = 0;
    length_p  = 0;
    i         = 0;

    while (find_next_group(group_val, nv, maxgrw, maxgrl, &nbits_per_group_val, &nvals_per_group, &group_ref_val) == GRIB_SUCCESS) {
        grib_encode_unsigned_longb(buf_ref, group_ref_val, &ref_p, bits_per_value);
        grib_encode_unsigned_longb(buf_width, nbits_per_group_val, &width_p, numberOfBitsUsedForTheGroupWidths);
        grib_encode_unsigned_longb(buf_length, nvals_per_group, &length_p, numberOfBitsUsedForTheScaledGroupLengths);

        if (nbits_per_group_val)
            for (j = 0; j < nvals_per_group; j++) {
                grib_encode_unsigned_longb(buf_vals, sec_val[i + j] - group_ref_val, &vals_p, nbits_per_group_val);
            }
        i += nvals_per_group;
        group_val += nvals_per_group;
        nv -= nvals_per_group;
    }

    /*fprintf(stdout,"spatial pack_double::  %ld  bytes %lu marked\n", (ref_p+7)/8 + (width_p+7)/8 + (length_p+7)/8 + (vals_p+7)/8, buf_size);*/

    grib_buffer_replace(a, buf, buf_size, 1, 1);
    grib_context_free(a->context, buf);
    grib_context_free(a->context, sec_val);

    if ((err = grib_set_long_internal(gh, self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_double_internal(gh, self->reference_value, reference_value)) != GRIB_SUCCESS)
        return err;
    {
        /* Make sure we can decode it again */
        double ref = 1e-100;
        grib_get_double_internal(gh, self->reference_value, &ref);
        Assert(ref == reference_value);
    }
    if ((err = grib_set_long_internal(gh, self->binary_scale_factor, binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->decimal_scale_factor, decimal_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->typeOfOriginalFieldValues, typeOfOriginalFieldValues)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->groupSplittingMethodUsed, groupSplittingMethodUsed)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->missingValueManagementUsed, missingValueManagementUsed)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->primaryMissingValueSubstitute, primaryMissingValueSubstitute)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->secondaryMissingValueSubstitute, secondaryMissingValueSubstitute)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->numberOfGroupsOfDataValues, numberOfGroupsOfDataValues)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->referenceForGroupWidths, referenceForGroupWidths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->numberOfBitsUsedForTheGroupWidths, numberOfBitsUsedForTheGroupWidths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->referenceForGroupLengths, referenceForGroupLengths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->lengthIncrementForTheGroupLengths, lengthIncrementForTheGroupLengths)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->trueLengthOfLastGroup, trueLengthOfLastGroup)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->numberOfBitsUsedForTheScaledGroupLengths, numberOfBitsUsedForTheScaledGroupLengths)) != GRIB_SUCCESS)
        return err;

    err = grib_get_string(gh, "packingType", packingType, &slen);
    if (!err && strcmp(packingType, "grid_complex_spatial_differencing") == 0) {
        if ((err = grib_set_long_internal(gh, self->orderOfSpatialDifferencing, 0)) != GRIB_SUCCESS)
            return err;
        if ((err = grib_set_long_internal(gh, self->numberOfOctetsExtraDescriptors, 0)) != GRIB_SUCCESS)
            return err;
    }

    /* ECC-259: Set correct number of values */
    if ((err = grib_set_long_internal(gh, self->numberOfValues, *len)) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    size_t size    = 0;
    double* values = NULL;
    int err        = grib_get_size(grib_handle_of_accessor(a), "codedValues", &size);
    if (err)
        return err;
    if (idx > size)
        return GRIB_INVALID_ARGUMENT;

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(grib_handle_of_accessor(a), "codedValues", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    *val = values[idx];
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}

static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    size_t size = 0, i = 0;
    double* values;
    int err = 0;

    /* GRIB-564: The indexes in index_array relate to codedValues NOT values! */
    err = grib_get_size(grib_handle_of_accessor(a), "codedValues", &size);
    if (err)
        return err;

    for (i = 0; i < len; i++) {
        if (index_array[i] > size) return GRIB_INVALID_ARGUMENT;
    }

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(grib_handle_of_accessor(a), "codedValues", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    for (i = 0; i < len; i++) {
        val_array[i] = values[index_array[i]];
    }
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_g22order_packing* self = (grib_accessor_data_g22order_packing*)a;
    *count                                    = 0;
    return grib_get_long_internal(grib_handle_of_accessor(a), self->numberOfValues, count);
}
