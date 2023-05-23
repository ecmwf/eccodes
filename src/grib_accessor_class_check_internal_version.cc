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
   SUPER      = grib_accessor_class_ascii
   IMPLEMENTS = value_count;string_length
   IMPLEMENTS = init
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_check_internal_version
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in ascii */
    /* Members defined in check_internal_version */
} grib_accessor_check_internal_version;

extern grib_accessor_class* grib_accessor_class_ascii;

static grib_accessor_class _grib_accessor_class_check_internal_version = {
    &grib_accessor_class_ascii,                      /* super */
    "check_internal_version",                      /* name */
    sizeof(grib_accessor_check_internal_version),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    &string_length,              /* get length of string */
    &value_count,                /* get number of values */
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


grib_accessor_class* grib_accessor_class_check_internal_version = &_grib_accessor_class_check_internal_version;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

/* This is the internal engine version number */
/* We check this against the version number found in the definitions boot.def file */
/* See the key "internalVersion"  */
#define LATEST_ENGINE_VERSION 30

static void init(grib_accessor* a, const long l, grib_arguments* args)
{
    /* Check version of definition files is compatible with the engine */
    int err                    = 0;
    long defs_file_version     = 0;
    grib_handle* h             = grib_handle_of_accessor(a);
    const char* s_defn_version = grib_arguments_get_name(h, args, 0);
    Assert(s_defn_version);

    err = grib_get_long_internal(h, s_defn_version, &defs_file_version);
    if (!err) {
        if (defs_file_version > LATEST_ENGINE_VERSION) {
            grib_context_log(h->context, GRIB_LOG_FATAL,
                             "Definition files version (%d) is greater than engine version (%d)!\n"
                             "                    " /* indent for 2nd line */
                             "These definition files are for a later version of the ecCodes engine.",
                             defs_file_version, LATEST_ENGINE_VERSION);
        }
    }
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    return 255;
}
