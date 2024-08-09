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
   CLASS      = expression
   IMPLEMENTS = destroy
   IMPLEMENTS = native_type
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = char *name
   MEMBERS    = grib_arguments *args
   END_CLASS_DEF

 */
/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "expression.class" and rerun ./make_class.pl

*/

typedef const char* string; /* to keep make_class.pl happy */

static void    destroy(grib_context*,grib_expression* e);
static void    print(grib_context*,grib_expression*,grib_handle*,FILE*);
static void    add_dependency(grib_expression* e, grib_accessor* observer);
static int     native_type(grib_expression*,grib_handle*);
static int     evaluate_long(grib_expression*,grib_handle*,long*);

typedef struct grib_expression_functor{
  grib_expression base;
    /* Members defined in functor */
    char *name;
    grib_arguments *args;
} grib_expression_functor;


static grib_expression_class _grib_expression_class_functor = {
    0,                    /* super                     */
    "functor",                    /* name                      */
    sizeof(grib_expression_functor),/* size of instance        */
    0,                           /* inited */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,
    &add_dependency,
    &native_type,
    0,
    &evaluate_long,
    0,
    0,
};

grib_expression_class* grib_expression_class_functor = &_grib_expression_class_functor;

/* END_CLASS_IMP */


#ifdef ECCODES_ON_WINDOWS
// Windows does not have strcasestr
static char* strcasestr(const char *haystack, const char* needle)
{
    char c, sc;
    size_t len = 0;

    if ((c = *needle++) != 0) {
        c = tolower((unsigned char)c);
        len = strlen(needle);
        do {
            do {
                if ((sc = *haystack++) == 0)
                    return (NULL);
            } while ((char)tolower((unsigned char)sc) != c);
        } while (_strnicmp(haystack, needle, len) != 0);
        haystack--;
    }
    return ((char *)haystack);
}
#endif

static int evaluate_long(grib_expression* g, grib_handle* h, long* lres)
{
    grib_expression_functor* e = (grib_expression_functor*)g;

    // if (STR_EQUAL(e->name, "lookup")) {
    //     return GRIB_SUCCESS;
    // }

    if (STR_EQUAL(e->name, "new")) {
        *lres = h->loader != NULL;
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "abs")) {
        grib_expression* exp = grib_arguments_get_expression(h, e->args, 0);
        long lval = 0;
        int ret = grib_expression_evaluate_long(h, exp, &lval);
        *lres = abs(lval);
        return ret;
    }

    if (STR_EQUAL(e->name, "size")) {
        *lres = 0;
        const char* keyName = grib_arguments_get_name(h, e->args, 0);
        if (keyName) {
            size_t size = 0;
            int err = grib_get_size(h, keyName, &size);
            if (err) return err;
            *lres = (long)size;
            return GRIB_SUCCESS;
        }
        return GRIB_INVALID_ARGUMENT;
    }

    if (STR_EQUAL(e->name, "debug_mode")) {
        const int n = grib_arguments_get_count(e->args);
        if (n != 1) return GRIB_INVALID_ARGUMENT;
        const int dmode = grib_arguments_get_long(h, e->args, 0);
        grib_context_set_debug(0, dmode);
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "missing")) {
        const char* keyName = grib_arguments_get_name(h, e->args, 0);
        if (keyName) {
            long val = 0;
            int err  = 0;
            if (h->product_kind == PRODUCT_BUFR) {
                int ismiss = grib_is_missing(h, keyName, &err);
                if (err) return err;
                *lres = ismiss;
                return GRIB_SUCCESS;
            }
            err = grib_get_long_internal(h, keyName, &val);
            if (err) return err;
            // Note: This does not cope with keys like typeOfSecondFixedSurface
            // which are codetable entries with values like 255: this value is
            // not classed as 'missing'!
            // (See ECC-594)
            *lres = (val == GRIB_MISSING_LONG);
            return GRIB_SUCCESS;
        }
        else {
            // No arguments means return the actual integer missing value
            *lres = GRIB_MISSING_LONG;
        }
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "defined")) {
        const char* keyName = grib_arguments_get_name(h, e->args, 0);
        if (keyName) {
            const grib_accessor* a = grib_find_accessor(h, keyName);
            *lres = a != NULL ? 1 : 0;
            return GRIB_SUCCESS;
        }
        *lres = 0;
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "environment_variable")) {
        // ECC-1520: This implementation has some limitations:
        // 1. Cannot distinguish between environment variable NOT SET
        //    and SET but equal to 0
        // 2. Cannot deal with string values
        const char* p = grib_arguments_get_name(h, e->args, 0);
        if (p) {
            const char* env = getenv(p);
            if (env) {
                long lval = 0;
                if (string_to_long(env, &lval, 1) == GRIB_SUCCESS) {
                    *lres = lval;
                    return GRIB_SUCCESS;
                }
            }
        }
        *lres = 0;
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "changed")) {
        *lres = 1;
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "contains")) {
        *lres = 0;
        const int n = grib_arguments_get_count(e->args);
        if (n != 3) return GRIB_INVALID_ARGUMENT;
        const char* keyName = grib_arguments_get_name(h, e->args, 0);
        if (!keyName) return GRIB_INVALID_ARGUMENT;
        int type = 0;
        int err = grib_get_native_type(h, keyName, &type);
        if (err) return err;
        if (type == GRIB_TYPE_STRING) {
            char keyValue[254] = {0,};
            size_t len = sizeof(keyValue);
            err = grib_get_string(h, keyName, keyValue, &len);
            if (err) return err;
            const char* sValue = grib_arguments_get_string(h, e->args, 1);
            const bool case_sens = grib_arguments_get_long(h, e->args, 2) != 0;
            const bool contains = case_sens? strcasestr(keyValue, sValue) : strstr(keyValue, sValue);
            if (sValue && contains) {
                *lres = 1;
                return GRIB_SUCCESS;
            }
        } else {
            // For now only keys of type string supported
            return GRIB_INVALID_ARGUMENT;
        }
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "is_one_of")) {
        *lres = 0;
        const char* keyName = grib_arguments_get_name(h, e->args, 0);
        if (!keyName) return GRIB_INVALID_ARGUMENT;
        int type = 0;
        int err = grib_get_native_type(h, keyName, &type);
        if (err) return err;
        int n = grib_arguments_get_count(e->args);
        if (type == GRIB_TYPE_STRING) {
            char keyValue[254] = {0,};
            size_t len = sizeof(keyValue);
            err = grib_get_string(h, keyName, keyValue, &len);
            if (err) return err;
            for (int i = 1; i < n; ++i) { // skip 1st argument which is the key
                const char* sValue = grib_arguments_get_string(h, e->args, i);
                if (sValue && STR_EQUAL(keyValue, sValue)) {
                    *lres = 1;
                    return GRIB_SUCCESS;
                }
            }
        }
        else if (type == GRIB_TYPE_LONG) {
            long keyValue = 0;
            err = grib_get_long(h, keyName, &keyValue);
            if (err) return err;
            for (int i = 1; i < n; ++i) { // skip 1st argument which is the key
                long lValue = grib_arguments_get_long(h, e->args, i);
                if (keyValue == lValue) {
                    *lres = 1;
                    return GRIB_SUCCESS;
                }
            }
        }
        else if (type == GRIB_TYPE_DOUBLE) {
            return GRIB_NOT_IMPLEMENTED;
        }
        return GRIB_SUCCESS;
    }

    if (STR_EQUAL(e->name, "gribex_mode_on")) {
        *lres = h->context->gribex_mode_on ? 1 : 0;
        return GRIB_SUCCESS;
    }

    grib_context_log(h->context, GRIB_LOG_ERROR, "grib_expression_class_functor::%s failed for '%s'", __func__, e->name);
    return GRIB_NOT_IMPLEMENTED;
}

static void print(grib_context* c, grib_expression* g, grib_handle* f, FILE* out)
{
    const grib_expression_functor* e = (grib_expression_functor*)g;
    fprintf(out, "%s(", e->name);
    // grib_expression_print(c,e->args,f);
    fprintf(out, ")");
}

static void destroy(grib_context* c, grib_expression* g)
{
    grib_expression_functor* e = (grib_expression_functor*)g;
    grib_context_free_persistent(c, e->name);
    grib_arguments_free(c, e->args);
}

static void add_dependency(grib_expression* g, grib_accessor* observer)
{
    grib_expression_functor* e = (grib_expression_functor*)g;
    if (strcmp(e->name, "defined"))
        grib_dependency_observe_arguments(observer, e->args);
}

grib_expression* new_func_expression(grib_context* c, const char* name, grib_arguments* args)
{
    grib_expression_functor* e = (grib_expression_functor*)grib_context_malloc_clear_persistent(c, sizeof(grib_expression_functor));
    e->base.cclass             = grib_expression_class_functor;
    e->name                    = grib_context_strdup_persistent(c, name);
    e->args                    = args;
    return (grib_expression*)e;
}

static int native_type(grib_expression* g, grib_handle* h)
{
    return GRIB_TYPE_LONG;
}
