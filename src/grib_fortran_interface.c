/*
 * Copyright 2005-2012 ECMWF.
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

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#include <ctype.h>

int GRIB_NULL=-1;
int GRIB_NULL_NEAREST=-1;
extern int errno;

typedef struct l_grib_file l_grib_file;

struct l_grib_file {
  int id;
  FILE* f;
  l_grib_file* next;
};

typedef struct l_grib_handle l_grib_handle;

struct l_grib_handle {
  int id;
  grib_handle* h;
  l_grib_handle* next;
};

typedef struct l_grib_iterator l_grib_iterator;

struct l_grib_iterator {
  int id;
  grib_iterator* i;
  l_grib_iterator* next;
};

typedef struct l_grib_keys_iterator l_grib_keys_iterator;

struct l_grib_keys_iterator {
  int id;
  grib_keys_iterator* i;
  l_grib_keys_iterator* next;
};

static l_grib_handle* handle_set = NULL;
static l_grib_file*   file_set   = NULL;
static l_grib_iterator* iterator_set = NULL;
static l_grib_keys_iterator* keys_iterator_set = NULL;

static char* cast_char(char* buf, char* fortstr,int len){
  if (len == 0 ) return NULL;
  memcpy(buf,fortstr,len);
  buf[len] = '\0';
  return buf;
}

static int push_file(FILE* f){
  l_grib_file* current  = file_set;
  l_grib_file* previous = file_set;
  l_grib_file* new      = NULL;
  int myindex = 1;

  if(!file_set){
    file_set = malloc(sizeof(l_grib_file));
    file_set->id   = myindex;
    file_set->f    = f;
    file_set->next = NULL;
    return myindex;
  }

  while(current){
    if(current->id < 0){
      current->id = -(current->id);
      current->f    = f;
      return current->id ;
    } else{
      myindex++;
      previous = current;
      current = current->next;
    }
  }

  new = malloc(sizeof(l_grib_handle));
  new->id   = myindex;
  new->f    = f;
  new->next = current;
  previous->next = new;
  return myindex;
}

static int push_handle(grib_handle *h){
  l_grib_handle* current  = handle_set;
  l_grib_handle* previous = handle_set;
  l_grib_handle* new      = NULL;
  int myindex = 1;
  Assert(sizeof(int)==4);
  if(!handle_set){
    handle_set = malloc(sizeof(l_grib_handle));
    handle_set->id   = myindex;
    handle_set->h    = h;
    handle_set->next = NULL;
    return myindex;
  }

  while(current){
    if(current->id < 0){
      current->id = -(current->id);
      current->h  = h;
      return current->id;
    }
    else{
      myindex++;
      previous = current;
      current = current->next;
    }
  }

  new = malloc(sizeof(l_grib_handle));
  new->id   = myindex;
  new->h    = h;
  new->next = current;
  previous->next = new;

  return myindex;
}

static int push_iterator(grib_iterator *i){
  l_grib_iterator* current  = iterator_set;
  l_grib_iterator* previous = iterator_set;
  l_grib_iterator* new      = NULL;
  int myindex = 1;
  Assert(sizeof(int)==4);
  if(!iterator_set){
    iterator_set = malloc(sizeof(l_grib_iterator));
    iterator_set->id   = myindex;
    iterator_set->i    = i;
    iterator_set->next = NULL;
    return myindex;
  }

  while(current){
    if(current->id < 0){
      current->id = -(current->id);
      current->i  = i;
      return current->id;
    }
    else{
      myindex++;
      previous = current;
      current = current->next;
    }
  }

  new = malloc(sizeof(l_grib_iterator));
  new->id   = myindex;
  new->i    = i;
  new->next = current;
  previous->next = new;

  return myindex;
}

static int push_keys_iterator(grib_keys_iterator *i){
  l_grib_keys_iterator* current  = keys_iterator_set;
  l_grib_keys_iterator* previous = keys_iterator_set;
  l_grib_keys_iterator* new      = NULL;
  int myindex = 1;
  Assert(sizeof(int)==4);
  if(!keys_iterator_set){
    keys_iterator_set = malloc(sizeof(l_grib_keys_iterator));
    keys_iterator_set->id   = myindex;
    keys_iterator_set->i    = i;
    keys_iterator_set->next = NULL;
    return myindex;
  }

  while(current){
    if(current->id < 0){
      current->id = -(current->id);
      current->i  = i;
      return current->id;
    }
    else{
      myindex++;
      previous = current;
      current = current->next;
    }
  }
  if(!previous) return -1;

  new = malloc(sizeof(l_grib_keys_iterator));
  new->id   = myindex;
  new->i    = i;
  new->next = current;
  previous->next = new;

  return myindex;
}

static grib_handle* get_handle(int handle_id){
  l_grib_handle* current  = handle_set;

  while(current){
    if(current->id == handle_id) return current->h;
    current = current->next;
  }
  return NULL;
}

static FILE* get_file(int file_id){
  l_grib_file* current  = file_set;
  while(current){
    if(current->id == file_id) return current->f;
    current = current->next;
  }
  return NULL;
}

static grib_iterator* get_iterator(int iterator_id){
  l_grib_iterator* current  = iterator_set;

  while(current){
    if(current->id == iterator_id) return current->i;
    current = current->next;
  }
  return NULL;
}

static grib_keys_iterator* get_keys_iterator(int keys_iterator_id){
  l_grib_keys_iterator* current  = keys_iterator_set;

  while(current){
    if(current->id == keys_iterator_id) return current->i;
    current = current->next;
  }
  return NULL;
}

static int clear_file(int file_id){
  l_grib_file* current  = file_set;
  while(current){
    if(current->id == file_id){
      current->id = -(current->id);
      if (fclose(current->f) !=0)
    	  return GRIB_IO_PROBLEM;
      return GRIB_SUCCESS;
    }
    current = current->next;
  }
  return GRIB_INVALID_FILE;
}

static int clear_handle(int handle_id){

  l_grib_handle* current  = handle_set;

  while(current){
    if(current->id == handle_id){
      current->id = -(current->id);
      if(current->h) return grib_handle_delete(current->h);
    }
    current = current->next;
  }
  return GRIB_INVALID_GRIB;
}

static int clear_iterator(int iterator_id){

  l_grib_iterator* current  = iterator_set;

  while(current){
    if(current->id == iterator_id){
      current->id = -(current->id);
      return grib_iterator_delete(current->i);
    }
    current = current->next;
  }
  return GRIB_INVALID_ITERATOR;
}

static int clear_keys_iterator(int keys_iterator_id){

  l_grib_keys_iterator* current  = keys_iterator_set;

  while(current){
    if(current->id == keys_iterator_id){
      current->id = -(current->id);
      return grib_keys_iterator_delete(current->i);
    }
    current = current->next;
  }
  return GRIB_INVALID_KEYS_ITERATOR;
}

int grib_read_file_(int* fid, char* buffer, int* nbytes) {
  grib_context* c;
  FILE* f=get_file(*fid);

  if (f) {
    int ioerr;
    c=grib_context_get_default( );
    if( fread(buffer, 1, *nbytes, f)  != *nbytes) {
       ioerr=errno;
       grib_context_log(c,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s",strerror(ioerr));
       return GRIB_IO_PROBLEM;
    }
    return GRIB_SUCCESS;
  } else {
    return GRIB_INVALID_FILE;
  }
}

int grib_read_file__(int* fid, char* buffer, int* nbytes) {
  return grib_read_file_(fid,buffer,nbytes);
}

int grib_open_file_(int* fid, char* name , char* op, int lname, int lop){
  FILE* f = NULL;
  int ioerr=0;
  char oper[8];
  char fname[1024];
  int ret=GRIB_SUCCESS;
  /*TODO Proper context passed as external parameter */
  grib_context* context=grib_context_get_default();

  f = fopen(cast_char(fname,name,lname),cast_char(oper,op,lop));

  if(!f) {
    ioerr=errno;
    grib_context_log(context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s: %s",strerror(ioerr),cast_char(fname,name,lname));
    *fid = -1;
     ret=GRIB_IO_PROBLEM;
  } else {
    *fid = push_file(f);
    ret=GRIB_SUCCESS;
  }
  return ret;
}

int grib_open_file__(int* fid, char* name , char* op,  int lname, int lop){
  return grib_open_file_(       fid,       name ,       op,     lname,      lop);
}

int grib_close_file_(int* fid){
  return clear_file(*fid);
}
int grib_close_file__(int* fid){
  return grib_close_file_(fid);
}

int grib_multi_support_on_(){
  grib_multi_support_on(0);
  return GRIB_SUCCESS;
}
int grib_multi_support_on__(){
  return grib_multi_support_on_();
}

int grib_multi_support_off_(){
  grib_multi_support_off(0);
  return GRIB_SUCCESS;
}
int grib_multi_support_off__(){
  return grib_multi_support_off_();
}


int grib_iterator_new_(int* gid,int* iterid,int* mode) {
  int err=0;
  grib_handle* h;
  grib_iterator* iter;

  h=get_handle(*gid);
  if (!h) {
    *iterid=-1;
  return GRIB_NULL_HANDLE;
  }
  iter=grib_iterator_new(h,*mode,&err);
  if (iter)
    *iterid=push_iterator(iter);
  else
    *iterid=-1;
  return err;
}

int grib_iterator_new__(int* gid,int* iterid,int* mode) {
  return grib_iterator_new_(gid,iterid,mode);
}

int grib_iterator_next_(int* iterid,double* lat,double* lon,double* value) {
  grib_iterator* iter=get_iterator(*iterid);
  if (!iter) return GRIB_INVALID_ITERATOR;
  return grib_iterator_next(iter,lat,lon,value);
}

int grib_iterator_next__(int* iterid,double* lat,double* lon,double* value) {
  return grib_iterator_next_(iterid,lat,lon,value);
}

int grib_iterator_delete_(int* iterid) {
  return clear_iterator(*iterid);
}

int grib_iterator_delete__(int* iterid) {
  return grib_iterator_delete_(iterid);
}

int grib_keys_iterator_new_(int* gid,int* iterid,char* name_space,int len) {
  int err=0;
  char buf[1024];
  grib_handle* h;
  grib_keys_iterator* iter;

  h=get_handle(*gid);
  if (!h) {
    *iterid=-1;
  return GRIB_NULL_HANDLE;
  }
  iter=grib_keys_iterator_new(h,0,cast_char(buf,name_space,len));
  if (iter)
    *iterid=push_keys_iterator(iter);
  else
    *iterid=-1;
  return err;
}
int grib_keys_iterator_new__(int* gid,int* iterid,char* name_space,int len) {
  return grib_keys_iterator_new_(gid,iterid,name_space,len);
}

int grib_keys_iterator_next_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_next(iter);
}

int grib_keys_iterator_next__(int* iterid) {
  return grib_keys_iterator_next_(iterid);
}

int grib_keys_iterator_delete_(int* iterid) {
  return clear_keys_iterator(*iterid);
}
int grib_keys_iterator_delete__(int* iterid) {
  return grib_keys_iterator_delete_(iterid);
}

int grib_gribex_mode_on_() {
   grib_gribex_mode_on(0);
   return GRIB_SUCCESS;
}
int grib_gribex_mode_on__() {
   grib_gribex_mode_on(0);
   return GRIB_SUCCESS;
}

int grib_gribex_mode_off_() {
   grib_gribex_mode_off(0);
   return GRIB_SUCCESS;
}
int grib_gribex_mode_off__() {
   grib_gribex_mode_off(0);
   return GRIB_SUCCESS;
}

int grib_skip_computed_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_COMPUTED);
}
int grib_skip_computed__(int* iterid) {
  return grib_skip_computed_(iterid);
}

int grib_skip_coded_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_CODED);
}
int grib_skip_coded__(int* iterid) {
  return grib_skip_coded_(iterid);
}

int grib_skip_edition_specific_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC);
}
int grib_skip_edition_specific__(int* iterid) {
  return grib_skip_edition_specific_(iterid);
}

int grib_skip_duplicates_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_DUPLICATES);
}
int grib_skip_duplicates__(int* iterid) {
  return grib_skip_duplicates_(iterid);
}

int grib_skip_read_only_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_READ_ONLY);
}
int grib_skip_read_only__(int* iterid) {
  return grib_skip_read_only_(iterid);
}

int grib_skip_function_(int* iterid) {
  grib_keys_iterator* iter=get_keys_iterator(*iterid);
  if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_FUNCTION);
}
int grib_skip_function__(int* iterid) {
  return grib_skip_function_(iterid);
}

int grib_keys_iterator_get_name_(int* iterid,char* name,int* retlen,int len) {
  size_t lsize=len;
  grib_keys_iterator* kiter=get_keys_iterator(*iterid);

  if (!kiter) return GRIB_INVALID_KEYS_ITERATOR;

  sprintf(name,grib_keys_iterator_get_name(kiter));
  lsize=strlen(name);
  name+= lsize;
  memset(name,'\0',len-lsize);
  *retlen=lsize;
  return 0;
}
int grib_keys_iterator_get_name__(int* kiter,char* name,int* retlen,int len) {
  return grib_keys_iterator_get_name_(kiter,name,retlen,len);
}

int grib_keys_iterator_rewind_(int* kiter) {
  grib_keys_iterator* i=get_keys_iterator(*kiter);

  if (!i) return GRIB_INVALID_KEYS_ITERATOR;
  return grib_keys_iterator_rewind(i);
}
int grib_keys_iterator_rewind__(int* kiter) {
   return grib_keys_iterator_rewind_(kiter);
}

int grib_new_from_message_(int* gid, void* buffer , int* bufsize){
  grib_handle *h = NULL;
  h = grib_handle_new_from_message_copy(0, buffer, (size_t) *bufsize);
  if(h){
    *gid = push_handle(h);
    return GRIB_SUCCESS;
  }

  *gid = -1;
  return  GRIB_INTERNAL_ERROR;
}
int grib_new_from_message__(int* gid, void* buffer , int* bufsize){
  return grib_new_from_message_(gid,  buffer ,  bufsize);
}

int grib_new_from_message_copy_(int* gid, void* buffer , int* bufsize){
  grib_handle *h = NULL;
  h = grib_handle_new_from_message_copy(0, buffer, (size_t) bufsize);
  if(h){
    *gid = push_handle(h);
    return GRIB_SUCCESS;
  }

  *gid = -1;
  return  GRIB_INTERNAL_ERROR;
}

int grib_new_from_message_copy__(int* gid, void* buffer , int* bufsize){
  return grib_new_from_message_copy_(gid,  buffer ,  bufsize);
}

int grib_new_from_template_(int* gid, char* name , int lname){
  char fname[1024];
  grib_handle *h = NULL;

  h = grib_handle_new_from_template(NULL,cast_char(fname,name,lname));
  /*   grib_context_set_debug(h->context,1);*/

  if(h){
    *gid = push_handle(h);
    return GRIB_SUCCESS;
  }

  *gid = -1;
  return  GRIB_INTERNAL_ERROR;
}

int grib_new_from_template__(int* gid, char* name , int lname){
  return  grib_new_from_template_( gid,  name ,  lname);
}

int grib_clone_(int* gidsrc,int* giddest){
  grib_handle *src  = get_handle(*gidsrc);
  grib_handle *dest = NULL;

  if(src){
    dest = grib_handle_clone(src);
    if(dest){
      *giddest = push_handle(dest);
      return GRIB_SUCCESS;
    }
  }

  *giddest = -1;
  return GRIB_INVALID_GRIB;
}

int grib_clone__(int* gidsrc,int* giddest){
  return grib_clone_(gidsrc, giddest);
}

int grib_new_from_file_(int* fid, int* gid){
  int err = 0;
  FILE* f = get_file(*fid);

  grib_handle *h = NULL;

  if(f){
    h = grib_handle_new_from_file(0,f,&err);
    if(h){
      *gid = push_handle(h);
      return GRIB_SUCCESS;
    } else {
      *gid=-1;
      return GRIB_END_OF_FILE;
    }
  }

  *gid=-1;
  return GRIB_INVALID_FILE;
}

int grib_new_from_file__(int* fid, int* gid){
  return grib_new_from_file_( fid, gid);
}

int grib_release_(int* hid){
  return clear_handle(*hid);
}
int grib_release__(int* hid){
  return grib_release_( hid);
}

int grib_dump_(int* gid){
  grib_handle *h = get_handle(*gid);

  if(!h)
    return GRIB_INVALID_GRIB;
  else
    grib_dump_content(h,stdout,NULL,0,NULL);

  return GRIB_SUCCESS;
}
int grib_dump__(int* gid){
  return grib_dump_( gid);
}


int grib_print_(int* gid, char* key, int len){
  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  grib_dumper* d = NULL;
  char buf[1024];

  if(!h){
    return GRIB_INVALID_GRIB;
  }else{
    d = grib_dumper_factory("file",h,stdout,0,0);
    err = grib_print(h, cast_char(buf,key,len), d);
    grib_dumper_delete(d);
    return  err;
  }
}
int grib_print__(int* gid, char* key,  int len){
  return grib_print_( gid,  key, len);
}

int grib_get_error_string_(int* err, char* buf,  int len){
  const char* err_msg = grib_get_error_message(*err);
  size_t erlen = strlen(err_msg);
  strncpy(buf, err_msg,(size_t)len);
  if( len <  erlen) return GRIB_ARRAY_TOO_SMALL;
  return GRIB_SUCCESS;
}
int grib_get_error_string__(int* err, char* buf,  int len){
  return grib_get_error_string_(err,buf,len);
}
int grib_get_error_string(int* err, char* buf,  int len){
  return grib_get_error_string_(err,buf,len);
}
int grib_get_size_(int* gid, char* key, int* val, int len){
  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  size_t tsize = 0;

  if(!h){
    return GRIB_INVALID_GRIB;
  }else{
    err = grib_get_size(h, cast_char(buf,key,len), &tsize);
    *val = tsize;
    return  err;
  }
}
int grib_get_size__(int* gid, char* key, int* val,  int len){
  return grib_get_size_( gid,  key,  val,  len);
}


int grib_get_int_(int* gid, char* key, int* val,  int len){
  grib_handle *h = get_handle(*gid);
  long long_val;
  int err = GRIB_SUCCESS;
  char buf[1024];

  if(!h) return GRIB_INVALID_GRIB;
  err = grib_get_long(h, cast_char(buf,key,len),&long_val);
  *val = long_val;
  return err;
}
int grib_get_int__(int* gid, char* key, int* val,  int len){
  return grib_get_int_( gid,  key,  val,   len);
}
int grib_get_int_array_(int* gid, char* key, int*val, int* size,  int len){

  grib_handle *h = get_handle(*gid);
  long* long_val = NULL;
  int err = GRIB_SUCCESS;
  char buf[1024];
  size_t lsize = *size;


  if(!h)  return GRIB_INVALID_GRIB;

  if(sizeof(long) == sizeof(int)){
    long_val = (long*)val;
    err = grib_get_long_array(h, cast_char(buf,key,len), long_val, &lsize);
    *size = lsize;
    return  err;
  }
  if(*size)
    long_val = grib_context_malloc(h->context,(*size)*(sizeof(long)));
  else
    long_val = grib_context_malloc(h->context,(sizeof(long)));

  if(!long_val) return GRIB_OUT_OF_MEMORY;
  err = grib_get_long_array(h, cast_char(buf,key,len), long_val, &lsize);

  for(*size=0;*size<lsize;(*size)++)
    val[*size] = long_val[*size];

  grib_context_free(h->context,long_val);
  return  err;
}
int grib_get_int_array__(int* gid, char* key, int*val, int* size, int len){
  return grib_get_int_array_( gid,  key, val,  size,  len);
}

int grib_set_int_array_(int* gid, char* key, int* val, int* size,  int len){
  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  long* long_val = NULL;
  size_t lsize = *size;

  if(!h) return GRIB_INVALID_GRIB;

  if(sizeof(long) == sizeof(int)){
    long_val = (long*)val;
    return  grib_set_long_array(h, cast_char(buf,key,len), long_val, lsize);
  }

  if(lsize)
    long_val = grib_context_malloc(h->context,(lsize)*(sizeof(long)));
  else
    long_val = grib_context_malloc(h->context,(sizeof(long)));

  if(!long_val) return GRIB_OUT_OF_MEMORY;

  for(lsize=0;lsize<(*size);lsize++)
    long_val[lsize] = val[lsize];

  err = grib_set_long_array(h, cast_char(buf,key,len), long_val, lsize);

  grib_context_free(h->context,long_val);
  return err;
}
int grib_set_int_array__(int* gid, char* key, int* val, int* size,  int len){
  return grib_set_int_array_( gid,  key,  val,  size,   len);
}


int grib_set_int_(int* gid, char* key, int* val, int len){
  grib_handle *h = get_handle(*gid);
  char buf[1024];
  long long_val = *val;
  if(!h) return GRIB_INVALID_GRIB;
  return grib_set_long(h, cast_char(buf,key,len), long_val);
}
int grib_set_int__(int* gid, char* key, int* val, int len){
  return  grib_set_int_( gid,  key,  val, len);
}


int grib_set_real4_(int* gid, char* key, float* val, int len){

  grib_handle *h = get_handle(*gid);
  char buf[1024];
  double val8 = *val;
  if(!h)  return GRIB_INVALID_GRIB;

  return grib_set_double(h, cast_char(buf,key,len), val8);
}
int grib_set_real4__(int* gid, char* key, float* val,  int len){
  return grib_set_real4_( gid,  key,  val,   len);
}

int grib_get_real4_(int* gid, char* key, float* val, int len){

  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  double val8 = 0;

  Assert(sizeof(float) == 4);

  if(!h)
    return GRIB_INVALID_GRIB;

  err = grib_get_double(h, cast_char(buf,key,len), &val8);
  *val = val8;
  return err;
}
int grib_get_real4__(int* gid, char* key, float* val,  int len){
  return grib_get_real4_( gid,  key,  val,  len);
}

int grib_get_real4_array_(int* gid, char* key, float*val, int* size,  int len){

  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  size_t lsize = *size;
  double* val8 = NULL;

  Assert(sizeof(float) == 4);

  if(!h) return GRIB_INVALID_GRIB;

  if(*size)
    val8 = grib_context_malloc(h->context,(*size)*(sizeof(double)));
  else
    val8 = grib_context_malloc(h->context,sizeof(double));

  if(!val8) return GRIB_OUT_OF_MEMORY;

  err  = grib_get_double_array(h, cast_char(buf,key,len), val8, &lsize);
  for(*size=0;*size<lsize;(*size)++)
    val[*size] = val8[*size];
  grib_context_free(h->context,val8);
  return  err;
}
int grib_get_real4_array__(int* gid, char* key, float* val, int* size, int len){
  return grib_get_real4_array_( gid,  key, val,  size,  len);
}

int grib_set_real4_array_(int* gid, char* key, float*val, int* size, int len){

  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  size_t lsize = *size;
  double* val8 = NULL;

  Assert(sizeof(float) == 4);

  if(!h)
    return GRIB_INVALID_GRIB;

  if(*size)
    val8 = grib_context_malloc(h->context,lsize*(sizeof(double)));
  else
    val8 = grib_context_malloc(h->context,sizeof(double));

  if(!val8) return GRIB_OUT_OF_MEMORY;

  for(lsize=0;lsize<*size;lsize++)
    val8[lsize] = val[lsize];

  err = grib_set_double_array(h, cast_char(buf,key,len), val8, lsize);
  grib_context_free(h->context,val8);
  return err;

}

int grib_set_real4_array__(int* gid, char* key, float*val, int* size, int len){
  return grib_set_real4_array_( gid,  key, val,  size, len);
}

int grib_set_real8_(int* gid, char* key, double* val, int len){

  grib_handle *h = get_handle(*gid);

  char buf[1024];

  Assert(sizeof(double) == 8);

  if(!h) return GRIB_INVALID_GRIB;
  return grib_set_double(h, cast_char(buf,key,len), *val);
}


int grib_set_real8__(int* gid, char* key, double* val, int len){
  Assert(sizeof(double) == 8);
  return  grib_set_real8_( gid,  key,  val, len);
}

int grib_get_real8_(int* gid, char* key, double* val, int len){

  grib_handle *h = get_handle(*gid);
  char buf[1024];
  Assert(sizeof(double) == 8);

  if(!h) return GRIB_INVALID_GRIB;

  return grib_get_double(h, cast_char(buf,key,len), val);

}
int grib_get_real8__(int* gid, char* key, double* val, int len){
  Assert(sizeof(double) == 8);
  return grib_get_real8_( gid,  key,  val, len);
}

int grib_get_real8_array_(int* gid, char* key, double*val, int* size, int len){

  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  size_t lsize = *size;
  Assert(sizeof(double) == 8);

  if(!h){
    return GRIB_INVALID_GRIB;
  }else{
    err = grib_get_double_array(h, cast_char(buf,key,len), val, &lsize);
    *size = lsize;
    return  err;
  }
}
int grib_get_real8_array__(int* gid, char* key, double*val, int* size, int len){
  Assert(sizeof(double) == 8);
  return grib_get_real8_array_( gid,  key, val,  size,  len);
}

int grib_set_real8_array_(int* gid, char* key, double*val, int* size, int len){

  grib_handle *h = get_handle(*gid);

  char buf[1024];

  size_t lsize = *size;
  Assert(sizeof(double) == 8);

  if(!h)   return GRIB_INVALID_GRIB;

  return grib_set_double_array(h, cast_char(buf,key,len), val, lsize);

}

int grib_set_real8_array__(int* gid, char* key, double *val, int* size, int len){
  Assert(sizeof(double) == 8);
  return grib_set_real8_array_( gid,  key, val,  size, len);
}

int grib_get_string_(int* gid, char* key, char* val, int len, int len2){

  grib_handle *h = get_handle(*gid);
  int err = GRIB_SUCCESS;
  char buf[1024];
  size_t lsize = len2;

  if(!h) return  GRIB_INVALID_GRIB;

  err = grib_get_string(h, cast_char(buf,key,len), val, &lsize);
  val+= lsize;
  memset(val,'\0',len2-lsize);
  return  err;
}

int grib_get_string__(int* gid, char* key, char* val,  int len, int len2){
  return  grib_get_string_( gid,  key,  val,   len,  len2);
}

int grib_set_string_(int* gid, char* key, char* val, int len, int len2){

  grib_handle *h = get_handle(*gid);

  char buf[1024];
  size_t lsize = len2;

  if(!h) return GRIB_INVALID_GRIB;

  return grib_set_string(h, cast_char(buf,key,len), val, &lsize);

}

int grib_set_string__(int* gid, char* key, char* val, int len, int len2){
  return  grib_set_string_( gid,  key,  val,   len, len2);
}

int grib_get_message_size_(int* gid, int *len){
  grib_handle *h = get_handle(*gid);
  if(!h) return GRIB_INVALID_GRIB;

  *len = h->buffer->ulength;
  return GRIB_SUCCESS;

}
int grib_get_message_size__(int* gid, int *len){
  return  grib_get_message_size_( gid, len);
}

int grib_copy_message_(int* gid, void* mess,int* len){
  grib_handle *h = get_handle(*gid);
  if(!h)
    return GRIB_INVALID_GRIB;

  if(*len < h->buffer->ulength)
    return GRIB_BUFFER_TOO_SMALL;


  memcpy(mess,h->buffer->data,h->buffer->ulength);
  return GRIB_SUCCESS;

}

int grib_copy_message__(int* gid, void* mess,int* len){
  return grib_copy_message_( gid,  mess, len);
}

void grib_check_(int* err){
  if ( *err != GRIB_END_OF_FILE )
   GRIB_CHECK(*err,0);
}

void grib_check__(int* err){
  grib_check_(err);
}

int grib_write_(int* gid, int* fid) {
  grib_handle *h = get_handle(*gid);
  FILE* f = get_file(*fid);
  const void* mess = NULL;
  size_t mess_len = 0;

  if(!f) return GRIB_INVALID_FILE;
  if (!h) return GRIB_INVALID_GRIB;

  grib_get_message(h,&mess,&mess_len);

  if(fwrite(mess,1, mess_len,f) != mess_len)
  {
    perror("grib_write");
    return GRIB_IO_PROBLEM;
  }

  return GRIB_SUCCESS;
}


int grib_write__(int* gid, int* fid) {
  return grib_write_(gid,fid);
}

int grib_write(int* gid, int* fid) {
  return grib_write_(gid,fid);
}


