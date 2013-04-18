/*
 * Copyright 2005-2013 ECMWF.
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

		if((b = search(sub,name,name_space)) != NULL)
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

static grib_accessor* search_and_cache(grib_handle* h, const char* name,const char *namespace)
{
	if(h->use_trie)
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

		} 
		else 
		{
			id = grib_hash_keys_get_id(h->context->keys,name);

			if ((a=h->accessors[id])!=NULL && (namespace==NULL || matching(a,name,namespace)))
				return a;
		}

		a = search(h->root,name,namespace);
		h->accessors[id] = a;

		return a;
	}
	else {
		return search(h->root,name,namespace);
	}

}

/* Only look in trie */
grib_accessor* grib_find_accessor_fast(grib_handle* h, const char* name)
{
	grib_accessor* a = NULL;
	char* p=NULL;
	char* basename=NULL;
	char name_space[1024];
	int i=0,len=0;

	p=(char*)name;

	/* Assert(name); */
	/*Assert(h->use_trie && !h->trie_invalid);*/

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

grib_accessor* grib_find_accessor(grib_handle* h, const char* name)
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

int grib_find_all_accessors(grib_handle* h, const char* name,search_all_callback_proc callback,void *data)
{
	int count = 0;
	visit(h->root,name,NULL,callback,data,&count);
	/* if(count == 0) */
	if(h->main)
		count += grib_find_all_accessors(h->main,name,callback,data);
	return count;
}



