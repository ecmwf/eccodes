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
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = unpack_long;pack_long; value_count
   IMPLEMENTS = init
   MEMBERS=const char* productDefinitionTemplateNumber
   MEMBERS=const char* stream
   MEMBERS=const char* type
   MEMBERS=const char* stepType
   MEMBERS=const char* derivedForecast
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_g2_eps
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
    /* Members defined in g2_eps */
    const char* productDefinitionTemplateNumber;
    const char* stream;
    const char* type;
    const char* stepType;
    const char* derivedForecast;
} grib_accessor_g2_eps;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_g2_eps = {
    &grib_accessor_class_unsigned,                      /* super */
    "g2_eps",                      /* name */
    sizeof(grib_accessor_g2_eps),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
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


grib_accessor_class* grib_accessor_class_g2_eps = &_grib_accessor_class_g2_eps;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_g2_eps* self = (grib_accessor_g2_eps*)a;
    int n                      = 0;

    self->productDefinitionTemplateNumber = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->type                            = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->stream                          = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->stepType                        = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->derivedForecast                 = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2_eps* self           = (grib_accessor_g2_eps*)a;
    long productDefinitionTemplateNumber = 0;
    int err = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber);
    if (err) return err;

    *val = 0;
    if (grib_is_defined(hand, "perturbationNumber")) {
        *val = 1;
    }
    //if (grib2_is_PDTN_EPS(productDefinitionTemplateNumber))
    //    *val = 1;

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_g2_eps* self              = (grib_accessor_g2_eps*)a;
    grib_handle* hand                       = grib_handle_of_accessor(a);
    long productDefinitionTemplateNumber    = -1;
    long productDefinitionTemplateNumberNew = -1;
    long type                               = -1;
    long stream                             = -1;
    long chemical                           = -1;
    long aerosol                            = -1;
    char stepType[15]                       = {0,};
    size_t slen          = 15;
    int eps              = *val;
    int isInstant        = 0;
    long derivedForecast = -1;

    if (grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber) != GRIB_SUCCESS)
        return GRIB_SUCCESS;

    grib_get_long(hand, self->type, &type);
    grib_get_long(hand, self->stream, &stream);
    grib_get_string(hand, self->stepType, stepType, &slen);
    if (!strcmp(stepType, "instant"))
        isInstant = 1;
    grib_get_long(hand, "is_chemical", &chemical);
    grib_get_long(hand, "is_aerosol", &aerosol);
    if (chemical == 1 && aerosol == 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Parameter cannot be both chemical and aerosol!");
        return GRIB_ENCODING_ERROR;
    }

    // eps or stream=(enda or elda or ewla)
    if (eps || stream == 1030 || stream == 1249 || stream == 1250) {
        if (isInstant) {
            // type=em || type=es
            if (type == 17) {
                productDefinitionTemplateNumberNew = 2;
                derivedForecast                    = 0;
            }
            else if (type == 18) {
                productDefinitionTemplateNumberNew = 2;
                derivedForecast                    = 4;
            }
            else {
                productDefinitionTemplateNumberNew = 1;
            }
        }
        else {
            // type=em || type=es
            if (type == 17) {
                productDefinitionTemplateNumberNew = 12;
                derivedForecast                    = 0;
            }
            else if (type == 18) {
                productDefinitionTemplateNumberNew = 12;
                derivedForecast                    = 4;
            }
            else {
                productDefinitionTemplateNumberNew = 11;
            }
        }
    }
    else {
        if (isInstant) {
            productDefinitionTemplateNumberNew = 0;
        }
        else {
            productDefinitionTemplateNumberNew = 8;
        }
    }

    // Adjust for chemical species
    if (chemical == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 41;
            }
            else {
                productDefinitionTemplateNumberNew = 43;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 40;
            }
            else {
                productDefinitionTemplateNumberNew = 42;
            }
        }
    }

    // Adjust for aerosols
    if (aerosol == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 45;
            }
            else {
                //productDefinitionTemplateNumberNew = 47;   This PDT is deprecated
                productDefinitionTemplateNumberNew = 85;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 48; //44 is deprecated*/
            }
            else {
                productDefinitionTemplateNumberNew = 46;
            }
        }
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        grib_set_long(hand, self->productDefinitionTemplateNumber, productDefinitionTemplateNumberNew);
        if (derivedForecast >= 0)
            grib_set_long(hand, self->derivedForecast, derivedForecast);
    }

    return 0;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}
