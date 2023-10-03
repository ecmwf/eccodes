/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_long; pack_long
   IMPLEMENTS = init; get_native_type
   MEMBERS=const char*    paramId
   MEMBERS=const char*    type
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_ifs_param
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in ifs_param */
    const char*    paramId;
    const char*    type;
} grib_accessor_ifs_param;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_ifs_param = {
    &grib_accessor_class_gen,                      /* super */
    "ifs_param",                      /* name */
    sizeof(grib_accessor_ifs_param),  /* size */
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
    &get_native_type,            /* get native type */
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


grib_accessor_class* grib_accessor_class_ifs_param = &_grib_accessor_class_ifs_param;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_ifs_param* self = (grib_accessor_ifs_param*)a;
    int n                         = 0;

    self->paramId = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->type    = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_ifs_param* self = (grib_accessor_ifs_param*)a;
    int ret                       = 0;
    long paramId                  = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->paramId, &paramId)) != GRIB_SUCCESS)
        return ret;

    if (paramId > 129000 && paramId < 129999)
        *val = paramId - 129000;
    else if (paramId > 200000 && paramId < 200999)
        *val = paramId - 200000;
    else if (paramId > 211000 && paramId < 211999)
        *val = paramId - 1000;
    else
        *val = paramId;

    return ret;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_ifs_param* self = (grib_accessor_ifs_param*)a;
    long type                     = 0;
    long table                    = 128;
    long paramId                  = *val;
    long param;

    grib_get_long(grib_handle_of_accessor(a), self->type, &type);

    if (type == 33 || type == 35) {
        if (paramId > 1000) {
            table   = paramId / 1000;
            param   = paramId - table * 1000;
            paramId = param;
        }
        switch (table) {
            case 210:
                paramId += 211000;
                break;
            case 128:
                paramId += 200000;
                break;
            default:
                break;
        }
    }
    if (type == 50 || type == 52) {
        if (paramId > 1000) {
            table   = paramId / 1000;
            param   = paramId - table * 1000;
            paramId = param;
        }
        switch (table) {
            case 128:
                paramId += 129000;
                break;
            default:
                break;
        }
    }

    return grib_set_long_internal(grib_handle_of_accessor(a), self->paramId, paramId);
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}
