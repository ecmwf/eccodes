/*
* (C) Copyright 2005- ECMWF.
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
*
* In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
* virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
*/

/* Sequences and replication (not delayed) are resolved in this class.
   Number of elements to which a delayed replication applies are recomputed to
   take account of the expansion.
   Expanded descriptors cannot contain sequences and only delayed replication
   can appear
*/

#include "grib_api_internal.h"
/*
This is used by make_class.pl

START_CLASS_DEF
CLASS      = accessor
SUPER      = grib_accessor_class_long
IMPLEMENTS = unpack_long
IMPLEMENTS = unpack_string_array
IMPLEMENTS = init;dump;destroy
IMPLEMENTS = value_count
MEMBERS    = const char* expandedDescriptors
MEMBERS    = grib_accessor* expandedDescriptorsAccessor

END_CLASS_DEF

*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string_array(grib_accessor*, char**, size_t* len);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);

typedef struct grib_accessor_bufrdc_expanded_descriptors
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in bufrdc_expanded_descriptors */
    const char* expandedDescriptors;
    grib_accessor* expandedDescriptorsAccessor;
} grib_accessor_bufrdc_expanded_descriptors;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_bufrdc_expanded_descriptors = {
    &grib_accessor_class_long,                      /* super */
    "bufrdc_expanded_descriptors",                      /* name */
    sizeof(grib_accessor_bufrdc_expanded_descriptors),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    &unpack_long,                /* unpack_long */
    0,                /* pack_double */
    0,                 /* pack_float */
    0,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    &unpack_string_array,        /* unpack_string_array */
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


grib_accessor_class* grib_accessor_class_bufrdc_expanded_descriptors = &_grib_accessor_class_bufrdc_expanded_descriptors;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* args)
{
    grib_accessor_bufrdc_expanded_descriptors* self = (grib_accessor_bufrdc_expanded_descriptors*)a;
    int n                                           = 0;
    self->expandedDescriptors                       = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->expandedDescriptorsAccessor               = 0;
    a->length                                       = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static grib_accessor* get_accessor(grib_accessor* a)
{
    grib_accessor_bufrdc_expanded_descriptors* self = (grib_accessor_bufrdc_expanded_descriptors*)a;
    if (!self->expandedDescriptorsAccessor) {
        self->expandedDescriptorsAccessor = grib_find_accessor(grib_handle_of_accessor(a), self->expandedDescriptors);
    }
    return self->expandedDescriptorsAccessor;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor* descriptors = 0;
    size_t rlen  = 0, l;
    long lenall  = 0;
    size_t i     = 0;
    long* v      = 0;
    grib_context* c = a->context;

    descriptors = get_accessor(a);
    if (!descriptors) return GRIB_NOT_FOUND;

    grib_value_count(a, &lenall);
    v = (long*)grib_context_malloc_clear(c, sizeof(long) * lenall);
    l = lenall;
    grib_unpack_long(descriptors, v, &l);

    rlen = 0;
    for (i = 0; i < l; i++) {
        if ((v[i] < 100000 || v[i] > 221999))
            val[rlen++] = v[i];
    }
    *len = rlen;
    grib_context_free(c,v);

    return GRIB_SUCCESS;
}

static int unpack_string_array(grib_accessor* a, char** buffer, size_t* len)
{
    int err = 0;
    grib_accessor* descriptors = 0;
    size_t l     = 0;
    long lenall  = 0;
    size_t i     = 0;
    long* v      = 0;
    char buf[25] = {0,};
    grib_context* c = a->context;

    descriptors = get_accessor(a);
    if (!descriptors) return GRIB_NOT_FOUND;

    err = grib_value_count(a, &lenall);
    if (err) return err;
    l = lenall;
    if (l > *len) return GRIB_ARRAY_TOO_SMALL;

    v = (long*)grib_context_malloc_clear(c, sizeof(long) * l);
    err = grib_unpack_long(descriptors, v, &l);
    if (err) return err;

    for (i = 0; i < l; i++) {
        snprintf(buf, sizeof(buf), "%06ld", v[i]);
        buffer[i] = grib_context_strdup(c, buf);
    }
    *len = l;
    grib_context_free(c,v);

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* rlen)
{
    grib_accessor* descriptors = get_accessor(a);

    return grib_value_count(descriptors, rlen);
}

static void destroy(grib_context* c, grib_accessor* a)
{
}
