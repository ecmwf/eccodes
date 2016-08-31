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
 *   Jean Baptiste Filippi - 01.11.2005
 *   Enrico Fucile
 *                                                                         *
 ***************************************************************************/

#include "grib_api_internal.h"

void  grib_accessor_dump(grib_accessor* a, grib_dumper* f)
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        if(c->dump)
        {
            c->dump(a, f);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

int grib_pack_missing(grib_accessor* a)
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->pack_missing)
        {
            return c->pack_missing(a);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_zero(grib_accessor* a)
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->clear)
        {
            return c->clear(a);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_is_missing_internal(grib_accessor* a)
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->is_missing)
        {
            return c->is_missing(a);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_double(grib_accessor* a, const double* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->pack_double)
        {
            return c->pack_double(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_expression(grib_accessor* a, grib_expression *e )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->pack_expression)
        {
            return c->pack_expression(a,e);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_string(grib_accessor* a, const char* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (string) %s",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?v:"(null)");*/
    while(c)
    {

        if(c->pack_string)
        {
            return c->pack_string(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_string_array(grib_accessor* a, const char** v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (string) %s",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?v:"(null)");*/
    while(c)
    {
        if(c->pack_string_array)
        {
            return c->pack_string_array(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_long(grib_accessor* a,const long* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (long) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->pack_long)
        {
            return c->pack_long(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_pack_bytes(grib_accessor* a, const unsigned char* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (bytes) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->pack_bytes)
        {
            return c->pack_bytes(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_unpack_bytes(grib_accessor* a, unsigned char* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (bytes)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
    while(c)
    {
        if(c->unpack_bytes)
        {
            return c->unpack_bytes(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_unpack_double_subarray(grib_accessor* a, double* v, size_t start, size_t len )
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        if(c->unpack_double_subarray)
        {
            return c->unpack_double_subarray(a,v,start,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_unpack_double(grib_accessor* a, double* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (double)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
    while(c)
    {
        if(c->unpack_double)
        {
            return c->unpack_double(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_unpack_double_element(grib_accessor* a, size_t i ,double* v )
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        if(c->unpack_double_element)
        {
            return c->unpack_double_element(a,i,v);
        }
        c = c->super ? *(c->super) : NULL;
    }
    return GRIB_NOT_IMPLEMENTED;
}

int grib_unpack_string(grib_accessor* a, char* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /* grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (string)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
    while(c)
    {
        if(c->unpack_string)
        {
            return c->unpack_string(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_unpack_string_array(grib_accessor* a, char** v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        if(c->unpack_string_array)
        {
            return c->unpack_string_array(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_accessors_list_unpack_long(grib_accessors_list* al,long* val,size_t* buffer_len)
{
    int err=GRIB_SUCCESS;
    size_t unpacked_len=0;
    size_t len=0;

    while (al && err==GRIB_SUCCESS ) {
        len=*buffer_len-unpacked_len;
        err=grib_unpack_long(al->accessor, val + unpacked_len, &len);
        unpacked_len += len;
        al=al->next;
    }

    *buffer_len=unpacked_len;
    return err;
}

int grib_accessors_list_unpack_double(grib_accessors_list* al,double* val,size_t* buffer_len)
{
    int err=GRIB_SUCCESS;
    size_t unpacked_len=0;
    size_t len=0;

    while (al && err==GRIB_SUCCESS ) {
        len=*buffer_len-unpacked_len;
        err=grib_unpack_double(al->accessor, val + unpacked_len, &len);
        unpacked_len += len;
        al=al->next;
    }

    *buffer_len=unpacked_len;
    return err;
}

int grib_accessors_list_unpack_string(grib_accessors_list* al,char** val,size_t* buffer_len)
{
    int err=GRIB_SUCCESS;
    size_t unpacked_len=0;
    size_t len=0;

    while (al && err==GRIB_SUCCESS ) {
        len=*buffer_len-unpacked_len;
        err=grib_unpack_string_array(al->accessor, val + unpacked_len, &len);
        unpacked_len += len;
        al=al->next;
    }

    *buffer_len=unpacked_len;
    return err;
}

int grib_unpack_long(grib_accessor* a,long* v, size_t *len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is unpacking (long)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
    while(c)
    {
        if(c->unpack_long)
        {
            return c->unpack_long(a,v,len);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

long grib_accessor_get_native_type(grib_accessor* a)
{
    grib_accessor_class *c = NULL;
    if (a)  c = a->cclass;

    while(c)
    {
        if(c->get_native_type)
            return c->get_native_type(a);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

long grib_get_next_position_offset(grib_accessor* a)
{
    grib_accessor_class *c = NULL;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is checking next (long)",(a->parent->owner)?(a->parent->owner->name):"root", a->name ); */
    if (a)  c = a->cclass;

    while(c)
    {
        if(c->next_offset)
            return c->next_offset(a);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

long grib_string_length(grib_accessor* a)
{
    grib_accessor_class *c = NULL;
    if (a)  c = a->cclass;

    while(c)
    {
        if(c->string_length)
            return c->string_length(a);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

long grib_byte_offset(grib_accessor* a)
{
    grib_accessor_class *c = NULL;
    if (a)  c = a->cclass;

    while(c)
    {
        if(c->byte_offset)
            return c->byte_offset(a);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

long grib_byte_count(grib_accessor* a)
{
    grib_accessor_class *c = NULL;
    if (a)  c = a->cclass;

    while(c)
    {
        if(c->byte_count)
            return c->byte_count(a);
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_value_count(grib_accessor* a,long* count)
{
    grib_accessor_class *c = NULL;
    int err = 0;
    if (a) c = a->cclass;

    while(c)
    {
        if(c->value_count) {
            err = c->value_count(a, count);
            return err;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

int grib_accessors_list_value_count(grib_accessors_list* al,size_t* count)
{
    long lcount=0;
    *count=0;
    while (al) {
        grib_value_count(al->accessor,&lcount);
        *count+=lcount;
        al=al->next;
    }
    return 0;
}

int grib_accessor_notify_change(grib_accessor* a,grib_accessor* changed)
{
    grib_accessor_class *c = NULL;
    if (a)  c = a->cclass;

    while(c)
    {
        if(c->notify_change)
            return c->notify_change(a,changed);
        c = c->super ? *(c->super) : NULL;
    }
    if (a) printf("notify_change not implemented for %s %s\n",a->cclass->name, a->name);
    Assert(0);
    return 0;
}

/* For this one, ALL init are called */

static void init_accessor(grib_accessor_class* c,grib_accessor* a, const long len, grib_arguments* args)
{
    if(c) {
        grib_accessor_class *s = c->super ? *(c->super) : NULL;
        if(!c->inited)
        {
            if(c->init_class) c->init_class(c);
            c->inited = 1;
        }
        init_accessor(s,a,len,args);
        if(c->init) c->init(a,len, args);
    }
}

void grib_init_accessor(grib_accessor* a, const long len, grib_arguments* args)
{
    init_accessor(a->cclass,a,len,args);
}

#if 0
static void post_init_accessor(grib_accessor_class* c,grib_accessor* a)
{
    if(c) {
        grib_accessor_class *s = c->super ? *(c->super) : NULL;
        post_init_accessor(s,a);
        if(c->post_init) c->post_init(a);
    }
}
#endif

/* For this one, ALL destroy are called */

void grib_accessor_delete(grib_context *ct, grib_accessor* a)
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        grib_accessor_class *s = c->super ? *(c->super) : NULL;
        /*grib_context_log(ct,GRIB_LOG_DEBUG,"destroy %s ==> %s",c->name,a->name);*/
        if(c->destroy) {
            c->destroy(ct,a);
        }
        c = s;
    }
    grib_context_free(ct,a);
}

grib_accessor* grib_accessor_clone(grib_accessor* a,grib_section* s,int* err)
{
    grib_accessor_class *c = a->cclass;
    grib_context* ct=a->context;
    while(c)
    {
        grib_accessor_class *super = c->super ? *(c->super) : NULL;
        grib_context_log(ct,GRIB_LOG_DEBUG,"clone %s ==> %s",c->name,a->name);
        if(c->make_clone) {
            return c->make_clone(a,s,err);
        }
        c = super;
    }
    return 0;
}

void grib_update_size(grib_accessor* a, size_t len )
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (double) %g",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->update_size)
        {
            c->update_size(a,len);
            return;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
}

int grib_nearest_smaller_value(grib_accessor* a,double val,double* nearest)
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        if(c->nearest_smaller_value)
        {
            return c->nearest_smaller_value(a,val,nearest);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}


size_t grib_preferred_size(grib_accessor* a,int from_handle)
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (long) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->preferred_size)
        {
            return c->preferred_size(a,from_handle);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

grib_accessor* grib_next_accessor(grib_accessor* a)
{
    grib_accessor_class *c = a->cclass;
    while(c)
    {
        if(c->next)
        {
            return c->next(a,1);
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return 0;
}

void grib_resize(grib_accessor* a,size_t new_size)
{
    grib_accessor_class *c = a->cclass;
    /*grib_context_log(a->context, GRIB_LOG_DEBUG, "(%s)%s is packing (long) %d",(a->parent->owner)?(a->parent->owner->name):"root", a->name ,v?(*v):0); */
    while(c)
    {
        if(c->resize)
        {
            c->resize(a,new_size);
            return ;
        }
        c = c->super ? *(c->super) : NULL;
    }
    Assert(0);
    return;
}

int grib_compare_accessors(grib_accessor* a1,grib_accessor* a2,int compare_flags)
{
    int ret=0;
    long type1=0;
    long type2=0;
    int type_mismatch=0;
    grib_accessor_class *c1=NULL;

    if ((compare_flags & GRIB_COMPARE_NAMES) && strcmp(a1->name,a2->name))
        return GRIB_NAME_MISMATCH;

    if ( compare_flags & GRIB_COMPARE_TYPES ) {
        type1=grib_accessor_get_native_type( a1 );
        type2=grib_accessor_get_native_type( a2 );

        type_mismatch = type1 != type2 ? 1 : 0;
    }

    ret=GRIB_UNABLE_TO_COMPARE_ACCESSORS;
    c1 = a1->cclass;
    while(c1)
    {

        if(c1->compare)
        {
            ret = c1->compare(a1, a2);
            break;
        }
        c1 = c1->super ? *(c1->super) : NULL;
    }

    if (ret == GRIB_VALUE_MISMATCH && type_mismatch)
        ret = GRIB_TYPE_AND_VALUE_MISMATCH;

    return ret;
}

const char* grib_get_type_name(int type)
{
    switch(type)
    {
    case GRIB_TYPE_LONG:    return "long"; break;
    case GRIB_TYPE_STRING:  return "string"; break;
    case GRIB_TYPE_BYTES:   return "bytes"; break;
    case GRIB_TYPE_DOUBLE:  return "double"; break;
    case GRIB_TYPE_LABEL:   return "label"; break;
    case GRIB_TYPE_SECTION: return "section"; break;
    }

    return "unknown";
}

/*
int grib_accessor_clear_attributes(grib_accessor* a)
{
    int id;
    for (id=0;id<MAX_ACCESSOR_ATTRIBUTES;id++) {
        a->attributes[id]=NULL;
    }
    return 0;
}
 */

int grib_accessor_add_attribute(grib_accessor* a,grib_accessor* attr,int nest_if_clash)
{
    int id=0;
    int idx=0;
    grib_accessor* same=NULL;
    grib_accessor* aloc=a;

    same=_grib_accessor_get_attribute(a,attr->name,&id);

    if (same) {
        if (nest_if_clash==0) return GRIB_ATTRIBUTE_CLASH;
        aloc=same;
    }

    for (id=0;id<MAX_ACCESSOR_ATTRIBUTES;id++) {
        if (aloc->attributes[id] == NULL) {
            /* attr->parent=a->parent; */
            aloc->attributes[id]=attr;
            attr->parent_as_attribute=aloc;
            if (aloc->same)
                attr->same=_grib_accessor_get_attribute(aloc->same,attr->name,&idx);

            grib_context_log(a->context,GRIB_LOG_DEBUG,"added attribute %s->%s",a->name,attr->name);
            return GRIB_SUCCESS;
        }
    }
    return GRIB_TOO_MANY_ATTRIBUTES;
}

int grib_accessor_replace_attribute(grib_accessor* a,grib_accessor* attr)
{
    int id=0;
    int idx=0;
    if (_grib_accessor_get_attribute(a,attr->name,&id) != NULL) {
        grib_accessor_delete(a->context,a->attributes[id]);
        a->attributes[id]=attr;
        attr->parent_as_attribute=a;
        if (a->same)
            attr->same=_grib_accessor_get_attribute(a->same,attr->name,&idx);
    } else {
        grib_accessor_add_attribute(a,attr,0);
    }
    return GRIB_SUCCESS;
}

int grib_accessor_delete_attribute(grib_accessor* a,const char* name)
{
    int id=0;
    if (_grib_accessor_get_attribute(a,name,&id) != NULL) {
        grib_accessor_delete(a->context,a->attributes[id]);
        a->attributes[id]=NULL;
        return GRIB_SUCCESS;
    } else {
        return GRIB_NOT_FOUND;
    }
}

grib_accessor* grib_accessor_get_attribute_by_index(grib_accessor* a,int index)
{
    if (index<MAX_ACCESSOR_ATTRIBUTES) return a->attributes[index];

    return NULL;
}

const char* grib_accessor_get_name(grib_accessor* a) {
    return a->name;
}

grib_accessor* _grib_accessor_get_attribute(grib_accessor* a,const char* name,int* index)
{
    int i=0;
    while (a->attributes[i] && i<MAX_ACCESSOR_ATTRIBUTES) {
        if (!strcmp(a->attributes[i]->name,name)) {
            *index=i;
            return a->attributes[i];
        }
        i++;
    }
    return NULL;
}

int grib_accessor_has_attributes(grib_accessor* a)
{
    return a->attributes[0] ? 1 : 0 ;
}

grib_accessor* grib_accessor_get_attribute(grib_accessor* a,const char* name)
{
    int index=0;
    char* p=0;
    char* basename=NULL;
    char* attribute_name=NULL;
    grib_accessor* acc=NULL;
    p=(char*)name;
    while ( *(p+1) != '\0' && ( *p != '-' || *(p+1)!= '>' ) ) p++;
    if (*(p+1) == '\0') {
        return _grib_accessor_get_attribute(a,name,&index);
    } else {
        size_t size=p-name;
        attribute_name=p+2;
        basename=(char*)grib_context_malloc_clear(a->context,size+1);
        basename=(char*)memcpy(basename,name,size);
        acc=_grib_accessor_get_attribute(a,basename,&index);
        grib_context_free(a->context,basename);
        if (acc) return grib_accessor_get_attribute(acc,attribute_name);
        else return NULL;
    }
}

grib_accessors_list* grib_accessors_list_create(grib_context* c)
{
    return (grib_accessors_list*)grib_context_malloc_clear(c,sizeof(grib_accessors_list));
}

void grib_accessors_list_push(grib_accessors_list* al,grib_accessor* a)
{
    grib_accessors_list* last;
    grib_context* c=a->context;

    last=grib_accessors_list_last(al);
    if (last && last->accessor) {
        last->next=(grib_accessors_list*)grib_context_malloc_clear(c,sizeof(grib_accessors_list));
        last->next->accessor=a;
        last->next->prev=last;
        al->last=last->next;
    } else {
        al->accessor=a;
        al->last=al;
    }
}

grib_accessors_list* grib_accessors_list_last(grib_accessors_list* al)
{
    /*grib_accessors_list* last=al;*/
    /*grib_accessors_list* next=al->next;*/

    /*
    while(next) {
      last=next;
      next=last->next;
    }
     */
    return al->last;
}

grib_accessors_list* grib_accessors_list_find(grib_accessors_list* al,grib_accessor* a)
{
    grib_accessors_list* last=al;
    grib_accessors_list* next=al->next;

    while(next) {
        if (last->accessor == a) return last;
        last=next;
        next=last->next;
    }

    return NULL;
}

void grib_accessors_list_delete(grib_context* c,grib_accessors_list* al)
{
    grib_accessors_list* tmp;

    while (al) {
        tmp=al->next;
        /*grib_accessor_delete(c, al->accessor);*/
        grib_context_free(c,al);
        al=tmp;
    }
}
