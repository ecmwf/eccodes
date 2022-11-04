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
   SUPER      = grib_accessor_class_double
   IMPLEMENTS = unpack_double;
   IMPLEMENTS = value_count
   IMPLEMENTS = init
   MEMBERS =const char* values
   MEMBERS =long distinct
   MEMBERS =double* lats
   MEMBERS =long size
   MEMBERS =int save
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_latitudes
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in double */
    /* Members defined in latitudes */
    const char* values;
    long distinct;
    double* lats;
    long size;
    int save;
} grib_accessor_latitudes;

extern grib_accessor_class* grib_accessor_class_double;

static grib_accessor_class _grib_accessor_class_latitudes = {
    &grib_accessor_class_double,                      /* super */
    "latitudes",                      /* name */
    sizeof(grib_accessor_latitudes),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    0,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    0,                  /* grib_pack procedures long */
    0,                /* grib_unpack procedures long */
    0,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    0,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_latitudes = &_grib_accessor_class_latitudes;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->get_native_type    =    (*(c->super))->get_native_type;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_long    =    (*(c->super))->pack_long;
    c->unpack_long    =    (*(c->super))->unpack_long;
    c->pack_double    =    (*(c->super))->pack_double;
    c->pack_string    =    (*(c->super))->pack_string;
    c->unpack_string    =    (*(c->super))->unpack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */


static int get_distinct(grib_accessor* a, double** val, long* len);

static int compare_doubles(const void* a, const void* b, int ascending)
{
    /* ascending is a boolean: 0 or 1 */
    double* arg1 = (double*)a;
    double* arg2 = (double*)b;
    if (ascending) {
        if (*arg1 < *arg2)
            return -1; /*Smaller values come before larger ones*/
    }
    else {
        if (*arg1 > *arg2)
            return -1; /*Larger values come before smaller ones*/
    }
    if (*arg1 == *arg2)
        return 0;
    else
        return 1;
}
static int compare_doubles_ascending(const void* a, const void* b)
{
    return compare_doubles(a, b, 1);
}
static int compare_doubles_descending(const void* a, const void* b)
{
    return compare_doubles(a, b, 0);
}

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_latitudes* self = (grib_accessor_latitudes*)a;
    int n                         = 0;

    self->values   = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->distinct = grib_arguments_get_long(grib_handle_of_accessor(a), c, n++);
    self->save     = 0;

    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_context* c               = a->context;
    grib_accessor_latitudes* self = (grib_accessor_latitudes*)a;
    int ret                       = 0;
    double* v                     = val;
    double dummyLon = 0, dummyVal = 0;
    size_t size         = 0;
    long count          = 0;
    grib_iterator* iter = NULL;

    self->save = 1;
    ret        = value_count(a, &count);
    if (ret)
        return ret;
    size = count;
    if (*len < size) {
        /* self->lats are computed in value_count so must free */
        if (self->lats) {
            grib_context_free(c, self->lats);
            self->lats = NULL;
        }
        return GRIB_ARRAY_TOO_SMALL;
    }
    self->save = 0;

    /* self->lats are computed in value_count*/
    if (self->lats) {
        int i;
        *len = self->size;
        for (i = 0; i < size; i++)
            val[i] = self->lats[i];
        grib_context_free(c, self->lats);
        self->lats = NULL;
        self->size = 0;
        return GRIB_SUCCESS;
    }

    iter = grib_iterator_new(grib_handle_of_accessor(a), 0, &ret);
    if (ret != GRIB_SUCCESS) {
        if (iter)
            grib_iterator_delete(iter);
        grib_context_log(c, GRIB_LOG_ERROR, "Unable to create iterator");
        return ret;
    }

    while (grib_iterator_next(iter, v++, &dummyLon, &dummyVal)) {}
    grib_iterator_delete(iter);

    *len = size;

    return ret;
}

static int value_count(grib_accessor* a, long* len)
{
    grib_accessor_latitudes* self = (grib_accessor_latitudes*)a;
    grib_handle* h                = grib_handle_of_accessor(a);
    grib_context* c               = a->context;
    double* val                   = NULL;
    int ret;
    size_t size;

    *len = 0;
    if ((ret = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Unable to get size of %s", self->values);
        return ret;
    }
    *len = size;

    if (self->distinct) {
        ret = get_distinct(a, &val, len);
        if (ret != GRIB_SUCCESS)
            return ret;
        if (self->save) {
            self->lats = val;
            self->size = *len;
        }
        else {
            grib_context_free(c, val);
        }
    }

    return ret;
}

static int get_distinct(grib_accessor* a, double** val, long* len)
{
    long count = 0;
    double prev;
    double* v       = NULL;
    double* v1      = NULL;
    double dummyLon = 0, dummyVal = 0;
    int ret = 0;
    int i;
    long jScansPositively = 0; /*default: north to south*/
    size_t size           = *len;
    grib_context* c       = a->context;
    grib_iterator* iter   = grib_iterator_new(grib_handle_of_accessor(a), 0, &ret);
    if (ret != GRIB_SUCCESS) {
        if (iter)
            grib_iterator_delete(iter);
        grib_context_log(c, GRIB_LOG_ERROR, "Unable to create iterator");
        return ret;
    }
    v = (double*)grib_context_malloc_clear(c, size * sizeof(double));
    if (!v) {
        grib_context_log(c, GRIB_LOG_ERROR,
                         "Error allocating %ld bytes", (long)size * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    *val = v;

    while (grib_iterator_next(iter, v++, &dummyLon, &dummyVal)) {}
    grib_iterator_delete(iter);
    v = *val;

    /* See which direction the latitudes are to be scanned */
    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), "jScansPositively", &jScansPositively)))
        return ret;
    if (jScansPositively) {
        qsort(v, *len, sizeof(double), &compare_doubles_ascending); /*South to North*/
    }
    else {
        qsort(v, *len, sizeof(double), &compare_doubles_descending); /*North to South*/
    }

    v1 = (double*)grib_context_malloc_clear(c, size * sizeof(double));
    if (!v1) {
        grib_context_log(c, GRIB_LOG_ERROR, "Error allocating %ld bytes", (long)size * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    /*Construct a unique set of lats by filtering out duplicates*/
    prev  = v[0];
    v1[0] = prev;
    count = 1;
    for (i = 1; i < *len; i++) {
        if (v[i] != prev) {
            prev      = v[i];
            v1[count] = prev; /*Value different from previous so store it*/
            count++;
        }
    }

    grib_context_free(c, v);

    *val = v1;

    *len = count;
    return GRIB_SUCCESS;
}
