/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = expression
   IMPLEMENTS = destroy
   IMPLEMENTS = native_type
   IMPLEMENTS = get_name
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = evaluate_string
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = const char *name
   MEMBERS    = const char *list
   END_CLASS_DEF

 */
/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "expression.class" and rerun ./make_class.pl

*/

typedef const char* string; /* to keep make_class.pl happy */


static void init_class              (grib_expression_class*);

static void        destroy(grib_context*,grib_expression* e);

static void        print(grib_context*,grib_expression*,grib_handle*);
static void        add_dependency(grib_expression* e, grib_accessor* observer);
static string get_name(grib_expression* e);

static int        native_type(grib_expression*,grib_handle*);

static int        evaluate_long(grib_expression*,grib_handle*,long*);
static int      evaluate_double(grib_expression*,grib_handle*,double*);
static string evaluate_string(grib_expression*,grib_handle*,char*,size_t*,int*);

typedef struct grib_expression_is_in_list{
  grib_expression base;
/* Members defined in is_in_list */
	const char *name;
	const char *list;
} grib_expression_is_in_list;


static grib_expression_class _grib_expression_class_is_in_list = {
    0,                    /* super                     */
    "is_in_list",                    /* name                      */
    sizeof(grib_expression_is_in_list),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &add_dependency,       

	&native_type,
	&get_name,

	&evaluate_long,
	&evaluate_double,
	&evaluate_string,
};

grib_expression_class* grib_expression_class_is_in_list = &_grib_expression_class_is_in_list;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */


static grib_trie* load_list(grib_context* c,grib_expression* e, int* err) {

  grib_expression_is_in_list* self = (grib_expression_is_in_list*)e;

  char* filename=NULL;
  char line[1024]={0,};
  grib_trie* list=NULL;
  FILE* f=NULL;

  *err=GRIB_SUCCESS;

  filename=grib_context_full_defs_path(c,self->list);
  if (!filename) {
    grib_context_log(c,GRIB_LOG_ERROR,"unable to find def file %s",self->list);
    *err=GRIB_FILE_NOT_FOUND;
    return NULL;
  } else {
    grib_context_log(c,GRIB_LOG_DEBUG,"found def file %s",filename);
  }
  list=(grib_trie*)grib_trie_get(c->lists,filename);
  if (list) {
	grib_context_log(c,GRIB_LOG_DEBUG,"using list %s from cache",self->list);
  	return list;
  } else {
	grib_context_log(c,GRIB_LOG_DEBUG,"using list %s from file %s",self->list,filename);
  }

  f=codes_fopen(filename,"r");
  if (!f) {*err=GRIB_IO_PROBLEM; return NULL;}

  list=grib_trie_new(c);

  while(fgets(line,sizeof(line)-1,f)) {
	unsigned char* p=(unsigned char*)line;
	while (*p!=0) {
		if (*p<33) {*p=0; break;}
		p++;
	}
  	grib_trie_insert(list,line,line);
  }

  grib_trie_insert(c->lists,filename,list);

  fclose(f);

  return list;

}

static const char* get_name(grib_expression* g)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  return e->name;
}

static int evaluate_long(grib_expression* g,grib_handle *h,long* result)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  int err=0;
  char mybuf[1024]={0,};
  size_t size=1024;

  grib_trie* list=load_list(h->context,g,&err);

  if((err=grib_get_string_internal(h,e->name,mybuf,&size)) != GRIB_SUCCESS)
      return err;

  if (grib_trie_get(list,mybuf)) *result=1;
  else *result=0;

  return err;
}

static int evaluate_double(grib_expression *g,grib_handle *h,double* result)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  int err=0;
  char mybuf[1024]={0,};
  size_t size=1024;

  grib_trie* list=load_list(h->context,g,&err);

  if((err=grib_get_string_internal(h,e->name,mybuf,&size)) != GRIB_SUCCESS)
      return err;

  if (grib_trie_get(list,mybuf)) *result=1;
  else *result=0;

  return err;
}

static string evaluate_string(grib_expression* g,grib_handle* h,char* buf,size_t* size,int* err)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  char mybuf[1024]={0,};
  size_t sizebuf=1024;
  long result;

  grib_trie* list=load_list(h->context,g,err);

  if((*err=grib_get_string_internal(h,e->name,mybuf,&sizebuf)) != GRIB_SUCCESS)
      return NULL;

  if (grib_trie_get(list,mybuf)) result=1;
  else result=0;

  sprintf(buf,"%ld",result);
  *size=strlen(buf);
  return buf;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  printf("access('%s",e->name);
  if(f)
  {
    long s = 0;
    grib_get_long(f,e->name,&s);
    printf("=%ld",s);
  }
  printf("')");
}

static void destroy(grib_context* c,grib_expression* g)
{
}


static void  add_dependency(grib_expression* g, grib_accessor* observer){
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  grib_accessor *observed = grib_find_accessor(grib_handle_of_accessor(observer),e->name);

  if(!observed)
  {
    /* grib_context_log(observer->context, GRIB_LOG_ERROR, */
         /* "Error in accessor_add_dependency: cannot find [%s]", e->name); */
       /* Assert(observed); */
    return;
  }

  grib_dependency_add(observer,observed);
}

grib_expression* new_is_in_list_expression(grib_context* c,const char* name,const char* list)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)grib_context_malloc_clear_persistent(c,sizeof(grib_expression_is_in_list));
  e->base.cclass            = grib_expression_class_is_in_list;
  e->name                   = grib_context_strdup_persistent(c,name);
  e->list                   = grib_context_strdup_persistent(c,list);
  return (grib_expression*)e;
}

static int native_type(grib_expression* g,grib_handle *h)
{
  grib_expression_is_in_list* e = (grib_expression_is_in_list*)g;
  int type = 0;
  int err;
  if((err=grib_get_native_type(h,e->name,&type)) != GRIB_SUCCESS)
    grib_context_log(h->context, GRIB_LOG_ERROR,
    "Error in native_type %s : %s", e->name,grib_get_error_message(err));
  return type;
}

