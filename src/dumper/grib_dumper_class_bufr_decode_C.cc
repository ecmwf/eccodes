/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_dumper_class_bufr_decode_C.h"
#include "grib_dumper_factory.h"
#include <cctype>

eccodes::dumper::BufrDecodeC _grib_dumper_bufr_decode_c{};
eccodes::Dumper* grib_dumper_bufr_decode_c = &_grib_dumper_bufr_decode_c;

namespace eccodes::dumper {

int BufrDecodeC::init()
{
    section_offset_ = 0;
    empty_          = 1;
    count_          = 1;
    isLeaf_         = 0;
    isAttribute_    = 0;
    keys_           = (grib_string_list*)grib_context_malloc_clear(context_, sizeof(grib_string_list));

    return GRIB_SUCCESS;
}

int BufrDecodeC::destroy()
{
    grib_string_list* next = keys_;
    grib_string_list* cur  = NULL;
    while (next) {
        cur  = next;
        next = next->next;
        grib_context_free(context_, cur->value);
        grib_context_free(context_, cur);
    }
    return GRIB_SUCCESS;
}

static char* dval_to_string(grib_context* c, double v)
{
    char* sval = (char*)grib_context_malloc_clear(c, sizeof(char) * 40);
    snprintf(sval, 1024, "%.18e", v);
    return sval;
}

void BufrDecodeC::dump_values(grib_accessor* a)
{
    double value = 0;
    size_t size  = 0;
    int err      = 0;
    int r        = 0;
    long count   = 0;
    char* sval;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    a->value_count(&count);
    size = count;

    if (size <= 1) {
        err = a->unpack_double(&value, &size);
    }

    empty_ = 0;

    if (size > 1) {
        fprintf(out_, "\n");
        fprintf(out_, "  free(dValues);\n");
        fprintf(out_, "  dValues = (double*)malloc(%lu*sizeof(double));\n", (unsigned long)size);
        fprintf(out_, "  if (!dValues) { fprintf(stderr, \"Failed to allocate memory (dValues).\\n\"); return 1; }\n");
        fprintf(out_, "  size = %lu;\n", (unsigned long)size);

        depth -= 2;

        if ((r = compute_bufr_key_rank(h, keys_, a->name_)) != 0)
            fprintf(out_, "  CODES_CHECK(codes_get_double_array(h, \"#%d#%s\",dValues, &size), 0);\n", r, a->name_);
        else
            fprintf(out_, "  CODES_CHECK(codes_get_double_array(h, \"%s\", dValues, &size), 0);\n", a->name_);
    }
    else {
        r = compute_bufr_key_rank(h, keys_, a->name_);
        if (!grib_is_missing_double(a, value)) {
            sval = dval_to_string(context_, value);
            if (r != 0)
                fprintf(out_, "  CODES_CHECK(codes_get_double(h, \"#%d#%s\", &dVal), 0);\n", r, a->name_);
            else
                fprintf(out_, "  CODES_CHECK(codes_get_double(h, \"%s\", &dVal), 0);\n", a->name_);

            grib_context_free(context_, sval);
        }
    }

    if (isLeaf_ == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(context_, sizeof(char) * (strlen(a->name_) + 10));
            dofree = 1;
            snprintf(prefix, 1024, "#%d#%s", r, a->name_);
        }
        else
            prefix = (char*)a->name_;

        dump_attributes(a, prefix);
        if (dofree)
            grib_context_free(context_, prefix);
        depth -= 2;
    }

    (void)err; /* TODO */
}

void BufrDecodeC::dump_values_attribute(grib_accessor* a, const char* prefix)
{
    double value    = 0;
    size_t size     = 0;
    int err         = 0;
    long count      = 0;
    grib_context* c = a->context_;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    a->value_count(&count);
    size = count;

    if (size <= 1) {
        err = a->unpack_double(&value, &size);
    }

    empty_ = 0;

    if (size > 1) {
        fprintf(out_, "\n");
        fprintf(out_, "  free(dValues);\n");
        fprintf(out_, "  dValues = (double*)malloc(%lu*sizeof(double));\n", (unsigned long)size);
        fprintf(out_, "  if (!dValues) { fprintf(stderr, \"Failed to allocate memory (dValues).\\n\"); return 1; }\n");
        fprintf(out_, "  size = %lu\n;", (unsigned long)size);

        depth -= 2;

        fprintf(out_, "  CODES_CHECK(codes_get_double_array(h, \"%s->%s\", dValues, &size), 0);\n", prefix, a->name_);
    }
    else {
        /* int r=compute_bufr_key_rank(h,keys_,a->name_); */
        if (!grib_is_missing_double(a, value)) {
            char* sval = dval_to_string(c, value);
            fprintf(out_, "  CODES_CHECK(codes_get_double(h, \"%s->%s\", &dVal), 0);\n", prefix, a->name_);

            grib_context_free(c, sval);
        }
    }

    if (isLeaf_ == 0) {
        char* prefix1;

        prefix1 = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + strlen(prefix) + 5));
        snprintf(prefix1, 1024, "%s->%s", prefix, a->name_);

        dump_attributes(a, prefix1);

        grib_context_free(c, prefix1);
        depth -= 2;
    }

    (void)err; /* TODO */
}

void BufrDecodeC::dump_long(grib_accessor* a, const char* comment)
{
    long value      = 0;
    size_t size     = 0;
    int err         = 0;
    int r           = 0;
    long count      = 0;
    grib_context* c = a->context_;
    grib_handle* h  = grib_handle_of_accessor(a);

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    a->value_count(&count);
    size = count;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0) {
        if (isLeaf_ == 0) {
            char* prefix;
            int dofree = 0;

            r = compute_bufr_key_rank(h, keys_, a->name_);
            if (r != 0) {
                prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + 10));
                dofree = 1;
                snprintf(prefix, 1024, "#%d#%s", r, a->name_);
            }
            else
                prefix = (char*)a->name_;

            dump_attributes(a, prefix);
            if (dofree)
                grib_context_free(c, prefix);
            depth -= 2;
        }
        return;
    }

    if (size <= 1) {
        err = a->unpack_long(&value, &size);
    }

    empty_ = 0;

    if (size > 1) {
        fprintf(out_, "\n");
        fprintf(out_, "  free(iValues);\n");
        fprintf(out_, "  iValues = (long*)malloc(%lu*sizeof(long));\n", (unsigned long)size);
        fprintf(out_, "  if (!iValues) { fprintf(stderr, \"Failed to allocate memory (iValues).\\n\"); return 1; }\n");
        fprintf(out_, "  size = %lu;\n", (unsigned long)size);

        depth -= 2;

        if ((r = compute_bufr_key_rank(h, keys_, a->name_)) != 0)
            fprintf(out_, "  CODES_CHECK(codes_get_long_array(h, \"#%d#%s\", iValues, &size), 0);\n", r, a->name_);
        else
            fprintf(out_, "  CODES_CHECK(codes_get_long_array(h, \"%s\", iValues, &size), 0);\n", a->name_);
    }
    else {
        r = compute_bufr_key_rank(h, keys_, a->name_);
        if (!codes_bufr_key_exclude_from_dump(a->name_)) {
            if (!grib_is_missing_long(a, value)) {
                if (r != 0)
                    fprintf(out_, "  CODES_CHECK(codes_get_long(h, \"#%d#%s\", &iVal), 0);\n", r, a->name_);
                else
                    fprintf(out_, "  CODES_CHECK(codes_get_long(h, \"%s\", &iVal), 0);\n", a->name_);
            }
        }
    }

    if (isLeaf_ == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + 10));
            dofree = 1;
            snprintf(prefix, 1024, "#%d#%s", r, a->name_);
        }
        else
            prefix = (char*)a->name_;

        dump_attributes(a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }
    (void)err; /* TODO */
}

void BufrDecodeC::dump_long_attribute(grib_accessor* a, const char* prefix)
{
    long value      = 0;
    size_t size     = 0;
    int err         = 0;
    long count      = 0;
    grib_context* c = a->context_;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    a->value_count(&count);
    size = count;

    if (size <= 1) {
        err = a->unpack_long(&value, &size);
    }

    empty_ = 0;

    if (size > 1) {
        fprintf(out_, "\n");
        fprintf(out_, "  free(iValues);\n");
        fprintf(out_, "  iValues = (long*)malloc(%lu*sizeof(long));\n", (unsigned long)size);
        fprintf(out_, "  if (!iValues) { fprintf(stderr, \"Failed to allocate memory (iValues).\\n\"); return 1; }\n");
        fprintf(out_, "  size = %lu;\n", (unsigned long)size);

        depth -= 2;

        fprintf(out_, "  CODES_CHECK(codes_get_long_array(h, \"%s->%s\", iValues, &size), 0);\n", prefix, a->name_);
    }
    else {
        /* int r=compute_bufr_key_rank(h,keys_,a->name_); */
        if (!codes_bufr_key_exclude_from_dump(prefix)) {
            if (!grib_is_missing_long(a, value)) {
                fprintf(out_, "  CODES_CHECK(codes_get_long(h, \"%s->%s\", &iVal), 0);\n", prefix, a->name_);
            }
        }
    }

    if (isLeaf_ == 0) {
        char* prefix1;

        prefix1 = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + strlen(prefix) + 5));
        snprintf(prefix1, 1024, "%s->%s", prefix, a->name_);

        dump_attributes(a, prefix1);

        grib_context_free(c, prefix1);
        depth -= 2;
    }
    (void)err; /* TODO */
}

void BufrDecodeC::dump_bits(grib_accessor* a, const char* comment)
{
}

void BufrDecodeC::dump_double(grib_accessor* a, const char* comment)
{
    double value = 0;
    size_t size  = 1;
    int r;
    char* sval;
    grib_handle* h  = grib_handle_of_accessor(a);
    grib_context* c = h->context;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    a->unpack_double(&value, &size);
    empty_ = 0;

    r = compute_bufr_key_rank(h, keys_, a->name_);
    if (!grib_is_missing_double(a, value)) {
        sval = dval_to_string(c, value);
        if (r != 0)
            fprintf(out_, "  CODES_CHECK(codes_get_double(h, \"#%d#%s\", &dVal), 0);\n", r, a->name_);
        else
            fprintf(out_, "  CODES_CHECK(codes_get_double(h, \"%s\", &dVal), 0);\n", a->name_);

        grib_context_free(c, sval);
    }

    if (isLeaf_ == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + 10));
            dofree = 1;
            snprintf(prefix, 1024, "#%d#%s", r, a->name_);
        }
        else
            prefix = (char*)a->name_;

        dump_attributes(a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }
}

void BufrDecodeC::dump_string_array(grib_accessor* a, const char* comment)
{
    char** values;
    size_t size = 0, i = 0;
    grib_context* c = NULL;
    int err         = 0;
    long count      = 0;
    int r           = 0;
    grib_handle* h  = grib_handle_of_accessor(a);

    c = a->context_;

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    a->value_count(&count);
    size = count;
    if (size == 1) {
        dump_string(a, comment);
        return;
    }

    fprintf(out_, "\n");
    fprintf(out_, "  free(sValues);\n");
    fprintf(out_, "  sValues = (char**)malloc(%lu * sizeof(char*));\n", (unsigned long)size);
    fprintf(out_, "  if (!sValues) { fprintf(stderr, \"Failed to allocate memory (sValues).\\n\"); return 1; }\n");
    fprintf(out_, "  size = %lu;\n", (unsigned long)size);

    empty_ = 0;
    values = (char**)grib_context_malloc_clear(c, size * sizeof(char*));
    if (!values) {
        grib_context_log(c, GRIB_LOG_ERROR, "unable to allocate %zu bytes", size);
        return;
    }

    err = a->unpack_string_array(values, &size);

    if (isLeaf_ == 0) {
        if ((r = compute_bufr_key_rank(h, keys_, a->name_)) != 0)
            fprintf(out_, "  codes_get_string_array(h, \"#%d#%s\", sValues, &size);\n", r, a->name_);
        else
            fprintf(out_, "  codes_get_string_array(h, \"%s\", sValues, &size);\n", a->name_);
    }

    if (isLeaf_ == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + 10));
            dofree = 1;
            snprintf(prefix, 1024, "#%d#%s", r, a->name_);
        }
        else
            prefix = (char*)a->name_;

        dump_attributes(a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }

    for (i = 0; i < size; i++)
        grib_context_free(c, values[i]);
    grib_context_free(c, values);
    (void)err; /* TODO */
}

#define MAX_STRING_SIZE 4096
void BufrDecodeC::dump_string(grib_accessor* a, const char* comment)
{
    char value[MAX_STRING_SIZE] = {0, }; /* See ECC-710 */
    size_t size     = MAX_STRING_SIZE;
    char* p         = NULL;
    grib_context* c = a->context_;
    int r = 0, err = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags_ & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    empty_ = 0;

    err = a->unpack_string(value, &size);
    p   = value;
    r   = compute_bufr_key_rank(h, keys_, a->name_);
    if (grib_is_missing_string(a, (unsigned char*)value, size)) {
        return;
    }

    while (*p) {
        if (!isprint(*p))
            *p = '?';
        p++;
    }

    fprintf(out_, "  size = 1024;\n"); /* TODO */
    if (isLeaf_ == 0) {
        depth += 2;
        if (r != 0)
            fprintf(out_, "  CODES_CHECK(codes_get_string(h, \"#%d#%s\", sVal, &size), 0);\n", r, a->name_);
        else
            fprintf(out_, "  CODES_CHECK(codes_get_string(h, \"%s\", sVal, &size), 0);\n", a->name_);
    }

    if (isLeaf_ == 0) {
        char* prefix;
        int dofree = 0;

        if (r != 0) {
            prefix = (char*)grib_context_malloc_clear(c, sizeof(char) * (strlen(a->name_) + 10));
            dofree = 1;
            snprintf(prefix, 1024, "#%d#%s", r, a->name_);
        }
        else
            prefix = (char*)a->name_;

        dump_attributes(a, prefix);
        if (dofree)
            grib_context_free(c, prefix);
        depth -= 2;
    }

    (void)err; /* TODO */
}

void BufrDecodeC::dump_bytes(grib_accessor* a, const char* comment)
{
}

void BufrDecodeC::dump_label(grib_accessor* a, const char* comment)
{
}

static void _dump_long_array(grib_handle* h, FILE* f, const char* key)
{
    size_t size = 0;
    if (grib_get_size(h, key, &size) == GRIB_NOT_FOUND)
        return;

    fprintf(f, "  free(iValues);\n");
    fprintf(f, "  iValues = (long*)malloc(%lu*sizeof(long));\n", (unsigned long)size);
    fprintf(f, "  if (!iValues) { fprintf(stderr, \"Failed to allocate memory (iValues).\\n\"); return 1; }\n");
    fprintf(f, "  size = %lu;", (unsigned long)size);

    fprintf(f, "  CODES_CHECK(codes_get_long_array(h, \"%s\", iValues, &size), 0);\n", key);
}

void BufrDecodeC::dump_section(grib_accessor* a, grib_block_of_accessors* block)
{
    if (strcmp(a->name_, "BUFR") == 0 ||
        strcmp(a->name_, "GRIB") == 0 ||
        strcmp(a->name_, "META") == 0) {
        grib_handle* h = grib_handle_of_accessor(a);
        depth          = 2;
        empty_         = 1;
        depth += 2;
        _dump_long_array(h, out_, "dataPresentIndicator");
        _dump_long_array(h, out_, "delayedDescriptorReplicationFactor");
        _dump_long_array(h, out_, "shortDelayedDescriptorReplicationFactor");
        _dump_long_array(h, out_, "extendedDelayedDescriptorReplicationFactor");
        /* Do not show the inputOverriddenReferenceValues array. That's more for ENCODING */
        /*_dump_long_array(h,out_,"inputOverriddenReferenceValues","inputOverriddenReferenceValues");*/
        grib_dump_accessors_block(this, block);
        depth -= 2;
    }
    else if (strcmp(a->name_, "groupNumber") == 0) {
        if ((a->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0)
            return;
        empty_ = 1;
        depth += 2;
        grib_dump_accessors_block(this, block);
        depth -= 2;
    }
    else {
        grib_dump_accessors_block(this, block);
    }
}

void BufrDecodeC::dump_attributes(grib_accessor* a, const char* prefix)
{
    int i                           = 0;
    unsigned long flags;
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes_[i]) {
        isAttribute_ = 1;
        if ((option_flags_ & GRIB_DUMP_FLAG_ALL_ATTRIBUTES) == 0 && (a->attributes_[i]->flags_ & GRIB_ACCESSOR_FLAG_DUMP) == 0) {
            i++;
            continue;
        }
        isLeaf_ = a->attributes_[i]->attributes_[0] == NULL ? 1 : 0;
        flags   = a->attributes_[i]->flags_;
        a->attributes_[i]->flags_ |= GRIB_ACCESSOR_FLAG_DUMP;
        switch (a->attributes_[i]->get_native_type()) {
            case GRIB_TYPE_LONG:
                dump_long_attribute(a->attributes_[i], prefix);
                break;
            case GRIB_TYPE_DOUBLE:
                dump_values_attribute( a->attributes_[i], prefix);
                break;
            case GRIB_TYPE_STRING:
                break;
        }
        a->attributes_[i]->flags_ = flags;
        i++;
    }
    isLeaf_      = 0;
    isAttribute_ = 0;
}

void BufrDecodeC::header(const grib_handle* h)
{
    Assert(h->product_kind == PRODUCT_BUFR);

    if (count_ < 2) {
        /* This is the first message being processed */
        fprintf(out_, "/* This program was automatically generated with bufr_dump -DC */\n");
        fprintf(out_, "/* Using ecCodes version: ");
        grib_print_api_version(out_);
        fprintf(out_, " */\n\n");
        fprintf(out_, "#include \"eccodes.h\"\n");
        fprintf(out_, "int main(int argc, char* argv[])\n");
        fprintf(out_, "{\n");
        fprintf(out_, "  size_t         size = 0;\n");
        fprintf(out_, "  int            err = 0;\n");
        fprintf(out_, "  FILE*          fin = NULL;\n");
        fprintf(out_, "  codes_handle*  h = NULL;\n");
        fprintf(out_, "  long           iVal = 0;\n");
        fprintf(out_, "  double         dVal = 0.0;\n");
        fprintf(out_, "  char           sVal[1024] = {0,};\n");
        fprintf(out_, "  long*          iValues = NULL;\n");
        fprintf(out_, "  char**         sValues = NULL;\n");
        fprintf(out_, "  double*        dValues = NULL;\n");
        fprintf(out_, "  const char*    infile_name = NULL;\n\n");

        fprintf(out_, "  if (argc != 2) {\n");
        fprintf(out_, "    fprintf(stderr, \"Usage: %%s BUFR_file\\n\", argv[0]);\n");
        fprintf(out_, "    return 1;\n");
        fprintf(out_, "  }\n");
        fprintf(out_, "  infile_name = argv[1];\n");
        fprintf(out_, "  fin = fopen(infile_name, \"r\");\n");
        fprintf(out_, "  if (!fin) {\n");
        fprintf(out_, "    fprintf(stderr,\"ERROR: Unable to open input BUFR file %%s\\n\", infile_name);\n");
        fprintf(out_, "    return 1;\n");
        fprintf(out_, "  }\n\n");
    }

    fprintf(out_, "  h = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);\n");
    fprintf(out_, "  if (h == NULL) {\n");
    fprintf(out_, "    fprintf(stderr, \"ERROR: cannot create BUFR handle\\n\");\n");
    fprintf(out_, "    return 1;\n");
    fprintf(out_, "  }\n");
    fprintf(out_, "  CODES_CHECK(codes_set_long(h, \"unpack\", 1),0);\n\n");
}

void BufrDecodeC::footer(const grib_handle* h)
{

    /*fprintf(out_,"  fout = fopen(\"outfile.bufr\", \"w\");");*/
    /*fprintf(out_,"  if (fclose(fout)) {\n");
    fprintf(out_,"    fprintf(stderr, \"Failed to close file handle.\\n\");\n");
    fprintf(out_,"    return 1;\n");
    fprintf(out_,"  }\n");
    */
    fprintf(out_, "\n");
    fprintf(out_, "  codes_handle_delete(h);\n");
    fprintf(out_, "  free(iValues); iValues = NULL;\n");
    fprintf(out_, "  free(dValues); dValues = NULL;\n");
    fprintf(out_, "  free(sValues); sValues = NULL;\n\n");
}

}  // namespace eccodes::dumper
