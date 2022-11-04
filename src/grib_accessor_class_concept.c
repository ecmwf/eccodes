/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


/*******************************************************
 *   Enrico Fucile
 ******************************************************/

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = unpack_double;pack_double
   IMPLEMENTS = unpack_string;pack_string;string_length
   IMPLEMENTS = unpack_long;pack_long;destroy
   IMPLEMENTS = init;dump;value_count;get_native_type
   IMPLEMENTS = compare
    END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int pack_string(grib_accessor*, const char*, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int unpack_string(grib_accessor*, char*, size_t* len);
static size_t string_length(grib_accessor*);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);
static int compare(grib_accessor*, grib_accessor*);

typedef struct grib_accessor_concept
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in concept */
} grib_accessor_concept;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_concept = {
    &grib_accessor_class_gen,                      /* super */
    "concept",                      /* name */
    sizeof(grib_accessor_concept),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* free mem */
    &dump,                       /* describes himself */
    0,                /* get length of section */
    &string_length,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    &pack_long,                  /* grib_pack procedures long */
    &unpack_long,                /* grib_unpack procedures long */
    &pack_double,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    &pack_string,                /* grib_pack procedures string */
    &unpack_string,              /* grib_unpack procedures string */
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
    &compare,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_concept = &_grib_accessor_class_concept;


static void init_class(grib_accessor_class* c)
{
    c->next_offset    =    (*(c->super))->next_offset;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
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
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

#define MAX_CONCEPT_STRING_LENGTH 255

#define FALSE 0
#define TRUE  1

/* Note: A fast cut-down version of strcmp which does NOT return -1 */
/* 0 means input strings are equal and 1 means not equal */
GRIB_INLINE static int grib_inline_strcmp(const char* a, const char* b)
{
    if (*a != *b)
        return 1;
    while ((*a != 0 && *b != 0) && *(a) == *(b)) {
        a++;
        b++;
    }
    return (*a == 0 && *b == 0) ? 0 : 1;
}

static void init(grib_accessor* a, const long len, grib_arguments* args)
{
    a->length = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_string(dumper, a, NULL);
}

/* Return 1 (=True) or 0 (=False) */
static int concept_condition_expression_true(grib_handle* h, grib_concept_condition* c)
{
    long lval;
    long lres      = 0;
    int ok         = FALSE; /* Boolean */
    int err        = 0;
    const int type = grib_expression_native_type(h, c->expression);

    switch (type) {
        case GRIB_TYPE_LONG:
            grib_expression_evaluate_long(h, c->expression, &lres);
            ok = (grib_get_long(h, c->name, &lval) == GRIB_SUCCESS) &&
                 (lval == lres);
            break;

        case GRIB_TYPE_DOUBLE: {
            double dval;
            double dres = 0.0;
            grib_expression_evaluate_double(h, c->expression, &dres);
            ok = (grib_get_double(h, c->name, &dval) == GRIB_SUCCESS) &&
                 (dval == dres);
            break;
        }

        case GRIB_TYPE_STRING: {
            const char* cval;
            char buf[80];
            char tmp[80];
            size_t len  = sizeof(buf);
            size_t size = sizeof(tmp);

            ok = (grib_get_string(h, c->name, buf, &len) == GRIB_SUCCESS) &&
                 ((cval = grib_expression_evaluate_string(h, c->expression, tmp, &size, &err)) != NULL) &&
                 (err == 0) && (grib_inline_strcmp(buf, cval) == 0);
            break;
        }

        default:
            /* TODO: */
            break;
    }
    return ok;
}

/* Return 1 (=True) or 0 (=False) */
static int concept_condition_iarray_true(grib_handle* h, grib_concept_condition* c)
{
    long* val = NULL;
    size_t size = 0, i;
    int ret; /* Boolean */
    int err = 0;

    err = grib_get_size(h, c->name, &size);
    if (err || size != grib_iarray_used_size(c->iarray))
        return FALSE;

    val = (long*)grib_context_malloc_clear(h->context, sizeof(long) * size);

    err = grib_get_long_array(h, c->name, val, &size);
    if (err) {
        grib_context_free(h->context, val);
        return FALSE;
    }
    ret = TRUE;
    for (i = 0; i < size; i++) {
        if (val[i] != c->iarray->v[i]) {
            ret = FALSE;
            break;
        }
    }

    grib_context_free(h->context, val);
    return ret;
}

/* Return 1 (=True) or 0 (=False) */
static int concept_condition_true(grib_handle* h, grib_concept_condition* c)
{
    if (c->expression == NULL)
        return concept_condition_iarray_true(h, c);
    else
        return concept_condition_expression_true(h, c);
}

static const char* concept_evaluate(grib_accessor* a)
{
    int match        = 0;
    const char* best = 0;
    /* const char* prev = 0; */
    grib_concept_value* c = action_concept_get_concept(a);
    grib_handle* h        = grib_handle_of_accessor(a);

    while (c) {
        grib_concept_condition* e = c->conditions;
        int cnt                   = 0;
        while (e) {
            if (!concept_condition_true(h, e))
                break;
            e = e->next;
            cnt++;
        }

        if (e == NULL) {
            if (cnt >= match) {
                /* prev  = (cnt > match) ? NULL : best; */
                match = cnt;
                best  = c->name;
            }
        }

        c = c->next;
    }

    return best;
}

#define MAX_NUM_CONCEPT_VALUES 40

static int concept_conditions_expression_apply(grib_handle* h, grib_concept_condition* e, grib_values* values, grib_sarray* sa, int* n)
{
    long lres   = 0;
    double dres = 0.0;
    int count   = *n;
    size_t size;
    int err = 0;

    Assert(count < 1024);
    values[count].name = e->name;

    values[count].type = grib_expression_native_type(h, e->expression);
    switch (values[count].type) {
        case GRIB_TYPE_LONG:
            grib_expression_evaluate_long(h, e->expression, &lres);
            values[count].long_value = lres;
            break;
        case GRIB_TYPE_DOUBLE:
            grib_expression_evaluate_double(h, e->expression, &dres);
            values[count].double_value = dres;
            break;
        case GRIB_TYPE_STRING:
            size                       = sizeof(sa->v[count]);
            values[count].string_value = grib_expression_evaluate_string(h, e->expression, sa->v[count], &size, &err);
            break;
        default:
            return GRIB_NOT_IMPLEMENTED;
            break;
    }
    (*n)++;
    return err;
}

static int concept_conditions_iarray_apply(grib_handle* h, grib_concept_condition* c)
{
    size_t size = grib_iarray_used_size(c->iarray);
    return grib_set_long_array(h, c->name, c->iarray->v, size);
}

static int concept_conditions_apply(grib_handle* h, grib_concept_condition* c, grib_values* values, grib_sarray* sa, int* n)
{
    if (c->expression == NULL)
        return concept_conditions_iarray_apply(h, c);
    else
        return concept_conditions_expression_apply(h, c, values, sa, n);
}

static int cmpstringp(const void* p1, const void* p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */
    return strcmp(*(char* const*)p1, *(char* const*)p2);
}

static int grib_concept_apply(grib_accessor* a, const char* name)
{
    int err                   = 0;
    int count                 = 0;
    grib_concept_condition* e = NULL;
    grib_values values[1024];
    grib_sarray* sa              = NULL;
    grib_concept_value* c        = NULL;
    grib_concept_value* concepts = action_concept_get_concept(a);
    grib_handle* h               = grib_handle_of_accessor(a);
    grib_action* act             = a->creator;
    int nofail                   = action_concept_get_nofail(a);

    DebugAssert(concepts);

    c = (grib_concept_value*)grib_trie_get(concepts->index, name);

    if (!c)
        c = (grib_concept_value*)grib_trie_get(concepts->index, "default");

    if (!c) {
        err = nofail ? GRIB_SUCCESS : GRIB_CONCEPT_NO_MATCH;
        if (err) {
            size_t i = 0, concept_count = 0;
            long dummy = 0, editionNumber = 0;
            char centre_s[32] = {0,};
            size_t centre_len = sizeof(centre_s);
            char* all_concept_vals[MAX_NUM_CONCEPT_VALUES] = {NULL,}; /* sorted array containing concept values */
            grib_concept_value* pCon = concepts;

            grib_context_log(h->context, GRIB_LOG_ERROR, "concept: no match for %s=%s", act->name, name);
            if (grib_get_long(h, "edition", &editionNumber) == GRIB_SUCCESS &&
                grib_get_string(h, "centre", centre_s, &centre_len) == GRIB_SUCCESS) {
                grib_context_log(h->context, GRIB_LOG_ERROR, "concept: input handle edition=%ld, centre=%s", editionNumber, centre_s);
            }
            if (strcmp(act->name, "paramId") == 0 && string_to_long(name, &dummy) == GRIB_SUCCESS) {
                grib_context_log(h->context, GRIB_LOG_ERROR,
                                 "Please check the Parameter Database 'https://apps.ecmwf.int/codes/grib/param-db/?id=%s'", name);
            }
            if (strcmp(act->name, "shortName") == 0) {
                grib_context_log(h->context, GRIB_LOG_ERROR,
                                 "Please check the Parameter Database 'https://apps.ecmwf.int/codes/grib/param-db/'");
            }

            /* Create a list of all possible values for this concept and sort it */
            while (pCon) {
                if (i >= MAX_NUM_CONCEPT_VALUES)
                    break;
                all_concept_vals[i++] = pCon->name;
                pCon                  = pCon->next;
            }
            concept_count = i;
            /* Only print out all concepts if fewer than MAX_NUM_CONCEPT_VALUES.
             * Printing out all values for concepts like paramId would be silly! */
            if (concept_count < MAX_NUM_CONCEPT_VALUES) {
                fprintf(stderr, "Here are the possible values for concept %s:\n", act->name);
                qsort(&all_concept_vals, concept_count, sizeof(char*), cmpstringp);
                for (i = 0; i < concept_count; ++i) {
                    if (all_concept_vals[i]) {
                        int print_it = 1;
                        if (i > 0 && all_concept_vals[i - 1] && strcmp(all_concept_vals[i], all_concept_vals[i - 1]) == 0) {
                            print_it = 0; /* skip duplicate entries */
                        }
                        if (print_it)
                            fprintf(stderr, "\t%s\n", all_concept_vals[i]);
                    }
                }
            }
        }
        return err;
    }
    e  = c->conditions;
    sa = grib_sarray_new(h->context, 10, 10);
    while (e) {
        concept_conditions_apply(h, e, values, sa, &count);
        e = e->next;
    }
    grib_sarray_delete(h->context, sa);

    if (count)
        err = grib_set_values(h, values, count);
    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    char buf[80];
    size_t s;
    sprintf(buf, "%ld", *val);
#if 0
    if(*len > 1)
        return GRIB_NOT_IMPLEMENTED;
#endif
    s = strlen(buf) + 1;
    return pack_string(a, buf, &s);
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    /*
     * If we want to have a condition which contains tests for paramId as well
     * as a floating point key, then need to be able to evaluate paramId as a
     * double. E.g.
     *   if (referenceValue > 0 && paramId == 129)
     */
    /*return GRIB_NOT_IMPLEMENTED*/
    int ret = 0;
    if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE) {
        long lval = 0;
        ret       = unpack_long(a, &lval, len);
        if (ret == GRIB_SUCCESS) {
            *val = lval;
        }
    }
    else if (a->flags & GRIB_ACCESSOR_FLAG_DOUBLE_TYPE) {
        const char* p = concept_evaluate(a);

        if (!p) {
            grib_handle* h = grib_handle_of_accessor(a);
            if (a->creator->defaultkey)
                return grib_get_double_internal(h, a->creator->defaultkey, val);

            return GRIB_NOT_FOUND;
        }
        *val = atof(p);
        *len = 1;
    }
    return ret;
}

static long guess_paramId(grib_handle* h)
{
    int err = 0;
    long discipline, category, number;
    err = grib_get_long(h, "discipline", &discipline);
    if (err || discipline != 192)
        return -1;
    err = grib_get_long(h, "parameterCategory", &category);
    if (err)
        return -1;
    err = grib_get_long(h, "parameterNumber", &number);
    if (err)
        return -1;

    if (category == 128)
        return number;
    else
        return (category * 1000 + number);
}

/* Return -1 if cannot match */
static long get_ECMWF_local_paramId(grib_accessor* a, grib_handle* h)
{
    int err = 0;
    long edition, centre;
    if (h->product_kind != PRODUCT_GRIB)
        return -1;
    err = grib_get_long(h, "centre", &centre);
    if (err)
        return -1;
    err = grib_get_long(h, "edition", &edition);
    if (err)
        return -1;
    if (edition == 2 && centre == 98 && strncmp(a->name, "paramId", 7) == 0) {
        return guess_paramId(h);
    }
    return -1;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    const char* p = concept_evaluate(a);

    if (!p) {
        grib_handle* h = grib_handle_of_accessor(a);
        const long pid = get_ECMWF_local_paramId(a, h);
        if (pid != -1) {
            grib_context_log(h->context, GRIB_LOG_DEBUG, "ECMWF local grib2: paramId guessed to be %ld", pid);
            *val = pid;
            *len = 1;
            return GRIB_SUCCESS;
        }
        if (a->creator->defaultkey)
            return grib_get_long_internal(h, a->creator->defaultkey, val);

        return GRIB_NOT_FOUND;
    }

    *val = atol(p);
    *len = 1;
#ifdef DEBUG
    /* ECC-980: Changes reverted because of side-effects!
     * e.g. marsType being a codetable and concept! see ifsParam.
     * Keep this check in DEBUG mode only
     */
    {
        char *endptr;
        *val = strtol(p, &endptr, 10);
        if (endptr == p || *endptr != '\0') {
            /* Failed to convert string into integer */
            int type = GRIB_TYPE_UNDEFINED;
            grib_context_log(a->context,GRIB_LOG_ERROR,"Cannot unpack %s as long",a->name);
            if (grib_get_native_type(grib_handle_of_accessor(a), a->name, &type) == GRIB_SUCCESS) {
                grib_context_log(a->context,GRIB_LOG_ERROR,"Hint: Try unpacking as %s", grib_get_type_name(type));
            }
            return GRIB_DECODING_ERROR;
        }
    }
#endif
    return GRIB_SUCCESS;
}

static int get_native_type(grib_accessor* a)
{
    int type = GRIB_TYPE_STRING;
    if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE)
        type = GRIB_TYPE_LONG;

    return type;
}

static void destroy(grib_context* c, grib_accessor* a)
{
    /*
     * grib_accessor_concept *self = (grib_accessor_concept*)a;
     * grib_context_free(c,self->cval);
     */
}

#if 0
static int is_local_ecmwf_grib2_param_key(grib_accessor* a, long edition, long centre)
{
    if (edition == 2 && centre == 98) {
        if (a->parent->owner && a->parent->owner->name && strcmp(a->parent->owner->name, "parameters") == 0)
            return 1;
    }
    return 0;
}
#endif

#if 0
/* Try to get the name, shortName, units etc for a GRIB2 message with
 * local ECMWF coding i.e. discipline=192 etc
 */
static const char* get_ECMWF_local_parameter(grib_accessor* a, grib_handle* h)
{
    int err              = 0;
    const char* key_name = a->name; /*this is the key whose value we want*/
    long edition, centre;
    if (h->product_kind != PRODUCT_GRIB)
        return NULL;
    err = grib_get_long(h, "centre", &centre);
    if (err)
        return NULL;
    err = grib_get_long(h, "edition", &edition);
    if (err)
        return NULL;
    if (is_local_ecmwf_grib2_param_key(a, edition, centre)) {
        /* Must be one of: 'name', 'shortName', 'units', 'cfName' etc */
        grib_accessor* a2    = NULL;
        const long pid_guess = guess_paramId(h);
        if (pid_guess == -1)
            return NULL;

        /* Change the paramId so we can get the other string key*/
        err = grib_set_long(h, "paramId", pid_guess);
        if (err)
            return NULL;
        /* Get the string value of key. Do not call grib_get_string() to avoid
         * dangers of infinite recursion as that calls unpack_string()!
         */
        a2 = grib_find_accessor(h, key_name);
        if (!a2)
            return NULL;
        return concept_evaluate(a2);
    }
    return NULL;
}
#endif

static int unpack_string(grib_accessor* a, char* val, size_t* len)
{
    size_t slen;
    const char* p = concept_evaluate(a);

    if (!p) {
        grib_handle* h = grib_handle_of_accessor(a);
        if (a->creator->defaultkey)
            return grib_get_string_internal(h, a->creator->defaultkey, val, len);

        return GRIB_NOT_FOUND;
    }

    slen = strlen(p) + 1;
    if (*len < slen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                        "Concept unpack_string. Wrong size for %s, value='%s' which requires %lu bytes (len=%lu)",
                         a->name, p, slen, *len);
        *len = slen;
        return GRIB_BUFFER_TOO_SMALL;
    }
    strcpy(val, p); /* NOLINT: CWE-119 clang-analyzer-security.insecureAPI.strcpy */
    *len = slen;
#if 0
    if (a->context->debug==1) {
        int err = 0;
        char result[1024] = {0,};
        err = get_concept_condition_string(grib_handle_of_accessor(a), a->name, val, result);
        if (!err) fprintf(stderr, "ECCODES DEBUG concept name=%s, value=%s, conditions=%s\n", a->name, val, result);
    }
#endif
    return GRIB_SUCCESS;
}

static int pack_string(grib_accessor* a, const char* val, size_t* len)
{
    return grib_concept_apply(a, val);
}

static size_t string_length(grib_accessor* a)
{
    return MAX_CONCEPT_STRING_LENGTH;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval = 0;
    char* aval = 0;
    char* bval = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = grib_value_count(a, &count);
    if (err)
        return err;
    alen = count;

    err = grib_value_count(b, &count);
    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (char*)grib_context_malloc(a->context, alen * sizeof(char));
    bval = (char*)grib_context_malloc(b->context, blen * sizeof(char));

    grib_unpack_string(a, aval, &alen);
    grib_unpack_string(b, bval, &blen);

    retval = GRIB_SUCCESS;
    if (!aval || !bval || grib_inline_strcmp(aval, bval))
        retval = GRIB_STRING_VALUE_MISMATCH;

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}
