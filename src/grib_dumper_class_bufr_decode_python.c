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
   IMPLEMENTS = header;footer
   MEMBERS = long section_offset
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
static void header         (grib_dumper*,grib_handle*);
static void footer         (grib_dumper*,grib_handle*);

typedef struct grib_dumper_bufr_decode_python {
    grib_dumper          dumper;  
    /* Members defined in bufr_decode_python */
    long section_offset;
    long empty;
    long end;
    long isLeaf;
    long isAttribute;
    grib_string_list* keys;
} grib_dumper_bufr_decode_python;


static grib_dumper_class _grib_dumper_class_bufr_decode_python = {
    0,                              /* super                     */
    "bufr_decode_python",                              /* name                      */
    sizeof(grib_dumper_bufr_decode_python),     /* size                      */
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
    &header,                             /* header   */
    &footer,                             /* footer   */
};

grib_dumper_class* grib_dumper_class_bufr_decode_python = &_grib_dumper_class_bufr_decode_python;

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
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    grib_context* c                      = d->context;
    self->section_offset                 = 0;
    self->empty                          = 1;
    d->count                             = 1;
    self->isLeaf                         = 0;
    self->isAttribute                    = 0;
    self->keys                           = (grib_string_list*)grib_context_malloc_clear(c, sizeof(grib_string_list));

    return GRIB_SUCCESS;
}

static int destroy(grib_dumper* d)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
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

static char* dval_to_string(const grib_context* c, double v)
{
    char* sval = (char*)grib_context_malloc_clear(c, sizeof(char) * 40);
    sprintf(sval, "%.18e", v);
    return sval;
}

static void dump_values(grib_dumper* d, grib_accessor* a)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    double value                         = 0;
    size_t size                          = 0;
    int err                              = 0;
    int r                                = 0;
    long count                           = 0;
    char* sval                           = NULL;
    grib_context* c                      = a->context;
    grib_handle* h                       = grib_handle_of_accessor(a);

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a, &count);
    size = count;

    if (size <= 1) {
        err = grib_unpack_double(a, &value, &size);
    }

    self->empty = 0;

    if (size > 1) {
        depth -= 2;
        if ((r = compute_bufr_key_rank(h, self->keys, a->name)) != 0)
            fprintf(self->dumper.out, "    dVals = codes_get_array(ibufr, '#%d#%s')\n", r, a->name);
        else
            fprintf(self->dumper.out, "    dVals = codes_get_array(ibufr, '%s')\n", a->name);
    }
    else {
        r = compute_bufr_key_rank(h, self->keys, a->name);
        if (!grib_is_missing_double(a, value)) {
            sval = dval_to_string(c, value);
            if (r != 0)
                fprintf(self->dumper.out, "    dVal = codes_get(ibufr, '#%d#%s')\n", r, a->name);
            else
                fprintf(self->dumper.out, "    dVal = codes_get(ibufr, '%s')\n", a->name);

            grib_context_free(c, sval);
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
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    double value                         = 0;
    size_t size                          = 0;
    int err                              = 0;
    long count                           = 0;
    char* sval;
    grib_context* c = a->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a, &count);
    size = count;

    if (size <= 1) {
        err = grib_unpack_double(a, &value, &size);
    }

    self->empty = 0;

    if (size > 1) {
        depth -= 2;
        fprintf(self->dumper.out, "    dVals = codes_get_array(ibufr, '%s->%s')\n", prefix, a->name);
    }
    else {
        /* int r=compute_bufr_key_rank(h,self->keys,a->name); */
        if (!grib_is_missing_double(a, value)) {
            sval = dval_to_string(c, value);
            fprintf(self->dumper.out, "    dVal = codes_get(ibufr, '%s->%s')\n", prefix, a->name);

            grib_context_free(c, sval);
        }
    }

    if (self->isLeaf == 0) {
        char* prefix1;

        prefix1 = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + strlen(prefix) + 5));
        sprintf(prefix1, "%s->%s", prefix, a->name);

        dump_attributes(d, a, prefix1);

        grib_context_free(c, prefix1);
        depth -= 2;
    }

    (void)err; /* TODO */
}

static void dump_long(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
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

    self->empty = 0;

    if (size > 1) {
        depth -= 2;
        if ((r = compute_bufr_key_rank(h, self->keys, a->name)) != 0)
            fprintf(self->dumper.out, "    iValues = codes_get_array(ibufr, '#%d#%s')\n", r, a->name);
        else
            fprintf(self->dumper.out, "    iValues = codes_get_array(ibufr, '%s')\n", a->name);
    }
    else {
        r = compute_bufr_key_rank(h, self->keys, a->name);
        if (!grib_is_missing_long(a, value)) {
            if (r != 0)
                fprintf(self->dumper.out, "    iVal = codes_get(ibufr, '#%d#%s')\n", r, a->name);
            else
                fprintf(self->dumper.out, "    iVal = codes_get(ibufr, '%s')\n", a->name);
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
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    long value                           = 0;
    size_t size                          = 0;
    int err                              = 0;
    long count                           = 0;
    grib_context* c                      = a->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a, &count);
    size = count;

    if (size <= 1) {
        err = grib_unpack_long(a, &value, &size);
    }

    self->empty = 0;

    if (size > 1) {
        depth -= 2;
        fprintf(self->dumper.out, "    iVals = codes_get_array(ibufr, '%s->%s')\n", prefix, a->name);
    }
    else {
        if (!grib_is_missing_long(a, value)) {
            fprintf(self->dumper.out, "    iVal = codes_get(ibufr, '%s->%s')\n", prefix, a->name);
        }
    }

    if (self->isLeaf == 0) {
        char* prefix1;

        prefix1 = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name) + strlen(prefix) + 5));
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
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    double value                         = 0;
    size_t size                          = 1;
    int r;
    char* sval      = NULL;
    grib_handle* h  = grib_handle_of_accessor(a);
    grib_context* c = h->context;

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_unpack_double(a, &value, &size);
    self->empty = 0;

    r = compute_bufr_key_rank(h, self->keys, a->name);
    if (!grib_is_missing_double(a, value)) {
        sval = dval_to_string(c, value);
        if (r != 0)
            fprintf(self->dumper.out, "    dVal = codes_get(ibufr, '#%d#%s')\n", r, a->name);
        else
            fprintf(self->dumper.out, "    dVal = codes_get(ibufr, '%s')\n", a->name);

        grib_context_free(c, sval);
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
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
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

    self->empty = 0;

    if (self->isLeaf == 0) {
        if ((r = compute_bufr_key_rank(h, self->keys, a->name)) != 0)
            fprintf(self->dumper.out, "    sVals = codes_get_string_array(ibufr, '#%d#%s')\n", r, a->name);
        else
            fprintf(self->dumper.out, "    sVals = codes_get_string_array(ibufr, '%s')\n", a->name);
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

#define MAX_STRING_SIZE 4096
static void dump_string(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    char value[MAX_STRING_SIZE] = {0,}; /* See ECC-710 */
    char* p                              = NULL;
    size_t size                          = MAX_STRING_SIZE;
    grib_context* c                      = a->context;
    int r = 0, err = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

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
            fprintf(self->dumper.out, "    sVal = codes_get(ibufr, '#%d#%s')\n", r, a->name);
        else
            fprintf(self->dumper.out, "    sVal = codes_get(ibufr, '%s')\n", a->name);
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

    fprintf(f, "    iVals = codes_get_array(ibufr, '%s')\n", key);
}

static void dump_section(grib_dumper* d, grib_accessor* a, grib_block_of_accessors* block)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    if (!grib_inline_strcmp(a->name, "BUFR") ||
        !grib_inline_strcmp(a->name, "GRIB") ||
        !grib_inline_strcmp(a->name, "META")) {
        grib_handle* h = grib_handle_of_accessor(a);
        depth          = 2;
        self->empty    = 1;
        depth += 2;
        _dump_long_array(h, self->dumper.out, "dataPresentIndicator");
        _dump_long_array(h, self->dumper.out, "delayedDescriptorReplicationFactor");
        _dump_long_array(h, self->dumper.out, "shortDelayedDescriptorReplicationFactor");
        _dump_long_array(h, self->dumper.out, "extendedDelayedDescriptorReplicationFactor");
        /* Do not show the inputOverriddenReferenceValues array. That's more for ENCODING */
        /* _dump_long_array(h,self->dumper.out,"inputOverriddenReferenceValues","inputOverriddenReferenceValues"); */
        grib_dump_accessors_block(d, block);
        depth -= 2;
    }
    else if (!grib_inline_strcmp(a->name, "groupNumber")) {
        if ((a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
            return;
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
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    unsigned long flags;
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes[i]) {
        self->isAttribute = 1;
        if ((d->option_flags & GRIB_DUMP_FLAG_ALL_ATTRIBUTES) == 0 && (a->attributes[i]->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0) {
            i++;
            continue;
        }
        self->isLeaf = a->attributes[i]->attributes[0] == NULL ? 1 : 0;
        flags        = a->attributes[i]->flags;
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

static void header(grib_dumper* d, grib_handle* h)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;

    if (d->count < 2) {
        /* This is the first message being processed */
        fprintf(self->dumper.out, "#  This program was automatically generated with bufr_dump -Dpython\n");
        fprintf(self->dumper.out, "#  Using ecCodes version: ");
        grib_print_api_version(self->dumper.out);
        fprintf(self->dumper.out, "\n\n");
        fprintf(self->dumper.out, "from __future__ import print_function\n");
        fprintf(self->dumper.out, "import traceback\n");
        fprintf(self->dumper.out, "import sys\n");
        fprintf(self->dumper.out, "from eccodes import *\n\n\n");
        fprintf(self->dumper.out, "def bufr_decode(input_file):\n");
        fprintf(self->dumper.out, "    f = open(input_file, 'rb')\n");
    }
    fprintf(self->dumper.out, "    # Message number %ld\n    # -----------------\n", d->count);
    fprintf(self->dumper.out, "    print ('Decoding message number %ld')\n", d->count);
    fprintf(self->dumper.out, "    ibufr = codes_bufr_new_from_file(f)\n");
    fprintf(self->dumper.out, "    codes_set(ibufr, 'unpack', 1)\n");
}

static void footer(grib_dumper* d, grib_handle* h)
{
    grib_dumper_bufr_decode_python* self = (grib_dumper_bufr_decode_python*)d;
    fprintf(self->dumper.out, "    codes_release(ibufr)\n");
}
