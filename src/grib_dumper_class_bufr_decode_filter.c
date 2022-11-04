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
#include <ctype.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = dumper
   IMPLEMENTS = dump_long;dump_bits
   IMPLEMENTS = dump_double;dump_string;dump_string_array
   IMPLEMENTS = dump_bytes;dump_values
   IMPLEMENTS = dump_label;dump_section
   IMPLEMENTS = init;destroy
   MEMBERS = long section_offset
   MEMBERS = long begin
   MEMBERS = long empty
   MEMBERS = long end
   MEMBERS = long isLeaf
   MEMBERS = long isAttribute
   MEMBERS = grib_string_list* keys
   END_CLASS_DEF

 */


/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "dumper.class" and rerun ./make_class.pl

*/

static void init_class      (grib_dumper_class*);
static int init            (grib_dumper* d);
static int destroy         (grib_dumper*);
static void dump_long       (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_bits       (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_double     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_string     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_string_array     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_bytes      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_values     (grib_dumper* d, grib_accessor* a);
static void dump_label      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_section    (grib_dumper* d, grib_accessor* a,grib_block_of_accessors* block);

typedef struct grib_dumper_bufr_decode_filter {
    grib_dumper          dumper;  
    /* Members defined in bufr_decode_filter */
    long section_offset;
    long begin;
    long empty;
    long end;
    long isLeaf;
    long isAttribute;
    grib_string_list* keys;
} grib_dumper_bufr_decode_filter;


static grib_dumper_class _grib_dumper_class_bufr_decode_filter = {
    0,                              /* super                     */
    "bufr_decode_filter",                              /* name                      */
    sizeof(grib_dumper_bufr_decode_filter),     /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    &init,                               /* init                      */
    &destroy,                            /* free mem                       */
    &dump_long,                          /* dump long         */
    &dump_double,                        /* dump double    */
    &dump_string,                        /* dump string    */
    &dump_string_array,                        /* dump string array   */
    &dump_label,                         /* dump labels  */
    &dump_bytes,                         /* dump bytes  */
    &dump_bits,                          /* dump bits   */
    &dump_section,                       /* dump section      */
    &dump_values,                        /* dump values   */
    0,                             /* header   */
    0,                             /* footer   */
};

grib_dumper_class* grib_dumper_class_bufr_decode_filter = &_grib_dumper_class_bufr_decode_filter;

/* END_CLASS_IMP */
static void dump_attributes(grib_dumper* d, grib_accessor* a, const char* prefix);

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

static int depth = 0;

static void init_class(grib_dumper_class* c) {}

static int init(grib_dumper* d)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    grib_context* c                      = d->context;
    self->section_offset                 = 0;
    self->empty                          = 1;
    self->isLeaf                         = 0;
    self->isAttribute                    = 0;
    self->keys                           = (grib_string_list*)grib_context_malloc_clear(c, sizeof(grib_string_list));

    return GRIB_SUCCESS;
}

static int destroy(grib_dumper* d)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    grib_string_list* next               = self->keys;
    grib_string_list* cur                = NULL;
    grib_context* c                      = d->context;
    while (next) {
        cur  = next;
        next = next->next;
        grib_context_free(c, cur->value);
        grib_context_free(c, cur);
    }
    return GRIB_SUCCESS;
}

static void dump_values(grib_dumper* d, grib_accessor* a)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    double value                         = 0;
    size_t size                          = 0;
    int err                              = 0;
    int r;
    long count      = 0;
    grib_context* c = a->context;
    grib_handle* h  = grib_handle_of_accessor(a);

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a, &count);
    size = count;

    if (size <= 1) {
        err = grib_unpack_double(a, &value, &size);
    }

    self->begin = 0;
    self->empty = 0;

    if (size > 1) {
        if ((r = compute_bufr_key_rank(h, self->keys, a->name)) != 0)
            fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
        else
            fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
    }
    else {
        r = compute_bufr_key_rank(h, self->keys, a->name);
        if (!grib_is_missing_double(a, value)) {
            if (r != 0)
                fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
            else
                fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
        }
    }

    if (self->isLeaf == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + 10));
            dofree = 1;
            sprintf(prefix, "#%d#%s", r, a->name);
        }
        else
            prefix = (char*)a->name;

        dump_attributes(d, a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }

    (void)err; /* TODO */
}

static void dump_values_attribute(grib_dumper* d, grib_accessor* a, const char* prefix)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    double value                         = 0;
    size_t size                          = 0;
    int err                              = 0;
    long count                           = 0;
    grib_context* c                      = a->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a, &count);
    size = count;

    if (size <= 1) {
        err = grib_unpack_double(a, &value, &size);
    }

    self->empty = 0;

    if (size > 1) {
        fprintf(self->dumper.out, "print \"%s->%s = [%s->%s]\";\n", prefix, a->name, prefix, a->name);
    }
    else {
        if (!grib_is_missing_double(a, value)) {
            fprintf(self->dumper.out, "print \"%s->%s = [%s->%s]\";\n", prefix, a->name, prefix, a->name);
        }
    }

    if (self->isLeaf == 0) {
        char* prefix1 = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + strlen(prefix) + 5));
        sprintf(prefix1, "%s->%s", prefix, a->name);

        dump_attributes(d, a, prefix1);

        grib_context_free(c, prefix1);
        depth -= 2;
    }

    (void)err; /* TODO */
}

static void dump_long(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    long value                           = 0;
    size_t size                          = 0;
    int err                              = 0;
    int r                                = 0;
    long count                           = 0;
    grib_context* c                      = a->context;
    grib_handle* h                       = grib_handle_of_accessor(a);

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    grib_value_count(a, &count);
    size = count;

    if ((a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0) {
        if (self->isLeaf == 0) {
            char* prefix;
            int dofree = 0;

            r = compute_bufr_key_rank(h, self->keys, a->name);
            if (r != 0) {
                prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + 10));
                dofree = 1;
                sprintf(prefix, "#%d#%s", r, a->name);
            }
            else
                prefix = (char*)a->name;

            dump_attributes(d, a, prefix);
            if (dofree)
                grib_context_free(c, prefix);
            depth -= 2;
        }
        return;
    }

    if (size <= 1) {
        err = grib_unpack_long(a, &value, &size);
    }

    self->begin = 0;
    self->empty = 0;

    if (size > 1) {
        if ((r = compute_bufr_key_rank(h, self->keys, a->name)) != 0)
            fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
        else
            fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
    }
    else {
        r = compute_bufr_key_rank(h, self->keys, a->name);
        if (!grib_is_missing_long(a, value)) {
            if (r != 0)
                fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
            else
                fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
        }
    }

    if (self->isLeaf == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + 10));
            dofree = 1;
            sprintf(prefix, "#%d#%s", r, a->name);
        }
        else
            prefix = (char*)a->name;

        dump_attributes(d, a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }
    (void)err; /* TODO */
}

static void dump_long_attribute(grib_dumper* d, grib_accessor* a, const char* prefix)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    int err                              = 0;
    grib_context* c                      = a->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    self->empty = 0;
    fprintf(self->dumper.out, "print \"%s->%s = [%s->%s]\";\n", prefix, a->name, prefix, a->name);

    if (self->isLeaf == 0) {
        char* prefix1 = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + strlen(prefix) + 5));
        sprintf(prefix1, "%s->%s", prefix, a->name);

        dump_attributes(d, a, prefix1);

        grib_context_free(c, prefix1);
        depth -= 2;
    }
    (void)err; /* TODO */
}

static void dump_bits(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_double(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    double value                         = 0;
    size_t size                          = 1;
    int r;
    grib_handle* h  = grib_handle_of_accessor(a);
    grib_context* c = h->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_unpack_double(a, &value, &size);
    self->begin = 0;
    self->empty = 0;

    r = compute_bufr_key_rank(h, self->keys, a->name);
    if (!grib_is_missing_double(a, value)) {
        if (r != 0)
            fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
        else
            fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
    }

    if (self->isLeaf == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + 10));
            dofree = 1;
            sprintf(prefix, "#%d#%s", r, a->name);
        }
        else
            prefix = (char*)a->name;

        dump_attributes(d, a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }
}

static void dump_string_array(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    size_t size                          = 0;
    grib_context* c                      = NULL;
    int err                              = 0;
    long count                           = 0;
    int r                                = 0;
    grib_handle* h                       = grib_handle_of_accessor(a);

    c = a->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a, &count);
    size = count;
    if (size == 1) {
        dump_string(d, a, comment);
        return;
    }

    self->begin = 0;

    if (self->isLeaf == 0) {
        depth += 2;
        if ((r = compute_bufr_key_rank(h, self->keys, a->name)) != 0)
            fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
        else
            fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
    }

    self->empty = 0;

    if (self->isLeaf == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + 10));
            dofree = 1;
            sprintf(prefix, "#%d#%s", r, a->name);
        }
        else
            prefix = (char*)a->name;

        dump_attributes(d, a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }

    (void)err; /* TODO */
}

#define MAX_STRING_SIZE 4096
static void dump_string(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    char value[MAX_STRING_SIZE] = {0,}; /* See ECC-710 */
    size_t size                 = MAX_STRING_SIZE;
    char* p                     = NULL;
    grib_context* c             = a->context;
    int r =0, err = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    self->begin = 0;

    self->empty = 0;

    err = grib_unpack_string(a, value, &size);
    p   = value;
    r   = compute_bufr_key_rank(h, self->keys, a->name);
    if (grib_is_missing_string(a, (unsigned char*)value, size)) {
        return;
    }

    while (*p) {
        if (!isprint(*p))
            *p = '.';
        p++;
    }

    if (self->isLeaf == 0) {
        depth += 2;
        if (r != 0)
            fprintf(self->dumper.out, "print \"#%d#%s=[#%d#%s]\";\n", r, a->name, r, a->name);
        else
            fprintf(self->dumper.out, "print \"%s=[%s]\";\n", a->name, a->name);
    }

    if (self->isLeaf == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + 10));
            dofree = 1;
            sprintf(prefix, "#%d#%s", r, a->name);
        }
        else
            prefix = (char*)a->name;

        dump_attributes(d, a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }

    (void)err; /* TODO */
}

static void dump_bytes(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_label(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void _dump_long_array(grib_handle* h, FILE* f, const char* key)
{
    size_t size = 0;
    if (grib_get_size(h, key, &size) == GRIB_NOT_FOUND)
        return;
    if (size == 0)
        return;

    fprintf(f, "print \"%s=[%s]\";\n", key, key);
}

static void dump_section(grib_dumper* d, grib_accessor* a, grib_block_of_accessors* block)
{
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    if (!grib_inline_strcmp(a->name, "BUFR") ||
        !grib_inline_strcmp(a->name, "GRIB") ||
        !grib_inline_strcmp(a->name, "META")) {
        grib_handle* h = grib_handle_of_accessor(a);
        depth          = 2;
        self->begin    = 1;
        self->empty    = 1;
        depth += 2;
        _dump_long_array(h, self->dumper.out, "dataPresentIndicator");
        _dump_long_array(h, self->dumper.out, "delayedDescriptorReplicationFactor");
        _dump_long_array(h, self->dumper.out, "shortDelayedDescriptorReplicationFactor");
        _dump_long_array(h, self->dumper.out, "extendedDelayedDescriptorReplicationFactor");
        /* Do not show the inputOverriddenReferenceValues array. That's more for ENCODING */
        /*_dump_long_array(h,self->dumper.out,"inputOverriddenReferenceValues","inputOverriddenReferenceValues");*/
        grib_dump_accessors_block(d, block);
        depth -= 2;
    }
    else if (!grib_inline_strcmp(a->name, "groupNumber")) {
        if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
            return;
        self->begin = 1;
        self->empty = 1;
        depth += 2;
        grib_dump_accessors_block(d, block);
        depth -= 2;
    }
    else {
        grib_dump_accessors_block(d, block);
    }
}

static void dump_attributes(grib_dumper* d, grib_accessor* a, const char* prefix)
{
    int i                                = 0;
    grib_dumper_bufr_decode_filter* self = (grib_dumper_bufr_decode_filter*)d;
    unsigned long flags;
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes[i]) {
        self->isAttribute = 1;
        if ((d->option_flags & GRIB_DUMP_FLAG_ALL_ATTRIBUTES) == 0 && (a->attributes[i]->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0) {
            i++;
            continue;
        }
        self->isLeaf = a->attributes[i]->attributes[0] == NULL ? 1 : 0;
        /* fprintf(self->dumper.out,","); */
        /* fprintf(self->dumper.out,"\n%-*s",depth," "); */
        /* fprintf(out,"\"%s\" : ",a->attributes[i]->name); */
        flags = a->attributes[i]->flags;
        a->attributes[i]->flags |= GRIB_ACCESSOR_FLAG_DUMP;
        switch (grib_accessor_get_native_type(a->attributes[i])) {
            case GRIB_TYPE_LONG:
                dump_long_attribute(d, a->attributes[i], prefix);
                break;
            case GRIB_TYPE_DOUBLE:
                dump_values_attribute(d, a->attributes[i], prefix);
                break;
            case GRIB_TYPE_STRING:
                break;
        }
        a->attributes[i]->flags = flags;
        i++;
    }
    self->isLeaf      = 0;
    self->isAttribute = 0;
}
