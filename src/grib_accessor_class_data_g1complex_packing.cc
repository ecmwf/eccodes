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
   SUPER      = grib_accessor_class_data_complex_packing
   IMPLEMENTS = pack_double
   IMPLEMENTS = init
   MEMBERS= const char*  N
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
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_data_g1complex_packing
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
    /* Members defined in data_g1complex_packing */
    const char*  N;
    const char*  half_byte;
    const char*  packingType;
    const char*  ieee_packing;
    const char*  precision;
} grib_accessor_data_g1complex_packing;

extern grib_accessor_class* grib_accessor_class_data_complex_packing;

static grib_accessor_class _grib_accessor_class_data_g1complex_packing = {
    &grib_accessor_class_data_complex_packing,                      /* super */
    "data_g1complex_packing",                      /* name */
    sizeof(grib_accessor_data_g1complex_packing),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_g1complex_packing = &_grib_accessor_class_data_g1complex_packing;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_g1complex_packing* self = (grib_accessor_data_g1complex_packing*)a;
    self->half_byte                            = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->N                                    = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->packingType                          = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->ieee_packing                         = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->precision                            = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->edition                              = 1;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_g1complex_packing* self = (grib_accessor_data_g1complex_packing*)a;
    int ret              = GRIB_SUCCESS;
    long seclen          = 0;
    long sub_j           = 0;
    long sub_k           = 0;
    long sub_m           = 0;
    long n               = 0;
    long half_byte       = 0;
    long bits_per_value  = 0;
    size_t buflen        = 0;
    grib_accessor_class* super = *(a->cclass->super);

    if (*len == 0)
        return GRIB_NO_VALUES;

//     /* TODO: spectral_ieee does not work */
//     if (c->ieee_packing && self->ieee_packing) {
//         grib_handle* h       = grib_handle_of_accessor(a);
//         grib_context* c      = a->context;
//         char* packingType_s  = NULL;
//         char* ieee_packing_s = NULL;
//         long precision = c->ieee_packing == 32 ? 1 : 2;
//         size_t lenstr  = strlen(self->ieee_packing);

//         packingType_s  = grib_context_strdup(c, self->packingType);
//         ieee_packing_s = grib_context_strdup(c, self->ieee_packing);
//         precision_s    = grib_context_strdup(c, self->precision);

//         grib_set_string(h, packingType_s, ieee_packing_s, &lenstr);
//         grib_set_long(h, precision_s, precision);

//         grib_context_free(c, packingType_s);
//         grib_context_free(c, ieee_packing_s);
//         grib_context_free(c, precision_s);
//         return grib_set_double_array(h, "values", val, *len);
//     }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->sub_j, &sub_j)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->sub_k, &sub_k)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->sub_m, &sub_m)) != GRIB_SUCCESS)
        return ret;

    self->dirty = 1;

    Assert((sub_j == sub_k) && (sub_m == sub_j));

    ret = super->pack_double(a, val, len);

    if (ret == GRIB_SUCCESS) {
        n = a->offset + 4 * ((sub_k + 1) * (sub_k + 2));

        /*     Octet number starts from beginning of message but shouldn't     */
        if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->N, n)) != GRIB_SUCCESS)
            return ret;

        // ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offsetsection, &offsetsection);
        // if (ret != GRIB_SUCCESS)
        //     return ret;
        // if ((ret = grib_set_long_internal(grib_handle_of_accessor(a), self->N, n - offsetsection)) != GRIB_SUCCESS)
        //     return ret;

        ret = grib_get_long_internal(grib_handle_of_accessor(a), self->bits_per_value, &bits_per_value);
        if (ret != GRIB_SUCCESS)
            return ret;

        ret = grib_get_long_internal(grib_handle_of_accessor(a), self->seclen, &seclen);
        if (ret != GRIB_SUCCESS)
            return ret;

        buflen    = 32 * (sub_k + 1) * (sub_k + 2) + (*len - (sub_k + 1) * (sub_k + 2)) * bits_per_value + 18 * 8;
        half_byte = seclen * 8 - buflen;
        if (a->context->debug == -1) {
            fprintf(stderr, "ECCODES DEBUG: half_byte=%ld\n", half_byte);
        }

        ret = grib_set_long_internal(grib_handle_of_accessor(a), self->half_byte, half_byte);
        if (ret != GRIB_SUCCESS)
            return ret;
    }
    return ret;
}
