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
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*  half_byte
   MEMBERS=const char*  packingType
   MEMBERS=const char*  ieee_packing
   MEMBERS=const char*  precision
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_g1simple_packing
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
    /* Members defined in data_g1simple_packing */
    const char*  half_byte;
    const char*  packingType;
    const char*  ieee_packing;
    const char*  precision;
} grib_accessor_data_g1simple_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_g1simple_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super */
    "data_g1simple_packing",                      /* name */
    sizeof(grib_accessor_data_g1simple_packing),  /* size */
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
    0,              /* grib_unpack procedures double */
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
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_g1simple_packing = &_grib_accessor_class_data_g1simple_packing;


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
    c->unpack_double    =    (*(c->super))->unpack_double;
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
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_g1simple_packing* self = (grib_accessor_data_g1simple_packing*)a;

    self->half_byte    = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->packingType  = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->ieee_packing = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->precision    = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->edition      = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a, long* number_of_values)
{
    grib_accessor_data_g1simple_packing* self = (grib_accessor_data_g1simple_packing*)a;
    *number_of_values                         = 0;

    /* Special case for when values are cleared */
    /*if(a->length == 0)
    return 0;*/

    return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_values, number_of_values);
}

static int pack_double(grib_accessor* a, const double* cval, size_t* len)
{
    grib_accessor_data_g1simple_packing* self = (grib_accessor_data_g1simple_packing*)a;
    grib_accessor_class* super                = *(a->cclass->super);

    size_t n_vals             = *len;
    long half_byte            = 0;
    int ret                   = 0;
    long offsetdata           = 0;
    long offsetsection        = 0;
    double reference_value    = 0;
    long binary_scale_factor  = 0;
    long bits_per_value       = 0;
    long decimal_scale_factor = 0;
    double decimal            = 1;
    size_t buflen             = 0;
    unsigned char* buf        = NULL;
    unsigned char* encoded    = NULL;
    double divisor            = 1;
    int i;
    long off                   = 0;
    grib_context* c            = a->context;
    grib_handle* h             = grib_handle_of_accessor(a);
    char* ieee_packing_s       = NULL;
    char* packingType_s        = NULL;
    char* precision_s          = NULL;
    double units_factor        = 1.0;
    double units_bias          = 0.0;
    double* val                = (double*)cval;
    double missingValue        = 9999.0;
    long constantFieldHalfByte = 0;
    int err                    = 0;

    if (*len != 0) {
        if (self->units_factor &&
            (grib_get_double_internal(grib_handle_of_accessor(a), self->units_factor, &units_factor) == GRIB_SUCCESS)) {
            grib_set_double_internal(grib_handle_of_accessor(a), self->units_factor, 1.0);
        }

        if (self->units_bias &&
            (grib_get_double_internal(grib_handle_of_accessor(a), self->units_bias, &units_bias) == GRIB_SUCCESS)) {
            grib_set_double_internal(grib_handle_of_accessor(a), self->units_bias, 0.0);
        }

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

        if (c->ieee_packing && self->ieee_packing) {
            long precision = 0; /* Either 1(=32 bits) or 2(=64 bits) */
            size_t lenstr  = strlen(self->ieee_packing);
            if ((ret = codes_check_grib_ieee_packing_value(c->ieee_packing)) != GRIB_SUCCESS)
                return ret;

            packingType_s  = grib_context_strdup(c, self->packingType);
            ieee_packing_s = grib_context_strdup(c, self->ieee_packing);
            precision_s    = grib_context_strdup(c, self->precision);
            precision = c->ieee_packing == 32 ? 1 : 2;

            if ((ret = grib_set_string(h, packingType_s, ieee_packing_s, &lenstr)) != GRIB_SUCCESS)
                return ret;
            if ((ret = grib_set_long(h, precision_s, precision)) != GRIB_SUCCESS)
                return ret;

            grib_context_free(c, packingType_s);
            grib_context_free(c, ieee_packing_s);
            grib_context_free(c, precision_s);
            return grib_set_double_array(h, "values", val, *len);
        }
    }

    ret = super->pack_double(a, val, len);
    switch (ret) {
        case GRIB_CONSTANT_FIELD:
            ret = grib_get_long(grib_handle_of_accessor(a), "constantFieldHalfByte", &constantFieldHalfByte);
            if (ret)
                constantFieldHalfByte = 0;
            if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->half_byte, constantFieldHalfByte)) != GRIB_SUCCESS)
                return ret;
            grib_buffer_replace(a, NULL, 0, 1, 1);
            return GRIB_SUCCESS;
            break;
        case GRIB_NO_VALUES:
            ret = grib_get_long(grib_handle_of_accessor(a), "constantFieldHalfByte", &constantFieldHalfByte);
            if (ret)
                constantFieldHalfByte = 0;
            /* TODO move to def file */
            grib_get_double(grib_handle_of_accessor(a), "missingValue", &missingValue);
            if ((err = grib_set_double_internal(grib_handle_of_accessor(a), self->reference_value, missingValue)) !=
                GRIB_SUCCESS)
                return err;
            if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->binary_scale_factor, binary_scale_factor)) != GRIB_SUCCESS)
                return ret;
            if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->half_byte, constantFieldHalfByte)) != GRIB_SUCCESS)
                return ret;
            grib_buffer_replace(a, NULL, 0, 1, 1);
            return GRIB_SUCCESS;
            break;
        case GRIB_INVALID_BPV:
            grib_context_log(a->context, GRIB_LOG_ERROR, "unable to compute packing parameters. Invalid bits per value\n");
            return ret;
        case GRIB_SUCCESS:
            break;
        default:
            grib_context_log(a->context, GRIB_LOG_ERROR, "GRIB1 simple packing: unable to set values (%s)", grib_get_error_message(ret));
            return ret;
    }

    if ((ret = grib_get_double_internal(grib_handle_of_accessor(a), self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->bits_per_value, &bits_per_value)) !=
        GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetdata, &offsetdata)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetsection, &offsetsection)) != GRIB_SUCCESS)
        return ret;

    decimal = grib_power(decimal_scale_factor, 10);
    divisor = grib_power(-binary_scale_factor, 2);

    buflen = (((bits_per_value * n_vals) + 7) / 8) * sizeof(unsigned char);
    if ((buflen + (offsetdata - offsetsection)) % 2) {
        buflen++;
        /*
    a->length++;
    grib_handle_of_accessor(a)->buffer->ulength++;
         */
    }
    half_byte = (buflen * 8) - ((*len) * bits_per_value);
    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "HALF byte: buflen=%d bits_per_value=%ld len=%d half_byte=%ld\n",
                     buflen, bits_per_value, *len, half_byte);

    Assert(half_byte <= 0x0f);

    if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->half_byte, half_byte)) != GRIB_SUCCESS)
        return ret;

    buf     = (unsigned char*)grib_context_buffer_malloc_clear(a->context, buflen);
    encoded = buf;

    grib_encode_double_array(n_vals, val, bits_per_value, reference_value, decimal, divisor, encoded, &off);

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_data_g1simple_packing : pack_double : packing %s, %d values", a->name, n_vals);

    grib_buffer_replace(a, buf, buflen, 1, 1);

    grib_context_buffer_free(a->context, buf);

    return GRIB_SUCCESS;
}
