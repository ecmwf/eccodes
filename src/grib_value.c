/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
/***************************************************************************
 * Jean Baptiste Filippi - 01.11.2005
 * Enrico Fucile                                                           *
 ***************************************************************************/
#include "grib_api_internal.h"

GRIB_INLINE static int grib_inline_strcmp(const char* a,const char* b)
{
    if (*a != *b) return 1;
    while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
    return (*a==0 && *b==0) ? 0 : 1;
}

int grib_set_expression(grib_handle* h, const char* name,grib_expression* e)
{
    grib_accessor* a = grib_find_accessor(h, name);
    int ret = GRIB_SUCCESS;

    if(a){

        if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
            return GRIB_READ_ONLY;

        ret = grib_pack_expression(a, e);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }
        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_set_expression_internal(grib_handle* h, const char* name,grib_expression* e)
{
    grib_accessor* a = grib_find_accessor(h, name);

    int ret = GRIB_SUCCESS;
    if(a){
        ret = grib_pack_expression(a, e);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }
        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_set_long_internal(grib_handle* h, const char* name, long val)
{
    grib_context* c=h->context;
    int ret = GRIB_SUCCESS;
    grib_accessor* a =NULL;
    size_t l = 1;

    a = grib_find_accessor(h, name);

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_long_internal %s=%ld\n",name,(long)val);

    if(a){
        ret = grib_pack_long(a, &val, &l);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }

        grib_context_log(c,GRIB_LOG_ERROR,"unable to set %s=%ld as long (%s)",
                name,val,grib_get_error_message(ret));
        return ret;
    }

    grib_context_log(c,GRIB_LOG_ERROR,"unable to find accessor %s",name);
    return GRIB_NOT_FOUND;
}

int grib_set_long(grib_handle* h, const char* name, long val)
{
    int ret = GRIB_SUCCESS;
    grib_accessor* a =NULL;
    size_t l = 1;

    a = grib_find_accessor(h, name);

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_long %s=%ld\n",name,(long)val);

    if(a){
        if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
            return GRIB_READ_ONLY;

        ret = grib_pack_long(a, &val, &l);
        if(ret == GRIB_SUCCESS)
            return grib_dependency_notify_change(a);

        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_set_double_internal(grib_handle* h, const char* name, double val)
{
    int ret = GRIB_SUCCESS;
    grib_accessor* a =NULL;
    size_t l = 1;

    a = grib_find_accessor(h, name);

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_double_internal %s=%g\n",name,val);

    if(a){
        ret = grib_pack_double(a, &val, &l);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }

        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set %s=%g as double (%s)",
                name,val,grib_get_error_message(ret));
        return ret;
    }

    grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",name);
    return GRIB_NOT_FOUND;
}

typedef struct grib_key_err grib_key_err;
struct grib_key_err {
    char* name;
    int err;
    grib_key_err* next;
};

int grib_copy_namespace(grib_handle* dest, const char* name, grib_handle* src)
{
    int *err=0;
    int type;
    size_t len;
    char *sval = NULL;
    unsigned char *uval = NULL;
    double *dval = NULL;
    long *lval = NULL;
    grib_key_err* key_err=NULL;
    grib_key_err* first=NULL;
    int todo=1,count=0;

    grib_keys_iterator* iter=NULL;

    if (!dest || !src) return GRIB_NULL_HANDLE;

    iter=grib_keys_iterator_new(src,0,(char*)name);

    if (!iter) {
        grib_context_log(src->context,GRIB_LOG_ERROR,"grib_copy_namespace: unable to get iterator for %s",name );
        return GRIB_INTERNAL_ERROR;
    }

    while(grib_keys_iterator_next(iter))  {
        grib_key_err* k=(grib_key_err*)grib_context_malloc_clear(src->context,sizeof(grib_key_err));
        k->err=GRIB_NOT_FOUND;
        k->name=grib_context_strdup(src->context,grib_keys_iterator_get_name(iter));
        if (key_err==NULL) {
            key_err=k;
            first=k;
        } else {
            key_err->next=k;
            key_err=key_err->next;
        }
    }

    count=0;
    todo=1;
    while (todo && count<4) {
        grib_accessor* a=NULL;
        key_err=first;
        while(key_err)  {
            char* key=key_err->name;
            err=&(key_err->err);

            if (*err==GRIB_SUCCESS) {
                key_err=key_err->next;
                continue;
            }

            if ((a=grib_find_accessor(dest,key))==NULL) {
                key_err->err=GRIB_NOT_FOUND;
                key_err=key_err->next;
                continue;
            }

            if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
                key_err->err=GRIB_SUCCESS;
                key_err=key_err->next;
                continue;
            }

            if ( grib_is_missing(src,key,err) && *err == 0 && (*err=grib_set_missing(dest,key))) {
                if ( *err!=GRIB_SUCCESS && *err!=GRIB_NOT_FOUND) return *err;
                key_err=key_err->next;
                continue;
            }

            if ((*err=grib_get_native_type(dest,key,&type))!=GRIB_SUCCESS) {
                if ( *err!=GRIB_SUCCESS && *err!=GRIB_NOT_FOUND) return *err;
                key_err=key_err->next;
                continue;
            }

            if((*err = grib_get_size(src,key,&len)) != GRIB_SUCCESS)  return *err;

            switch (type) {
            case GRIB_TYPE_STRING:
                len=512;
                sval = (char*)grib_context_malloc(src->context,len*sizeof(char));

                if((*err = grib_get_string(src,key,sval,&len)) != GRIB_SUCCESS)
                    return *err;

                if((*err = grib_set_string(dest,key,sval,&len)) != GRIB_SUCCESS)
                    return *err;

                grib_context_free(src->context,sval);
                break;

            case GRIB_TYPE_LONG:
                lval = (long*)grib_context_malloc(src->context,len*sizeof(long));

                if((*err = grib_get_long_array(src,key,lval,&len)) != GRIB_SUCCESS)
                    return *err;

                if((*err = grib_set_long_array(dest,key,lval,len)) != GRIB_SUCCESS)
                    return *err;

                grib_context_free(src->context,lval);
                break;

            case GRIB_TYPE_DOUBLE:
                dval = (double*)grib_context_malloc(src->context,len*sizeof(double));

                if((*err = grib_get_double_array(src,key,dval,&len)) != GRIB_SUCCESS)
                    return *err;

                if((*err = grib_set_double_array(dest,key,dval,len)) != GRIB_SUCCESS)
                    return *err;

                break;

            case GRIB_TYPE_BYTES:
                if (len==0) len=512;
                uval = (unsigned char *)grib_context_malloc(src->context,len*sizeof(unsigned char));

                if((*err = grib_get_bytes(src,key,uval,&len)) != GRIB_SUCCESS)
                    return *err;

                if((*err = grib_get_bytes(dest,key,uval,&len)) != GRIB_SUCCESS)
                    return *err;

                grib_context_free(src->context,uval);

                break;

            default:
                break;
            }
            key_err=key_err->next;

        }
        count++;
        key_err=first;
        todo=0;
        while(key_err)  {
            if (key_err->err==GRIB_NOT_FOUND) {
                todo=1;
                break;
            }
            key_err=key_err->next;
        }
    }
    grib_keys_iterator_delete(iter);
    key_err=first;
    while (key_err) {
        grib_key_err* next=key_err->next;
        grib_context_free(src->context,key_err->name);
        grib_context_free(src->context,key_err);
        key_err=next;
    }

    return *err;
}

int grib_set_double(grib_handle* h, const char* name, double val)
{
    int ret = GRIB_SUCCESS;
    grib_accessor* a =NULL;
    size_t l = 1;

    a = grib_find_accessor(h, name);

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_double %s=%g\n",name,val);

    if(a){

        if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
            return GRIB_READ_ONLY;

        ret = grib_pack_double(a, &val, &l);
        if(ret == GRIB_SUCCESS)
            return grib_dependency_notify_change(a);

        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_set_string_internal(grib_handle* h, const char* name,
        const char* val, size_t *length)
{
    int ret = GRIB_SUCCESS;
    grib_accessor* a =NULL;

    a = grib_find_accessor(h, name);

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_string_internal %s=%s\n",name,val);

    if(a){
        ret = grib_pack_string(a, val, length);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }

        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set %s=%s as string (%s)",
                name,val,grib_get_error_message(ret));
        return ret;
    }

    grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",name);
    return GRIB_NOT_FOUND;
}

int grib_set_string(grib_handle* h, const char* name, const char* val, size_t *length)
{
    int ret=0;
    grib_accessor* a = NULL;

    /* Second order doesn't have a proper representation for constant fields.
       So best not to do the change of packing type.
       Use strncmp to catch all flavours of second order packing e.g. grid_second_order_boustrophedonic
     */
    if (!grib_inline_strcmp(name,"packingType") && !strncmp(val,"grid_second_order", 17)) {
        long bitsPerValue=0;
        size_t numCodedVals = 0;
        grib_get_long(h,"bitsPerValue",&bitsPerValue);
        if (bitsPerValue==0) {
            if (h->context->debug) {
                printf("ECCODES DEBUG grib_set_string packingType: Constant field cannot be encoded in second order. Packing not changed\n");
            }
            return 0;
        }

        /* GRIB-883: check if there are enough coded values */
        ret = grib_get_size(h, "codedValues", &numCodedVals);
        if (ret == GRIB_SUCCESS && numCodedVals < 3) {
            if (h->context->debug) {
                printf("ECCODES DEBUG grib_set_string packingType: not enough coded values for second order. Packing not changed\n");
            }
            return 0;
        }
    }

    a = grib_find_accessor(h, name);

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_string %s=|%s|\n",name,val);

    if(a)
    {
        if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
            return GRIB_READ_ONLY;

        ret=grib_pack_string(a, val, length);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }
        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_set_string_array(grib_handle* h, const char* name, const char** val, size_t length)
{
    int ret=0;
    grib_accessor* a;

    a = grib_find_accessor(h, name);

    grib_context_log(h->context,GRIB_LOG_DEBUG,"grib_set_string %s=%s\n",name,val);

    if(a)
    {
        if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
            return GRIB_READ_ONLY;

        ret=grib_pack_string_array(a, val, &length);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }
        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_set_bytes_internal(grib_handle* h, const char* name, const unsigned char* val, size_t *length)
{
    int ret = GRIB_SUCCESS;
    grib_accessor* a =NULL;

    a = grib_find_accessor(h, name);

    if(a){
        ret = grib_pack_bytes(a, val, length);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }

        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set %s=%ld as bytes (%s)",
                name,val,grib_get_error_message(ret));
        return ret;
    }

    grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",name);
    return GRIB_NOT_FOUND;
}

int grib_set_bytes(grib_handle* h, const char* name, const unsigned char* val, size_t *length)
{
    int ret=0;
    grib_accessor* a = grib_find_accessor(h, name);

    if(a)
    {
        /* if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) */
        /* return GRIB_READ_ONLY; */

        ret=grib_pack_bytes(a, val, length);
        if(ret == GRIB_SUCCESS){
            return grib_dependency_notify_change(a);
        }
        return ret;
    }
    return GRIB_NOT_FOUND;
}

int grib_clear(grib_handle* h, const char* name)
{
    int ret=0;
    grib_accessor* a =NULL;

    a = grib_find_accessor(h, name);

    if(a) {
        if (a->length ==0) return 0;
        if ((ret=grib_pack_zero(a)) != GRIB_SUCCESS)
            grib_context_log(h->context,GRIB_LOG_ERROR,"unable to clear %s (%s)",
                    name,grib_get_error_message(ret));
        return ret;
    }

    /*grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",name);*/
    return GRIB_NOT_FOUND;
}

int grib_set_missing_internal(grib_handle* h, const char* name)
{
    int ret=0;
    grib_accessor* a =NULL;

    a = grib_find_accessor(h, name);

    if(a) {
        if(a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
            ret=grib_pack_missing(a);
            if(ret == GRIB_SUCCESS)
                return grib_dependency_notify_change(a);
        } else
            ret=GRIB_VALUE_CANNOT_BE_MISSING;

        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set %s=missing (%s)",
                name,grib_get_error_message(ret));
        return ret;
    }

    grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",name);
    return GRIB_NOT_FOUND;
}

int grib_set_missing(grib_handle* h, const char* name)
{
    int ret=0;
    grib_accessor* a =NULL;

    a = grib_find_accessor(h, name);

    if(a) {
        if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
            return GRIB_READ_ONLY;

        if(a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) {
            if (h->context->debug) printf("ECCODES DEBUG grib_set_missing %s\n",name);

            ret=grib_pack_missing(a);
            if(ret == GRIB_SUCCESS)
                return grib_dependency_notify_change(a);
        } else ret=GRIB_VALUE_CANNOT_BE_MISSING;

        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set %s=missing (%s)",
                name,grib_get_error_message(ret));
        return ret;
    }

    grib_context_log(h->context,GRIB_LOG_ERROR,"unable to find accessor %s",name);
    return GRIB_NOT_FOUND;
}

int grib_is_missing_long(grib_accessor* a,long x)
{
    int ret = ( a==NULL || (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)) && (x==GRIB_MISSING_LONG) ? 1 : 0;
    return ret;
}
int grib_is_missing_double(grib_accessor* a,double x)
{
    int ret = ( a==NULL || (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)) && (x==GRIB_MISSING_DOUBLE) ? 1 : 0;
    return ret;
}

int grib_is_missing_string(grib_accessor* a, unsigned char* x, size_t len)
{
    /* For a string value to be missing, every character has to be */
    /* all 1's (i.e. 0xFF) */
    int ret;
    size_t i=0;

    if (len==0) return 0;
    ret=1;
    for (i=0;i<len;i++) {
        if (x[i] != 0xFF ) {
            ret=0;
            break;
        }
    }

    ret = ( a==NULL || ( (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) && ret==1 )) ? 1 : 0;
    return ret;
}

int grib_accessor_is_missing(grib_accessor* a,int* err)
{
    *err=GRIB_SUCCESS;
    if(a)
    {
        if(a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)
            return grib_is_missing_internal(a);
        else
            return 0;
    }
    else
    {
        *err=GRIB_NOT_FOUND;
        return 1;
    }
}

int grib_is_missing(grib_handle* h, const char* name,int* err)
{
    grib_accessor* a = grib_find_accessor(h, name);
    return grib_accessor_is_missing(a,err);
}

/* Return true if the given key exists (is defined) in our grib message */
int grib_is_defined(grib_handle* h, const char* name)
{
    grib_accessor* a = grib_find_accessor(h, name);
    return (a ? 1 : 0);
}

int grib_set_flag(grib_handle *h,const char* name,unsigned long flag)
{
    grib_accessor* a=grib_find_accessor(h,name);

    if (!a) return GRIB_NOT_FOUND;

    a->flags|=flag;

    return GRIB_SUCCESS;
}

static int _grib_set_double_array_internal(grib_handle* h,grib_accessor* a,
        const double* val, size_t buffer_len,size_t *encoded_length,int check)
{
    if(a) {
        int err = _grib_set_double_array_internal(h,a->same,val,buffer_len,encoded_length,check);

        if(check && (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY))
            return GRIB_READ_ONLY;

        if(err == GRIB_SUCCESS)
        {
            size_t len = buffer_len - *encoded_length;
            if(len) {
                err  = grib_pack_double(a, val + *encoded_length, &len);
                *encoded_length += len;
                if(err == GRIB_SUCCESS){
                    return grib_dependency_notify_change(a);
                }
            }
            else {
                grib_get_size(h,a->name,encoded_length);
                err = GRIB_WRONG_ARRAY_SIZE;
            }
        }

        return err;

    }
    else {
        return GRIB_SUCCESS;
    }
}

static int _grib_set_double_array(grib_handle* h, const char* name,
        const double* val, size_t length,int check)
{
    size_t encoded = 0;
    grib_accessor* a = grib_find_accessor(h, name);
    int err=0;

    if (!a) return GRIB_NOT_FOUND ;
    if (name[0]=='/' || name[0]=='#' ) {
        if(check && (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY))
            return GRIB_READ_ONLY;
        err=grib_pack_double(a, val, &length);
        encoded=length;
    } else err=_grib_set_double_array_internal(h,a,val,length,&encoded,check);

    if(err == GRIB_SUCCESS && length > encoded)
        err = GRIB_ARRAY_TOO_SMALL;

    if(err == GRIB_SUCCESS)
        return grib_dependency_notify_change(a);

    return err;
}

int grib_set_double_array_internal(grib_handle* h, const char* name, const double* val, size_t length)
{
    int ret=0;

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_double_array_internal key=%s %ld values\n",name, (long)length);

    if (length==0) {
        grib_accessor* a = grib_find_accessor(h, name);
        ret=grib_pack_double(a, val , &length);
    } else {
        ret=_grib_set_double_array(h,name,val,length,0);
    }

    if (ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set double array %s (%s)",
                name,grib_get_error_message(ret));
    /*if (h->context->debug) printf("ECCODES DEBUG grib_set_double_array_internal key=%s --DONE\n",name);*/
    return ret;
}

static int __grib_set_double_array(grib_handle* h, const char* name, const double* val, size_t length, int check)
{
    double v=val[0];
    int constant,i;

    if (h->context->debug)
        printf("ECCODES DEBUG grib_set_double_array key=%s %ld values\n",name,(long)length);

    if (length==0) {
        grib_accessor* a = grib_find_accessor(h, name);
        return grib_pack_double(a, val , &length);
    }

    /*second order doesn't have a proper representation for constant fields
      the best is not to do the change of packing type if the field is constant
     */
    if (!strcmp(name,"values") || !strcmp(name,"codedValues")) {
        double missingValue;
        int ret=0;

        ret=grib_get_double(h,"missingValue",&missingValue);
        if (ret) missingValue=9999;

        v=missingValue;
        constant=1;
        for (i=0;i<length;i++) {
            if (val[i]!=missingValue) {
                if (v==missingValue) {
                    v=val[i];
                } else if (v!=val[i]) {
                    constant=0;
                    break;
                }
            }
        }
        if (constant) {
            char packingType[50]={0,};
            size_t slen=50;

            grib_get_string(h,"packingType",packingType,&slen);
            if (!strcmp(packingType,"grid_second_order") ||
                    !strcmp(packingType,"grid_second_order_no_SPD") ||
                    !strcmp(packingType,"grid_second_order_SPD1") ||
                    !strcmp(packingType,"grid_second_order_SPD2") ||
                    !strcmp(packingType,"grid_second_order_SPD3")
            ) {
                ret = 0;
                slen=11; /*length of 'grid_simple' */
                if (h->context->debug) {
                    printf("ECCODES DEBUG __grib_set_double_array: Cannot use second order packing for constant fields. Using simple packing\n");
                }
                ret = grib_set_string(h,"packingType","grid_simple",&slen);
                if (ret != GRIB_SUCCESS) {
                    if (h->context->debug) {
                        printf("ECCODES DEBUG __grib_set_double_array: could not switch to simple packing!\n");
                    }
                }
            }
        }
    }

    return _grib_set_double_array(h,name,val,length,check);
}

int grib_set_force_double_array(grib_handle* h, const char* name, const double* val, size_t length)
{
    /* GRIB-285: Same as grib_set_double_array but allows setting of READ-ONLY keys like codedValues */
    /* Use with great caution!! */
    return __grib_set_double_array(h, name, val, length, /*check=*/0);
}

int grib_set_double_array(grib_handle* h, const char* name, const double* val, size_t length)
{
    return __grib_set_double_array(h, name, val, length, /*check=*/1);
}

static int _grib_set_long_array_internal(grib_handle* h,grib_accessor* a,const long* val, size_t buffer_len,size_t *encoded_length,int check)
{
    if(a) {
        int err = _grib_set_long_array_internal(h,a->same,val,buffer_len,encoded_length,check);

        if(check && (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY))
            return GRIB_READ_ONLY;

        if(err == GRIB_SUCCESS)
        {
            size_t len = buffer_len - *encoded_length;
            if(len) {
                err  = grib_pack_long(a, val + *encoded_length, &len);
                *encoded_length += len;
            }
            else {
                grib_get_size(h,a->name,encoded_length);
                err = GRIB_WRONG_ARRAY_SIZE;
            }
        }

        return err;

    }
    else {
        return GRIB_SUCCESS;
    }
}

static int _grib_set_long_array(grib_handle* h, const char* name, const long* val, size_t length,int check)
{
    size_t encoded = 0;
    grib_accessor* a = grib_find_accessor(h, name);
    int err =0;

    if (!a) return GRIB_NOT_FOUND ;
    if (name[0]=='/' || name[0]=='#' ) {
        if(check && (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY))
            return GRIB_READ_ONLY;
        err=grib_pack_long(a, val, &length);
        encoded=length;
    } else err=_grib_set_long_array_internal(h,a,val,length,&encoded,check);

    if(err == GRIB_SUCCESS && length > encoded)
        err = GRIB_ARRAY_TOO_SMALL;

    if(err == GRIB_SUCCESS)
        return grib_dependency_notify_change(a);

    return err;
}

int grib_set_long_array_internal(grib_handle* h, const char* name, const long* val, size_t length)
{
    int ret=_grib_set_long_array(h,name,val,length,0);
    if (ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set long array %s (%s)",
                name,grib_get_error_message(ret));
    return ret;
}

int grib_set_long_array(grib_handle* h, const char* name, const long* val, size_t length)
{
    return _grib_set_long_array(h,name,val,length,1);
}

int grib_get_long_internal(grib_handle* h, const char* name, long* val)
{
    int ret = grib_get_long(h,name,val);

    if(ret!=GRIB_SUCCESS) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as long (%s)",
                name, grib_get_error_message(ret));
    }

    return ret;
}

int grib_is_in_dump(grib_handle* h, const char* name)
{
    grib_accessor* a=grib_find_accessor(h, name);
    if (a!=NULL && (a->flags & GRIB_ACCESSOR_FLAG_DUMP))
        return 1;
    else
        return 0;
}

int grib_attributes_count(grib_accessor* a, size_t* size)
{
    if (a) {
        *size=0;
        while (a->attributes[*size]!=NULL) {(*size)++;}
        return GRIB_SUCCESS;
    }

    return GRIB_NOT_FOUND;
}

int grib_get_long(grib_handle* h, const char* name, long* val)
{
    size_t length = 1;
    grib_accessor* a = NULL;
    grib_accessors_list* al=NULL;
    int ret=0;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_unpack_long(al->accessor, val , &length);
        grib_context_free(h->context,al);
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        ret=grib_unpack_long(a, val , &length);
    }
    return ret;
}

int grib_get_double_internal(grib_handle* h, const char* name, double* val)
{
    int ret = grib_get_double(h,name,val);

    if(ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as double (%s)",
                name, grib_get_error_message(ret));

    return ret;
}

int grib_get_double(grib_handle* h, const char* name, double* val)
{
    size_t length = 1;
    grib_accessor* a = NULL;
    grib_accessors_list* al=NULL;
    int ret=0;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_unpack_double(al->accessor, val , &length);
        grib_context_free(h->context,al);
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        ret=grib_unpack_double(a, val , &length);
    }
    return ret;
}

int grib_get_double_element_internal(grib_handle* h, const char* name, int i,double* val)
{
    int ret = grib_get_double_element(h,name,i,val);

    if(ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as double element (%s)",
                name, grib_get_error_message(ret));

    return ret;
}

int grib_get_double_element(grib_handle* h, const char* name, int i, double* val)
{
    grib_accessor* act = grib_find_accessor(h, name);

    if(act)
        return grib_unpack_double_element(act, i,val);

    return GRIB_NOT_FOUND;
}

int grib_points_get_values(grib_handle* h, grib_points* points, double* val)
{
    int i,ret;
    grib_accessor* a=NULL;

    a=grib_find_accessor(h,"values");

    for (i=0;i<points->n_groups;i++) {
        ret=grib_unpack_double_subarray(a,val,points->group_start[i],points->group_len[i]);
        if (ret) return ret;
        val+=points->group_len[i];
    }
    return 0;
}

int grib_get_double_elements(grib_handle* h, const char* name, int* i, long len,double* val)
{
    double* values=0;
    int ret=0;
    size_t size=0;
    int j=0;
    grib_accessor* act =NULL;

    act= grib_find_accessor(h, name);

    ret=_grib_get_size(h,act,&size);

    if (ret!=GRIB_SUCCESS) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"grib_get_double_elements: cannot get size of %s\n",name);
        return ret;
    }

    values=(double*)grib_context_malloc( h->context,size * sizeof(double));

    if (!values) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"grib_get_double_elements: unable to allocate %ld bytes\n",
                size*sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    ret = grib_unpack_double(act, values, &size);

    for (j=0;j<len;j++) val[j]=values[i[j]];

    grib_context_free(h->context,values);

    return GRIB_SUCCESS;
}

int grib_get_string_internal(grib_handle* h, const char* name, char* val, size_t *length)
{
    int ret = grib_get_string(h,name,val,length);

    if(ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as string (%s)",
                name, grib_get_error_message(ret));

    return ret;
}

int grib_get_string(grib_handle* h, const char* name, char* val, size_t *length)
{
    grib_accessor* a = NULL;
    grib_accessors_list* al=NULL;
    int ret=0;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_unpack_string(al->accessor, val, length);
        grib_context_free(h->context,al);
        return ret;
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        return grib_unpack_string(a, val , length);
    }
}

int grib_get_bytes_internal(grib_handle* h, const char* name, unsigned char* val, size_t *length)
{
    int ret = grib_get_bytes(h,name,val,length);

    if(ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as bytes (%s)",
                name, grib_get_error_message(ret));

    return ret;
}

int grib_get_bytes(grib_handle* h, const char* name, unsigned char* val, size_t *length)
{
    int err=0;
    grib_accessor* act = grib_find_accessor(h, name);
    err = act? grib_unpack_bytes(act, val, length) : GRIB_NOT_FOUND;
    if(err) grib_context_log(h->context,GRIB_LOG_ERROR,
            "grib_get_bytes_internal %s failed %s",  name, grib_get_error_message(err));
    return err;
}

int grib_get_native_type(grib_handle* h, const char* name,int* type)
{
    grib_accessors_list* al=NULL;
    grib_accessor* a =NULL;
    *type = GRIB_TYPE_UNDEFINED;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        *type = grib_accessor_get_native_type(al->accessor);
        grib_context_free(h->context,al);
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        *type = grib_accessor_get_native_type(a);
    }

    return GRIB_SUCCESS;
}

const char* grib_get_accessor_class_name(grib_handle* h, const char* name)
{
    grib_accessor* act = grib_find_accessor(h, name);
    return act?act->cclass->name:NULL;
}

int _grib_get_double_array_internal(grib_handle* h,grib_accessor* a,double* val, size_t buffer_len,size_t *decoded_length)
{
    if(a) {
        int err = _grib_get_double_array_internal(h,a->same,val,buffer_len,decoded_length);

        if ( err == GRIB_SUCCESS )
        {
            size_t len = buffer_len - *decoded_length;
            err  = grib_unpack_double(a, val + *decoded_length, &len);
            *decoded_length += len;
        }

        return err;
    }
    else {
        return GRIB_SUCCESS;
    }
}

int grib_get_double_array_internal(grib_handle* h, const char* name, double* val, size_t *length)
{
    int ret = grib_get_double_array(h,name,val,length);

    if(ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as double array (%s)",
                name, grib_get_error_message(ret));

    return ret;
}

int grib_get_double_array(grib_handle* h, const char* name, double* val, size_t *length)
{
    size_t len = *length;
    grib_accessor* a = NULL;
    grib_accessors_list* al=NULL;
    int ret=0;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_accessors_list_unpack_double(al,val,length);
        grib_context_free(h->context,al);
        return ret;
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        if (name[0] == '#') {
            return grib_unpack_double(a, val , length);
        } else {
            *length = 0;
            return _grib_get_double_array_internal(h,a,val,len,length);
        }
    }
}

int _grib_get_string_length(grib_accessor* a, size_t* size)
{
    size_t s=0;

    *size = 0;
    while(a) {
        s = grib_string_length(a);
        if (s > *size) *size=s;
        a = a->same;
    }
    (*size)+=1;

    return GRIB_SUCCESS;
}

int grib_get_string_length(grib_handle* h, const char* name,size_t* size)
{
    grib_accessor* a = grib_find_accessor(h, name);
    if(!a) return GRIB_NOT_FOUND;

    return _grib_get_string_length(a,size);
}

int _grib_get_size(grib_handle* h, grib_accessor* a,size_t* size)
{
    long count=0;
    int err=0;

    if(!a) return GRIB_NOT_FOUND;

    *size = 0;
    while(a) {
        if (err==0)
        {
            err=grib_value_count(a,&count);
            if (err) return err;
            *size += count;
        }
        a = a->same;
    }
    return GRIB_SUCCESS;
}

int grib_get_size(grib_handle* h, const char* name,size_t* size)
{
    grib_accessor* a =NULL;
    grib_accessors_list* al=NULL;
    int ret=0;
    *size=0;

    if (name[0] == '/') {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_accessors_list_value_count(al,size);
        grib_context_free(h->context,al);
        return ret;
    } else {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        if (name[0]=='#') {
            long count=*size;
            ret=grib_value_count(a,&count);
            *size=count;
            return ret;
        } else return _grib_get_size(h, a,size);
    }
}

int grib_get_length(grib_handle* h, const char* name, size_t* length)
{
    return grib_get_string_length(h, name, length);
}

int grib_get_count(grib_handle* h, const char* name,size_t* size)
{
    grib_accessor* a = grib_find_accessor(h, name);
    if(!a) return GRIB_NOT_FOUND;

    *size = 0;
    while(a) {
        (*size)++;
        a = a->same;
    }
    return GRIB_SUCCESS;
}

int grib_get_offset(grib_handle* h, const char* key,size_t* val)
{
    grib_accessor* act = grib_find_accessor(h, key);
    if(act) {
        *val = (size_t)grib_byte_offset(act);
        return GRIB_SUCCESS;
    }
    return GRIB_NOT_FOUND;
}

int _grib_get_string_array_internal(grib_handle* h,grib_accessor* a,char** val, size_t buffer_len,size_t *decoded_length)
{
    if(a) {
        int err = _grib_get_string_array_internal(h,a->same,val,buffer_len,decoded_length);

        if(err == GRIB_SUCCESS)
        {
            size_t len = buffer_len - *decoded_length;
            err  = grib_unpack_string_array(a, val + *decoded_length, &len);
            *decoded_length += len;
        }

        return err;

    } else {
        return GRIB_SUCCESS;
    }
}

int grib_get_string_array(grib_handle* h, const char* name, char** val, size_t *length)
{
    size_t len = *length;
    grib_accessor* a = NULL;
    grib_accessors_list* al=NULL;
    int ret=0;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_accessors_list_unpack_string(al,val,length);
        grib_context_free(h->context,al);
        return ret;
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        if (name[0]=='#') {
            return grib_unpack_string_array(a, val , length);
        } else {
            *length = 0;
            return _grib_get_string_array_internal(h,a,val,len,length);
        }
    }
}

int _grib_get_long_array_internal(grib_handle* h,grib_accessor* a,long* val, size_t buffer_len,size_t *decoded_length)
{
    if(a) {
        int err = _grib_get_long_array_internal(h,a->same,val,buffer_len,decoded_length);

        if(err == GRIB_SUCCESS)
        {
            size_t len = buffer_len - *decoded_length;
            err  = grib_unpack_long(a, val + *decoded_length, &len);
            *decoded_length += len;
        }

        return err;

    } else {
        return GRIB_SUCCESS;
    }
}

int grib_get_long_array_internal(grib_handle* h, const char* name, long* val, size_t *length)
{
    int ret = grib_get_long_array(h,name,val,length);

    if(ret!=GRIB_SUCCESS)
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "unable to get %s as long array (%s)",
                name, grib_get_error_message(ret));

    return ret;
}

int grib_get_long_array(grib_handle* h, const char* name, long* val, size_t *length)
{
    size_t len = *length;
    grib_accessor* a = NULL;
    grib_accessors_list* al=NULL;
    int ret=0;

    if (name[0] == '/' ) {
        al=grib_find_accessors_list(h,name);
        if (!al) return GRIB_NOT_FOUND;
        ret=grib_accessors_list_unpack_long(al,val,length);
        grib_context_free(h->context,al);
        return ret;
    } else  {
        a=grib_find_accessor(h, name);
        if(!a) return GRIB_NOT_FOUND;
        if (name[0]=='#') {
            return grib_unpack_long(a, val , length);
        } else {
            *length = 0;
            return _grib_get_long_array_internal(h,a,val,len,length);
        }
    }
    return ret;
}

static void grib_clean_key_value(grib_context* c,grib_key_value_list* kv)
{
    if (kv->long_value) grib_context_free(c,kv->long_value);
    kv->long_value=NULL;
    if (kv->double_value) grib_context_free(c,kv->double_value);
    kv->double_value=NULL;
    if (kv->string_value) grib_context_free(c,kv->string_value);
    kv->string_value=NULL;
    if (kv->namespace_value) grib_key_value_list_delete(c,kv->namespace_value);
    kv->namespace_value=NULL;
    kv->error=0;
    kv->has_value=0;
    kv->size=0;
}

static int grib_get_key_value(grib_handle* h,grib_key_value_list* kv)
{
    int ret=0;
    size_t size=0;
    grib_keys_iterator* iter=NULL;
    grib_key_value_list* list=NULL;

    if (kv->has_value) grib_clean_key_value(h->context,kv);

    ret=grib_get_size(h,kv->name,&size);
    if (ret) {
        kv->error=ret;
        return ret;
    }
    if (size==0) size=512;

    switch (kv->type) {
    case GRIB_TYPE_LONG:
        kv->long_value=(long*)grib_context_malloc_clear(h->context,size*sizeof(long));
        ret=grib_get_long_array(h,kv->name,kv->long_value,&size);
        kv->error=ret;
        break;
    case GRIB_TYPE_DOUBLE:
        kv->double_value=(double*)grib_context_malloc_clear(h->context,size*sizeof(double));
        ret=grib_get_double_array(h,kv->name,kv->double_value,&size);
        kv->error=ret;
        break;
    case GRIB_TYPE_STRING:
        grib_get_string_length(h,kv->name,&size);
        kv->string_value=(char*)grib_context_malloc_clear(h->context,size*sizeof(char));
        ret=grib_get_string(h,kv->name,kv->string_value,&size);
        kv->error=ret;
        break;
    case GRIB_TYPE_BYTES:
        kv->string_value=(char*)grib_context_malloc_clear(h->context,size*sizeof(char));
        ret=grib_get_bytes(h,kv->name,(unsigned char*)kv->string_value,&size);
        kv->error=ret;
        break;
    case GRIB_NAMESPACE:
        iter=grib_keys_iterator_new(h,0,(char*)kv->name);
        list=(grib_key_value_list*)grib_context_malloc_clear(h->context,sizeof(grib_key_value_list));
        kv->namespace_value=list;
        while(grib_keys_iterator_next(iter))
        {
            list->name=grib_keys_iterator_get_name(iter);
            ret=grib_get_native_type(h,list->name,&(list->type));
            ret=grib_get_key_value(h,list);
            list->next=(grib_key_value_list*)grib_context_malloc_clear(h->context,sizeof(grib_key_value_list));
            list=list->next;
        }
        grib_keys_iterator_delete(iter);
        break;

    default:
        ret=grib_get_native_type(h,kv->name,&(kv->type));
        ret=grib_get_key_value(h,kv);
        break;
    }
    kv->has_value=1;
    return ret;
}

grib_key_value_list* grib_key_value_list_clone(grib_context* c,grib_key_value_list* list)
{
    grib_key_value_list* next=list;
    grib_key_value_list* the_clone=(grib_key_value_list*)grib_context_malloc_clear(c,sizeof(grib_key_value_list));
    grib_key_value_list* p=the_clone;

    while (next && next->name) {
        p->name=grib_context_strdup(c,next->name);
        p->type=next->type;
        next=next->next;
    }
    return the_clone;
}

void grib_key_value_list_delete(grib_context* c,grib_key_value_list* kvl)
{
    grib_key_value_list* next=kvl;
    grib_key_value_list* p=NULL;
    while (next) {
        p=next->next;
        if (next->type == GRIB_NAMESPACE)
            grib_key_value_list_delete(c,next->namespace_value);

        grib_clean_key_value(c,next);
        grib_context_free(c,next);
        next=p;
    }
}

int grib_get_key_value_list(grib_handle* h,grib_key_value_list* list)
{
    int ret=0;
    grib_key_value_list* kvl=list;
    while (kvl) {
        ret=grib_get_key_value(h,kvl);
        kvl=kvl->next;
    }
    return ret;
}

int grib_get_values(grib_handle* h,grib_values* args,size_t count)
{
    int ret=0;
    int i=0;

    for (i=0; i<count; i++) {
        char buff[1024]={0,};
        size_t len=sizeof(buff)/sizeof(*buff);

        if (!args[i].name) {args[i].error=GRIB_INVALID_ARGUMENT;continue;}

        if ( args[i].type == 0 ) {
            args[i].error=grib_get_native_type(h,args[i].name,&(args[i].type));
            if (args[i].error != GRIB_SUCCESS) ret=args[i].error;
        }

        switch(args[i].type) {
        case GRIB_TYPE_LONG:
            args[i].error = grib_get_long(h,args[i].name,&(args[i].long_value));
            if (args[i].error != GRIB_SUCCESS) ret=args[i].error;
            break;

        case GRIB_TYPE_DOUBLE:
            args[i].error = grib_get_double(h,args[i].name,&(args[i].double_value));
            if (args[i].error != GRIB_SUCCESS) ret=args[i].error;
            break;

        case GRIB_TYPE_STRING:
            args[i].error = grib_get_string(h,args[i].name,buff,&len);
            args[i].string_value=strdup(buff);
            if (args[i].error != GRIB_SUCCESS) ret=args[i].error;
            break;

        default:
            args[i].error = grib_get_string(h,args[i].name,buff,&len);
            args[i].string_value=strdup(buff);
            if (args[i].error != GRIB_SUCCESS) ret=args[i].error;
            break;
        }
    }

    return ret;
}

int grib_set_values(grib_handle* h,grib_values* args,size_t count)
{
    int i, error=0;
    int err=0;
    size_t len;
    int more = 1;
    int stack = h->values_stack++;

    Assert(h->values_stack < MAX_SET_VALUES - 1);

    h->values[stack]       = args;
    h->values_count[stack] = count;

    for(i = 0; i < count ; i++)
        args[i].error = GRIB_NOT_FOUND;

    while(more)
    {
        more = 0;
        for(i = 0; i < count ; i++)
        {
            if(args[i].error != GRIB_NOT_FOUND)
                continue;

            switch(args[i].type)
            {
            case GRIB_TYPE_LONG:
                error = grib_set_long(h,args[i].name,args[i].long_value);
                args[i].error=error;
                if(args[i].error == GRIB_SUCCESS) more = 1;
                break;

            case GRIB_TYPE_DOUBLE:
                args[i].error = grib_set_double(h,args[i].name,args[i].double_value);
                if(args[i].error == GRIB_SUCCESS) more = 1;
                break;

            case GRIB_TYPE_STRING:
                len = strlen(args[i].string_value);
                args[i].error = grib_set_string(h,args[i].name,args[i].string_value,&len);
                if(args[i].error == GRIB_SUCCESS) more = 1;
                break;

            case GRIB_TYPE_MISSING:
                args[i].error = grib_set_missing(h,args[i].name);
                if(args[i].error == GRIB_SUCCESS) more = 1;
                break;

            default:
                grib_context_log(h->context,GRIB_LOG_ERROR, "grib_set_values[%d] %s invalid type %d",  i, args[i].name, args[i].type);
                args[i].error =  GRIB_INVALID_ARGUMENT;
                break;
            }
            /*if (args[i].error != GRIB_SUCCESS)
         grib_context_log(h->context,GRIB_LOG_ERROR,"unable to set %s (%s)", 
                          args[i].name,grib_get_error_message(args[i].error)); */
        }
    }

    h->values[stack]       = NULL;
    h->values_count[stack] = 0;

    h->values_stack--;

    for(i = 0; i < count ; i++) {
        if(args[i].error != GRIB_SUCCESS)
        {
            grib_context_log(h->context,GRIB_LOG_ERROR,
                    "grib_set_values[%d] %s (%d) failed: %s",  i, args[i].name, args[i].type,
                    grib_get_error_message(args[i].error));
            err= err==GRIB_SUCCESS ? args[i].error : err;
        }
    }

    return err;
}

int grib_get_nearest_smaller_value(grib_handle* h, const char* name,
        double val,double* nearest)
{
    grib_accessor* act = grib_find_accessor(h, name);
    Assert(act);
    return grib_nearest_smaller_value(act,val,nearest);
}

void grib_print_values(grib_values* values,int count)
{
    int i;
    for(i = 0; i < count ; i++)
    {
        printf("%s = ",values[i].name);
        switch(values[i].type)
        {
        case GRIB_TYPE_LONG: printf("%ld",values[i].long_value); break;
        case GRIB_TYPE_DOUBLE: printf("%g",values[i].double_value); break;
        case GRIB_TYPE_STRING: printf("%s",values[i].string_value); break;
        }
        printf("\n");
    }
}

int grib_values_check(grib_handle* h, grib_values* values, int count)
{
    int i=0;
    long long_value;
    double double_value;
    unsigned char ubuff[1024]={0,};
    char buff[1024]={0,};
    size_t len=1024;

    for (i=0; i<count; i++) {

        if ( values[i].type == 0 ) {
            values[i].error=GRIB_INVALID_TYPE;
            return values[i].error;
        }

        switch(values[i].type) {
        case GRIB_TYPE_LONG:
            values[i].error = grib_get_long(h,values[i].name,&long_value);
            if (values[i].error != GRIB_SUCCESS) return values[i].error;
            if (long_value != values[i].long_value) {
                values[i].error=GRIB_VALUE_DIFFERENT;
                return values[i].error;
            }
            break;

        case GRIB_TYPE_DOUBLE:
            values[i].error = grib_get_double(h,values[i].name,&double_value);
            if (values[i].error != GRIB_SUCCESS) return values[i].error;
            if (double_value != values[i].double_value) {
                values[i].error=GRIB_VALUE_DIFFERENT;
                return values[i].error;
            }
            break;

        case GRIB_TYPE_STRING:
            values[i].error = grib_get_string(h,values[i].name,buff,&len);
            if (values[i].error != GRIB_SUCCESS) return values[i].error;
            if (strcmp(values[i].string_value,buff)) {
                values[i].error=GRIB_VALUE_DIFFERENT;
                return values[i].error;
            }
            break;

        case GRIB_TYPE_BYTES:
            values[i].error = grib_get_bytes(h,values[i].name,ubuff,&len);
            if (values[i].error != GRIB_SUCCESS) return values[i].error;
            if (memcmp(values[i].string_value,ubuff,len)) {
                values[i].error=GRIB_VALUE_DIFFERENT;
                return values[i].error;
            }
            break;

        default:
            values[i].error=GRIB_INVALID_TYPE;
            return values[i].error;
        }
    }

    return 0;
}

int grib_key_equal(grib_handle* h1,grib_handle* h2,const char* key,int type,int *err)
{
    double d1,d2;
    long l1,l2;
    char s1[500]={0,};
    char s2[500]={0,};
    size_t len1,len2;

    if (  type != GRIB_TYPE_DOUBLE &&
            type != GRIB_TYPE_LONG   &&
            type != GRIB_TYPE_STRING    ) {
        *err=grib_get_native_type(h1,key,&type);
    }
    switch (type) {
    case GRIB_TYPE_DOUBLE:
        *err=grib_get_double( h1,key,&d1);
        *err=grib_get_double( h2,key,&d2);
        if (d1 != d2) return 0;
        break;
    case GRIB_TYPE_LONG:
        *err=grib_get_long( h1,key,&l1);
        *err=grib_get_long( h2,key,&l2);
        if (l1 != l2) return 0;
        break;
    default:
        len1=sizeof(s1)/sizeof(*s1);
        len2=sizeof(s2)/sizeof(*s2);
        *err=grib_get_string( h1,key,s1,&len1);
        *err=grib_get_string( h2,key,s2,&len2);
        if (grib_inline_strcmp(s1,s2)) return 0;
        break;
    }
    return 1;
}
