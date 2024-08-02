/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor.h"
#include "grib_accessor_class_gen.h"
#include <stdexcept>

// ======================== grib_accessor_gen ========================
void grib_accessor_gen_t::dump(grib_dumper* f)
{
    return cclass->dump(this, f);
}
int grib_accessor_gen_t::pack_missing()
{
    // grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0);
    return cclass->pack_missing(this);
}
// int grib_accessor_gen_t::pack_zero(grib_accessor* a)
// {
//     return cclass->clear(a);
// }

int grib_accessor_gen_t::is_missing_internal()
{
    return cclass->is_missing(this);
}

int grib_accessor_gen_t::pack_double(const double* v, size_t* len)
{
    return cclass->pack_double(this, v, len);
}

int grib_accessor_gen_t::pack_float(const float* v, size_t* len)
{
    return cclass->pack_float(this, v, len);
}

int grib_accessor_gen_t::pack_expression(grib_expression* e)
{
    return cclass->pack_expression(this, e);
}

int grib_accessor_gen_t::pack_string(const char* v, size_t* len)
{
    return cclass->pack_string(this, v, len);
}

int grib_accessor_gen_t::pack_string_array(const char** v, size_t* len)
{
    return cclass->pack_string_array(this, v, len);
}

int grib_accessor_gen_t::pack_long(const long* v, size_t* len)
{
    return cclass->pack_long(this, v, len);
}

int grib_accessor_gen_t::pack_bytes(const unsigned char* v, size_t* len)
{
    return cclass->pack_bytes(this, v, len);
}

int grib_accessor_gen_t::unpack_bytes(unsigned char* v, size_t* len)
{
    return cclass->unpack_bytes(this, v, len);
}

int grib_accessor_gen_t::unpack_double_subarray(double* v, size_t start, size_t len)
{
    return cclass->unpack_double_subarray(this, v, start, len);
}

int grib_accessor_gen_t::unpack_double(double* v, size_t* len)
{
    return cclass->unpack_double(this, v, len);
}

int grib_accessor_gen_t::unpack_float(float* v, size_t* len)
{
    return cclass->unpack_float(this, v, len);
}

int grib_accessor_gen_t::unpack_double_element(size_t i, double* v)
{
    return cclass->unpack_double_element(this, i, v);
}

int grib_accessor_gen_t::unpack_float_element(size_t i, float* v)
{
    // return cclass->unpack_float_element(this, i, v);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_gen_t::unpack_long(long* v, size_t* len)
{
    return cclass->unpack_long(this, v, len);
}

long grib_accessor_gen_t::get_native_type()
{
    return cclass->get_native_type(this);
}

long grib_accessor_gen_t::get_next_position_offset()
{
    return cclass->next_offset(this);
}

long grib_accessor_gen_t::string_length()
{
    return cclass->string_length(this);
}

long grib_accessor_gen_t::byte_offset()
{
    return cclass->byte_offset(this);
}

long grib_accessor_gen_t::byte_count()
{
    return cclass->byte_count(this);
}

int grib_accessor_gen_t::value_count(long* count)
{
    return cclass->value_count(this, count);
}

int grib_accessor_gen_t::notify_change(grib_accessor* changed)
{
    return cclass->notify_change(this, changed);
}

// Get the values for the indices passed in.
// The length of the 'index_array' argument should be 'len' and 'val_array' should also contain at least 'len' elements
int grib_accessor_gen_t::unpack_double_element_set(const size_t* index_array, size_t len, double* val_array)
{
    return cclass->unpack_double_element_set(this, index_array, len, val_array);
}
int grib_accessor_gen_t::unpack_float_element_set(const size_t* index_array, size_t len, float* val_array)
{
    throw std::runtime_error("grib_unpack_float_element_set not implemented");
}
int grib_accessor_gen_t::unpack_string(char* v, size_t* len)
{
    return cclass->unpack_string(this, v, len);
}
int grib_accessor_gen_t::unpack_string_array(char** v, size_t* len)
{
    return cclass->unpack_string_array(this, v, len);
}

void grib_accessor_gen_t::init_accessor(const long len, grib_arguments* args)
{
    this->cclass->init(this, len, args);
}

grib_accessor* grib_accessor_gen_t::clone(grib_section* s, int* err)
{
    grib_context* ct = this->context;
    grib_context_log(ct, GRIB_LOG_DEBUG, "clone %s ==> %s", cclass->name, this->name);
    return cclass->make_clone(this, s, err);
}

void grib_accessor_gen_t::update_size(size_t len)
{
    cclass->update_size(this, len);
}

int grib_accessor_gen_t::nearest_smaller_value(double val, double* nearest)
{
    return cclass->nearest_smaller_value(this, val, nearest);
}

size_t grib_accessor_gen_t::preferred_size(int from_handle)
{
    return cclass->preferred_size(this, from_handle);
}

grib_accessor* grib_accessor_gen_t::next_accessor()
{
    return cclass->next(this, 1);
}

void grib_accessor_gen_t::resize(size_t new_size)
{
    cclass->resize(this, new_size);
}

// For this one, ALL destroy are called
void grib_accessor_gen_t::destroy(grib_context* ct)
{
    cclass->destroy(ct, this);
    delete this;
}


// ======================== grib_accessor_class_gen ========================


grib_accessor_class_gen_t _grib_accessor_class_gen = grib_accessor_class_gen_t("gen");
grib_accessor_class* grib_accessor_class_gen       = &_grib_accessor_class_gen;

void grib_accessor_class_gen_t::init(grib_accessor* a, const long len, grib_arguments* param)
{
    grib_action* act = (grib_action*)(a->creator);
    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        a->length = 0;
        if (!a->vvalue)
            a->vvalue = (grib_virtual_value*)grib_context_malloc_clear(a->context, sizeof(grib_virtual_value));
        a->vvalue->type   = a->get_native_type();
        a->vvalue->length = len;
        if (act->default_value != NULL) {
            const char* p = 0;
            size_t s_len  = 1;
            long l;
            int ret = 0;
            double d;
            char tmp[1024];
            grib_expression* expression = grib_arguments_get_expression(grib_handle_of_accessor(a), act->default_value, 0);
            int type                    = grib_expression_native_type(grib_handle_of_accessor(a), expression);
            switch (type) {
                    // TODO(maee): add single-precision case

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
                        grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to evaluate %s as string", a->name);
                        Assert(0);
                    }
                    s_len = strlen(p) + 1;
                    a->pack_string(p, &s_len);
                    break;
            }
        }
    }
    else {
        a->length = len;
    }
}

void grib_accessor_class_gen_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    const int type = a->get_native_type();
    switch (type) {
        case GRIB_TYPE_STRING:
            grib_dump_string(dumper, a, NULL);
            break;
        case GRIB_TYPE_DOUBLE:
            grib_dump_double(dumper, a, NULL);
            break;
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, a, NULL);
            break;
        default:
            grib_dump_bytes(dumper, a, NULL);
    }
}

long grib_accessor_class_gen_t::next_offset(grib_accessor* a)
{
    return a->offset + a->length;
}

int grib_accessor_class_gen_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

size_t grib_accessor_class_gen_t::string_length(grib_accessor* a)
{
    return 1024;
}

long grib_accessor_class_gen_t::byte_count(grib_accessor* a)
{
    return a->length;
}

int grib_accessor_class_gen_t::get_native_type(grib_accessor* a)
{
    grib_context_log(a->context, GRIB_LOG_ERROR,
                     "Accessor %s [%s] must implement 'get_native_type'", a->name, a->cclass->name);
    return GRIB_TYPE_UNDEFINED;
}

long grib_accessor_class_gen_t::byte_offset(grib_accessor* a)
{
    return a->offset;
}

int grib_accessor_class_gen_t::unpack_bytes(grib_accessor* a, unsigned char* val, size_t* len)
{
    unsigned char* buf = grib_handle_of_accessor(a)->buffer->data;
    const long length  = a->byte_count();
    const long offset  = a->byte_offset();

    if (*len < length) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it is %ld bytes long", a->name, length);
        *len = length;
        return GRIB_ARRAY_TOO_SMALL;
    }

    memcpy(val, buf + offset, length);
    *len = length;

    return GRIB_SUCCESS;
}

int grib_accessor_class_gen_t::clear(grib_accessor* a)
{
    unsigned char* buf = grib_handle_of_accessor(a)->buffer->data;
    const long length  = a->byte_count();
    const long offset  = a->byte_offset();

    memset(buf + offset, 0, length);

    return GRIB_SUCCESS;
}

int grib_accessor_class_gen_t::unpack_long(grib_accessor* a, long* v, size_t* len)
{
    is_overridden_[UNPACK_LONG] = 0;
    int type                    = GRIB_TYPE_UNDEFINED;
    if (is_overridden_[UNPACK_DOUBLE] == 1) {
        double val = 0.0;
        size_t l   = 1;
        a->unpack_double(&val, &l);
        if (is_overridden_[UNPACK_DOUBLE] == 1) {
            if (val == GRIB_MISSING_DOUBLE)
                *v = GRIB_MISSING_LONG;
            else
                *v = (long)val;
            grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting double %s to long", a->name);
            return GRIB_SUCCESS;
        }
    }

    if (is_overridden_[UNPACK_STRING] == 1) {
        char val[1024];
        size_t l   = sizeof(val);
        char* last = NULL;
        a->unpack_string(val, &l);

        if (is_overridden_[UNPACK_STRING] == 1) {
            *v = strtol(val, &last, 10);

            if (*last == 0) {
                grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting string %s to long", a->name);
                return GRIB_SUCCESS;
            }
        }
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "Cannot unpack key '%s' as long", a->name);
    if (grib_get_native_type(grib_handle_of_accessor(a), a->name, &type) == GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Hint: Try unpacking as %s", grib_get_type_name(type));
    }
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::unpack_double(grib_accessor* a, double* v, size_t* len)
{
    return unpack_helper<double>(a, v, len);
}

int grib_accessor_class_gen_t::unpack_float(grib_accessor* a, float* v, size_t* len)
{
    return unpack_helper<float>(a, v, len);
}

int grib_accessor_class_gen_t::unpack_string(grib_accessor* a, char* v, size_t* len)
{
    is_overridden_[UNPACK_STRING] = 0;

    int err = 0;
    if (is_overridden_[UNPACK_DOUBLE] == 1) {
        double val = 0.0;
        size_t l   = 1;
        err        = a->unpack_double(&val, &l);
        if (is_overridden_[UNPACK_DOUBLE] == 1) {
            if (err) return err;
            snprintf(v, 64, "%g", val);
            *len = strlen(v);
            grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting double %s to string", a->name);
            return GRIB_SUCCESS;
        }
    }

    if (is_overridden_[UNPACK_LONG] == 1) {
        long val = 0;
        size_t l = 1;
        err      = a->unpack_long(&val, &l);
        if (is_overridden_[UNPACK_LONG] == 1) {
            if (err) return err;
            snprintf(v, 64, "%ld", val);
            *len = strlen(v);
            grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting long %s to string\n", a->name);
            return GRIB_SUCCESS;
        }
    }

    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::unpack_string_array(grib_accessor* a, char** v, size_t* len)
{
    size_t length = 0;

    int err = grib_get_string_length_acc(a, &length);
    if (err)
        return err;
    v[0] = (char*)grib_context_malloc_clear(a->context, length);
    a->unpack_string(v[0], &length);  // TODO(masn): check return value
    *len = 1;

    return GRIB_SUCCESS;
}

int grib_accessor_class_gen_t::pack_expression(grib_accessor* a, grib_expression* e)
{
    size_t len        = 1;
    long lval         = 0;
    double dval       = 0;
    const char* cval  = NULL;
    int ret           = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    // Use the native type of the expression not the accessor
    switch (grib_expression_native_type(hand, e)) {
        case GRIB_TYPE_LONG: {
            len = 1;
            ret = grib_expression_evaluate_long(hand, e, &lval);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to set %s as long (from %s)",
                                 a->name, e->cclass->name);
                return ret;
            }
            /*if (hand->context->debug)
                printf("ECCODES DEBUG grib_accessor_class_gen_t::pack_expression %s %ld\n", a->name,lval);*/
            return a->pack_long(&lval, &len);
        }

        case GRIB_TYPE_DOUBLE: {
            len = 1;
            ret = grib_expression_evaluate_double(hand, e, &dval);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to set %s as double (from %s)",
                                 a->name, e->cclass->name);
                return ret;
            }
            /*if (hand->context->debug)
                printf("ECCODES DEBUG grib_accessor_class_gen_t::pack_expression %s %g\n", a->name, dval);*/
            return a->pack_double(&dval, &len);
        }

        case GRIB_TYPE_STRING: {
            char tmp[1024];
            len  = sizeof(tmp);
            cval = grib_expression_evaluate_string(hand, e, tmp, &len, &ret);
            if (ret != GRIB_SUCCESS) {
                grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to set %s as string (from %s)",
                                 a->name, e->cclass->name);
                return ret;
            }
            len = strlen(cval);
            /*if (hand->context->debug)
                printf("ECCODES DEBUG grib_accessor_class_gen_t::pack_expression %s %s\n", a->name, cval);*/
            return a->pack_string(cval, &len);
        }
    }

    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::pack_long(grib_accessor* a, const long* v, size_t* len)
{
    is_overridden_[PACK_LONG] = 0;
    grib_context* c           = a->context;
    if (is_overridden_[PACK_DOUBLE]) {
        double* val = (double*)grib_context_malloc(c, *len * (sizeof(double)));
        if (!val) {
            grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %zu bytes", *len * (sizeof(double)));
            return GRIB_OUT_OF_MEMORY;
        }
        for (size_t i = 0; i < *len; i++)
            val[i] = v[i];
        int ret = a->pack_double(val, len);
        grib_context_free(c, val);
        if (is_overridden_[PACK_DOUBLE]) {
            return ret;
        }
    }

    grib_context_log(c, GRIB_LOG_ERROR, "Should not pack '%s' as an integer", a->name);
    if (is_overridden_[PACK_STRING]) {
        grib_context_log(c, GRIB_LOG_ERROR, "Try packing as a string");
    }

    return GRIB_NOT_IMPLEMENTED;
}

int pack_double_array_as_long(grib_accessor* a, const double* v, size_t* len)
{
    grib_context* c = a->context;
    int ret         = GRIB_SUCCESS;
    size_t numBytes = *len * (sizeof(long));
    long* lValues   = (long*)grib_context_malloc(c, numBytes);
    if (!lValues) {
        grib_context_log(c, GRIB_LOG_ERROR, "Unable to allocate %ld bytes", numBytes);
        return GRIB_OUT_OF_MEMORY;
    }
    for (size_t i = 0; i < *len; i++)
        lValues[i] = (long)v[i]; /* convert from double to long */
    ret = a->pack_long(lValues, len);
    grib_context_free(c, lValues);
    return ret;
}

int grib_accessor_class_gen_t::pack_double(grib_accessor* a, const double* v, size_t* len)
{
    is_overridden_[PACK_DOUBLE] = 0;
    grib_context* c             = a->context;

    if (is_overridden_[PACK_LONG] || strcmp(a->cclass->name, "codetable") == 0) {
        /* ECC-648: Special case of codetable */
        return pack_double_array_as_long(a, v, len);
    }

    grib_context_log(c, GRIB_LOG_ERROR, "Should not pack '%s' as a double", a->name);
    if (is_overridden_[PACK_STRING]) {
        grib_context_log(c, GRIB_LOG_ERROR, "Try packing as a string");
    }
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::pack_string_array(grib_accessor* a, const char** v, size_t* len)
{
    int err           = 0;
    size_t length     = 0;
    grib_accessor* as = 0;

    as     = a;
    long i = (long)*len - 1;
    while (as && i >= 0) {
        length = strlen(v[i]);
        err    = as->pack_string(v[i], &length);
        if (err)
            return err;
        --i;
        as = as->same;
    }
    return GRIB_SUCCESS;
}

int grib_accessor_class_gen_t::pack_string(grib_accessor* a, const char* v, size_t* len)
{
    is_overridden_[PACK_STRING] = 0;
    if (is_overridden_[PACK_DOUBLE]) {
        size_t l     = 1;
        char* endPtr = NULL; /* for error handling */
        double val   = strtod(v, &endPtr);
        if (*endPtr) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "%s: Invalid value (%s) for key '%s'. String cannot be converted to a double",
                             __func__, v, a->name);
            return GRIB_WRONG_TYPE;
        }
        return a->pack_double(&val, &l);
    }

    if (is_overridden_[PACK_LONG]) {
        size_t l = 1;
        long val = atol(v);
        int err  = a->pack_long(&val, &l);
        if (is_overridden_[PACK_LONG]) {
            return err;
        }
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "Should not pack '%s' as string", a->name);
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::pack_bytes(grib_accessor* a, const unsigned char* val, size_t* len)
{
    const size_t length = *len;
    if (length != a->length) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "pack_bytes: Wrong size (%zu) for %s. It is %ld bytes long",
                         length, a->name, a->length);
        return GRIB_BUFFER_TOO_SMALL;
    }
    grib_buffer_replace(a, val, length, 1, 1);
    return GRIB_SUCCESS;
}

void grib_accessor_class_gen_t::destroy(grib_context* ct, grib_accessor* a)
{
    grib_dependency_remove_observed(a);
    grib_dependency_remove_observer(a);
    if (a->vvalue != NULL) {
        grib_context_free(ct, a->vvalue);
        a->vvalue = NULL;
    }
    /*grib_context_log(ct,GRIB_LOG_DEBUG,"address=%p",a);*/
}

grib_section* grib_accessor_class_gen_t::sub_section(grib_accessor* a)
{
    return NULL;
}

int grib_accessor_class_gen_t::notify_change(grib_accessor* self, grib_accessor* observed)
{
    /* Default behaviour is to notify creator */
    return grib_action_notify_change(self->creator, self, observed);
}

void grib_accessor_class_gen_t::update_size(grib_accessor* a, size_t s)
{
    grib_context_log(a->context, GRIB_LOG_FATAL,
                     "Accessor %s [%s] must implement 'update_size'", a->name, a->cclass->name);
}

grib_accessor* grib_accessor_class_gen_t::next(grib_accessor* a, int mod)
{
    grib_accessor* next = NULL;
    if (a->next_) {
        next = a->next_;
    }
    else {
        if (a->parent->owner)
            next = a->parent->owner->cclass->next(a->parent->owner, 0);
    }
    return next;
}

int grib_accessor_class_gen_t::compare(grib_accessor* a, grib_accessor* b)
{
    return GRIB_NOT_IMPLEMENTED;
}

/* Redefined in all padding */

size_t grib_accessor_class_gen_t::preferred_size(grib_accessor* a, int from_handle)
{
    return a->length;
}

int grib_accessor_class_gen_t::is_missing(grib_accessor* a)
{
    int i              = 0;
    int is_missing     = 1;
    unsigned char ones = 0xff;
    unsigned char* v   = NULL;

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        if (a->vvalue == NULL) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s internal error (flags=0x%lX)", a->name, a->flags);
            Assert(!"grib_accessor_class_gen_t::is_missing(): a->vvalue == NULL");
            return 0;
        }
        return a->vvalue->missing;
    }
    Assert(a->length >= 0);

    v = grib_handle_of_accessor(a)->buffer->data + a->offset;

    for (i = 0; i < a->length; i++) {
        if (*v != ones) {
            is_missing = 0;
            break;
        }
        v++;
    }

    return is_missing;
}

int grib_accessor_class_gen_t::unpack_double_element(grib_accessor* a, size_t i, double* val)
{
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_gen_t::unpack_double_subarray(grib_accessor* a, double* val, size_t start, size_t len)
{
    return GRIB_NOT_IMPLEMENTED;
}

grib_accessor* grib_accessor_class_gen_t::make_clone(grib_accessor* a, grib_section* s, int* err)
{
    *err = GRIB_NOT_IMPLEMENTED;
    return NULL;
}


grib_accessor_class_gen_t::~grib_accessor_class_gen_t() {};

void grib_accessor_class_gen_t::post_init(grib_accessor*)
{
    return;
};
int grib_accessor_class_gen_t::pack_missing(grib_accessor*)
{
    throw std::runtime_error("grib_accessor_class_gen_t::pack_missing not implemented");
};
int grib_accessor_class_gen_t::pack_float(grib_accessor*, const float*, size_t* len)
{
    throw std::runtime_error("grib_accessor_class_gen_t::pack_float not implemented");
};
void grib_accessor_class_gen_t::resize(grib_accessor*, size_t)
{
    throw std::runtime_error("grib_accessor_class_gen_t::resize not implemented");
};
int grib_accessor_class_gen_t::nearest_smaller_value(grib_accessor*, double, double*)
{
    throw std::runtime_error("grib_accessor_class_gen_t::nearest_smaller_value not implemented");
};
int grib_accessor_class_gen_t::unpack_float_element(grib_accessor*, size_t, float*)
{
    throw std::runtime_error("grib_accessor_class_gen_t::unpack_float_element not implemented");
};
int unpack_element_set(grib_accessor*, const size_t*, size_t, double*)
{
    throw std::runtime_error("grib_accessor_class_gen_t::unpack_element_set not implemented");
};
int grib_accessor_class_gen_t::unpack_float_element_set(grib_accessor*, const size_t*, size_t, float*)
{
    throw std::runtime_error("grib_accessor_class_gen_t::unpack_float_element_set not implemented");
};
