/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Md5.h"
#include "md5_core.h"

eccodes::accessor::Md5 _grib_accessor_md5;
eccodes::Accessor* grib_accessor_md5 = &_grib_accessor_md5;

namespace eccodes::accessor
{

void Md5::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    char* b                   = 0;
    int n                     = 0;
    grib_string_list* current = 0;
    grib_context* context     = context_;

    offset_key_    = arg->get_name(grib_handle_of_accessor(this), n++);
    length_key_    = arg->get_expression(grib_handle_of_accessor(this), n++);
    blocklist_ = NULL;
    while ((b = (char*)arg->get_name(grib_handle_of_accessor(this), n++)) != NULL) {
        if (!blocklist_) {
            blocklist_        = (grib_string_list*)grib_context_malloc_clear(context, sizeof(grib_string_list));
            blocklist_->value = grib_context_strdup(context, b);
            current           = blocklist_;
        }
        else {
            ECCODES_ASSERT(current);
            if (current) {
                current->next        = (grib_string_list*)grib_context_malloc_clear(context, sizeof(grib_string_list));
                current->next->value = grib_context_strdup(context, b);
                current              = current->next;
            }
        }
    }

    grib_accessor::length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

long Md5::get_native_type()
{
    return GRIB_TYPE_STRING;
}

int Md5::compare(grib_accessor* b)
{
    int retval = GRIB_SUCCESS;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = value_count(&count);
    if (err)
        return err;
    alen = count;

    err = b->value_count(&count);
    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    return retval;
}

int Md5::unpack_string(char* v, size_t* len)
{
    unsigned mess_len;
    unsigned char* mess;
    unsigned char* p;
    long offset = 0, length = 0;
    grib_string_list* blocklist = NULL;
    int ret                     = GRIB_SUCCESS;
    long i                      = 0;
    struct grib_md5_state md5c;

    if (*len < 32) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %d bytes long (len=%zu)",
                         accessor_type().get().c_str(), name_, 32, *len);
        *len = 32;
        return GRIB_BUFFER_TOO_SMALL;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), offset_key_, &offset)) != GRIB_SUCCESS)
        return ret;
    if ((ret = length_key_->evaluate_long(grib_handle_of_accessor(this), &length)) != GRIB_SUCCESS)
        return ret;
    mess = (unsigned char*)grib_context_malloc(context_, length);
    memcpy(mess, grib_handle_of_accessor(this)->buffer->data + offset, length);
    mess_len = length;
    const unsigned char* pEnd = mess + length - 1;

    blocklist = context_->blocklist;
    /* passed blocklist overrides context blocklist.
     Consider to modify following line to extend context blocklist.
     */
    if (blocklist_)
        blocklist = blocklist_;
    while (blocklist && blocklist->value) {
        const grib_accessor* b = grib_find_accessor(grib_handle_of_accessor(this), blocklist->value);
        if (!b) {
            grib_context_free(context_, mess);
            return GRIB_NOT_FOUND;
        }

        p = mess + b->offset_ - offset;
        for (i = 0; i < b->length_; i++) {
            if (p > pEnd) break;
            *(p++) = 0;
        }

        blocklist = blocklist->next;
    }

    grib_md5_init(&md5c);
    grib_md5_add(&md5c, mess, mess_len);
    grib_md5_end(&md5c, v);
    grib_context_free(context_, mess);
    *len = strlen(v) + 1;

    return ret;
}

void Md5::destroy(grib_context* c)
{
    if (blocklist_) {
        grib_string_list* next = blocklist_;
        grib_string_list* cur  = NULL;
        while (next) {
            cur  = next;
            next = next->next;
            grib_context_free(c, cur->value);
            grib_context_free(c, cur);
        }
    }
    Gen::destroy(c);
}

int Md5::value_count(long* count)
{
    *count = 1; /* ECC-1475 */
    return 0;
}

}  // namespace eccodes::accessor
