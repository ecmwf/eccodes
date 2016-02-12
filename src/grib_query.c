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
 *  Enrico Fucile
 *  Jean Baptiste Filippi - 01.11.2005                                    *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

#if 0
GRIB_INLINE static int strcmp(const char* a,const char* b) {
    if (*a != *b) return 1;
    while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
    return (*a==0 && *b==0) ? 0 : 1;
}
#endif

static int matching(grib_accessor* a,const char* name,const char* name_space)
{
    int i = 0;
    while(i < MAX_ACCESSOR_NAMES)
    {
        if(a->all_names[i] == 0) return 0;

        if((strcmp(name,a->all_names[i]) == 0) &&
                ((name_space == NULL) || (a->all_name_spaces[i] != NULL &&
                        strcmp(a->all_name_spaces[i],name_space) == 0))
        )
            return 1;
        i++;
    }
    return 0;
}

static grib_accessor* search(grib_section* s,const char* name,const char* name_space)
{
    grib_accessor* match = NULL;

    grib_accessor* a = s ? s->block->first : NULL;
    grib_accessor* b=NULL;

    if(!a || !s) return NULL;

    while(a)
    {
        grib_section* sub = a->sub_section;

        if(matching(a,name,name_space))
            match = a;

        if((b = search(sub,name,name_space) ) != NULL )
            match = b;

        a = a->next;
    }

    return match;
}

static void rebuild_hash_keys (grib_handle* h,grib_section* s)
{
    grib_accessor* a = s ? s->block->first : NULL;

    while(a)
    {
        grib_section* sub = a->sub_section;
        int i=0;
        int id=-1;
        const char *p;
        Assert(h == grib_handle_of_accessor(a));

        while(i<MAX_ACCESSOR_NAMES && ((p = a->all_names[i]) != NULL)) {
            if(*p != '_')   {
                id=grib_hash_keys_get_id(a->context->keys,p);

                if (a->same != a && i==0) {
                    a->same=grib_handle_of_accessor(a)->accessors[id];
                    grib_handle_of_accessor(a)->accessors[id]=a;
                    Assert(a->same != a);
                }

            }
            i++;
        }
        rebuild_hash_keys(h,sub);
        a = a->next;
    }
}

static grib_accessor* search_and_cache(grib_handle* h, const char* name,const char *the_namespace);

static grib_accessor* _search_and_cache(grib_handle* h, const char* name,const char *the_namespace)
{
    if (h->use_trie)
    {
        grib_accessor* a = NULL;
        int id=-1;

        if (h->trie_invalid && h->kid == NULL)
        {
            int i=0;
            for (i=0;i<ACCESSORS_ARRAY_SIZE;i++)
                h->accessors[i]=NULL;

            if (h->root)
                rebuild_hash_keys(h,h->root);

            h->trie_invalid=0;
            id = grib_hash_keys_get_id(h->context->keys,name);

        } else {
            id = grib_hash_keys_get_id(h->context->keys,name);

            if ((a=h->accessors[id])!=NULL &&
                    (the_namespace==NULL || matching(a,name,the_namespace) ))
                return a;
        }

        a = search(h->root,name,the_namespace);
        h->accessors[id] = a;

        return a;
    } else {
        return search(h->root,name,the_namespace);
    }
}

char* get_rank(const char* name,long *rank) {
    char* p=(char*)name;
    char* end=p;
    char* ret=NULL;

    *rank=-1;

    if (*p=='#') {
        *rank=strtol(++p,&end,10);
        if ( *end != '#') {
            *rank=-1;
        } else {
            grib_context* c=grib_context_get_default();
            end++;
            ret=grib_context_strdup(c,end);
        }
    }
    return ret;
}

char* get_condition(const char* name,codes_condition* condition)
{
    char* equal=(char*)name;
    char* endCondition=NULL;
    char* str=NULL;
    char* end=NULL;
    long lval;
    grib_context* c=grib_context_get_default();

    condition->rightType=GRIB_TYPE_UNDEFINED;

    Assert(name[0]=='/');

    while (*equal!=0 && *equal!='=') equal++;
    if (*equal==0) return NULL;

    endCondition=equal;
    while (*endCondition!=0 && *endCondition!='/') endCondition++;
    if (*endCondition==0) return NULL;

    str=(char*)grib_context_malloc_clear(c,strlen(name));
    memcpy(str,equal+1,endCondition-equal-1);

    end=NULL;
    lval=strtol(str,&end,10);
    if (*end != 0) {
        double dval;
        dval=strtod(str,&end);
        if (*end != 0) {
            condition->rightType=GRIB_TYPE_DOUBLE;
            condition->rightDouble=dval;
        }
    } else {
        condition->rightType=GRIB_TYPE_LONG;
        condition->rightLong=lval;
    }

    if (condition->rightType!=GRIB_TYPE_UNDEFINED) {
        strcpy(str,endCondition+1);
        condition->left=(char*)grib_context_malloc_clear(c,equal-name);
        memcpy(condition->left,name+1,equal-name-1);
    } else {
        grib_context_free(c,str);
        str=NULL;
    }
    return str;
}

static grib_accessor* _search_by_rank(grib_accessor* a,const char* name,long rank) {
    long r=1;
    grib_accessors_list* al=accessor_bufr_data_array_get_dataAccessors(a);

    while (al) {
        if (!strcmp(al->accessor->name,name)) {
            if (r==rank) return al->accessor;
            r++;
        }
        al=al->next;
    }

    return NULL;
}

static grib_accessor* search_by_rank(grib_handle* h, const char* name,const char *the_namespace,long rank)
{
    grib_accessor* data=search_and_cache(h,"dataAccessors",the_namespace);
    if (data) {
        return _search_by_rank(data,name,rank);
    } else {
        return _search_and_cache(h,name,the_namespace);
    }
}

static int condition_true(grib_accessor* a,codes_condition* condition) {
    int ret=0, err=0;
    size_t size=1;
    long lval=0;
    double dval=0;
    switch (condition->rightType) {
    case GRIB_TYPE_LONG:
        err = grib_unpack_long(a,&lval,&size);
        if (err) ret = 0;
        else     ret = lval==condition->rightLong ? 1 : 0;
        break;
    case GRIB_TYPE_DOUBLE:
        err = grib_unpack_double(a,&dval,&size);
        if (err) ret = 0;
        else     ret = dval==condition->rightDouble ? 1 : 0;
        break;
    default :
        ret=0;
        break;
    }
    return ret;
}

static void search_from_accessors_list(grib_accessors_list* al,grib_accessors_list* end,const char* name,grib_accessors_list* result)
{
    char* accessor_name=NULL;
    char attribute_name[200]={0,};
    grib_accessor* accessor_result=0;

    accessor_name=grib_split_name_attribute(al->accessor->context,name,attribute_name);

    while (al && al!=end && al->accessor) {
        if (strcmp(al->accessor->name,accessor_name)==0) {
            if (attribute_name[0]) {
                accessor_result=grib_accessor_get_attribute(al->accessor,attribute_name);
            } else {
                accessor_result=al->accessor;
            }
            if (accessor_result) {
                grib_accessors_list_push(result,accessor_result);
            }
        }
        al=al->next;
    }
}

static void search_accessors_list_by_condition(grib_accessors_list* al,const char* name,codes_condition* condition,grib_accessors_list* result)
{
    grib_accessors_list* start=NULL;
    grib_accessors_list* end=NULL;

    while (al) {
        if (!strcmp(al->accessor->name,condition->left)) {
            if (start==NULL && condition_true(al->accessor,condition)) start=al;
            if (start && !condition_true(al->accessor,condition)) end=al;
        }
        if (start!=NULL && ( end!=NULL || al->next==NULL) ) {
            if (end==NULL) end=al;
            search_from_accessors_list(start,end,name,result);
            al=end;
            start=NULL;
            end=NULL;
        }
        al=al->next;
    }
}

static grib_accessors_list* search_by_condition(grib_handle* h,const char* name,codes_condition* condition)
{
    grib_accessors_list* al;
    grib_accessors_list* result=NULL;
    grib_accessor* data=search_and_cache(h,"dataAccessors",0);
    if (data && condition->left) {
        al=accessor_bufr_data_array_get_dataAccessors(data);
        if (!al) return NULL;
        result=(grib_accessors_list*)grib_context_malloc_clear(al->accessor->context,sizeof(grib_accessors_list));
        search_accessors_list_by_condition(al,name,condition,result);
        if (!result->accessor) {
            grib_accessors_list_delete(h->context,result);
            result=NULL;
        }
    }

    return result;
}

static void grib_find_same_and_push(grib_accessors_list* al,grib_accessor* a)
{
    if (a) {
        grib_find_same_and_push(al,a->same);
        grib_accessors_list_push(al,a);
    }
}

grib_accessors_list* grib_find_accessors_list(grib_handle* h,const char* name)
{
    char* str=NULL;
    grib_accessors_list* al=NULL;
    codes_condition* condition=NULL;
    grib_accessor* a=NULL;

    if (name[0]=='/') {
        condition=(codes_condition*)grib_context_malloc_clear(h->context,sizeof(codes_condition));
        str=get_condition(name,condition);
        if (str) {
            al=search_by_condition(h,str,condition);
            grib_context_free(h->context,str);
            if (condition->left) grib_context_free(h->context,condition->left);
            if (condition->rightString) grib_context_free(h->context,condition->rightString);
        }
        grib_context_free(h->context,condition);
    } else if (name[0]=='#') {
        a=grib_find_accessor(h, name);
        if (a) {
            al=(grib_accessors_list*)grib_context_malloc_clear(h->context,sizeof(grib_accessors_list));
            grib_accessors_list_push(al,a);
        }
    } else {
        a=grib_find_accessor(h, name);
        if (a) {
            al=(grib_accessors_list*)grib_context_malloc_clear(h->context,sizeof(grib_accessors_list));
            grib_find_same_and_push(al,a);
        }
    }

    return al;
}

static grib_accessor* search_and_cache(grib_handle* h, const char* name,const char *the_namespace)
{
    char* str=0;
    grib_accessor* a=NULL;
    long rank;

    if (name[0]=='#') {
        str=get_rank(name,&rank);
        a=search_by_rank(h,str,the_namespace,rank);
        grib_context_free(h->context,str);
    } else {
        a=_search_and_cache(h,name,the_namespace);
    }

    return a;
}

static grib_accessor* _grib_find_accessor(grib_handle* h, const char* name)
{
    grib_accessor* a = NULL;
    char* p = (char*)name;
    DebugAssert(name);

    while ( *p != '.' && *p != '\0' ) p++;
    if ( *p == '.' ) {
        int i=0,len=0;
        char name_space[1024];
        char* basename=NULL;
        basename=p+1;
        p--;
        i=0;
        len=p-name+1;

        for ( i=0;i<len;i++ ) name_space[i] = *(name+i);

        name_space[len]='\0';

        a = search_and_cache(h,basename,name_space);
    } else {
        a = search_and_cache(h,name,NULL);
    }

    if(a == NULL && h->main)
        a = grib_find_accessor(h->main,name);

    return a;
}

char* grib_split_name_attribute(grib_context* c,const char* name,char* attribute_name)
{
    /*returns accessor name and attribute*/
    char* p=0;
    size_t size=0;
    char* accessor_name=NULL;
    p=(char*)name;
    while ( *(p+1) != '\0' && ( *p != '-' || *(p+1)!= '>' ) ) p++;
    if (*(p+1) == '\0') {
        *attribute_name=0;
        return (char*)name;
    }
    size=p-name;
    accessor_name=(char*)grib_context_malloc_clear(c,size+1);
    accessor_name=(char*)memcpy(accessor_name,name,size);
    p+=2;
    strcpy(attribute_name,p);
    return accessor_name;
}

grib_accessor* grib_find_accessor(grib_handle* h, const char* name)
{
    grib_accessor* aret = NULL;

    if (h->product_kind == PRODUCT_GRIB) {
        aret = _grib_find_accessor(h, name); /* ECC-144: Performance */
    }
    else {
        char* accessor_name=NULL;
        char attribute_name[512]={0,};
        grib_accessor* a=NULL;

        accessor_name=grib_split_name_attribute(h->context,name,attribute_name);

        a=_grib_find_accessor(h,accessor_name);

        if (*attribute_name==0) {
            aret=a;
        } else if (a) {
            aret=grib_accessor_get_attribute(a,attribute_name);
            grib_context_free(h->context,accessor_name);
        }
    }
    return aret;
}

grib_accessor* grib_find_attribute(grib_handle* h, const char* name,const char* attr_name, int* err)
{
    grib_accessor* a = NULL;
    grib_accessor* act = NULL;

    if ((a = grib_find_accessor(h, name))==NULL) {
        *err=GRIB_NOT_FOUND;
        return NULL;
    }

    if((act=grib_accessor_get_attribute(a,attr_name))==NULL) {
        *err=GRIB_ATTRIBUTE_NOT_FOUND;
        return NULL;
    }
    return act;
}

/* Only look in trie. Used only in alias. Should not be used in other cases.*/
grib_accessor* grib_find_accessor_fast(grib_handle* h, const char* name)
{
    grib_accessor* a = NULL;
    char* p = (char*)name;

    while ( *p != '.' && *p != '\0' ) p++;
    if ( *p == '.' ) {
        int i=0,len=0;
        char name_space[1024];
        p--;
        i=0;
        len=p-name+1;

        for ( i=0;i<len;i++ ) name_space[i] = *(name+i);

        name_space[len]='\0';

        a = h->accessors[grib_hash_keys_get_id(h->context->keys,name)];
        if(a && !matching(a,name,name_space))
            a = NULL;

    } else {
        a = h->accessors[grib_hash_keys_get_id(h->context->keys,name)];
    }

    if(a == NULL && h->main)
        a = grib_find_accessor_fast(h->main,name);

    return a;
}
