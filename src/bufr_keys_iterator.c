/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

GRIB_INLINE static int grib_inline_strcmp(const char* a,const char* b)
{
    if (*a != *b) return 1;
    while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
    return (*a==0 && *b==0) ? 0 : 1;
}

grib_keys_iterator* codes_bufr_keys_iterator_new(grib_handle* h)
{
    grib_keys_iterator* ki=NULL;

    if (!h) return NULL;

    ki= (grib_keys_iterator*)grib_context_malloc_clear(h->context,sizeof(grib_keys_iterator));
    if (!ki) return NULL;

    ki->filter_flags = 0;
    ki->handle       = h;
    ki->i_curr_attribute=0;
    ki->accessor_flags_only= GRIB_ACCESSOR_FLAG_DUMP;
    ki->accessor_flags_skip= GRIB_ACCESSOR_FLAG_HIDDEN | GRIB_ACCESSOR_FLAG_READ_ONLY;

    ki->at_start     = 1;
    ki->match        = 0;

    if(ki->seen==NULL ) ki->seen = grib_trie_new(h->context);

    return ki;
}

grib_keys_iterator* codes_bufr_data_section_keys_iterator_new(grib_handle* h)
{
    grib_keys_iterator* ki=NULL;

    if (!h) return NULL;

    ki= (grib_keys_iterator*)grib_context_malloc_clear(h->context,sizeof(grib_keys_iterator));
    if (!ki) return NULL;

    ki->filter_flags = 0;
    ki->handle       = h;
    ki->i_curr_attribute=0;
    ki->accessor_flags_only= GRIB_ACCESSOR_FLAG_BUFR_DATA | GRIB_ACCESSOR_FLAG_DUMP;
    ki->accessor_flags_skip= GRIB_ACCESSOR_FLAG_HIDDEN | GRIB_ACCESSOR_FLAG_READ_ONLY;

    ki->at_start     = 1;
    ki->match        = 0;

    if(ki->seen==NULL ) ki->seen = grib_trie_new(h->context);

    return ki;
}

static void mark_seen(grib_keys_iterator* ki,const char* name)
{
  int* r=(int*)grib_trie_get(ki->seen,name);

  if (r) (*r)++;
  else {
    r=grib_context_malloc(ki->handle->context,sizeof(int));
    *r=1;
    grib_trie_insert(ki->seen,name,(void*)r);
  }
}

static int was_seen(grib_keys_iterator* ki,const char* name)
{
    return grib_trie_get(ki->seen,name) != NULL;
}

int codes_bufr_keys_iterator_rewind(grib_keys_iterator* ki)
{
    ki->at_start = 1;
    return GRIB_SUCCESS;
}

static int skip(grib_keys_iterator* kiter)
{
    if(kiter->current->sub_section)
        return 1;

    if(kiter->current->flags & kiter->accessor_flags_skip)
      return 1;

    if(kiter->accessor_flags_only == (kiter->current->flags & kiter->accessor_flags_only))  {
      mark_seen(kiter,kiter->current->name);
      return 0;
    } else  {
      return 1;
    }

    mark_seen(kiter,kiter->current->name);

    return 0;
}

static int next_attribute(grib_keys_iterator* kiter)
{
  int *r=0;
  int i_curr_attribute;
  if (!kiter->current) return 0;
  if (!kiter->attributes) {
    kiter->attributes=kiter->current->attributes;
    kiter->prefix=0;
    kiter->i_curr_attribute=0;
  }
  i_curr_attribute=kiter->i_curr_attribute-1;

  while (kiter->i_curr_attribute < MAX_ACCESSOR_ATTRIBUTES && kiter->attributes[kiter->i_curr_attribute] ) {
        if ((kiter->attributes[kiter->i_curr_attribute]->flags & GRIB_ACCESSOR_FLAG_DUMP) != 0
         && (kiter->attributes[kiter->i_curr_attribute]->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) ==0) break;
    kiter->i_curr_attribute++;
  }

  if (kiter->attributes[kiter->i_curr_attribute]) {
    if (!kiter->prefix) {
      kiter->prefix=grib_context_malloc_clear(kiter->current->context,strlen(kiter->current->name)+10);
      r=(int*)grib_trie_get(kiter->seen,kiter->current->name);
      sprintf(kiter->prefix,"#%d#%s",*r,kiter->current->name);
    }
    kiter->i_curr_attribute++;
    return 1;
  } else {
    char* prefix=0;
    if (!kiter->prefix) return 0;
    if (!kiter->attributes[i_curr_attribute]) {
      kiter->prefix=0;
      return 0;
    }
    prefix=grib_context_malloc_clear(kiter->current->context,strlen(kiter->prefix)+strlen(kiter->attributes[i_curr_attribute]->name)+3);
    sprintf(prefix,"%s->%s",kiter->prefix,kiter->attributes[i_curr_attribute]->name);
    grib_context_free(kiter->current->context,kiter->prefix);
    kiter->prefix=prefix;
    kiter->attributes=kiter->attributes[i_curr_attribute]->attributes;
    kiter->i_curr_attribute=0;
    return next_attribute(kiter);
  }

}

int codes_bufr_keys_iterator_next(grib_keys_iterator* kiter)
{
    if(kiter->at_start)
    {
        kiter->current  = kiter->handle->root->block->first;
        kiter->at_start = 0;
        kiter->i_curr_attribute=0;
        kiter->prefix=0;
        kiter->attributes=0;
    }
    else
    {
        if (next_attribute(kiter)) {
          return 1;
        } else {
          kiter->current = grib_next_accessor(kiter->current);
          kiter->attributes=0;
          if (kiter->prefix) {
            grib_context_free(kiter->current->context,kiter->prefix);
            kiter->prefix=0;
          }
          kiter->i_curr_attribute=0;
        }
    }

    while(kiter->current && skip(kiter))
        kiter->current = grib_next_accessor(kiter->current);

    return kiter->current != NULL;
}

char* codes_bufr_keys_iterator_get_name(grib_keys_iterator* kiter)
{
  int *r=0;
  char* ret=0;
  Assert(kiter->current);

  if (kiter->prefix) {
    int iattribute=kiter->i_curr_attribute-1;
    ret=grib_context_malloc_clear(kiter->handle->context,strlen(kiter->prefix)+strlen(kiter->attributes[iattribute]->name)+10);
    sprintf(ret,"%s->%s",kiter->prefix,kiter->attributes[iattribute]->name);
  } else {
    ret=grib_context_malloc_clear(kiter->handle->context,strlen(kiter->current->name)+10);

    if (kiter->current->flags & GRIB_ACCESSOR_FLAG_BUFR_DATA) {
      r=(int*)grib_trie_get(kiter->seen,kiter->current->name);
      sprintf(ret,"#%d#%s",*r,kiter->current->name);
    } else {
      sprintf(ret,"%s",kiter->current->name);
    }
  }
  return ret;
}

grib_accessor* codes_bufr_keys_iterator_get_accessor(grib_keys_iterator* kiter)
{
    return kiter->current;
}

int codes_bufr_keys_iterator_delete( grib_keys_iterator* kiter)
{
    if (kiter) {
        if(kiter->seen)
            grib_trie_delete(kiter->seen);
        if (kiter->name_space)
            grib_context_free(kiter->handle->context,kiter->name_space);
        grib_context_free(kiter->handle->context,kiter);
    }
    return 0;
}

char** codes_bufr_copy_data(grib_handle* hin,grib_handle* hout, size_t* nkeys, int* err) {
  grib_keys_iterator* kiter=NULL;;
  char* name=0;
  char** keys=NULL;
  grib_sarray* k=0;

  if (hin==NULL || hout==NULL) return GRIB_NULL_HANDLE;

  kiter=codes_bufr_data_section_keys_iterator_new(hin);
  if (!kiter) return NULL;
  k=grib_sarray_new(hin->context,50,10);

  while(codes_bufr_keys_iterator_next(kiter))
  {
    name = codes_bufr_keys_iterator_get_name(kiter);
    /* if the copy fail we want to keep copying without error message
      this is because the copy can be between structures that are not 
      identical and we want to copy what can be copied and skip what 
      cannot be copied because is not in the output handle
    */
    *err=codes_copy_key(hin,hout,name,0);
    if (*err==0) k=grib_sarray_push(hin->context,k,name);

  }

  *nkeys=grib_sarray_used_size(k);
  keys=grib_sarray_get_array(hin->context,k);
  grib_sarray_delete(hin->context,k);

  *err=codes_set_long(hout,"pack",1);

  codes_keys_iterator_delete(kiter);
  return keys;
}
