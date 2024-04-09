
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
#include "grib_accessor_class_hash_array.h"

grib_accessor_class_hash_array_t _grib_accessor_class_hash_array{"hash_array"};
grib_accessor_class* grib_accessor_class_hash_array = &_grib_accessor_class_hash_array;


#define MAX_HASH_ARRAY_STRING_LENGTH 255

void grib_accessor_class_hash_array_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_gen_t::init(a, len, args);
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    a->length                      = 0;
    self->key                      = 0;
    self->ha                       = NULL;
}

void grib_accessor_class_hash_array_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_string(dumper, a, NULL);
}

int grib_accessor_class_hash_array_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    char s[200]                    = {0,};
    snprintf(s, sizeof(s), "%g", *val);
    self->key = grib_context_strdup(a->context, s);
    self->ha  = 0;
    return GRIB_SUCCESS;
}

int grib_accessor_class_hash_array_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    char s[200]                    = {0,};
    snprintf(s, sizeof(s), "%ld", *val);
    if (self->key)
        grib_context_free(a->context, self->key);
    self->key = grib_context_strdup(a->context, s);
    self->ha  = 0;
    return GRIB_SUCCESS;
}

int grib_accessor_class_hash_array_t::pack_string(grib_accessor* a, const char* v, size_t* len){
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    self->key                      = grib_context_strdup(a->context, v);
    self->ha                       = 0;
    return GRIB_SUCCESS;
}

int grib_accessor_class_hash_array_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

static grib_hash_array_value* find_hash_value(grib_accessor* a, int* err)
{
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    grib_hash_array_value* ha_ret  = 0;
    grib_hash_array_value* ha      = NULL;

    ha = get_hash_array(grib_handle_of_accessor(a), a->creator);
    if (!ha) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to get hash value for %s", a->creator->name);
        *err = GRIB_HASH_ARRAY_NO_MATCH;
        return NULL;
    }

    *err = GRIB_SUCCESS;

    Assert(ha != NULL);
    if (!self->key) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "unable to get hash value for %s, set before getting", a->creator->name);
        *err = GRIB_HASH_ARRAY_NO_MATCH;
        return NULL;
    }

    ha_ret = (grib_hash_array_value*)grib_trie_get(ha->index, self->key);
    if (!ha_ret)
        ha_ret = (grib_hash_array_value*)grib_trie_get(ha->index, "default");

    if (!ha_ret) {
        *err = GRIB_HASH_ARRAY_NO_MATCH;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "hash_array: no match for %s=%s",
                         a->creator->name, self->key);
        const char* full_path = get_hash_array_full_path(a->creator);
        if (full_path) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "hash_array: file path = %s", full_path);
        }
        grib_context_log(a->context, GRIB_LOG_ERROR, "Hint: Check the key 'masterTablesVersionNumber'");
        return NULL;
    }
    return ha_ret;
}

int grib_accessor_class_hash_array_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_hash_array_value* ha = 0;
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    int err = 0;
    size_t i = 0;

    if (!self->ha) {
        ha = find_hash_value(a, &err);
        if (err)
            return err;
        self->ha = ha;
    }

    switch (self->ha->type) {
        case GRIB_HASH_ARRAY_TYPE_INTEGER:
            if (*len < self->ha->iarray->n) {
                return GRIB_ARRAY_TOO_SMALL;
            }
            *len = self->ha->iarray->n;
            for (i = 0; i < *len; i++)
                val[i] = self->ha->iarray->v[i];
            break;

        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    return GRIB_SUCCESS;
}

int grib_accessor_class_hash_array_t::get_native_type(grib_accessor* a){
    int type = GRIB_TYPE_STRING;
    if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE)
        type = GRIB_TYPE_LONG;

    return type;
}

void grib_accessor_class_hash_array_t::destroy(grib_context* c, grib_accessor* a){
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    if (self->key)
        grib_context_free(c, self->key);
    grib_accessor_class_gen_t::destroy(c, a);
}

int grib_accessor_class_hash_array_t::unpack_string(grib_accessor* a, char* val, size_t* len){
    return GRIB_NOT_IMPLEMENTED;
}

size_t grib_accessor_class_hash_array_t::string_length(grib_accessor* a){
    return MAX_HASH_ARRAY_STRING_LENGTH;
}

int grib_accessor_class_hash_array_t::value_count(grib_accessor* a, long* count){
    int err                        = 0;
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    grib_hash_array_value* ha      = 0;

    if (!self->ha) {
        ha = find_hash_value(a, &err);
        if (err)
            return err;
        self->ha = ha;
    }

    *count = self->ha->iarray->n;
    return err;
}

int grib_accessor_class_hash_array_t::compare(grib_accessor* a, grib_accessor* b){
    return GRIB_NOT_IMPLEMENTED;
}
