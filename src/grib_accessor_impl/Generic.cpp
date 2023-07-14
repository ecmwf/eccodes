#include "Generic.h"
//#include <iostream>

namespace eccodes {

Generic::Generic(grib_section* p, grib_action* creator, const long len, grib_arguments* arg)
{
//    std::cout << "Generic() addr=0x" << std::hex << this << '\n';

    // att is default initialised at this point...
    att.name                = creator->name;
    att.name_space          = creator->name_space;
    att.context             = p->h->context;
    att.creator             = creator;

    if (p->block->last) { att.offset = grib_get_next_position_offset(p->block->last); }
    else {
        if (p->owner)   { att.offset = p->owner->offset; }
        else            { att.offset = 0; }
    }

    att.parent              = p;
    att.flags               = creator->flags;

    att.all_names[0]        = creator->name;
    att.all_name_spaces[0]  = creator->name_space;
    att.set                 = creator->set;

    // init() functionality
    grib_action* act = (grib_action*)(att.creator);
    if (att.flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        att.length = 0;
        if (!att.vvalue)
            att.vvalue = (grib_virtual_value*)grib_context_malloc_clear(att.context, sizeof(grib_virtual_value));
        att.vvalue->type   = grib_accessor_get_native_type(*this);
        att.vvalue->length = len;
        if (act->default_value != NULL) {
            const char* p = 0;
            size_t s_len  = 1;
            long l;
            int ret = 0;
            double d;
            char tmp[1024];
            grib_expression* expression = grib_arguments_get_expression(grib_handle_of_accessor(*this), act->default_value, 0);
            int type                    = grib_expression_native_type(grib_handle_of_accessor(*this), expression);
            switch (type) {
                case GRIB_TYPE_DOUBLE:
                    grib_expression_evaluate_double(grib_handle_of_accessor(*this), expression, &d);
                    grib_pack_double(*this, &d, &s_len);
                    break;

                case GRIB_TYPE_LONG:
                    grib_expression_evaluate_long(grib_handle_of_accessor(*this), expression, &l);
                    grib_pack_long(*this, &l, &s_len);
                    break;

                default:
                    s_len = sizeof(tmp);
                    p     = grib_expression_evaluate_string(grib_handle_of_accessor(*this), expression, tmp, &s_len, &ret);
                    if (ret != GRIB_SUCCESS) {
                        grib_context_log(att.context, GRIB_LOG_ERROR, "Unable to evaluate %s as string", att.name);
                        Assert(0);
                    }
                    s_len = strlen(p) + 1;
                    grib_pack_string(*this, p, &s_len);
                    break;
            }
        }
    }
    else {
        att.length = len;
    }

}

Generic::~Generic()
{
//    std::cout << "~Generic() addr=0x" << std::hex << this << '\n';
    grib_dependency_remove_observed(*this);
    grib_dependency_remove_observer(*this);
    if (att.vvalue != NULL) {
        free(att.vvalue);
        att.vvalue = NULL;
    }
}

}
