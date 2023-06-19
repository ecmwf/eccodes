#include "grib_accessor_impl_gen.h"

namespace eccodes {  
    grib_accessor_impl_gen::grib_accessor_impl_gen(grib_section* p, grib_action* creator)
    {
        name       = creator->name;
        name_space = creator->name_space;

        all_names[0]       = creator->name;
        all_name_spaces[0] = creator->name_space;

        creator  = creator;
        context  = p->h->context;
        h        = NULL;
        next     = NULL;
        previous = NULL;
        parent   = p;
        length   = 0;
        offset   = 0;
        flags    = creator->flags;
        set      = creator->set;

        if (p->block->last) {
            offset = grib_get_next_position_offset(p->block->last);
        }
        else {
            if (p->owner) {
                offset = p->owner->offset;
            }
            else
                offset = 0;
        }
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
        pack_buffer in_buffer(context, const_double_view{val, *len});

        // Try double, and fall back to long if not available...
        // Note: the C code has a special-case for the "codetable" accessor, but this
        //       is handled automatically here by the virtual function implementation
        if(ret = pack_double(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_long(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}

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
        pack_buffer in_buffer(context, const_long_view{val, *len});

        // Try long, and fall back to double if not available...
        if(ret = pack_long(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_double(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}

        grib_context_log(context, GRIB_LOG_ERROR, "Should not grib_pack %s as long", name);
        return GRIB_NOT_IMPLEMENTED;
    }
    
    int grib_accessor_impl_gen::pack_string(const char* val, size_t* len)
    {
        int ret = GRIB_NOT_IMPLEMENTED;
        pack_buffer in_buffer(context, const_char_view{val, *len});

        // Try string, then double, then long...
        if(ret = pack_string(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_double(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}
        if(ret = pack_long(in_buffer, *len); ret != GRIB_NOT_IMPLEMENTED) { return ret;}

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
    grib_handle* hand = grib_handle_of_accessor(as_accessor());

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
    
    int grib_accessor_impl_gen::unpack_bytes(unsigned char* /* val */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_double(double* /* val */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_float(float* /* val */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_long(long* /* val */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_string(char* /* val */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_string_array(char** /* v */, size_t* /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
   
    int grib_accessor_impl_gen::value_count(long* count)
    {
        *count = 1;
        return 0;
    }
    
    void grib_accessor_impl_gen::dump(grib_dumper* dumper)
    {
        switch (native_type_def) {
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
    
    int grib_accessor_impl_gen::notify_change()
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    void grib_accessor_impl_gen::update_size(size_t /* s */)
    {
    }
    
    size_t grib_accessor_impl_gen::preferred_size(int /* from_handle */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    void grib_accessor_impl_gen::resize(size_t /* new_size */)
    {
    }
    
    int grib_accessor_impl_gen::nearest_smaller_value (double /* val */, double* /* nearest */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    grib_accessor_impl* grib_accessor_impl_gen::next_accessor(int /* mod */)
    {
        return nullptr; // TO DO
    }
    
    int grib_accessor_impl_gen::compare()
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_double_element(size_t /* i */, double* /* val */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_float_element(size_t /* i */, float* /* val */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_double_element_set(const size_t* /* index_array */, size_t /* len */, double* /* val_array */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_float_element_set(const size_t* /* index_array */, size_t /* len */, float* /* val_array */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::unpack_double_subarray(double* /* val */, size_t /* start */, size_t /* len */)
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    int grib_accessor_impl_gen::clear()
    {
        return GRIB_NOT_IMPLEMENTED; // TO DO
    }
    
    grib_accessor_impl* grib_accessor_impl_gen::make_clone(grib_section* /* s */, int* /* err */)
    {
        return nullptr; // TO DO
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
