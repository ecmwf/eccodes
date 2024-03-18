
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
#include "grib_accessor_class_md5.h"
#include "md5.h"

grib_accessor_class_md5_t _grib_accessor_class_md5{"md5"};
grib_accessor_class* grib_accessor_class_md5 = &_grib_accessor_class_md5;


void grib_accessor_class_md5_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_md5_t* self   = (grib_accessor_md5_t*)a;
    char* b                   = 0;
    int n                     = 0;
    grib_string_list* current = 0;
    grib_context* context     = a->context;

    self->offset    = grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++);
    self->length    = grib_arguments_get_expression(grib_handle_of_accessor(a), arg, n++);
    self->blocklist = NULL;
    while ((b = (char*)grib_arguments_get_name(grib_handle_of_accessor(a), arg, n++)) != NULL) {
        if (!self->blocklist) {
            self->blocklist        = (grib_string_list*)grib_context_malloc_clear(context, sizeof(grib_string_list));
            self->blocklist->value = grib_context_strdup(context, b);
            current                = self->blocklist;
        }
        else {
            Assert(current);
            if (current) {
                current->next        = (grib_string_list*)grib_context_malloc_clear(context, sizeof(grib_string_list));
                current->next->value = grib_context_strdup(context, b);
                current              = current->next;
            }
        }
    }
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

int grib_accessor_class_md5_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_STRING;
}

int grib_accessor_class_md5_t::compare(grib_accessor* a, grib_accessor* b){
    int retval = GRIB_SUCCESS;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = a->value_count(&count);    if (err)
        return err;
    alen = count;

    err = b->value_count(&count);    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

int grib_accessor_class_md5_t::unpack_string(grib_accessor* a, char* v, size_t* len){
    grib_accessor_md5_t* self = (grib_accessor_md5_t*)a;
    unsigned mess_len;
    unsigned char* mess;
    unsigned char* p;
    long offset = 0, length = 0;
    grib_string_list* blocklist = NULL;
    int ret = GRIB_SUCCESS;
    long i = 0;
    struct grib_md5_state md5c;

    if (*len < 32) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %d bytes long (len=%zu)",
                         cclass_name, a->name, 32, *len);
        *len = 32;
        return GRIB_BUFFER_TOO_SMALL;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->offset, &offset)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_expression_evaluate_long(grib_handle_of_accessor(a), self->length, &length)) != GRIB_SUCCESS)
        return ret;
    mess = (unsigned char*)grib_context_malloc(a->context, length);
    memcpy(mess, grib_handle_of_accessor(a)->buffer->data + offset, length);
    mess_len = length;

    blocklist = a->context->blocklist;
    /* passed blocklist overrides context blocklist.
     Consider to modify following line to extend context blocklist.
     */
    if (self->blocklist)
        blocklist = self->blocklist;
    while (blocklist && blocklist->value) {
        const grib_accessor* b = grib_find_accessor(grib_handle_of_accessor(a), blocklist->value);
        if (!b) {
            grib_context_free(a->context, mess);
            return GRIB_NOT_FOUND;
        }

        p = mess + b->offset - offset;
        for (i = 0; i < b->length; i++)
            *(p++) = 0;

        blocklist = blocklist->next;
    }

    grib_md5_init(&md5c);
    grib_md5_add(&md5c, mess, mess_len);
    grib_md5_end(&md5c, v);
    grib_context_free(a->context, mess);
    *len = strlen(v) + 1;

    return ret;
}

void grib_accessor_class_md5_t::destroy(grib_context* c, grib_accessor* a){
    grib_accessor_md5_t* self = (grib_accessor_md5_t*)a;
    if (self->blocklist) {
        grib_string_list* next = self->blocklist;
        grib_string_list* cur  = NULL;
        while (next) {
            cur  = next;
            next = next->next;
            grib_context_free(c, cur->value);
            grib_context_free(c, cur);
        }
    }
}

int grib_accessor_class_md5_t::value_count(grib_accessor* a, long* count){
    *count = 1; /* ECC-1475 */
    return 0;
}
