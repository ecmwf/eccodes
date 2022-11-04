/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/********************************
 *   Enrico Fucile
 *******************************/

#include "grib_api_internal.h"
#include "grib_optimize_decimal_factor.h"
#include <float.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_values
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = unpack_double_element;unpack_double_element_set
   IMPLEMENTS = unpack_double_subarray
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=int edition
   MEMBERS=const char*  units_factor
   MEMBERS=const char*  units_bias
   MEMBERS=const char*  changing_precision
   MEMBERS=const char*  number_of_values
   MEMBERS=const char*  bits_per_value
   MEMBERS=const char*  reference_value
   MEMBERS=const char*  binary_scale_factor
   MEMBERS=const char*  decimal_scale_factor
   MEMBERS=const char*  optimize_scaling_factor
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
static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);

typedef struct grib_accessor_data_simple_packing
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
} grib_accessor_data_simple_packing;

extern grib_accessor_class* grib_accessor_class_values;

static grib_accessor_class _grib_accessor_class_data_simple_packing = {
    &grib_accessor_class_values,                      /* super */
    "data_simple_packing",                      /* name */
    sizeof(grib_accessor_data_simple_packing),  /* size */
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
    &unpack_double_subarray,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_simple_packing = &_grib_accessor_class_data_simple_packing;


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
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_simple_packing* self = (grib_accessor_data_simple_packing*)a;
    grib_handle* gh                         = grib_handle_of_accessor(a);
    self->units_factor                      = grib_arguments_get_name(gh, args, self->carg++);
    self->units_bias                        = grib_arguments_get_name(gh, args, self->carg++);
    self->changing_precision                = grib_arguments_get_name(gh, args, self->carg++);
    self->number_of_values                  = grib_arguments_get_name(gh, args, self->carg++);
    self->bits_per_value                    = grib_arguments_get_name(gh, args, self->carg++);
    self->reference_value                   = grib_arguments_get_name(gh, args, self->carg++);
    self->binary_scale_factor               = grib_arguments_get_name(gh, args, self->carg++);
    self->decimal_scale_factor              = grib_arguments_get_name(gh, args, self->carg++);
    self->optimize_scaling_factor           = grib_arguments_get_name(gh, args, self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
    self->dirty = 1;
}

static const unsigned long nbits[32] = {
    0x1, 0x2, 0x4, 0x8, 0x10, 0x20,
    0x40, 0x80, 0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000,
    0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000,
    0x40000000, 0x80000000
};

static int number_of_bits(unsigned long x, long* result)
{
    const int count        = sizeof(nbits) / sizeof(nbits[0]);
    const unsigned long* n = nbits;
    *result                = 0;
    while (x >= *n) {
        n++;
        (*result)++;
        if (*result >= count) {
            return GRIB_ENCODING_ERROR;
        }
    }
    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* number_of_values)
{
    grib_accessor_data_simple_packing* self = (grib_accessor_data_simple_packing*)a;
    *number_of_values                       = 0;

    return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_values, number_of_values);
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    grib_accessor_data_simple_packing* self = (grib_accessor_data_simple_packing*)a;

    long n_vals;
    int err         = 0;
    grib_handle* gh = grib_handle_of_accessor(a);

    double reference_value;
    long binary_scale_factor;
    long bits_per_value;
    long decimal_scale_factor;
    unsigned char* buf = (unsigned char*)gh->buffer->data;
    double s           = 0;
    double d           = 0;
    long pos           = 0;

    n_vals = 0;
    err    = grib_value_count(a, &n_vals);
    if (err)
        return err;

    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;

    self->dirty = 0;

    if ((err = grib_get_double_internal(gh, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    /* Special case */
    if (bits_per_value == 0) {
        *val = reference_value;
        return GRIB_SUCCESS;
    }

    Assert(idx < n_vals);
    s = grib_power(binary_scale_factor, 2);
    d = grib_power(-decimal_scale_factor, 10);

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_data_simple_packing: unpack_double_element: creating %s, %d values (idx=%ld)",
                     a->name, n_vals, idx);

    buf += grib_byte_offset(a);

    /*Assert(((bits_per_value*n_vals)/8) < (1<<29));*/ /* See GRIB-787 */

    if (bits_per_value % 8) {
        grib_context_log(a->context, GRIB_LOG_DEBUG,
                         "unpack_double_element: calling outline function : bpv %d, rv : %g, sf : %d, dsf : %d ",
                         bits_per_value, reference_value, binary_scale_factor, decimal_scale_factor);
        pos  = idx * bits_per_value;
        *val = (double)(((
                             grib_decode_unsigned_long(buf, &pos, bits_per_value) * s) +
                         reference_value) *
                        d);
        /* val[i] = grib_decode_unsigned_long(buf, &pos, bits_per_value);                                   */
        /* fprintf(stdout,"unpck uuu-o: %d vals %d bitspv buf %d by long \n", n_vals, bits_per_value, pos/8);*/
    }
    else {
        int bc       = 0;
        size_t octet = 0;
        long lvalue  = 0;
        int l        = bits_per_value / 8;

        pos = idx * l;
        buf += pos;
        lvalue |= buf[octet++];

        for (bc = 1; bc < l; bc++) {
            lvalue <<= 8;
            lvalue |= buf[octet++];
        }
        *val = (double)(((lvalue * s) + reference_value) * d);
    }

    return err;
}

static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    int err = 0;
    size_t i = 0;
    for (i=0; i<len; ++i) {
        if ((err = unpack_double_element(a, index_array[i], val_array+i)) != GRIB_SUCCESS)
            return err;
    }
    return GRIB_SUCCESS;
}

static int _unpack_double(grib_accessor* a, double* val, size_t* len, unsigned char* buf, long pos, size_t n_vals)
{
    grib_accessor_data_simple_packing* self = (grib_accessor_data_simple_packing*)a;
    grib_handle* gh                         = grib_handle_of_accessor(a);

    size_t i = 0;
    int err  = 0;

    double reference_value;
    long binary_scale_factor;
    long bits_per_value;
    long decimal_scale_factor;
    long offsetBeforeData;
    double s            = 0;
    double d            = 0;
    double units_factor = 1.0;
    double units_bias   = 0.0;

    if (*len < n_vals) {
        *len = (long)n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;

    /*
     * check we don't decode bpv > max(ulong) as it is
     * not currently supported by the algorithm
     */
    if (bits_per_value > (sizeof(long) * 8)) {
        return GRIB_INVALID_BPV;
    }

    if (self->units_factor &&
        (grib_get_double_internal(gh, self->units_factor, &units_factor) == GRIB_SUCCESS)) {
        grib_set_double_internal(gh, self->units_factor, 1.0);
    }

    if (self->units_bias &&
        (grib_get_double_internal(gh, self->units_bias, &units_bias) == GRIB_SUCCESS)) {
        grib_set_double_internal(gh, self->units_bias, 0.0);
    }

    if (n_vals == 0) {
        *len = 0;
        return GRIB_SUCCESS;
    }

    self->dirty = 0;

    if ((err = grib_get_double_internal(gh, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    /* Special case */

    if (bits_per_value == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    s = grib_power(binary_scale_factor, 2);
    d = grib_power(-decimal_scale_factor, 10);

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_data_simple_packing: unpack_double : creating %s, %d values",
                     a->name, n_vals);

    offsetBeforeData = grib_byte_offset(a);
    buf += offsetBeforeData;

    /*Assert(((bits_per_value*n_vals)/8) < (1<<29));*/ /* See GRIB-787 */

    /* ECC-941 */
    if (!a->context->ieee_packing) {
        /* Must turn off this check when the environment variable ECCODES_GRIB_IEEE_PACKING is on */
        long offsetAfterData = 0;
        err                  = grib_get_long(gh, "offsetAfterData", &offsetAfterData);
        if (!err && offsetAfterData > offsetBeforeData) {
            const long valuesSize = (bits_per_value * n_vals) / 8; /*in bytes*/
            if (offsetBeforeData + valuesSize > offsetAfterData) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "Data section size mismatch: offset before data=%ld, offset after data=%ld (num values=%ld, bits per value=%ld)",
                                 offsetBeforeData, offsetAfterData, n_vals, bits_per_value);
                return GRIB_DECODING_ERROR;
            }
        }
#if 0
        if (offsetBeforeData == offsetAfterData) {
            /* Crazy case: Constant field with bitsPerValue > 0 */
            for (i = 0; i < n_vals; i++)
                val[i] = reference_value;
            *len = n_vals;
            return GRIB_SUCCESS;
        }
#endif
    }

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "unpack_double: calling outline function : bpv %d, rv : %g, sf : %d, dsf : %d ",
                     bits_per_value, reference_value, binary_scale_factor, decimal_scale_factor);
    grib_decode_double_array(buf, &pos, bits_per_value, reference_value, s, d, n_vals, val);

    *len = (long)n_vals;

    if (units_factor != 1.0) {
        if (units_bias != 0.0)
            for (i = 0; i < n_vals; i++)
                val[i] = val[i] * units_factor + units_bias;
        else
            for (i = 0; i < n_vals; i++)
                val[i] *= units_factor;
    }
    else if (units_bias != 0.0)
        for (i = 0; i < n_vals; i++)
            val[i] += units_bias;

    return err;
}

static int unpack_double_subarray(grib_accessor* a, double* val, size_t start, size_t len)
{
    grib_accessor_data_simple_packing* self = (grib_accessor_data_simple_packing*)a;
    unsigned char* buf                      = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    size_t nvals                            = len;
    size_t* plen                            = &len;
    long bits_per_value                     = 0;
    long pos;
    int err;

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->bits_per_value, &bits_per_value)) !=
        GRIB_SUCCESS)
        return err;

    buf += (start * bits_per_value) / 8;
    pos = start * bits_per_value % 8;
    return _unpack_double(a, val, plen, buf, pos, nvals);
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    unsigned char* buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    size_t nvals       = 0;
    long pos           = 0;
    int err            = 0;
    long count         = 0;

    err = grib_value_count(a, &count);
    if (err)
        return err;
    nvals = count;

    return _unpack_double(a, val, len, buf, pos, nvals);
}

#if GRIB_IBMPOWER67_OPT
#define restrict
#include "minmax_val.c"
#undef restrict
#endif

#if 0
static int grib_producing_large_constant_fields(const grib_context* c, grib_handle* h, int edition)
{
    /* GRIB-802: If override key is set, ignore env. var and produce compressed fields */
    if (c->large_constant_fields) {  /* This is set by the environment variable */
        /* check the override key */
        int err = 0;
        long override_large_constant_fields = 0;
        err = grib_get_long_internal(h, "override_large_constant_fields", &override_large_constant_fields);
        if (err == GRIB_SUCCESS && override_large_constant_fields) {
            return 0;
        }
        return 1;
    }
    if (c->gribex_mode_on==1 && edition==1) {
        return 1;
    }

    return 0;
}
#endif

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_simple_packing* self = (grib_accessor_data_simple_packing*)a;
    grib_handle* gh                         = grib_handle_of_accessor(a);

    size_t i = 0;
    size_t n_vals = *len;
    int err       = 0;
    double reference_value        = 0;
    long binary_scale_factor      = 0;
    long bits_per_value           = 0;
    long decimal_scale_factor     = 0;
    long decimal_scale_factor_get = 0;
    long optimize_scaling_factor  = 0;
    double decimal                = 1;
    double max                    = 0;
    double min                    = 0;
    double unscaled_max           = 0;
    double unscaled_min           = 0;
    double f                      = 0;
    double range                  = 0;
    double minrange = 0, maxrange = 0;
    long changing_precision = 0;
    grib_context* c         = a->context;

    decimal_scale_factor = 0;

    if (*len == 0) {
        return GRIB_NO_VALUES;
    }

    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;

    if (*len == 0)
        return GRIB_SUCCESS;

    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor_get)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->optimize_scaling_factor, &optimize_scaling_factor)) != GRIB_SUCCESS)
        return err;

    /* check we don't encode bpv > max(ulong)-1 as it is not currently supported by the algorithm */
    if (bits_per_value > (sizeof(long) * 8 - 1)) {
        return GRIB_INVALID_BPV;
    }

    self->dirty = 1;

    max = val[0];
    min = max;
#if GRIB_IBMPOWER67_OPT
    minmax_val(val + 1, n_vals - 1, &min, &max);
#else
    for (i = 1; i < n_vals; i++) {
        if (val[i] > max)      max = val[i];
        else if (val[i] < min) min = val[i];
    }
#endif
    if ((err = grib_check_data_values_range(gh, min, max)) != GRIB_SUCCESS) {
        return err;
    }

    /* constant field only reference_value is set and bits_per_value=0 */
    if (max == min) {
        int large_constant_fields = 0;
        if (grib_get_nearest_smaller_value(gh, self->reference_value, val[0], &reference_value) != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "unable to find nearest_smaller_value of %g for %s", min, self->reference_value);
            return GRIB_INTERNAL_ERROR;
        }
        if ((err = grib_set_double_internal(gh, self->reference_value, reference_value)) != GRIB_SUCCESS)
            return err;

        {
            /* Make sure we can decode it again */
            double ref = 1e-100;
            grib_get_double_internal(gh, self->reference_value, &ref);
            if (ref != reference_value)
                printf("%.20e  !=  %.20e", ref, reference_value);
            Assert(ref == reference_value);
        }

        large_constant_fields = grib_producing_large_constant_fields(gh, self->edition);
        if (large_constant_fields) {
            if ((err = grib_set_long_internal(gh, self->binary_scale_factor, 0)) != GRIB_SUCCESS)
                return err;

            if ((err = grib_set_long_internal(gh, self->decimal_scale_factor, 0)) != GRIB_SUCCESS)
                return err;

            if (bits_per_value == 0) {
                if ((err = grib_set_long_internal(gh, self->bits_per_value, 16)) != GRIB_SUCCESS)
                    return err;
            }

            return GRIB_SUCCESS;
        }
        else {
            bits_per_value = 0;
            if ((err = grib_set_long_internal(gh, self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
                return err;

            return GRIB_CONSTANT_FIELD;
        }
    }

    if ((err = grib_get_long_internal(gh, self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(gh, self->changing_precision, &changing_precision)) != GRIB_SUCCESS)
        return err;

    /* the packing parameters are not properly defined, this is a safe way of fixing the problem */
    if (changing_precision == 0 && bits_per_value == 0 && decimal_scale_factor_get == 0) {
        grib_context_log(a->context, GRIB_LOG_WARNING,
                         "%s==0 and %s==0 (setting %s=24)",
                         self->bits_per_value,
                         self->decimal_scale_factor,
                         self->bits_per_value);

        bits_per_value = 24;
        if ((err = grib_set_long_internal(gh, self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
            return err;
    }

    if (bits_per_value == 0 || (binary_scale_factor == 0 && decimal_scale_factor_get != 0)) {
        /* decimal_scale_factor is given, binary_scale_factor=0 and bits_per_value is computed */
        binary_scale_factor  = 0;
        decimal_scale_factor = decimal_scale_factor_get;
        decimal              = grib_power(decimal_scale_factor, 10);
        min *= decimal;
        max *= decimal;

        /* bits_per_value=(long)ceil(log((double)(imax-imin+1))/log(2.0)); */
        /* See GRIB-540 for why we use ceil */
        err = number_of_bits((unsigned long)ceil(fabs(max - min)), &bits_per_value);
        if (err) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "Range of values too large. Try a smaller value for decimal precision (less than %ld)",
                             decimal_scale_factor);
            return err;
        }

        if ((err = grib_set_long_internal(gh, self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
            return err;
        if (grib_get_nearest_smaller_value(gh, self->reference_value, min, &reference_value) != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "unable to find nearest_smaller_value of %g for %s", min, self->reference_value);
            return GRIB_INTERNAL_ERROR;
        }
        /* divisor=1; */
    }
    else {
        int last = 127;  /* 'last' should be a parameter coming from a definitions file */
        if (c->gribex_mode_on && self->edition == 1)
            last = 99;
        /* bits_per_value is given and decimal_scale_factor and binary_scale_factor are calcualated */
        if (max == min) {
            binary_scale_factor = 0;
            /* divisor=1; */
            if (grib_get_nearest_smaller_value(gh, self->reference_value, min, &reference_value) != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "unable to find nearest_smaller_value of %g for %s", min, self->reference_value);
                return GRIB_INTERNAL_ERROR;
            }
        }
        else if (optimize_scaling_factor) {
            int compat_gribex = c->gribex_mode_on && self->edition == 1;
            if ((err = grib_optimize_decimal_factor(a, self->reference_value,
                                                    max, min, bits_per_value,
                                                    compat_gribex, 1,
                                                    &decimal_scale_factor, &binary_scale_factor, &reference_value)) != GRIB_SUCCESS)
                return err;
        }
        else {
            /* printf("max=%g reference_value=%g grib_power(-last,2)=%g decimal_scale_factor=%ld bits_per_value=%ld\n",
               max,reference_value,grib_power(-last,2),decimal_scale_factor,bits_per_value);*/
            range        = (max - min);
            unscaled_min = min;
            unscaled_max = max;
            f            = (grib_power(bits_per_value, 2) - 1);
            minrange     = grib_power(-last, 2) * f;
            maxrange     = grib_power(last, 2) * f;

            while (range < minrange) {
                decimal_scale_factor += 1;
                decimal *= 10;
                min   = unscaled_min * decimal;
                max   = unscaled_max * decimal;
                range = (max - min);
            }
            while (range > maxrange) {
                decimal_scale_factor -= 1;
                decimal /= 10;
                min   = unscaled_min * decimal;
                max   = unscaled_max * decimal;
                range = (max - min);
            }

            if (grib_get_nearest_smaller_value(gh, self->reference_value,
                                               min, &reference_value) != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "unable to find nearest_smaller_value of %g for %s", min, self->reference_value);
                return GRIB_INTERNAL_ERROR;
            }

            binary_scale_factor = grib_get_binary_scale_fact(max, reference_value, bits_per_value, &err);
            if (err) return err;
        }
    }

    if ((err = grib_set_double_internal(gh, self->reference_value, reference_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->changing_precision, 0)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->binary_scale_factor, binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(gh, self->decimal_scale_factor, decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    return GRIB_SUCCESS;
}
