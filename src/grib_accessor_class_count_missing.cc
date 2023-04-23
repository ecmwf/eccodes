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
   IMPLEMENTS = unpack_long
   IMPLEMENTS = init;
   IMPLEMENTS = value_count
   MEMBERS    = const char* bitmap
   MEMBERS    = const char* unusedBitsInBitmap
   MEMBERS    = const char* numberOfDataPoints
   MEMBERS    = const char* missingValueManagementUsed
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_long(grib_accessor*, long* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_count_missing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in count_missing */
    const char* bitmap;
    const char* unusedBitsInBitmap;
    const char* numberOfDataPoints;
    const char* missingValueManagementUsed;
} grib_accessor_count_missing;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_count_missing = {
    &grib_accessor_class_long,                      /* super */
    "count_missing",                      /* name */
    sizeof(grib_accessor_count_missing),  /* size */
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
    0,                  /* pack_long */
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


grib_accessor_class* grib_accessor_class_count_missing = &_grib_accessor_class_count_missing;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static const unsigned char bitsoff[] = {
    8, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4, 7,
    6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 7, 6,
    6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 6, 5, 5,
    4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2, 7, 6, 6, 5,
    6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 6, 5, 5, 4, 5,
    4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2, 6, 5, 5, 4, 5, 4,
    4, 3, 5, 4, 4, 3, 4, 3, 3, 2, 5, 4, 4, 3, 4, 3, 3,
    2, 4, 3, 3, 2, 3, 2, 2, 1, 7, 6, 6, 5, 6, 5, 5, 4,
    6, 5, 5, 4, 5, 4, 4, 3, 6, 5, 5, 4, 5, 4, 4, 3, 5,
    4, 4, 3, 4, 3, 3, 2, 6, 5, 5, 4, 5, 4, 4, 3, 5, 4,
    4, 3, 4, 3, 3, 2, 5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3,
    2, 3, 2, 2, 1, 6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3,
    4, 3, 3, 2, 5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3,
    2, 2, 1, 5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2,
    2, 1, 4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1,
    0
};

/*
static const unsigned char bitson[]={
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1,
2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2,
2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3,
4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3,
2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3,
4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4,
4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4,
2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3,
4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5,
6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6,
6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7,
8 };
*/

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    int n                             = 0;
    grib_accessor_count_missing* self = (grib_accessor_count_missing*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    a->length                         = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    self->bitmap             = grib_arguments_get_name(h, arg, n++);
    self->unusedBitsInBitmap = grib_arguments_get_name(h, arg, n++);
    self->numberOfDataPoints = grib_arguments_get_name(h, arg, n++);
    self->missingValueManagementUsed = grib_arguments_get_name(h, arg, n++); /* Can be NULL */
}

static const int used[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

static int get_count_of_missing_values(grib_handle* h, long* p_count_of_missing)
{
    int err = 0;
    long count_of_missing = 0;
    size_t vsize = 0, ii = 0;
    double* values = NULL;
    double mv      = 0;
    if ((err = grib_get_double(h, "missingValue", &mv)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_size(h, "values", &vsize)) != GRIB_SUCCESS)
        return err;
    values = (double*)grib_context_malloc(h->context, vsize * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;
    if ((err = grib_get_double_array(h, "values", values, &vsize)) != GRIB_SUCCESS)
        return err;
    for (ii = 0; ii < vsize; ii++) {
        if (values[ii] == mv) ++count_of_missing;
    }
    grib_context_free(h->context, values);
    *p_count_of_missing = count_of_missing;

    return GRIB_SUCCESS;
}
static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_count_missing* self = (grib_accessor_count_missing*)a;
    unsigned char* p;
    int i;
    long size               = 0;
    long offset             = 0;
    long unusedBitsInBitmap = 0;
    long numberOfDataPoints = 0;
    grib_handle* h          = grib_handle_of_accessor(a);
    grib_accessor* bitmap   = grib_find_accessor(h, self->bitmap);

    *val = 0; /* By default assume none are missing */
    *len = 1;
    if (!bitmap) {
        long mvmu = 0;
        if (self->missingValueManagementUsed &&
            grib_get_long(h, self->missingValueManagementUsed, &mvmu) == GRIB_SUCCESS && mvmu != 0)
        {
            /* ECC-523: No bitmap. Missing values are encoded in the Data Section.
             * So we must decode all the data values and count how many are missing
            */
            long count_of_missing = 0;
            if (get_count_of_missing_values(h, &count_of_missing) == GRIB_SUCCESS) {
                *val = count_of_missing;
            }
        }
        return GRIB_SUCCESS;
    }

    size   = grib_byte_count(bitmap);
    offset = grib_byte_offset(bitmap);

    if (grib_get_long(h, self->unusedBitsInBitmap, &unusedBitsInBitmap) != GRIB_SUCCESS) {
        if (grib_get_long(h, self->numberOfDataPoints, &numberOfDataPoints) != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "unable to count missing values");
            return GRIB_INTERNAL_ERROR;
        }
        unusedBitsInBitmap = size * 8 - numberOfDataPoints;
    }

    p = h->buffer->data + offset;

    size -= unusedBitsInBitmap / 8;
    unusedBitsInBitmap = unusedBitsInBitmap % 8;

    for (i = 0; i < size - 1; i++)
        *val += bitsoff[*(p++)];

    *val += bitsoff[(*p) | used[unusedBitsInBitmap]];

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}
