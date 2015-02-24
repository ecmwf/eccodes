/*
 * Copyright 2005-2015 ECMWF.
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

static void visit(grib_section* s,const char* name,const char* name_space,
		search_all_callback_proc callback,void *data,int* count)
{

	grib_accessor* a = s ? s->block->first : NULL;

	if(!s) return ;

	while(a)
	{
		grib_section* sub = a->sub_section;

		if(matching(a,name,name_space))
		{
			callback(a,data);
			(*count)++;
		}

		visit(sub,name,name_space,callback,data,count);

		a = a->next;
	}

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
		Assert(h == a->parent->h);

		while(i<MAX_ACCESSOR_NAMES && ((p = a->all_names[i]) != NULL)) {
			if(*p != '_')   {
				id=grib_hash_keys_get_id(a->parent->h->context->keys,p);

				if (a->same != a && i==0) {
					a->same=a->parent->h->accessors[id];
					a->parent->h->accessors[id]=a;
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
  size_t len;

  *rank=-1;
  while (*p!=0 && *p!='#') p++;

  if (*p=='#') {
    *rank=strtol(++p,&end,10);
    if (errno!=0 || end == p) {
      *rank=-1;
    } else {
      grib_context* c=grib_context_get_default();
      len=p-name;
      ret=grib_context_malloc_clear(c,len);
      memcpy(ret,name,len-1);
    }
  }
  return ret;
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
  grib_accessor* data=search_and_cache(h,"numericValues",the_namespace);
  if (data) {
    return _search_by_rank(data,name,rank);
  } else {
    return _search_and_cache(h,name,the_namespace);
  }

}

static grib_accessor* search_and_cache(grib_handle* h, const char* name,const char *the_namespace)
{
  char* str=0;
  long rank;

  str=get_rank(name,&rank);
  if (rank>0) {
    grib_accessor* a=NULL;
    a=search_by_rank(h,str,the_namespace,rank);
    grib_context_free(h->context,str);
    return a;
  } else {
    return _search_and_cache(h,name,the_namespace);
  }

}



static grib_accessor* _grib_find_accessor(grib_handle* h, const char* name)
{
	grib_accessor* a = NULL;
	char* p=NULL;
	char* basename=NULL;
	char name_space[1024];
	int i=0,len=0;

	p=(char*)name;
	Assert(name);

	while ( *p != '.' && *p != '\0' ) p++;
	if ( *p == '.' ) {
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

int has_rank(const char* name) {
  char* p=(char*)name;
  while (*p!=0) {
    if (*p == '#') return 1;
    p++;
  }
  return 0;
}

char* grib_split_name_attribute(grib_context* c,const char* name,char* attribute_name) {
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
  accessor_name=grib_context_malloc_clear(c,size+1);
  accessor_name=memcpy(accessor_name,name,size);
  p+=2;
  strcpy(attribute_name,p);
  return accessor_name;
}

grib_accessor* grib_find_accessor(grib_handle* h, const char* name)
{
  char* accessor_name=NULL;
  char attribute_name[512]={0,};
  grib_accessor* a=NULL;
  grib_accessor* aret=NULL;

  accessor_name=grib_split_name_attribute(h->context,name,attribute_name);

  a=_grib_find_accessor(h,accessor_name);

  if (*attribute_name==0) {
    aret=a;
  } else {
    aret=grib_accessor_get_attribute(a,attribute_name);
    grib_context_free(h->context,accessor_name);
  }

  return aret;
}

grib_accessor* grib_find_attribute(grib_handle* h, const char* name,const char* attr_name, int* err)
{
    grib_accessor* a = NULL;
    grib_accessor* act = NULL;
	int id=0;

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


int grib_find_all_accessors(grib_handle* h, const char* name,search_all_callback_proc callback,void *data)
{
	int count = 0;
	visit(h->root,name,NULL,callback,data,&count);
	/* if(count == 0) */
	if(h->main)
		count += grib_find_all_accessors(h->main,name,callback,data);
	return count;
}

/* Only look in trie. Used only in alias. Should not be used in other cases.*/
grib_accessor* grib_find_accessor_fast(grib_handle* h, const char* name)
{
	grib_accessor* a = NULL;
	char* p=NULL;
	char* basename=NULL;
	char name_space[1024];
	int i=0,len=0;

	p=(char*)name;

	while ( *p != '.' && *p != '\0' ) p++;
	if ( *p == '.' ) {
		basename=p+1;
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



