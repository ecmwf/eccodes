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
   IMPLEMENTS = unpack_long;pack_long
   IMPLEMENTS = init;dump;is_missing
   IMPLEMENTS = next_offset
   IMPLEMENTS = byte_count
   IMPLEMENTS = value_count
   IMPLEMENTS = byte_offset
   IMPLEMENTS = update_size; destroy
   MEMBERS    = long nbytes
   MEMBERS    = grib_arguments* arg
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int is_missing(grib_accessor*);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void update_size(grib_accessor*, size_t);

typedef struct grib_accessor_unsigned
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
} grib_accessor_unsigned;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_unsigned = {
    &grib_accessor_class_long,                      /* super */
    "unsigned",                      /* name */
    sizeof(grib_accessor_unsigned),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* destroy */
    &dump,                       /* dump */
    &next_offset,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    &byte_count,                 /* get number of bytes */
    &byte_offset,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    &is_missing,                 /* is_missing */
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
    &update_size,                /* update_size */
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


grib_accessor_class* grib_accessor_class_unsigned = &_grib_accessor_class_unsigned;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_unsigned* self = (grib_accessor_unsigned*)a;
    self->arg                    = NULL;
    self->arg                    = arg;
    self->nbytes                 = len;

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        a->length = 0;
        if (!a->vvalue)
            a->vvalue = (grib_virtual_value*)grib_context_malloc_clear(a->context, sizeof(grib_virtual_value));
        a->vvalue->type   = GRIB_TYPE_LONG;
        a->vvalue->length = len;
    }
    else {
        long count = 0;
        grib_value_count(a, &count);

        a->length = len * count;
        a->vvalue = NULL;
    }
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    long rlen = 0;
    grib_value_count(a, &rlen);
    if (rlen == 1)
        grib_dump_long(dumper, a, NULL);
    else
        grib_dump_values(dumper, a);
}

static const unsigned long ones[] = {
    0,
    0xff,
    0xffff,
    0xffffff,
    0xffffffff,
};

/* See GRIB-490 */
static const unsigned long all_ones = -1;
static int value_is_missing(long val)
{
    return (val == GRIB_MISSING_LONG || val == all_ones);
}

int pack_long_unsigned_helper(grib_accessor* a, const long* val, size_t* len, int check)
{
    grib_accessor_unsigned* self = (grib_accessor_unsigned*)a;
    int ret                      = 0;
    long off                     = 0;
    long rlen                    = 0;
    int err                      = 0;

    size_t buflen         = 0;
    unsigned char* buf    = NULL;
    unsigned long i       = 0;
    unsigned long missing = 0;

    err = grib_value_count(a, &rlen);
    if (err)
        return err;

    if (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        Assert(self->nbytes <= 4);
        missing = ones[self->nbytes];
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        a->vvalue->lval = val[0];

        if (missing && val[0] == GRIB_MISSING_LONG)
            a->vvalue->missing = 1;
        else
            a->vvalue->missing = 0;

        return GRIB_SUCCESS;
    }

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        len[0] = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {
        long v = val[0];

        if (missing)
            if (v == GRIB_MISSING_LONG)
                v = missing;

        /* Check if value fits into number of bits */
        if (check) {
            if (val[0] < 0) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                        "Key \"%s\": Trying to encode a negative value of %ld for key of type unsigned", a->name, val[0]);
                return GRIB_ENCODING_ERROR;
            }
            /* See GRIB-23 and GRIB-262 */
            if (!value_is_missing(v)) {
                const long nbits = self->nbytes * 8;
                if (nbits < 33) {
                    unsigned long maxval = (1UL << nbits) - 1;
                    if (maxval > 0 && v > maxval) { /* See ECC-1002 */
                        grib_context_log(a->context, GRIB_LOG_ERROR,
                                "Key \"%s\": Trying to encode value of %ld but the maximum allowable value is %lu (number of bits=%ld)",
                                 a->name, v, maxval, nbits);
                        return GRIB_ENCODING_ERROR;
                    }
                }
            }
        }

        off = a->offset * 8;
        ret = grib_encode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, v, &off, self->nbytes * 8);
        if (ret == GRIB_SUCCESS)
            len[0] = 1;
        if (*len > 1)
            grib_context_log(a->context, GRIB_LOG_WARNING, "grib_accessor_unsigned : Trying to pack %d values in a scalar %s, packing first value", *len, a->name);
        len[0] = 1;
        return ret;
    }

    /* TODO: We assume that there are no missing values if there are more that 1 value */
    buflen = *len * self->nbytes;

    buf = (unsigned char*)grib_context_malloc(a->context, buflen);

    for (i = 0; i < *len; i++)
        grib_encode_unsigned_long(buf, val[i], &off, self->nbytes * 8);

    ret = grib_set_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), *len);

    if (ret == GRIB_SUCCESS)
        grib_buffer_replace(a, buf, buflen, 1, 1);
    else
        *len = 0;

    grib_context_free(a->context, buf);
    return ret;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_unsigned* self = (grib_accessor_unsigned*)a;
    long rlen                    = 0;
    unsigned long i              = 0;
    unsigned long missing        = 0;
    long count                   = 0;
    int err                      = 0;
    long pos                     = a->offset * 8;
    grib_handle* hand            = grib_handle_of_accessor(a);

    err = grib_value_count(a, &count);
    if (err)
        return err;
    rlen = count;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size (%ld) for %s, it contains %ld values", *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        *val = a->vvalue->lval;
        *len = 1;
        return GRIB_SUCCESS;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        Assert(self->nbytes <= 4);
        missing = ones[self->nbytes];
    }

    for (i = 0; i < rlen; i++) {
        val[i] = (long)grib_decode_unsigned_long(hand->buffer->data, &pos, self->nbytes * 8);
        if (missing)
            if (val[i] == missing)
                val[i] = GRIB_MISSING_LONG;
    }

    *len = rlen;
    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    /* See GRIB-262 as example of why we do the checks */
    return pack_long_unsigned_helper(a, val, len, /*check=*/1);
}

static long byte_count(grib_accessor* a)
{
    return a->length;
}

static int value_count(grib_accessor* a, long* len)
{
    grib_accessor_unsigned* self = (grib_accessor_unsigned*)a;
    if (!self->arg) {
        *len = 1;
        return 0;
    }
    return grib_get_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), len);
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static void update_size(grib_accessor* a, size_t s)
{
    a->length = s;
}

static long next_offset(grib_accessor* a)
{
    return grib_byte_offset(a) + grib_byte_count(a);
}

static int is_missing(grib_accessor* a)
{
    int i                = 0;
    unsigned char ff     = 0xff;
    unsigned long offset = a->offset;
    grib_handle* hand    = grib_handle_of_accessor(a);

    if (a->length == 0) {
        Assert(a->vvalue != NULL);
        return a->vvalue->missing;
    }

    for (i = 0; i < a->length; i++) {
        if (hand->buffer->data[offset] != ff) {
            return 0;
        }
        offset++;
    }
    return 1;
}

static void destroy(grib_context* context, grib_accessor* a)
{
    if (a->vvalue != NULL)
        grib_context_free(context, a->vvalue);

    a->vvalue = NULL;
}
