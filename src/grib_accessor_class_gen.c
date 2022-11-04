/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                    *
 *   Enrico Fucile                                                         *
 *   Shahram Najm                                                          *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   IMPLEMENTS = unpack_long;pack_long; clear
   IMPLEMENTS = unpack_double;pack_double;unpack_double_element;unpack_double_element_set
   IMPLEMENTS = unpack_string;pack_string
   IMPLEMENTS = unpack_string_array;pack_string_array
   IMPLEMENTS = unpack_bytes;pack_bytes
   IMPLEMENTS = unpack_double_subarray 
   IMPLEMENTS = init;dump;destroy;string_length
   IMPLEMENTS = get_native_type;sub_section
   IMPLEMENTS = next_offset;value_count;byte_offset;byte_count
   IMPLEMENTS = notify_change;pack_expression
   IMPLEMENTS  = update_size; next; preferred_size
   IMPLEMENTS = compare;is_missing;make_clone
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static grib_section* sub_section(grib_accessor* a);
static int get_native_type(grib_accessor*);
static int is_missing(grib_accessor*);
static int pack_bytes(grib_accessor*, const unsigned char*, size_t* len);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int pack_string_array(grib_accessor*, const char**, size_t* len);
static int pack_expression(grib_accessor*, grib_expression*);
static int unpack_bytes(grib_accessor*, unsigned char*, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static int unpack_string_array(grib_accessor*, char**, size_t* len);
static size_t string_length(grib_accessor*);
static long byte_count(grib_accessor*);
static long byte_offset(grib_accessor*);
static long next_offset(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);
static int notify_change(grib_accessor*, grib_accessor*);
static void update_size(grib_accessor*, size_t);
static size_t preferred_size(grib_accessor*, int);
static grib_accessor* next(grib_accessor*, int);
static int compare(grib_accessor*, grib_accessor*);
static int unpack_double_element(grib_accessor*, size_t i, double* val);
static int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array);
static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);
static int clear(grib_accessor*);
static grib_accessor* make_clone(grib_accessor*, grib_section*, int*);

typedef struct grib_accessor_gen
{
    grib_accessor att;
    /* Members defined in gen */
} grib_accessor_gen;


static grib_accessor_class _grib_accessor_class_gen = {
    0,                      /* super */
    "gen",                      /* name */
    sizeof(grib_accessor_gen),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* free mem */
    &dump,                       /* describes himself */
    &next_offset,                /* get length of section */
    &string_length,              /* get length of string */
    &value_count,                /* get number of values */
    &byte_count,                 /* get number of bytes */
    &byte_offset,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    &sub_section,                /* get sub_section */
    0,               /* grib_pack procedures long */
    &is_missing,                 /* grib_pack procedures long */
    &pack_long,                  /* grib_pack procedures long */
    &unpack_long,                /* grib_unpack procedures long */
    &pack_double,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    &pack_string,                /* grib_pack procedures string */
    &unpack_string,              /* grib_unpack procedures string */
    &pack_string_array,          /* grib_pack array procedures string */
    &unpack_string_array,        /* grib_unpack array procedures string */
    &pack_bytes,                 /* grib_pack procedures bytes */
    &unpack_bytes,               /* grib_unpack procedures bytes */
    &pack_expression,            /* pack_expression */
    &notify_change,              /* notify_change */
    &update_size,                /* update_size */
    &preferred_size,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    &next,                       /* next accessor */
    &compare,                    /* compare vs. another accessor */
    &unpack_double_element,      /* unpack only ith value */
    &unpack_double_element_set,  /* unpack a given set of elements */
    &unpack_double_subarray,     /* unpack a subarray */
    &clear,                      /* clear */
    &make_clone,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_gen = &_grib_accessor_class_gen;


static void init_class(grib_accessor_class* c)
{
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* param)
{
    grib_action* act = (grib_action*)(a->creator);
    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        a->length = 0;
        if (!a->vvalue)
            a->vvalue = (grib_virtual_value*)grib_context_malloc_clear(a->context, sizeof(grib_virtual_value));
        a->vvalue->type   = grib_accessor_get_native_type(a);
        a->vvalue->length = len;
        if (act->default_value != NULL) {
            const char* p = 0;
            size_t s_len  = 1;
            long l;
            int ret = 0;
            double d;
            char tmp[1024];
            grib_expression* expression = grib_arguments_get_expression(grib_handle_of_accessor(a), act->default_value, 0);
            int type                    = grib_expression_native_type(grib_handle_of_accessor(a), expression);
            switch (type) {
                case GRIB_TYPE_DOUBLE:
                    grib_expression_evaluate_double(grib_handle_of_accessor(a), expression, &d);
                    grib_pack_double(a, &d, &s_len);
                    break;

                case GRIB_TYPE_LONG:
                    grib_expression_evaluate_long(grib_handle_of_accessor(a), expression, &l);
                    grib_pack_long(a, &l, &s_len);
                    break;

                default:
                    s_len = sizeof(tmp);
                    p     = grib_expression_evaluate_string(grib_handle_of_accessor(a), expression, tmp, &s_len, &ret);
                    if (ret != GRIB_SUCCESS) {
                        grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to evaluate %s as string", a->name);
                        Assert(0);
                    }
                    s_len = strlen(p) + 1;
                    grib_pack_string(a, p, &s_len);
                    break;
            }
        }
    }
    else {
        a->length = len;
    }
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    int type = grib_accessor_get_native_type(a);

    switch (type) {
        case GRIB_TYPE_STRING:
            grib_dump_string(dumper, a, NULL);
            break;
        case GRIB_TYPE_DOUBLE:
            grib_dump_double(dumper, a, NULL);
            break;
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, a, NULL);
            break;
        default:
            grib_dump_bytes(dumper, a, NULL);
    }
}

static long next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static size_t string_length(grib_accessor* a)
{
    return 1024;
}

static long byte_count(grib_accessor* a)
{
    return a->length;
}

static int get_native_type(grib_accessor* a)
{
    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "Accessor %s [%s] must implement 'get_native_type'", a->name, a->cclass->name);
    return GRIB_TYPE_UNDEFINED;
}

static long byte_offset(grib_accessor* a)
{
    return a->offset;
}

static int unpack_bytes(grib_accessor* a, unsigned char* val, size_t* len)
{
    unsigned char* buf = grib_handle_of_accessor(a)->buffer->data;
    long length        = grib_byte_count(a);
    long offset        = grib_byte_offset(a);

    if (*len < length) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it is %ld bytes long", a->name, length);
        *len = length;
        return GRIB_ARRAY_TOO_SMALL;
    }

    memcpy(val, buf + offset, length);
    *len = length;

    return GRIB_SUCCESS;
}

static int clear(grib_accessor* a)
{
    unsigned char* buf = grib_handle_of_accessor(a)->buffer->data;
    long length        = grib_byte_count(a);
    long offset        = grib_byte_offset(a);

    memset(buf + offset, 0, length);

    return GRIB_SUCCESS;
}

static int unpack_long(grib_accessor* a, long* v, size_t* len)
{
    int type = GRIB_TYPE_UNDEFINED;
    if (a->cclass->unpack_double && a->cclass->unpack_double != &unpack_double) {
        double val = 0.0;
        size_t l   = 1;
        grib_unpack_double(a, &val, &l);
        if (val == GRIB_MISSING_DOUBLE)
            *v = GRIB_MISSING_LONG;
        else
            *v = (long)val;
        grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting double %s to long", a->name);
        return GRIB_SUCCESS;
    }

    if (a->cclass->unpack_string && a->cclass->unpack_string != &unpack_string) {
        char val[1024];
        size_t l   = sizeof(val);
        char* last = NULL;
        grib_unpack_string(a, val, &l);

        *v = strtol(val, &last, 10);

        if (*last == 0) {
            grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting string %s to long", a->name);
            return GRIB_SUCCESS;
        }
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "Cannot unpack %s as long", a->name);
    if (grib_get_native_type(grib_handle_of_accessor(a), a->name, &type) == GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Hint: Try unpacking as %s", grib_get_type_name(type));
    }
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_double(grib_accessor* a, double* v, size_t* len)
{
    int type = GRIB_TYPE_UNDEFINED;
    if (a->cclass->unpack_long && a->cclass->unpack_long != &unpack_long) {
        long val = 0;
        size_t l = 1;
        grib_unpack_long(a, &val, &l);
        *v = val;
        grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting long %s to double", a->name);
        return GRIB_SUCCESS;
    }

    if (a->cclass->unpack_string && a->cclass->unpack_string != &unpack_string) {
        char val[1024];
        size_t l   = sizeof(val);
        char* last = NULL;
        grib_unpack_string(a, val, &l);

        *v = strtod(val, &last);
        if (*last == 0) { /* conversion of string to double worked */
            grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting string %s to long", a->name);
            return GRIB_SUCCESS;
        }
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "Cannot unpack %s as double", a->name);
    if (grib_get_native_type(grib_handle_of_accessor(a), a->name, &type) == GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Hint: Try unpacking as %s", grib_get_type_name(type));
    }

    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_string(grib_accessor* a, char* v, size_t* len)
{
    if (a->cclass->unpack_double && a->cclass->unpack_double != &unpack_double) {
        double val = 0.0;
        size_t l   = 1;
        grib_unpack_double(a, &val, &l);
        sprintf(v, "%g", val);
        *len = strlen(v);
        grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting double %s to string", a->name);
        return GRIB_SUCCESS;
    }

    if (a->cclass->unpack_long && a->cclass->unpack_long != &unpack_long) {
        long val = 0;
        size_t l = 1;
        grib_unpack_long(a, &val, &l);
        sprintf(v, "%ld", val);
        *len = strlen(v);
        grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting long %s to string  \n", a->name);
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_string_array(grib_accessor* a, char** v, size_t* len)
{
    int err       = 0;
    size_t length = 0;

    err = _grib_get_string_length(a, &length);
    if (err)
        return err;
    v[0] = (char*)grib_context_malloc_clear(a->context, length);
    grib_unpack_string(a, v[0], &length);
    *len = 1;

    return GRIB_SUCCESS;
}

static int pack_expression(grib_accessor* a, grib_expression* e)
{
    size_t len        = 1;
    long lval         = 0;
    double dval       = 0;
    const char* cval  = NULL;
    int ret           = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    switch (grib_accessor_get_native_type(a)) {
        case GRIB_TYPE_LONG: {
            len = 1;
            ret = grib_expression_evaluate_long(hand, e, &lval);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to set %s as long (from %s)",
                                 a->name, e->cclass->name);
                return ret;
            }
            /*if (hand->context->debug)
                printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %ld\n", a->name,lval);*/
            return grib_pack_long(a, &lval, &len);
        }

        case GRIB_TYPE_DOUBLE: {
            len = 1;
            ret = grib_expression_evaluate_double(hand, e, &dval);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "unable to set %s as double (from %s)",
                                 a->name, e->cclass->name);
                return ret;
            }
            /*if (hand->context->debug)
                printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %g\n", a->name, dval);*/
            return grib_pack_double(a, &dval, &len);
        }

        case GRIB_TYPE_STRING: {
            char tmp[1024];
            len  = sizeof(tmp);
            cval = grib_expression_evaluate_string(hand, e, tmp, &len, &ret);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "unable to set %s as string (from %s)",
                                 a->name, e->cclass->name);
                return ret;
            }
            len = strlen(cval);
            /*if (hand->context->debug)
                printf("ECCODES DEBUG grib_accessor_class_gen::pack_expression %s %s\n", a->name, cval);*/
            return grib_pack_string(a, cval, &len);
        }
    }

    return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long* v, size_t* len)
{
    grib_context* c = a->context;
    if (a->cclass->pack_double && a->cclass->pack_double != &pack_double) {
        int i = 0, ret = 0;
        double* val = (double*)grib_context_malloc(c, *len * (sizeof(double)));
        if (!val) {
            grib_context_log(c, GRIB_LOG_ERROR,
                             "Unable to allocate %d bytes\n", (int)(*len * (sizeof(double))));
            return GRIB_OUT_OF_MEMORY;
        }
        for (i = 0; i < *len; i++)
            val[i] = (long)v[i];
        ret = grib_pack_double(a, val, len);
        grib_context_free(c, val);
        return ret;
    }
    grib_context_log(c, GRIB_LOG_ERROR, "Should not grib_pack %s as long", a->name);
    Assert(0);
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double_array_as_long(grib_accessor* a, const double* v, size_t* len)
{
    grib_context* c = a->context;
    int ret         = GRIB_SUCCESS;
    size_t i        = 0;
    size_t numBytes = *len * (sizeof(long));
    long* lValues   = (long*)grib_context_malloc(c, numBytes);
    if (!lValues) {
        grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes\n", numBytes);
        return GRIB_OUT_OF_MEMORY;
    }
    for (i = 0; i < *len; i++)
        lValues[i] = (long)v[i]; /* convert from double to long */
    ret = grib_pack_long(a, lValues, len);
    grib_context_free(c, lValues);
    return ret;
}

static int pack_double(grib_accessor* a, const double* v, size_t* len)
{
    int do_pack_as_long = 0;
    grib_context* c     = a->context;
    if (a->cclass->pack_long && a->cclass->pack_long != &pack_long) {
        do_pack_as_long = 1;
    }
    else {
        /* ECC-648: Special case of codetable */
        if (strcmp(a->cclass->name, "codetable") == 0) {
            do_pack_as_long = 1;
        }
    }
    if (do_pack_as_long) {
        return pack_double_array_as_long(a, v, len);
    }
    grib_context_log(c, GRIB_LOG_ERROR, "Should not grib_pack %s as double", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_string_array(grib_accessor* a, const char** v, size_t* len)
{
    long i;
    int err           = 0;
    size_t length     = 0;
    grib_accessor* as = 0;

    as = a;
    i  = (long)*len - 1;
    while (as && i >= 0) {
        length = strlen(v[i]);
        err    = grib_pack_string(as, v[i], &length);
        if (err)
            return err;
        --i;
        as = as->same;
    }
    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* v, size_t* len)
{
    if (a->cclass->pack_double && a->cclass->pack_double != &pack_double) {
        size_t l   = 1;
        char* endPtr = NULL; /* for error handling */
        double val = strtod(v, &endPtr);
        if (*endPtr) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "pack_string: Invalid value (%s) for %s. String cannot be converted to a double",
                             v, a->name);
            return GRIB_WRONG_TYPE;
        }
        return grib_pack_double(a, &val, &l);
    }

    if (a->cclass->pack_long && a->cclass->pack_long != &pack_long) {
        size_t l = 1;
        long val = atof(v);
        return grib_pack_long(a, &val, &l);
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack '%s' as string", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_bytes(grib_accessor* a, const unsigned char* val, size_t* len)
{
    const size_t length = *len;
    if (length != a->length) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "pack_bytes: Wrong size (%lu) for %s. It is %lu bytes long",
                         length, a->name, a->length);
        return GRIB_BUFFER_TOO_SMALL;
    }
    grib_buffer_replace(a, val, length, 1, 1);
    return GRIB_SUCCESS;
}

static void destroy(grib_context* ct, grib_accessor* a)
{
    grib_dependency_remove_observed(a);
    grib_dependency_remove_observer(a);
    if (a->vvalue != NULL) {
        grib_context_free(ct, a->vvalue);
        a->vvalue = NULL;
    }
    /*grib_context_log(ct,GRIB_LOG_DEBUG,"address=%p",a);*/
}

static grib_section* sub_section(grib_accessor* a)
{
    return NULL;
}

static int notify_change(grib_accessor* self, grib_accessor* observed)
{
    /* Default behaviour is to notify creator */
    return grib_action_notify_change(self->creator, self, observed);
}

static void update_size(grib_accessor* a, size_t s)
{
    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "Accessor %s [%s] must implement 'update_size'", a->name, a->cclass->name);
    Assert(0 == 1);
}

static grib_accessor* next(grib_accessor* a, int mod)
{
    grib_accessor* next = NULL;
    if (a->next) {
        next = a->next;
    }
    else {
        if (a->parent->owner)
            next = a->parent->owner->cclass->next(a->parent->owner, 0);
    }
    return next;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    return GRIB_NOT_IMPLEMENTED;
}

/* Redefined in all padding */

static size_t preferred_size(grib_accessor* a, int from_handle)
{
    return a->length;
}

static int is_missing(grib_accessor* a)
{
    int i              = 0;
    int is_missing     = 1;
    unsigned char ones = 0xff;
    unsigned char* v   = NULL;

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        if (a->vvalue == NULL) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s internal error (flags=0x%lX)", a->name, a->flags);
            Assert(!"grib_accessor_class_gen::is_missing(): a->vvalue == NULL");
            return 0;
        }
        return a->vvalue->missing;
    }
    Assert(a->length >= 0);

    v = grib_handle_of_accessor(a)->buffer->data + a->offset;

    for (i = 0; i < a->length; i++) {
        if (*v != ones) {
            is_missing = 0;
            break;
        }
        v++;
    }

    return is_missing;
}

static int unpack_double_element(grib_accessor* a, size_t i, double* val)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int unpack_double_subarray(grib_accessor* a, double* val, size_t start, size_t len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static grib_accessor* make_clone(grib_accessor* a, grib_section* s, int* err)
{
    *err = GRIB_NOT_IMPLEMENTED;
    return NULL;
}
