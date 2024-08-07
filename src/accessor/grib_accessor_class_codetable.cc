/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_codetable.h"
#include <cctype>


grib_accessor_class_codetable_t _grib_accessor_class_codetable("codetable");
grib_accessor_class* grib_accessor_class_codetable = &_grib_accessor_class_codetable;


#if GRIB_PTHREADS
static pthread_once_t once    = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

static void init_mutex()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex1, &attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex1;

static void init_mutex()
{
    GRIB_OMP_CRITICAL(lock_grib_accessor_class_codetable_c)
    {
        if (once == 0) {
            omp_init_nest_lock(&mutex1);
            once = 1;
        }
    }
}
#endif

static int grib_load_codetable(grib_context* c, const char* filename, const char* recomposed_name, size_t size, grib_codetable* t);

void grib_accessor_class_codetable_t::init(grib_accessor* a, const long len, grib_arguments* params)
{
    grib_accessor_class_unsigned_t::init(a, len, params);

    int n                         = 0;
    long new_len                  = len;
    grib_handle* hand             = grib_handle_of_accessor(a);
    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    grib_action* act              = (grib_action*)(a->creator);
    DEBUG_ASSERT(len == self->nbytes);

    if (new_len == 0) {
        /* ECC-485: When the codetable length is 0, it means we are passing
         * its length as an identifier not an integer. This identifier is
         * added to the argument list (at the beginning)
         */
        new_len = grib_arguments_get_long(hand, params, n++);
        if (new_len <= 0) {
            grib_context_log(a->context, GRIB_LOG_FATAL, "%s: codetable length must be a positive integer", a->name);
        }
        self->nbytes = new_len;
    }

    self->tablename = grib_arguments_get_string(hand, params, n++);
    if (self->tablename == NULL) {
        grib_context_log(a->context, GRIB_LOG_FATAL, "%s: codetable table is invalid", a->name);
    }
    self->masterDir = grib_arguments_get_name(hand, params, n++); /* can be NULL */
    self->localDir  = grib_arguments_get_name(hand, params, n++); /* can be NULL */

    /*if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
    printf("-------- %s type string (%ld)\n",a->name,a->flags);*/
#ifdef DEBUG
    if (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
        grib_context_log(a->context, GRIB_LOG_FATAL, "codetable '%s' has flag can_be_missing!", a->name);
        Assert(!"codetable with can_be_missing?");
    }
#endif

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        a->length = 0;
        if (!a->vvalue)
            a->vvalue = (grib_virtual_value*)grib_context_malloc_clear(a->context, sizeof(grib_virtual_value));
        a->vvalue->type   = a->get_native_type();
        a->vvalue->length = new_len;
        if (act->default_value != NULL) {
            const char* p = 0;
            size_t s_len  = 1;
            long l;
            int ret = 0;
            double d;
            char tmp[1024];
            grib_expression* expression = grib_arguments_get_expression(hand, act->default_value, 0);
            int type                    = grib_expression_native_type(hand, expression);
            switch (type) {
                case GRIB_TYPE_DOUBLE:
                    grib_expression_evaluate_double(hand, expression, &d);
                    a->pack_double(&d, &s_len);
                    break;

                case GRIB_TYPE_LONG:
                    grib_expression_evaluate_long(grib_handle_of_accessor(a), expression, &l);
                    a->pack_long(&l, &s_len);
                    break;

                default:
                    s_len = sizeof(tmp);
                    p     = grib_expression_evaluate_string(grib_handle_of_accessor(a), expression, tmp, &s_len, &ret);
                    if (ret != GRIB_SUCCESS) {
                        grib_context_log(a->context, GRIB_LOG_FATAL,
                                         "Unable to evaluate %s as string", a->name);
                    }
                    s_len = strlen(p) + 1;
                    pack_string(a, p, &s_len);
                    break;
            }
        }
    }
    else {
        a->length = new_len;
    }
}

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

// Cater for parameters being NULL
static int str_eq(const char* a, const char* b)
{
    if (a && b && (grib_inline_strcmp(a, b) == 0))
        return 1;
    return 0;
}

#ifdef DEBUGGING
static void dump_codetable(grib_codetable* atable)
{
    grib_codetable* next = NULL;
    int count            = 0;

    next = atable;
    while (next) {
        printf("[%.2d] CodeTable Dump: f0=%s f1=%s\n", count, next->filename[0], next->filename[1]);
        count++;
        next = next->next;
    }
}
#endif
static grib_codetable* load_table(grib_accessor* a)
{
    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    size_t size           = 0;
    grib_handle* h        = ((grib_accessor*)self)->parent->h;
    grib_context* c       = h->context;
    grib_codetable* t     = NULL;
    grib_codetable* next  = NULL;
    char* filename        = 0;
    char recomposed[1024] = {0,};
    char localRecomposed[1024] = {0,};
    char* localFilename  = 0;
    char masterDir[1024] = {0,};
    char localDir[1024] = {0,};
    size_t len = 1024;

    if (self->masterDir != NULL)
        grib_get_string(h, self->masterDir, masterDir, &len);

    len = 1024;
    if (self->localDir != NULL)
        grib_get_string(h, self->localDir, localDir, &len);

    if (*masterDir != 0) {
        char name[2048] = {0,};
        snprintf(name, sizeof(name), "%s/%s", masterDir, self->tablename);
        grib_recompose_name(h, NULL, name, recomposed, 0);
        filename = grib_context_full_defs_path(c, recomposed);
    }
    else {
        grib_recompose_name(h, NULL, self->tablename, recomposed, 0);
        filename = grib_context_full_defs_path(c, recomposed);
    }

    if (*localDir != 0) {
        char localName[2048] = {0,};
        snprintf(localName, sizeof(localName), "%s/%s", localDir, self->tablename);
        grib_recompose_name(h, NULL, localName, localRecomposed, 0);
        localFilename = grib_context_full_defs_path(c, localRecomposed);
    }

    GRIB_MUTEX_INIT_ONCE(&once, &init_mutex);
    GRIB_MUTEX_LOCK(&mutex1); /* GRIB-930 */

    /*printf("DBG %s: Look in cache: f=%s lf=%s (recomposed=%s)\n", self->att.name, filename, localFilename,recomposed);*/
    if (filename == NULL && localFilename == NULL) {
        t = NULL;
        goto the_end;
    }
    next = c->codetable;
    while (next) {
        if ((filename && next->filename[0] && grib_inline_strcmp(filename, next->filename[0]) == 0) &&
            ((localFilename == 0 && next->filename[1] == NULL) ||
             ((localFilename != 0 && next->filename[1] != NULL) && grib_inline_strcmp(localFilename, next->filename[1]) == 0))) {
            t = next;
            goto the_end;
        }
        /* Special case: see GRIB-735 */
        if (filename == NULL && localFilename != NULL) {
            if (str_eq(localFilename, next->filename[0]) ||
                str_eq(localFilename, next->filename[1])) {
                t = next;
                goto the_end;
            }
        }
        next = next->next;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        Assert(a->vvalue != NULL);
        size = a->vvalue->length * 8;
    }
    else {
        size = ((grib_accessor*)self)->byte_count() * 8;
    }

    size = (1ULL << size); /* 2^size - 64bits */

    t = (grib_codetable*)grib_context_malloc_clear_persistent(c, sizeof(grib_codetable) +
                                                                     (size - 1) * sizeof(code_table_entry));

    if (filename != 0)
        grib_load_codetable(c, filename, recomposed, size, t);

    if (localFilename != 0)
        grib_load_codetable(c, localFilename, localRecomposed, size, t);

    /*dump_codetable(c->codetable);*/

    if (t->filename[0] == NULL && t->filename[1] == NULL) {
        grib_context_free_persistent(c, t);
        t = NULL;
        goto the_end;
    }

the_end:
    GRIB_MUTEX_UNLOCK(&mutex1);

    return t;
}

static int grib_load_codetable(grib_context* c, const char* filename,
                               const char* recomposed_name, size_t size, grib_codetable* t)
{
    char line[1024];
    FILE* f        = NULL;
    int lineNumber = 0;
    grib_context_log(c, GRIB_LOG_DEBUG, "Loading code table from %s", filename);

    f = codes_fopen(filename, "r");
    if (!f)
        return GRIB_IO_PROBLEM;

    Assert(t != NULL);

    if (t->filename[0] == NULL) {
        t->filename[0]        = grib_context_strdup_persistent(c, filename);
        t->recomposed_name[0] = grib_context_strdup_persistent(c, recomposed_name);
        t->next               = c->codetable;
        t->size               = size;
        c->codetable          = t;
    }
    else {
        t->filename[1]        = grib_context_strdup_persistent(c, filename);
        t->recomposed_name[1] = grib_context_strdup_persistent(c, recomposed_name);
    }

    while (fgets(line, sizeof(line) - 1, f)) {
        char* p                 = line;
        int code                = 0;
        char abbreviation[1024] = {0,};
        char title[1024] = {0,};
        char* pAbbrev         = abbreviation;
        char* pTitle          = title;
        char* units           = 0;
        char unknown[]        = "unknown";
        char* last_open_paren = NULL;
        char* last_clos_paren = NULL;

        ++lineNumber;

        line[strlen(line) - 1] = 0;

        while (*p != '\0' && isspace(*p))
            p++;

        if (*p == '#')
            continue;

        last_open_paren = strrchr(line, '(');

        while (*p != '\0' && isspace(*p))
            p++;

        if (*p == '\0')
            continue;

        if (!isdigit(*p)) {
            grib_context_log(c, GRIB_LOG_ERROR, "Invalid entry in file %s: line %d", filename, lineNumber);
            continue; /* skip this line */
        }
        Assert(isdigit(*p));

        while (*p != '\0') {
            if (isspace(*p))
                break;
            code *= 10;
            code += *p - '0';
            p++;
        }

        if (code < 0 || code >= size) {
            grib_context_log(c, GRIB_LOG_WARNING, "code_table_entry: invalid code in %s: %d (table size=%ld)", filename, code, size);
            continue;
        }

        while (*p != '\0' && isspace(*p))
            p++;

        while (*p != '\0') {
            if (isspace(*p))
                break;
            *pAbbrev++ = *p++;
        }
        *pAbbrev = 0;
        while (*p != '\0' && isspace(*p))
            p++;

        /* The title goes as far as the last open paren */
        while (*p != '\0') {
            if (last_open_paren && p >= last_open_paren && *p == '(')
                break;
            *pTitle++ = *p++;
        }
        *pTitle = 0;

        /* units at the end */
        if (last_open_paren) {
            last_clos_paren = strrchr(line, ')');
            if (last_clos_paren && last_open_paren != last_clos_paren) {
                units = last_open_paren + 1;
                p     = units;
                p += (last_clos_paren - last_open_paren - 1);
                *p = '\0';
            }
        }
        if (!units)
            units = unknown;

        Assert(*abbreviation);
        Assert(*title);
        string_rtrim(title); /* ECC-1315 */

        if (t->entries[code].abbreviation != NULL) {
            grib_context_log(c, GRIB_LOG_WARNING, "code_table_entry: duplicate code in %s: %d (table size=%ld)", filename, code, size);
            continue;
        }

        Assert(t->entries[code].abbreviation == NULL);
        Assert(t->entries[code].title == NULL);

        t->entries[code].abbreviation = grib_context_strdup_persistent(c, abbreviation);
        t->entries[code].title        = grib_context_strdup_persistent(c, title);
        t->entries[code].units        = grib_context_strdup_persistent(c, units);
    }

    fclose(f);

    return 0;
}

void grib_codetable_delete(grib_context* c)
{
    grib_codetable* t = c->codetable;

    while (t) {
        grib_codetable* s = t->next;
        int i;

        for (i = 0; i < t->size; i++) {
            grib_context_free_persistent(c, t->entries[i].abbreviation);
            grib_context_free_persistent(c, t->entries[i].title);
            grib_context_free_persistent(c, t->entries[i].units);
        }
        grib_context_free_persistent(c, t->filename[0]);
        if (t->filename[1])
            grib_context_free_persistent(c, t->filename[1]);
        grib_context_free_persistent(c, t->recomposed_name[0]);
        if (t->recomposed_name[1])
            grib_context_free_persistent(c, t->recomposed_name[1]);
        grib_context_free_persistent(c, t);
        t = s;
    }
}

int codes_codetable_get_contents_malloc(const grib_handle* h, const char* key, code_table_entry** entries, size_t* num_entries)
{
    long lvalue = 0;
    size_t size = 1;
    int err = 0;
    grib_context* c = h->context;

    grib_accessor* aa = grib_find_accessor(h, key);
    if (!aa) return GRIB_NOT_FOUND;

    if (!STR_EQUAL(aa->cclass->name, "codetable")) {
        return GRIB_INVALID_ARGUMENT; // key is not a codetable
    }

    const grib_accessor_codetable_t* ca = (const grib_accessor_codetable_t*)aa; // could be dynamic_cast

    // Decode the key itself. This will either fetch it from the cache or place it there
    if ((err = aa->unpack_long(&lvalue, &size)) != GRIB_SUCCESS) {
        return err;
    }

    const grib_codetable* table = ca->table;
    if (!table) return GRIB_INTERNAL_ERROR;

    grib_codetable* cached_table = c->codetable; // Access the codetable cache
    while (cached_table) {
        if (STR_EQUAL(table->recomposed_name[0], cached_table->recomposed_name[0])) {
            // Found a cache entry that matches the recomposed name of ours
            *num_entries = cached_table->size;
            *entries = (code_table_entry*)calloc(cached_table->size, sizeof(code_table_entry));
            if (!*entries) {
                return GRIB_OUT_OF_MEMORY;
            }
            for (size_t i = 0; i < cached_table->size; i++) {
                (*entries)[i] = cached_table->entries[i];
            }
            return GRIB_SUCCESS;
        }
        cached_table = cached_table->next;
    }

    return GRIB_CODE_NOT_FOUND_IN_TABLE;
}

int codes_codetable_check_code_figure(const grib_handle* h, const char* key, long code_figure)
{
    code_table_entry* entries = NULL;
    size_t num_entries = 0;
    int err = 0;
    err = codes_codetable_get_contents_malloc(h, key, &entries, &num_entries);
    if (err) return err;

    if (code_figure < 0 || (size_t)code_figure >= num_entries) {
        err = GRIB_OUT_OF_RANGE;
        goto cleanup;
    }

    if (entries[code_figure].abbreviation == NULL) {
        err = GRIB_INVALID_KEY_VALUE;
        goto cleanup;
    }
cleanup:
    free(entries);
    return err;
}

int codes_codetable_check_abbreviation(const grib_handle* h, const char* key, const char* abbreviation)
{
    code_table_entry* entries = NULL;
    size_t num_entries = 0;
    int err = 0;
    err = codes_codetable_get_contents_malloc(h, key, &entries, &num_entries);
    if (err) return err;

    bool found = false;
    for (size_t i=0; i<num_entries; ++i) {
        const char* abbrev = entries[i].abbreviation;
        if (abbrev && STR_EQUAL(abbrev, abbreviation)) {
            found = true;
            break;
        }
    }
    if (!found) err = GRIB_INVALID_KEY_VALUE;

    free(entries);
    return err;
}

void grib_accessor_class_codetable_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    char comment[2048];
    grib_codetable* table;

    size_t llen = 1;
    long value;

    if (!self->table_loaded) {
        self->table        = load_table(a); /* may return NULL */
        self->table_loaded = 1;
    }
    table = self->table;

    a->unpack_long(&value, &llen);

    if (value == GRIB_MISSING_LONG) {
        if (a->length < 4) {
            value = (1L << a->length) - 1;
        }
    }

    if (table && value >= 0 && value < table->size) {
        if (table->entries[value].abbreviation) {
            long b = atol(table->entries[value].abbreviation);
            if (b == value)
                strcpy(comment, table->entries[value].title);
            else
                snprintf(comment, sizeof(comment), "%s", table->entries[value].title);

            if (table->entries[value].units != NULL && grib_inline_strcmp(table->entries[value].units, "unknown")) {
                strcat(comment, " (");
                strcat(comment, table->entries[value].units);
                strcat(comment, ") ");
            }
        }
        else {
            strcpy(comment, "Unknown code table entry");
        }
    }
    else {
        strcpy(comment, "Unknown code table entry");
    }

    strcat(comment, " (");
    if (table) {
        strcat(comment, table->recomposed_name[0]);
        if (table->recomposed_name[1] != NULL) {
            strcat(comment, " , ");
            strcat(comment, table->recomposed_name[1]);
        }
    }
    strcat(comment, ") ");

    grib_dump_long(dumper, a, comment);
}

int grib_accessor_class_codetable_t::unpack_string(grib_accessor* a, char* buffer, size_t* len)
{
    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    grib_codetable* table         = NULL;

    size_t size = 1;
    long value;
    int err = GRIB_SUCCESS;
    char tmp[1024];
    size_t l = 0;

    if ((err = a->unpack_long(&value, &size)) != GRIB_SUCCESS)
        return err;

    if (!self->table_loaded) {
        self->table        = load_table(a); /* may return NULL */
        self->table_loaded = 1;
    }
    table = self->table;

    if (table && (value >= 0) && (value < table->size) && table->entries[value].abbreviation) {
        strcpy(tmp, table->entries[value].abbreviation);
    }
    else {
        snprintf(tmp, sizeof(tmp), "%d", (int)value);
    }

    l = strlen(tmp) + 1;

    if (*len < l) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(buffer, tmp);
    *len = l;

    return GRIB_SUCCESS;
}

int grib_accessor_class_codetable_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

// Return true if the input is an integer (non-negative)
bool is_number(const char* s)
{
    while (*s) {
        if (!isdigit(*s))
            return false;
        s++;
    }
    return true;
}

bool strings_equal(const char* s1, const char* s2, bool case_sensitive)
{
    if (case_sensitive) return (strcmp(s1, s2) == 0);
    return (strcmp_nocase(s1, s2) == 0);
}

int grib_accessor_class_codetable_t::pack_string(grib_accessor* a, const char* buffer, size_t* len)
{
    long lValue = 0;
    Assert(buffer);
    if (is_number(buffer) && string_to_long(buffer, &lValue, 1) == GRIB_SUCCESS) {
        // ECC-1654: If value is a pure number, just pack as long
        size_t l = 1;
        return a->pack_long(&lValue, &l);
    }

    if (STR_EQUAL_NOCASE(buffer, "missing")) {
        return pack_missing(a);
    }

    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    grib_codetable* table = NULL;
    long i = 0;
    size_t size = 1;

    if (!self->table_loaded) {
        self->table        = load_table(a); /* may return NULL */
        self->table_loaded = 1;
    }
    table = self->table;

    if (!table)
        return GRIB_ENCODING_ERROR;

    if (a->set) {
        int err = grib_set_string(grib_handle_of_accessor(a), a->set, buffer, len);
        if (err != 0)
            return err;
    }

    // If the key has the "lowercase" flag set, then the string comparison
    // should ignore the case
    bool case_sensitive = true;
    if (a->flags & GRIB_ACCESSOR_FLAG_LOWERCASE) case_sensitive = false;

    for (i = 0; i < table->size; i++) {
        if (table->entries[i].abbreviation) {
            if (strings_equal(table->entries[i].abbreviation, buffer, case_sensitive)) {
                return a->pack_long(&i, &size);
            }
        }
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_NO_FAIL) {
        grib_action* act = (grib_action*)(a->creator);
        if (act->default_value != NULL) {
            const char* p = 0;
            size_t s_len  = 1;
            long l = 0;
            int ret = 0;
            double d = 0;
            char tmp[1024] = {0,};
            grib_expression* expression = grib_arguments_get_expression(grib_handle_of_accessor(a), act->default_value, 0);
            int type                    = grib_expression_native_type(grib_handle_of_accessor(a), expression);
            switch (type) {
                case GRIB_TYPE_DOUBLE:
                    grib_expression_evaluate_double(grib_handle_of_accessor(a), expression, &d);
                    a->pack_double(&d, &s_len);
                    break;

                case GRIB_TYPE_LONG:
                    grib_expression_evaluate_long(grib_handle_of_accessor(a), expression, &l);
                    a->pack_long(&l, &s_len);
                    break;

                default:
                    s_len = sizeof(tmp);
                    p     = grib_expression_evaluate_string(grib_handle_of_accessor(a), expression, tmp, &s_len, &ret);
                    if (ret != GRIB_SUCCESS) {
                        grib_context_log(a->context, GRIB_LOG_ERROR,
                                         "%s: Unable to evaluate default value of %s as string expression", __func__, a->name);
                        return ret;
                    }
                    s_len = strlen(p) + 1;
                    pack_string(a, p, &s_len);
                    break;
            }
            return GRIB_SUCCESS;
        }
    }

    // ECC-1652: Failed. Now do a case-insensitive compare to give the user a hint
    for (i = 0; i < table->size; i++) {
        if (table->entries[i].abbreviation) {
            if (strcmp_nocase(table->entries[i].abbreviation, buffer) == 0) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "%s: No such code table entry: '%s' "
                                 "(Did you mean '%s'?)",
                                 a->name, buffer, table->entries[i].abbreviation);
            }
        }
    }

    return GRIB_ENCODING_ERROR;
}

int grib_accessor_class_codetable_t::pack_expression(grib_accessor* a, grib_expression* e)
{
    const char* cval  = NULL;
    int ret           = 0;
    long lval         = 0;
    size_t len        = 1;
    grib_handle* hand = grib_handle_of_accessor(a);

    if (strcmp(e->cclass->name, "long") == 0) {
        grib_expression_evaluate_long(hand, e, &lval); /* TODO: check return value */
        //if (hand->context->debug) printf("ECCODES DEBUG grib_accessor_class_codetable::pack_expression %s %ld\n", a->name,lval);
        ret = a->pack_long(&lval, &len);
    }
    else {
        char tmp[1024];
        len  = sizeof(tmp);
        cval = grib_expression_evaluate_string(hand, e, tmp, &len, &ret);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                "grib_accessor_codetable.%s: Unable to evaluate string %s to be set in %s",
                __func__, grib_expression_get_name(e), a->name);
            return ret;
        }
        len = strlen(cval) + 1;
        //if (hand->context->debug)
        //    printf("ECCODES DEBUG grib_accessor_class_codetable::pack_expression %s %s\n", a->name, cval);
        ret = a->pack_string(cval, &len);
    }
    return ret;
}

void grib_accessor_class_codetable_t::destroy(grib_context* context, grib_accessor* a)
{
    if (a->vvalue != NULL) {
        grib_context_free(context, a->vvalue);
        a->vvalue = NULL;
    }
    grib_accessor_class_unsigned_t::destroy(context, a);
}

int grib_accessor_class_codetable_t::get_native_type(grib_accessor* a)
{
    int type = GRIB_TYPE_LONG;
    /*printf("---------- %s flags=%ld GRIB_ACCESSOR_FLAG_STRING_TYPE=%d\n",
         a->name,a->flags,GRIB_ACCESSOR_FLAG_STRING_TYPE);*/
    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type = GRIB_TYPE_STRING;
    return type;
}

int grib_accessor_class_codetable_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    long rlen = 0, i = 0;
    long pos = a->offset * 8;
    grib_handle* hand = NULL;

#ifdef DEBUG
    {
        int err = a->value_count(&rlen);
        Assert(!err);
        Assert(rlen == 1);
    }
#endif
    rlen = 1; /* ECC-480 Performance: avoid func call overhead of grib_value_count */

    if (!self->table_loaded) {
        self->table        = load_table(a); /* may return NULL */
        self->table_loaded = 1;
    }

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size (%lu) for %s, it contains %ld values",
                *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        *val = a->vvalue->lval;
        *len = 1;
        return GRIB_SUCCESS;
    }

    /* ECC-480 Performance: inline the grib_handle_of_accessor here to reduce func call overhead */
    if (a->parent == NULL)
        hand = a->h;
    else
        hand = a->parent->h;

    for (i = 0; i < rlen; i++) {
        val[i] = (long)grib_decode_unsigned_long(hand->buffer->data, &pos, self->nbytes * 8);
    }

    *len = rlen;
    return GRIB_SUCCESS;
}

int grib_accessor_class_codetable_t::pack_missing(grib_accessor* a)
{
    // Many of the code tables do have a 'Missing' entry (all bits = 1)
    // So it is more user-friendly to allow setting codetable keys to
    // missing. For tables that do not have such an entry, an error is issued
    grib_accessor_codetable_t* self = (grib_accessor_codetable_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);

    const long nbytes = a->length;
    const long nbits = nbytes*8;
    const long maxVal = (1<<nbits) - 1;

    int err = codes_codetable_check_code_figure(h, a->name, maxVal);
    if (!err) {
        size_t l = 1;
        return a->pack_long(&maxVal, &l);
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "There is no 'missing' entry in Code Table %s (%s)",
            self->tablename, grib_get_error_message(err));

    return err;
}
