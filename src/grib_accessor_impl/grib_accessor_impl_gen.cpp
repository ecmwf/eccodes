#include "grib_accessor_impl_gen.h"
#include "grib_accessor_impl_factory.h"
#include "grib_value.h"

namespace eccodes {  
    grib_accessor_impl_gen::grib_accessor_impl_gen(grib_section* p, grib_action* ga_creator)
    {
        // Due to the way we're building this class from an existing grib_accessor struct
        // we need to manually init all the members...
        name                = ga_creator->name;
        name_space          = ga_creator->name_space;
        context             = p->h->context;
        h                   = NULL;
        creator             = ga_creator;
        length              = 0;

        if (p->block->last) { offset = grib_get_next_position_offset(p->block->last); }
        else {
            if (p->owner)   { offset = p->owner->offset; }
            else            { offset = 0; }
        }

        parent              = p;
        next                = NULL;
        previous            = NULL;
        cclass              = NULL;
        flags               = ga_creator->flags;
        sub_section         = NULL;

        memset(all_names, 0 , sizeof(all_names));
        memset(all_name_spaces, 0 , sizeof(all_name_spaces));

        all_names[0]        = ga_creator->name;
        all_name_spaces[0]  = ga_creator->name_space;
        dirty               = 0;
        same                = NULL;
        loop                = 0;
        bufr_subset_number  = 0;
        bufr_group_number   = 0;
        vvalue              = NULL;
        set                 = ga_creator->set;

        memset(attributes, 0 , sizeof(attributes));

        parent_as_attribute = NULL;
    }

    grib_accessor_impl_gen::~grib_accessor_impl_gen()
    {
        grib_dependency_remove_observed(as_accessor());
        grib_dependency_remove_observer(as_accessor());
        if (vvalue != NULL) {
            grib_context_free(context, vvalue);
            vvalue = NULL;
        }
    }

    int grib_accessor_impl_gen::native_type() 
    {
        Assert(0); // MUST override!
        return GRIB_TYPE_UNDEFINED;
    }

    void grib_accessor_impl_gen::init(const long len, grib_arguments* params)
    {
        init_gen(len, params);
    }

    void grib_accessor_impl_gen::init_gen(const long len, grib_arguments* /* params */)
    {
        grib_action* act = (grib_action*)(creator);
        if (flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
            length = 0;
            if (!vvalue)
                vvalue = (grib_virtual_value*)grib_context_malloc_clear(context, sizeof(grib_virtual_value));
            vvalue->type   = native_type();
            vvalue->length = len;
            if (act->default_value != NULL) {
                const char* p = 0;
                size_t s_len  = 1;
                long l;
                int ret = 0;
                double d;
                char tmp[1024];
                grib_expression* expression = grib_arguments_get_expression(accessor_handle(), act->default_value, 0);
                int type                    = grib_expression_native_type(accessor_handle(), expression);
                switch (type) {
                    case GRIB_TYPE_DOUBLE:
                        grib_expression_evaluate_double(accessor_handle(), expression, &d);
                        pack_double(&d, &s_len);
                        break;

                    case GRIB_TYPE_LONG:
                        grib_expression_evaluate_long(accessor_handle(), expression, &l);
                        pack_long(&l, &s_len);
                        break;

                    default:
                        s_len = sizeof(tmp);
                        p     = grib_expression_evaluate_string(accessor_handle(), expression, tmp, &s_len, &ret);
                        if (ret != GRIB_SUCCESS) {
                            grib_context_log(context, GRIB_LOG_ERROR, "Unable to evaluate %s as string", name);
                            Assert(0);
                        }
                        s_len = strlen(p) + 1;
                        pack_string(p, &s_len);
                        break;
                }
            }
        }
        else {
            length = len;
        }
    }
    
    int grib_accessor_impl_gen::is_missing()
    {
        int i              = 0;
        int is_missing     = 1;
        unsigned char ones = 0xff;
        unsigned char* v   = NULL;

        if (flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
            if (vvalue == NULL) {
                grib_context_log(context, GRIB_LOG_ERROR, "%s internal error (flags=0x%lX)", name, flags);
                Assert(!"grib_accessor_class_gen::is_missing(): a->vvalue == NULL");
                return 0;
            }
            return vvalue->missing;
        }
        Assert(length >= 0);

        v = accessor_handle()->buffer->data + offset;

        for (i = 0; i < length; i++) {
            if (*v != ones) {
                is_missing = 0;
                break;
            }
            v++;
        }

        return is_missing;
    }
    
    int grib_accessor_impl_gen::pack_bytes(const unsigned char* val, size_t* len)
    {
        const size_t chlength = *len;
        if (chlength != length) {
            grib_context_log(context, GRIB_LOG_ERROR,
                            "pack_bytes: Wrong size (%lu) for %s. It is %lu bytes long",
                            length, name, length);
            return GRIB_BUFFER_TOO_SMALL;
        }
        grib_buffer_replace(as_accessor(), val, chlength, 1, 1);
        return GRIB_SUCCESS;
    }
    
    int grib_accessor_impl_gen::pack_double(const double* val, size_t* len)
    {
        int ret = GRIB_NOT_IMPLEMENTED;
        pack_buffer in_buffer(context, const_double_view{val, len});

        // Try double, and fall back to long if not available...
        // Note: the C code has a special-case for the "codetable" accessor, but this
        //       is handled automatically here by the virtual function implementation
        if(ret = pack_double(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_long(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}

        grib_context_log(context, GRIB_LOG_ERROR, "Should not grib_pack %s as double", name);
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::pack_float(const float* /* val */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::pack_long(const long* val, size_t* len)
    {
        int ret = GRIB_NOT_IMPLEMENTED;
        pack_buffer in_buffer(context, const_long_view{val, len});

        // Try long, and fall back to double if not available...
        if(ret = pack_long(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_double(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}

        grib_context_log(context, GRIB_LOG_ERROR, "Should not grib_pack %s as long", name);
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::pack_string(const char* val, size_t* len)
    {
        int ret = GRIB_NOT_IMPLEMENTED;
        pack_buffer in_buffer(context, const_char_view{val, len});

        // Try string, then double, then long...
        if(ret = pack_string(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_double(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_long(in_buffer, len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}

        grib_context_log(context, GRIB_LOG_ERROR, "Should not grib_pack %s as string", name);
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::pack_string_array(const char** v, size_t* len)
    {
        long i;
        int err           = 0;
        size_t length     = 0;
        grib_accessor* as = 0;

        as = as_accessor();

        i  = (long)*len - 1;
        while (as && i >= 0) {
            length = strlen(v[i]);
//            err    = grib_pack_string(as, v[i], &length);
            err    = pack_string(v[i], &length);
            if (err)
                return err;
            --i;

            // TODO - Is this the same accessor (as we don't reference it in the pack_string call)?
            as = as->same;
        }
        return GRIB_SUCCESS;
    }
    
    int grib_accessor_impl_gen::pack_expression(grib_expression* e)
    {
        size_t len    = 1;
        long lval         = 0;
        double dval       = 0;
        const char* cval  = NULL;
        int ret           = 0;
        grib_handle* hand = accessor_handle();

        switch (native_type()) {
            case GRIB_TYPE_LONG: {
                len = 1;
                ret = grib_expression_evaluate_long(hand, e, &lval);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(context, GRIB_LOG_ERROR, "Unable to set %s as long (from %s)",
                                    name, e->cclass->name);
                    return ret;
                }

                return pack_long(&lval, &len);
            }

            case GRIB_TYPE_DOUBLE: {
                len = 1;
                ret = grib_expression_evaluate_double(hand, e, &dval);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(context, GRIB_LOG_ERROR, "unable to set %s as double (from %s)",
                                    name, e->cclass->name);
                    return ret;
                }

                return pack_double(&dval, &len);
            }

            case GRIB_TYPE_STRING: {
                char tmp[1024];
                len  = sizeof(tmp);
                cval = grib_expression_evaluate_string(hand, e, tmp, &len, &ret);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(context, GRIB_LOG_ERROR, "unable to set %s as string (from %s)",
                                    name, e->cclass->name);
                    return ret;
                }
                len = strlen(cval);

                return pack_string(cval, &len);
            }
        }

        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_bytes(unsigned char* val, size_t* len)
    {
        unsigned char* buf = accessor_handle()->buffer->data;
        long length        = grib_byte_count(as_accessor());
        long offset        = grib_byte_offset(as_accessor());

        if (*len < length) {
            grib_context_log(context, GRIB_LOG_ERROR, "Wrong size for %s, it is %ld bytes long", name, length);
            *len = length;
            return GRIB_ARRAY_TOO_SMALL;
        }

        memcpy(val, buf + offset, length);
        *len = length;

        return GRIB_SUCCESS;
    }
    
    // We define this template here as it is only called within the class, so
    // all instantiations will be created in this cpp file...
    template<typename T>
    int grib_accessor_impl_gen::unpack(T* val, size_t* len)
    {
        static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
        int ret = GRIB_NOT_IMPLEMENTED;
        int type = GRIB_TYPE_UNDEFINED;

        // first, try the requested type...
        // Note: the "constexpr if" will select the correct branch for double/float, however the "discarded" branch
        //       must still compile, hence the casts to avoid double* to float* errors for these...
        if constexpr(std::is_same_v<T,double>)
            if(ret = unpack_double(double_view{(double*)val, len}); ret!= GRIB_NOT_IMPLEMENTED) { return ret; }
        else
            if(ret = unpack_float(float_view{(float*)val, len}); ret!= GRIB_NOT_IMPLEMENTED) { return ret; }

        // Now fall back to long, then string
        long l_val{};
        size_t l_len{1};
        if(ret = unpack_long(long_view{&l_val, &l_len}); ret!= GRIB_NOT_IMPLEMENTED) {
            *val = l_val;
            *len = l_len;
            grib_context_log(context, GRIB_LOG_DEBUG, "Casting long %s to %s", name, type_to_string(*val));
            return GRIB_SUCCESS;
        }

        char c_val[1024];
        size_t c_len = sizeof(c_val);
        if(ret = unpack_string(char_view{c_val, &c_len}); ret!= GRIB_NOT_IMPLEMENTED) {
            char* last = NULL;
            *val = strtod(c_val, &last);
            *len = c_len;

            if (*last == 0) { /* conversion of string to double worked */
                grib_context_log(context, GRIB_LOG_DEBUG, "Casting string %s to long", name);
                return GRIB_SUCCESS;
            }
        }

        grib_context_log(context, GRIB_LOG_ERROR, "Cannot unpack as %s", name);
        grib_context_log(context, GRIB_LOG_ERROR, "Hint: Try unpacking as %s", grib_get_type_name(native_type()));

        return GRIB_NOT_IMPLEMENTED;
    }

    int grib_accessor_impl_gen::unpack_double(double* val, size_t* len)
    {
        return unpack(val, len);
    }
    
    int grib_accessor_impl_gen::unpack_float(float* val, size_t* len)
    {
        return unpack(val, len);
    }
    
    int grib_accessor_impl_gen::unpack_long(long* val, size_t* len)
    {
        int ret = GRIB_NOT_IMPLEMENTED;
        int type = GRIB_TYPE_UNDEFINED;

        // Try long, then fall back to double, then string...
        if(ret = unpack_long(long_view{val, len}); ret!= GRIB_NOT_IMPLEMENTED) { return ret; }

        double d_val{};
        size_t d_len{1};
        if(ret = unpack_double(double_view{&d_val, &d_len}); ret!= GRIB_NOT_IMPLEMENTED) {
            if (d_val == GRIB_MISSING_DOUBLE)
                *val = GRIB_MISSING_LONG;
            else {
                *val = (long)val;
                *len = d_len;
            }
            grib_context_log(context, GRIB_LOG_DEBUG, "Casting double %s to long", name);
            return GRIB_SUCCESS;
        }

        char c_val[1024];
        size_t c_len = sizeof(c_val);
        if(ret = unpack_string(char_view{c_val, &c_len}); ret!= GRIB_NOT_IMPLEMENTED) {
            char* last = NULL;
            *val = strtol(c_val, &last, 10);
            *len = c_len;

            if (*last == 0) {
                grib_context_log(context, GRIB_LOG_DEBUG, "Casting string %s to long", name);
                return GRIB_SUCCESS;
            }
        }

        grib_context_log(context, GRIB_LOG_ERROR, "Cannot unpack %s as long", name);
        grib_context_log(context, GRIB_LOG_ERROR, "Hint: Try unpacking as %s", grib_get_type_name(native_type()));
        
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_string(char* val, size_t* len)
    {
        int ret = GRIB_NOT_IMPLEMENTED;
        int type = GRIB_TYPE_UNDEFINED;

        // Try string, then fall back to double, then long...
        if(ret = unpack_string(char_view{val, len}); ret!= GRIB_NOT_IMPLEMENTED) { return ret; }

        double d_val{};
        size_t d_len{1};
        if(ret = unpack_double(double_view{&d_val, &d_len}); ret!= GRIB_NOT_IMPLEMENTED) {
            snprintf(val, 64, "%g", d_val);
            *len = strlen(val);
            grib_context_log(context, GRIB_LOG_DEBUG, "Casting double %s to string", name);
            return GRIB_SUCCESS;
        }

        long l_val{};
        size_t l_len{1};
        if(ret = unpack_long(long_view{&l_val, &l_len}); ret!= GRIB_NOT_IMPLEMENTED) {
            snprintf(val, 64, "%ld", l_val);
            *len = strlen(val);
            grib_context_log(context, GRIB_LOG_DEBUG, "Casting long %s to string\n", name);
            return GRIB_SUCCESS;
        }

        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_string_array(char** v, size_t* len)
    {
        int err       = 0;
        size_t length = 0;

        err = ecc__grib_get_string_length(as_accessor(), &length);
        if (err)
            return err;
        v[0] = (char*)grib_context_malloc_clear(context, length);
        unpack_string(v[0], &length);
        *len = 1;

        return GRIB_SUCCESS;
    }
   
    int grib_accessor_impl_gen::value_count(long* count)
    {
        *count = 1;
        return 0;
    }
    
    void grib_accessor_impl_gen::dump(grib_dumper* dumper)
    {
        switch (native_type()) {
        case GRIB_TYPE_STRING:
            grib_dump_string(dumper, as_accessor(), NULL);
            break;
        case GRIB_TYPE_DOUBLE:
            grib_dump_double(dumper, as_accessor(), NULL);
            break;
        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, as_accessor(), NULL);
            break;
        default:
            grib_dump_bytes(dumper, as_accessor(), NULL);
        }
    }
    
    void grib_accessor_impl_gen::post_init()
    {
    }
    
    int grib_accessor_impl_gen::notify_change(grib_accessor* observed)
    {
        /* Default behaviour is to notify creator */
        return grib_action_notify_change(creator, as_accessor(), observed);
    }
    
    void grib_accessor_impl_gen::update_size(size_t /* s */)
    {
        grib_context_log(context, GRIB_LOG_ERROR,
                            "Accessor %s [%s] must implement 'update_size'", name, /* a->cclass-> */name);
        Assert(0 == 1);
    }
    
    size_t grib_accessor_impl_gen::preferred_size(int /* from_handle */)
    {
        return length;
    }
    
    void grib_accessor_impl_gen::resize(size_t /* new_size */)
    {
    }
    
    int grib_accessor_impl_gen::nearest_smaller_value (double /* val */, double* /* nearest */)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    grib_accessor* grib_accessor_impl_gen::next_accessor(int mod)
    {
        grib_accessor* ga_next = NULL;
        
        if (next) {
            ga_next = next;
        }
        else if (parent->owner) {
            // TODO - Whilst porting the C code there may be instances of parent->owner
            //        that do not have a C++ implementation so we have to support the
            //        'C' way of doing it too!
            if(eccodes::grib_accessor_impl_gen* ga_impl = eccodes::get_grib_accessor_impl(parent->owner); ga_impl) {
                ga_next = ga_impl->next_accessor(0);
            }
            else if(parent->owner->cclass) {
                ga_next = parent->owner->cclass->next(parent->owner, 0);
            }
        }

        return ga_next;
    }
    
    int grib_accessor_impl_gen::compare(grib_accessor_impl* ga_impl)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_double_element(size_t /* i */, double* /* val */)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_float_element(size_t /* i */, float* /* val */)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_double_element_set(const size_t* /* index_array */, size_t /* len */, double* /* val_array */)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_float_element_set(const size_t* /* index_array */, size_t /* len */, float* /* val_array */)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::unpack_double_subarray(double* /* val */, size_t /* start */, size_t /* len */)
    {
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::clear()
    {
        unsigned char* buf = accessor_handle()->buffer->data;
        long length        = grib_byte_count(as_accessor());
        long offset        = grib_byte_offset(as_accessor());

        memset(buf + offset, 0, length);

        return GRIB_SUCCESS;
    }
    
    grib_accessor_impl* grib_accessor_impl_gen::make_clone(grib_section* /* s */, int* err)
    {
        *err = GRIB_NOT_IMPLEMENTED;
        return NULL;
    }
    
    // Helpers

    grib_handle* grib_accessor_impl_gen::accessor_handle()
    {
        if (parent == NULL) {
            return h;
        }
        else {
            return parent->h;
        }
    }

}
