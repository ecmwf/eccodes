/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
#include <ctype.h>

int grib_lookup_long_from_array(grib_context* gc,grib_loader* loader,const char* name,long* lvalue)
{
    char* strvalue=0;
    size_t size=0;
    char* pstr=NULL;
    int retval;
    grib_associative_array* ar;
    grib_runtime_type* rt=grib_runtimetype_new(gc);
    int type=0;

    ar=(grib_associative_array*)loader->data;

    if ((retval=grib_associative_array_get(ar,(char*)name,&rt)) != GRIB_ARRAY_SUCCESS)
        return GRIB_NOT_FOUND;

    grib_runtimetype_get_type(rt,&type);

    switch (type) {
    case GRIB_RUNTIMETYPE_LONG:
        grib_runtimetype_get_long(rt,&lvalue,&size);
        printf("lookup: getting long %ld\n",*lvalue);
        break;
    case GRIB_RUNTIMETYPE_CHAR:
        *lvalue = 0;
        grib_runtimetype_get_char(rt,&strvalue,&size);
        printf("lookup: getting char %s\n",strvalue);
        pstr=strvalue;
        while(*pstr)
        {
            *lvalue <<= 8;
            *lvalue |=  *pstr++;
        }
        break;
    default:
        printf("lookup: wrong type (%d)\n",type);
        return GRIB_NOT_FOUND;
        break;
    }

    return GRIB_SUCCESS;
}

int grib_init_accessor_from_array(grib_loader* loader,grib_accessor* ga,grib_arguments* default_value)
{
    grib_handle* h = grib_handle_of_accessor(ga);
    int retval =GRIB_SUCCESS;
    char* strvalue;
    size_t len=0;
    int type=0;
    double* dvalue=0;
    long* lvalue=0;
    long lval=0;
    double dval=0;
    int i=0;
    grib_associative_array* ar=NULL;
    grib_runtime_type* value=NULL;

    ar=(grib_associative_array*)loader->data;

    grib_context_log(h->context,GRIB_LOG_DEBUG, "Initialize  %s",   ga->name);

    retval= grib_associative_array_get(ar,ga->name,&value);

    if((retval != GRIB_ARRAY_SUCCESS) && default_value) {
        grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to default value", ga->name);
        grib_pack_expression(ga,grib_arguments_get_expression(h,default_value,0));
        return GRIB_SUCCESS;
    }

    if(ga->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
    {
        grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting  %s  ignored (read only)",   ga->name);
        return GRIB_SUCCESS;
    }

    if (retval != GRIB_ARRAY_SUCCESS) {
        grib_context_log(h->context,GRIB_LOG_DEBUG,
                "Grib array error %d",retval);
        if ((ga->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)) {
            grib_context_log(h->context,GRIB_LOG_DEBUG,
                    "Setting  %s  ignored (optional or can be missing)",   ga->name);
            return GRIB_SUCCESS;
        } else {
            grib_context_log(h->context,GRIB_LOG_ERROR, "%s  required",   ga->name);
            return retval;
        }
    }

    if (value && grib_runtimetype_get_type(value,&type) == GRIB_RUNTIMETYPE_SUCCESS) {
        if (type == GRIB_RUNTIMETYPE_CHAR ) {
            grib_runtimetype_get_char(value,&strvalue,&len);
            switch(grib_accessor_get_native_type(ga))  {

            case GRIB_TYPE_STRING:
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to string %s",
                        ga->name, strvalue);
                grib_set_string_internal(h,ga->name,strvalue,&len);
                break;

            case GRIB_TYPE_LONG:
                if (!strcmp(strvalue,"MISSING")) lval=GRIB_MISSING_LONG;
                else lval=atol(strvalue);
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to long %d", ga->name, lval);
                grib_set_long_internal(h,ga->name,lval);
                break;

            case GRIB_TYPE_DOUBLE:
                if (!strcmp(strvalue,"MISSING")) dval=GRIB_MISSING_DOUBLE;
                else if (sscanf(strvalue,"%lg",&dval)!=1) {
                    grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to set %s wrong value format",   ga->name);
                }
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to double %lg (%s)", ga->name, dval,strvalue);
                grib_set_double_internal(h,ga->name,dval);
                break;

            case GRIB_TYPE_BYTES:
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to string %s",
                        ga->name, strvalue);
                grib_set_bytes_internal(h,ga->name,(unsigned char*)strvalue,&len);
                break;

            case GRIB_TYPE_LABEL:
                break;

            default:
                grib_context_log(h->context,GRIB_LOG_ERROR, "Setting %s, cannot establish type %d [%s]"
                        , ga->name,grib_accessor_get_native_type(ga),ga->creator->cclass->name);
                break;
            }
        } else {

            if (grib_runtimetype_get_double(value,&dvalue,&len) == GRIB_RUNTIMETYPE_SUCCESS) {
                switch(grib_accessor_get_native_type(ga))  {

                case GRIB_TYPE_LONG:
                    lvalue=(long*)malloc(sizeof(long)*len);
                    Assert(lvalue);
                    for (i=0;i<len;i++) lvalue[i]=(long)dvalue[i];
                    grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting long array %s",ga->name);
                    grib_set_long_array(h,ga->name,lvalue,len);
                    free(lvalue);
                    break;

                case GRIB_TYPE_DOUBLE:
                    grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting double array %s",ga->name);
                    grib_set_double_array(h,ga->name,dvalue,len);
                    break;

                default:
                    grib_context_log(h->context,GRIB_LOG_ERROR, "Setting array %s, wrong type [%d]"
                            , ga->name,grib_accessor_get_native_type(ga));
                    break;
                }

            } else
                grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to set %s to double, wrong type (%d)",
                        ga->name,type);
        }
#if 0
        switch(grib_accessor_get_native_type(ga))
        {
        case GRIB_TYPE_STRING:
            if (grib_runtimetype_get_char(value,&strvalue,&strvalue_size) == GRIB_RUNTIMETYPE_SUCCESS) {
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to string %s",
                        ga->name, strvalue);
                len=strlen(strvalue);
                grib_set_string_internal(h,ga->name,strvalue,&len);
            } else
                grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to set %s to string, wrong type",
                        ga->name);
            break;

        case GRIB_TYPE_LONG:
            if (grib_runtimetype_get_long(value,&lval,&len) == GRIB_RUNTIMETYPE_SUCCESS) {
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to long %d", ga->name, lval);
                grib_set_long_internal(h,ga->name,*lval);
            } else
                grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to set %s to long, wrong type",
                        ga->name);
            break;

        case GRIB_TYPE_DOUBLE:
            if (grib_runtimetype_get_double(value,&dval,&len) == GRIB_RUNTIMETYPE_SUCCESS) {
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to long %d", ga->name, dval);
                grib_set_double_internal(h,ga->name,*dval);
            } else
                grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to set %s to double, wrong type",
                        ga->name);
            break;

        case GRIB_TYPE_BYTES:
            if (grib_runtimetype_get_char(value,&strvalue,&len) == GRIB_RUNTIMETYPE_SUCCESS) {
                grib_context_log(h->context,GRIB_LOG_DEBUG, "Setting %s to string %s",
                        ga->name, strvalue);
                grib_set_bytes_internal(h,ga->name,(unsigned char*)strvalue,&len);
            } else
                grib_context_log(h->context,GRIB_LOG_ERROR, "Unable to set %s to string, wrong type",
                        ga->name);
            break;

        case GRIB_TYPE_LABEL:
            break;

        default:
            grib_context_log(h->context,GRIB_LOG_ERROR, "Setting %s, cannot establish type %d [%s]"
                    , ga->name,grib_accessor_get_native_type(ga),ga->creator->cclass->name);
            break;
        }
#endif

    }

    return retval;
}
