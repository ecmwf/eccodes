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
#include <cctype>
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

typedef struct grib_dumper_json {
    grib_dumper          dumper;
    /* Members defined in json */
    long section_offset;
    long begin;
    long empty;
    long end;
    long isLeaf;
    long isAttribute;
} grib_dumper_json;


static grib_dumper_class _grib_dumper_class_json = {
    0,                              /* super                     */
    "json",                              /* name                      */
    sizeof(grib_dumper_json),     /* size                      */
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

grib_dumper_class* grib_dumper_class_json = &_grib_dumper_class_json;

/* END_CLASS_IMP */
static void dump_attributes(grib_dumper* d, grib_accessor* a);

static int depth = 0;

static void init_class(grib_dumper_class* c) {}

static int init(grib_dumper* d)
{
    grib_dumper_json* self = (grib_dumper_json*)d;
    self->section_offset   = 0;
    self->empty            = 1;
    self->isLeaf           = 0;
    self->isAttribute      = 0;

    return GRIB_SUCCESS;
}

static int destroy(grib_dumper* d)
{
    return GRIB_SUCCESS;
}

static void dump_values(grib_dumper* d, grib_accessor* a)
{
    grib_dumper_json* self = (grib_dumper_json*)d;
    double value           = 0;
    size_t size = 1, size2 = 0;
    double* values         = NULL;
    int err                = 0;
    int i;
    int cols             = 9;
    long count           = 0;
    double missing_value = GRIB_MISSING_DOUBLE;
    grib_handle* h       = NULL;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    h = grib_handle_of_accessor(a);
    a->value_count(&count);
    size = size2 = count;

    if (size > 1) {
        values = (double*)grib_context_malloc_clear(a->context_, sizeof(double) * size);
        err    = a->unpack_double(values, &size2);
    }
    else {
        err = a->unpack_double(&value, &size2);
    }
    Assert(size2 == size);
    (void)err; /* TODO */

    if (self->begin == 0 && self->empty == 0 && self->isAttribute == 0)
        fprintf(self->dumper.out, ",");
    else
        self->begin = 0;

    self->empty = 0;

    if (self->isLeaf == 0) {
        fprintf(self->dumper.out, "\n%-*s{\n", depth, " ");
        depth += 2;
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "\"key\" : \"%s\",\n", a->name_);
    }

    err = grib_set_double(h, "missingValue", missing_value);
    if (size > 1) {
        int icount = 0;
        if (self->isLeaf == 0) {
            fprintf(self->dumper.out, "%-*s", depth, " ");
            fprintf(self->dumper.out, "\"value\" :\n");
        }
        fprintf(self->dumper.out, "%-*s[", depth, " ");
        depth += 2;
        for (i = 0; i < size - 1; ++i) {
            if (icount > cols || i == 0) {
                fprintf(self->dumper.out, "\n%-*s", depth, " ");
                icount = 0;
            }
            if (values[i] == missing_value)
                fprintf(self->dumper.out, "null, ");
            else
                fprintf(self->dumper.out, "%g, ", values[i]);
            icount++;
        }
        if (icount > cols)
            fprintf(self->dumper.out, "\n%-*s", depth, " ");
        if (grib_is_missing_double(a, values[i]))
            fprintf(self->dumper.out, "%s ", "null");
        else
            fprintf(self->dumper.out, "%g ", values[i]);

        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s]", depth, " ");
        /* if (a->attributes_[0]) fprintf(self->dumper.out,","); */
        grib_context_free(a->context_, values);
    }
    else {
        if (self->isLeaf == 0) {
            fprintf(self->dumper.out, "%-*s", depth, " ");
            fprintf(self->dumper.out, "\"value\" : ");
        }
        if (grib_is_missing_double(a, value))
            fprintf(self->dumper.out, "null");
        else
            fprintf(self->dumper.out, "%g", value);
    }

    if (self->isLeaf == 0) {
        dump_attributes(d, a);
        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s}", depth, " ");
    }

    (void)err; /* TODO */
}

static void dump_long(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_json* self = (grib_dumper_json*)d;
    long value             = 0;
    size_t size = 1, size2 = 0;
    long* values           = NULL;
    int err                = 0;
    int i;
    int cols   = 9;
    long count = 0;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    a->value_count(&count);
    size = size2 = count;

    if (size > 1) {
        values = (long*)grib_context_malloc_clear(a->context_, sizeof(long) * size);
        err    = a->unpack_long(values, &size2);
    }
    else {
        err = a->unpack_long(&value, &size2);
    }
    Assert(size2 == size);

    if (self->begin == 0 && self->empty == 0 && self->isAttribute == 0)
        fprintf(self->dumper.out, ",");
    else
        self->begin = 0;

    self->empty = 0;

    if (self->isLeaf == 0) {
        fprintf(self->dumper.out, "\n%-*s{\n", depth, " ");
        depth += 2;
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "\"key\" : \"%s\",\n", a->name_);
    }

    if (size > 1) {
        int doing_unexpandedDescriptors = 0;
        int icount                      = 0;
        if (self->isLeaf == 0) {
            fprintf(self->dumper.out, "%-*s", depth, " ");
            fprintf(self->dumper.out, "\"value\" :\n");
        }
        fprintf(self->dumper.out, "%-*s[", depth, " ");
        /* See ECC-637: unfortunately json_xs says:
         *  malformed number (leading zero must not be followed by another digit
          if (strcmp(a->name_, "unexpandedDescriptors")==0)
            doing_unexpandedDescriptors = 1;
          */
        depth += 2;
        for (i = 0; i < size - 1; i++) {
            if (icount > cols || i == 0) {
                fprintf(self->dumper.out, "\n%-*s", depth, " ");
                icount = 0;
            }
            if (grib_is_missing_long(a, values[i])) {
                fprintf(self->dumper.out, "null, ");
            }
            else {
                if (doing_unexpandedDescriptors)
                    fprintf(self->dumper.out, "%06ld, ", values[i]);
                else
                    fprintf(self->dumper.out, "%ld, ", values[i]);
            }
            icount++;
        }
        if (icount > cols)
            fprintf(self->dumper.out, "\n%-*s", depth, " ");
        if (doing_unexpandedDescriptors) {
            fprintf(self->dumper.out, "%06ld ", values[i]);
        } else {
            if (grib_is_missing_long(a, values[i]))
                fprintf(self->dumper.out, "%s", "null");
            else
                fprintf(self->dumper.out, "%ld ", values[i]);
        }

        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s]", depth, " ");
        /* if (a->attributes_[0]) fprintf(self->dumper.out,","); */
        grib_context_free(a->context_, values);
    }
    else {
        if (self->isLeaf == 0) {
            fprintf(self->dumper.out, "%-*s", depth, " ");
            fprintf(self->dumper.out, "\"value\" : ");
        }
        if (grib_is_missing_long(a, value))
            fprintf(self->dumper.out, "null");
        else
            fprintf(self->dumper.out, "%ld", value);
        /* if (a->attributes_[0]) fprintf(self->dumper.out,","); */
    }

    if (self->isLeaf == 0) {
        dump_attributes(d, a);
        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s}", depth, " ");
    }
    (void)err; /* TODO */
}

static void dump_bits(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_double(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_json* self = (grib_dumper_json*)d;
    double value           = 0;
    size_t size            = 1;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    a->unpack_double(&value, &size);

    if (self->begin == 0 && self->empty == 0 && self->isAttribute == 0)
        fprintf(self->dumper.out, ",\n");
    else
        self->begin = 0;

    self->empty = 0;

    if (self->isLeaf == 0) {
        fprintf(self->dumper.out, "%-*s{\n", depth, " ");
        depth += 2;
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "\"key\" : \"%s\",\n", a->name_);

        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "\"value\" : ");
    }

    if (grib_is_missing_double(a, value))
        fprintf(self->dumper.out, "null");
    else
        fprintf(self->dumper.out, "%g", value);

    /* if (a->attributes_[0]) fprintf(self->dumper.out,","); */

    if (self->isLeaf == 0) {
        dump_attributes(d, a);
        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s}", depth, " ");
    }
}

static void dump_string_array(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_json* self = (grib_dumper_json*)d;
    char** values          = NULL;
    size_t size = 0, i = 0;
    grib_context* c = NULL;
    int err         = 0;
    int is_missing  = 0;
    long count      = 0;
    c               = a->context_;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    a->value_count(&count);
    size = count;
    if (size == 1) {
        dump_string(d, a, comment);
        return;
    }

    if (self->begin == 0 && self->empty == 0 && self->isAttribute == 0)
        fprintf(self->dumper.out, ",");
    else
        self->begin = 0;

    if (self->isLeaf == 0) {
        fprintf(self->dumper.out, "\n%-*s{\n", depth, " ");
        depth += 2;
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "\"key\" : \"%s\",\n", a->name_);
    }

    self->empty = 0;

    values = (char**)grib_context_malloc_clear(c, size * sizeof(char*));
    if (!values) {
        grib_context_log(c, GRIB_LOG_ERROR, "Memory allocation error: %zu bytes", size);
        return;
    }

    err = a->unpack_string_array(values, &size);

    if (self->isLeaf == 0) {
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "\"value\" : ");
    }
    fprintf(self->dumper.out, "\n%-*s[", depth, " ");
    depth += 2;
    for (i = 0; i < size - 1; i++) {
        is_missing = grib_is_missing_string(a, (unsigned char*)values[i], strlen(values[i]));
        if (is_missing) fprintf(self->dumper.out, "%-*s%s,\n", depth, " ", "null");
        else            fprintf(self->dumper.out, "%-*s\"%s\",\n", depth, " ", values[i]);
    }
    is_missing = grib_is_missing_string(a, (unsigned char*)values[i], strlen(values[i]));
    if (is_missing) fprintf(self->dumper.out, "%-*s%s", depth, " ", "null");
    else            fprintf(self->dumper.out, "%-*s\"%s\"", depth, " ", values[i]);

    depth -= 2;
    fprintf(self->dumper.out, "\n%-*s]", depth, " ");

    /* if (a->attributes_[0]) fprintf(self->dumper.out,","); */

    if (self->isLeaf == 0) {
        dump_attributes(d, a);
        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s}", depth, " ");
    }

    for (i = 0; i < size; i++) {
        grib_context_free(c, values[i]);
    }
    grib_context_free(c, values);
    (void)err; /* TODO */
}

#define MAX_STRING_SIZE 4096
static void dump_string(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_json* self      = (grib_dumper_json*)d;
    char value[MAX_STRING_SIZE] = {0,}; /* See ECC-710 */
    size_t size                 = MAX_STRING_SIZE;
    char* p              = NULL;
    int is_missing       = 0;
    int err              = 0;
    const char* acc_name = a->name_;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0) {
        return;
    }

    /* ECC-710: It is MUCH slower determining the string length here
     * than using a maximum size (and no need for malloc).
     * Specially for BUFR elements */
    /* err = grib_get_string_length_acc(a,&size);
     * if (size==0) return;
     * value=(char*)grib_context_malloc_clear(a->context_,size);
     * if (!value) {
     *   grib_context_log(a->context_,GRIB_LOG_ERROR,"Unable to allocate %zu bytes",size);
     *   return;
     * }
    */

    if (self->begin == 0 && self->empty == 0 && self->isAttribute == 0)
        fprintf(self->dumper.out, ",");
    else
        self->begin = 0;

    self->empty = 0;

    err = a->unpack_string(value, &size);
    if (err) {
        snprintf(value, sizeof(value), " *** ERR=%d (%s) [dump_string on '%s']",
                err, grib_get_error_message(err), a->name_);
    } else {
        Assert(size < MAX_STRING_SIZE);
    }
    p = value;
    if (grib_is_missing_string(a, (unsigned char*)value, size)) {
        is_missing = 1;
    }

    while (*p) {
        if (!isprint(*p))
            *p = '?';
        if (*p == '"')
            *p = '\''; /* ECC-1401 */
        p++;
    }

    if (self->isLeaf == 0) {
        fprintf(self->dumper.out, "\n%-*s{", depth, " ");
        depth += 2;
        fprintf(self->dumper.out, "\n%-*s", depth, " ");
        fprintf(self->dumper.out, "\"key\" : \"%s\",", acc_name);
        fprintf(self->dumper.out, "\n%-*s", depth, " ");
        fprintf(self->dumper.out, "\"value\" : ");
    }
    if (is_missing)
        fprintf(self->dumper.out, "%s", "null");
    else
        fprintf(self->dumper.out, "\"%s\"", value);

    /* if (a->attributes_[0]) fprintf(self->dumper.out,","); */

    if (self->isLeaf == 0) {
        dump_attributes(d, a);
        depth -= 2;
        fprintf(self->dumper.out, "\n%-*s}", depth, " ");
    }

    /* grib_context_free(a->context_,value); */
    (void)err; /* TODO */
}

static void dump_bytes(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_label(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_section(grib_dumper* d, grib_accessor* a, grib_block_of_accessors* block)
{
    grib_dumper_json* self = (grib_dumper_json*)d;
    if (strcmp(a->name_, "BUFR")==0 ||
        strcmp(a->name_, "GRIB")==0 ||
        strcmp(a->name_, "META")==0) {
        depth = 2;
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "[\n");
        self->begin = 1;
        self->empty = 1;
        depth += 2;
        grib_dump_accessors_block(d, block);
        depth -= 2;
        fprintf(self->dumper.out, "\n]\n");
    }
    else if (strcmp(a->name_, "groupNumber")==0) {
        if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
            return;
        if (!self->empty)
            fprintf(self->dumper.out, ",\n");
        fprintf(self->dumper.out, "%-*s", depth, " ");

        fprintf(self->dumper.out, "[");

        fprintf(self->dumper.out, "\n");
        /* fprintf(self->dumper.out,"%-*s",depth," "); */
        self->begin = 1;
        self->empty = 1;
        depth += 2;
        grib_dump_accessors_block(d, block);
        depth -= 2;
        fprintf(self->dumper.out, "\n");
        fprintf(self->dumper.out, "%-*s", depth, " ");
        fprintf(self->dumper.out, "]");
    }
    else {
        grib_dump_accessors_block(d, block);
    }
}

static void dump_attributes(grib_dumper* d, grib_accessor* a)
{
    int i                  = 0;
    grib_dumper_json* self = (grib_dumper_json*)d;
    FILE* out              = self->dumper.out;
    unsigned long flags;
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes_[i]) {
        self->isAttribute = 1;
        if ((d->option_flags & GRIB_DUMP_FLAG_ALL_ATTRIBUTES) == 0 && (a->attributes_[i]->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0) {
            i++;
            continue;
        }
        self->isLeaf = a->attributes_[i]->attributes_[0] == NULL ? 1 : 0;
        fprintf(self->dumper.out, ",");
        fprintf(self->dumper.out, "\n%-*s", depth, " ");
        fprintf(out, "\"%s\" : ", a->attributes_[i]->name_);
        flags = a->attributes_[i]->flags_;
        a->attributes_[i]->flags_ |= GRIB_ACCESSOR_FLAG_DUMP;
        switch (a->attributes_[i]->get_native_type()) {
            case GRIB_TYPE_LONG:
                dump_long(d, a->attributes_[i], 0);
                break;
            case GRIB_TYPE_DOUBLE:
                dump_values(d, a->attributes_[i]);
                break;
            case GRIB_TYPE_STRING:
                dump_string_array(d, a->attributes_[i], 0);
                break;
        }
        a->attributes_[i]->flags_ = flags;
        i++;
    }
    self->isLeaf      = 0;
    self->isAttribute = 0;
}
