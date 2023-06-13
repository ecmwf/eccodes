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
   SUPER      = grib_accessor_class_g1step_range
   IMPLEMENTS = unpack_string
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_string(grib_accessor*, char*, size_t* len);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g1fcperiod
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in abstract_long_vector */
    long* v;
    long pack_index;
    int number_of_elements;
    /* Members defined in g1step_range */
    const char* p1;
    const char* p2;
    const char* timeRangeIndicator;
    const char *unit;
    const char *step_unit;
    const char *stepType;
    const char *patch_fp_precip;
    int error_on_units;
    /* Members defined in g1fcperiod */
} grib_accessor_g1fcperiod;

extern grib_accessor_class* grib_accessor_class_g1step_range;

static grib_accessor_class _grib_accessor_class_g1fcperiod = {
    &grib_accessor_class_g1step_range,                      /* super */
    "g1fcperiod",                      /* name */
    sizeof(grib_accessor_g1fcperiod),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    0,                       /* init */
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
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    &unpack_string,              /* unpack_string */
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


grib_accessor_class* grib_accessor_class_g1fcperiod = &_grib_accessor_class_g1fcperiod;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    long start = 0, theEnd = 0;
    char tmp[1024];
    const size_t tmpLen = sizeof(tmp);
    int err  = grib_g1_step_get_steps(a, &start, &theEnd);
    size_t l = 0;

    if (err)
        return err;

    snprintf(tmp, tmpLen, "%ld-%ld", start / 24, theEnd / 24);
    /*printf("---- FCPERIOD %s [start:%g, end:%g]",tmp,start,end);*/

    l = strlen(tmp) + 1;
    if (*len < l) {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    *len = l;
    strcpy(val, tmp); /* NOLINT: CWE-119 clang-analyzer-security.insecureAPI.strcpy */

    return GRIB_SUCCESS;
}
