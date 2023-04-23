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
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_long;pack_long;init;dump;is_missing
   MEMBERS=const char* type_first
   MEMBERS=const char* scale_first
   MEMBERS=const char* value_first
   MEMBERS=const char* pressure_units
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int is_missing(grib_accessor*);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2level
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in g2level */
    const char* type_first;
    const char* scale_first;
    const char* value_first;
    const char* pressure_units;
} grib_accessor_g2level;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_g2level = {
    &grib_accessor_class_long,                      /* super */
    "g2level",                      /* name */
    sizeof(grib_accessor_g2level),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    &is_missing,                 /* is_missing */
    &pack_long,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
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


grib_accessor_class* grib_accessor_class_g2level = &_grib_accessor_class_g2level;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_g2level* self = (grib_accessor_g2level*)a;
    grib_handle* hand           = grib_handle_of_accessor(a);
    int n                       = 0;

    self->type_first     = grib_arguments_get_name(hand, c, n++);
    self->scale_first    = grib_arguments_get_name(hand, c, n++);
    self->value_first    = grib_arguments_get_name(hand, c, n++);
    self->pressure_units = grib_arguments_get_name(hand, c, n++);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static bool is_tigge(grib_handle* h)
{
    long productionStatus = 0;
    int err = grib_get_long(h, "productionStatusOfProcessedData", &productionStatus);
    if (err) return false;
    return (productionStatus == 4 || productionStatus == 5);
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    int ret                     = 0;
    grib_accessor_g2level* self = (grib_accessor_g2level*)a;
    grib_handle* hand           = grib_handle_of_accessor(a);

    long type_first         = 0;
    long scale_first        = 0;
    long value_first        = 0;
    char pressure_units[10] = {0,};
    size_t pressure_units_len = 10;
    bool tigge = is_tigge(hand);

    double v;

    if ((ret = grib_get_long_internal(hand, self->type_first, &type_first)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->scale_first, &scale_first)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(hand, self->value_first, &value_first)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_string_internal(hand, self->pressure_units, pressure_units, &pressure_units_len)) != GRIB_SUCCESS)
        return ret;

    if (value_first == GRIB_MISSING_LONG) {
        *val = 0;
        return GRIB_SUCCESS;
    }
    // value = value_first * 10 ^ -scale_first

    if (*len < 1)
        return GRIB_WRONG_ARRAY_SIZE;

    v = value_first;

    if (scale_first != GRIB_MISSING_LONG) {
        // GRIB-637, ECC-1081: Potential vorticity surface
        if (type_first == 109) {
            if (tigge) {
                scale_first -= 6; // TIGGE data follows different rules
            } else {
                scale_first -= 9;
            }
        }

        while (scale_first < 0 && v != 0) {
            v *= 10.0;
            scale_first++;
        }
        while (scale_first > 0 && v != 0) {
            v /= 10.0;
            scale_first--;
        }
    }

    switch (type_first) {
        case 100: // Isobaric surface (Pa)
            if (!strcmp(pressure_units, "hPa")) {
                long x = v / 100.0; // 1 hPa = 100 Pa
                if (scale_first == 0 && x == 0) {
                    // Switch to Pa instead of hPa as the value is less than a hectoPascal
                    char pa[]  = "Pa";
                    size_t lpa = strlen(pa);
                    if ((ret = grib_set_string_internal(hand, self->pressure_units, pa, &lpa)) != GRIB_SUCCESS)
                        return ret;
                }
                else {
                    v = x;
                }
            }
            break;
    }

    *val = v;

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    double dval = 0;
    int ret     = unpack_double(a, &dval, len);
    if (ret == GRIB_SUCCESS) {
        *val = (long)(dval + 0.5); // round up
    }
    return ret;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_g2level* self = (grib_accessor_g2level*)a;
    grib_handle* hand          = grib_handle_of_accessor(a);
    int ret                    = 0;
    double value_first         = *val;
    //long scale_first           = 0;
    long type_first            = 0;
    char pressure_units[10]    = {0,};
    size_t pressure_units_len  = 10;
    const long lval            = (long)value_first;

    if (value_first == lval) { // input is a whole number; process it as an integer
        return pack_long(a, &lval, len);
    }

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    if ((ret = grib_get_long_internal(hand, self->type_first, &type_first)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_string_internal(hand, self->pressure_units, pressure_units, &pressure_units_len)) != GRIB_SUCCESS)
        return ret;

    switch (type_first) {
        case 100: // Pa
            if (!strcmp(pressure_units, "hPa"))
                value_first *= 100;
            break;

        default:
            break;
    }

    // final = scaled_value * 10 ^ -scale_factor

    //scale_first = 2;
    //value_first *= 100;
    //value_first = value_first + 0.5; //round up

    // TODO(masn): These maxima should come from the respective accessors
    const int64_t scaled_value_max = (1ULL << 32) - 1; // scaledValueOf*FixedSurface is 4 octets
    const int64_t scale_factor_max = (1ULL << 8) - 1;  // scaleFactorOf*FixedSurface is 1 octet
    int64_t lscaled_value=0, lscale_factor=0;

    ret = compute_scaled_value_and_scale_factor(value_first, scaled_value_max, scale_factor_max, &lscaled_value, &lscale_factor);
    if (ret) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Key %s (unpack_double): Failed to compute %s and %s from %g",
                         a->name, self->scale_first, self->value_first, value_first);
        return ret;
    }

    if (type_first > 9) {
        if ((ret = grib_set_long_internal(hand, self->scale_first, (long)lscale_factor)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_set_long_internal(hand, self->value_first, (long)lscaled_value)) != GRIB_SUCCESS)
            return ret;
    }

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    int ret                 = 0;
    long value_first        = *val;
    long scale_first        = 0;
    long type_first         = 0;
    char pressure_units[10] = {0,};
    size_t pressure_units_len = 10;

    grib_accessor_g2level* self = (grib_accessor_g2level*)a;
    grib_handle* hand           = grib_handle_of_accessor(a);
    int change_scale_and_value  = 1;
    bool tigge = is_tigge(hand);

    if (*len != 1)
        return GRIB_WRONG_ARRAY_SIZE;

    // Not sure if this is necessary
    //   if (value_first == GRIB_MISSING_LONG) {
    //       if ((ret=grib_set_missing_internal(hand, self->scale_first)) != GRIB_SUCCESS)
    //           return ret;
    //       if ((ret=grib_set_missing_internal(hand, self->value_first)) != GRIB_SUCCESS)
    //           return ret;
    //       return GRIB_SUCCESS;
    //   }

    if ((ret = grib_get_long_internal(hand, self->type_first, &type_first)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_string_internal(hand, self->pressure_units, pressure_units, &pressure_units_len)) != GRIB_SUCCESS)
        return ret;

    switch (type_first) {
        case 100: // Pa
            scale_first = 0;
            if (!strcmp(pressure_units, "hPa"))
                value_first *= 100;
            break;
        case 109: // Potential vorticity surface (See ECC-1081)
            if (!tigge) {
                scale_first = 9;
            } else {
                scale_first = 6; // TIGGE data follows different rules
            }
            break;

        default:
            break;
    }

    // ECC-530:
    // The pack_long function can get called when key "typeOfSecondFixedSurface" is
    // changed (via the trigger rule in the definitions). That can have an undesired
    // side-effect that it sets the scale factor and scaled value keys
    // (e.g. scaleFactorOfFirstFixedSurface, scaledValueOfFirstFixedSurface)
    // overwriting their previous values.
    // In this scenario we do not want to change the scale/value.
    // However when the user directly sets the level or when we are changing edition, then
    // we do want to change the scale/value.
#if 0
    if (hand->loader && hand->loader->changing_edition==0) {
        change_scale_and_value = 0;
    }
#endif
    if (change_scale_and_value) {
        if (type_first > 9) {
            if ((ret = grib_set_long_internal(hand, self->scale_first, scale_first)) != GRIB_SUCCESS)
                return ret;
            if ((ret = grib_set_long_internal(hand, self->value_first, value_first)) != GRIB_SUCCESS)
                return ret;
        }
    }

    return GRIB_SUCCESS;
}

static int is_missing(grib_accessor* a)
{
    grib_accessor_g2level* self = (grib_accessor_g2level*)a;
    grib_handle* hand           = grib_handle_of_accessor(a);
    int err                     = 0;
    int ret                     = 0;

    ret = grib_is_missing(hand, self->scale_first, &err) +
          grib_is_missing(hand, self->value_first, &err);
    return ret;
}
