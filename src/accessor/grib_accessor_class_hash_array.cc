/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_hash_array.h"

AccessorBuilder<grib_accessor_hash_array_t> _grib_accessor_hash_array_builder{};

#define MAX_HASH_ARRAY_STRING_LENGTH 255

void grib_accessor_hash_array_t::init(const long len, grib_arguments* args)
{
    grib_accessor_gen_t::init(len, args);
    length_ = 0;
    key_    = 0;
    ha_     = NULL;
}

void grib_accessor_hash_array_t::dump(grib_dumper* dumper)
{
    grib_dump_string(dumper, this, NULL);
}

int grib_accessor_hash_array_t::pack_double(const double* val, size_t* len)
{
    char s[200] = {
        0,
    };
    snprintf(s, sizeof(s), "%g", *val);
    key_ = grib_context_strdup(context_, s);
    ha_  = 0;
    return GRIB_SUCCESS;
}

int grib_accessor_hash_array_t::pack_long(const long* val, size_t* len)
{
    char s[200] = {
        0,
    };
    snprintf(s, sizeof(s), "%ld", *val);
    if (key_)
        grib_context_free(context_, key_);
    key_ = grib_context_strdup(context_, s);
    ha_  = 0;
    return GRIB_SUCCESS;
}

int grib_accessor_hash_array_t::pack_string(const char* v, size_t* len)
{
    key_ = grib_context_strdup(context_, v);
    ha_  = 0;
    return GRIB_SUCCESS;
}

int grib_accessor_hash_array_t::unpack_double(double* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

static grib_hash_array_value* find_hash_value(grib_accessor* a, int* err)
{
    grib_accessor_hash_array_t* self = (grib_accessor_hash_array_t*)a;
    grib_hash_array_value* ha_ret    = 0;
    grib_hash_array_value* ha        = NULL;

    ha = get_hash_array(grib_handle_of_accessor(a), self->creator_);
    if (!ha) {
        grib_context_log(a->context_, GRIB_LOG_ERROR,
                         "unable to get hash value for %s", self->creator_->name);
        *err = GRIB_HASH_ARRAY_NO_MATCH;
        return NULL;
    }

    *err = GRIB_SUCCESS;

    Assert(ha != NULL);
    if (!self->key_) {
        grib_context_log(a->context_, GRIB_LOG_ERROR,
                         "unable to get hash value for %s, set before getting", self->creator_->name);
        *err = GRIB_HASH_ARRAY_NO_MATCH;
        return NULL;
    }

    ha_ret = (grib_hash_array_value*)grib_trie_get(ha->index, self->key_);
    if (!ha_ret)
        ha_ret = (grib_hash_array_value*)grib_trie_get(ha->index, "default");

    if (!ha_ret) {
        *err = GRIB_HASH_ARRAY_NO_MATCH;
        grib_context_log(a->context_, GRIB_LOG_ERROR,
                         "hash_array: no match for %s=%s",
                         self->creator_->name, self->key_);
        const char* full_path = get_hash_array_full_path(self->creator_);
        if (full_path) {
            grib_context_log(a->context_, GRIB_LOG_ERROR, "hash_array: file path = %s", full_path);
        }
        grib_context_log(a->context_, GRIB_LOG_ERROR, "Hint: Check the key 'masterTablesVersionNumber'");
        return NULL;
    }
    return ha_ret;
}

int grib_accessor_hash_array_t::unpack_long(long* val, size_t* len)
{
    grib_hash_array_value* ha = 0;
    int err                   = 0;
    size_t i                  = 0;

    if (!ha_) {
        ha = find_hash_value(this, &err);
        if (err)
            return err;
        ha_ = ha;
    }

    switch (ha_->type) {
        case GRIB_HASH_ARRAY_TYPE_INTEGER:
            if (*len < ha_->iarray->n) {
                return GRIB_ARRAY_TOO_SMALL;
            }
            *len = ha_->iarray->n;
            for (i = 0; i < *len; i++)
                val[i] = ha_->iarray->v[i];
            break;

        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    return GRIB_SUCCESS;
}

long grib_accessor_hash_array_t::get_native_type()
{
    int type = GRIB_TYPE_STRING;
    if (flags_ & GRIB_ACCESSOR_FLAG_LONG_TYPE)
        type = GRIB_TYPE_LONG;

    return type;
}

void grib_accessor_hash_array_t::destroy(grib_context* c)
{
    if (key_)
        grib_context_free(c, key_);
    grib_accessor_gen_t::destroy(c);
}

int grib_accessor_hash_array_t::unpack_string(char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

size_t grib_accessor_hash_array_t::string_length()
{
    return MAX_HASH_ARRAY_STRING_LENGTH;
}

int grib_accessor_hash_array_t::value_count(long* count)
{
    int err                   = 0;
    grib_hash_array_value* ha = 0;

    if (!ha_) {
        ha = find_hash_value(this, &err);
        if (err)
            return err;
        ha_ = ha;
    }

    *count = ha_->iarray->n;
    return err;
}

int grib_accessor_hash_array_t::compare(grib_accessor* b)
{
    return GRIB_NOT_IMPLEMENTED;
}
