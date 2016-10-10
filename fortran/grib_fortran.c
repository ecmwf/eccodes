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
#include "grib_fortran_prototypes.h"

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

/* Have file ids distinct from grib ids, in order to be
 *  protected against user errors where a file id is given
 *  instead of a grib id or viceversa
 */
#define MIN_FILE_ID 50000

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t handle_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t index_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t multi_handle_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t iterator_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t keys_iterator_mutex = PTHREAD_MUTEX_INITIALIZER;

static void init() {
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&handle_mutex,&attr);
    pthread_mutex_init(&index_mutex,&attr);
    pthread_mutex_init(&multi_handle_mutex,&attr);
    pthread_mutex_init(&iterator_mutex,&attr);
    pthread_mutex_init(&keys_iterator_mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t handle_mutex;
static omp_nest_lock_t index_mutex;
static omp_nest_lock_t multi_handle_mutex;
static omp_nest_lock_t iterator_mutex;
static omp_nest_lock_t keys_iterator_mutex;

static void init()
{
    GRIB_OMP_CRITICAL(lock_fortran)
            {
        if (once == 0)
        {
            omp_init_nest_lock(&handle_mutex);
            omp_init_nest_lock(&index_mutex);
            omp_init_nest_lock(&multi_handle_mutex);
            omp_init_nest_lock(&iterator_mutex);
            omp_init_nest_lock(&keys_iterator_mutex);
            once = 1;
        }
            }
}
#endif

int GRIB_NULL=-1;
int GRIB_NULL_NEAREST=-1;
/*extern int errno;*/

typedef struct l_grib_file l_grib_file;

struct l_grib_file {
    FILE* f;
    char* buffer;
    int id;
    l_grib_file* next;
};

typedef struct l_grib_handle l_grib_handle;

struct l_grib_handle {
    int id;
    grib_handle* h;
    l_grib_handle* next;
};

typedef struct l_grib_index l_grib_index;

struct l_grib_index {
    int id;
    grib_index* h;
    l_grib_index* next;
};

typedef struct l_grib_multi_handle l_grib_multi_handle;

struct l_grib_multi_handle {
    int id;
    grib_multi_handle* h;
    l_grib_multi_handle* next;
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
static l_grib_index* index_set = NULL;
static l_grib_multi_handle* multi_handle_set = NULL;
static l_grib_file*   file_set   = NULL;
static l_grib_iterator* iterator_set = NULL;
static l_grib_keys_iterator* keys_iterator_set = NULL;

static char* cast_char(char* buf, char* fortstr,int len)
{
    char *p,*end;
    if (len == 0 || fortstr == NULL) return NULL;
    memcpy(buf,fortstr,len);
    p=buf;
    end=buf+len-1;
    while (isgraph(*p) && p != end) {
        p++;
    }
    if (*p==' ') *p='\0';
    if (p==end) *(p+1)='\0';
    else *p='\0';

    return buf;
}

static char* cast_char_no_cut(char* buf, char* fortstr,int len)
{
    if (len == 0 || fortstr == NULL) return NULL;
    memcpy(buf,fortstr,len);
    buf[len]='\0';

    return buf;
}

static void czstr_to_fortran(char* str,int len)
{
    char *p,*end;
    p=str; end=str+len-1;
    while (*p != '\0' && p != end) p++;
    while (p !=end) *(p++)=' ';
    *p=' ';
}

/*static void czstr_to_fortran_replace0(char* str,int len)
{
    char *p,*end;
    p=str; end=str+len-1;
    while (p != end) {
      if (*p=='\0') *p=' ';
      p++;
    }
}*/

static void fort_char_clean(char* str,int len)
{
    char *p,*end;
    p=str; end=str+len-1;
    while (p != end) *(p++)=' ';
    *p=' ';
}

static int push_file(FILE* f,char* buffer)
{
    l_grib_file* current  = file_set;
    l_grib_file* previous = file_set;
    l_grib_file* the_new      = NULL;
    int myindex = MIN_FILE_ID;

    if(!file_set){
        file_set = (l_grib_file*)malloc(sizeof(l_grib_file));
        Assert(file_set);
        file_set->id   = myindex;
        file_set->f    = f;
        file_set->buffer =buffer;
        file_set->next = NULL;
        return myindex;
    }

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->f    = f;
            current->buffer    = buffer;
            return current->id ;
        } else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_file*)malloc(sizeof(l_grib_file));
    Assert(the_new);
    the_new->id   = myindex;
    the_new->f    = f;
    the_new->buffer    = buffer;
    the_new->next = current;
    previous->next = the_new;
    return myindex;
}

static void _push_handle(grib_handle *h,int *gid)
{
    l_grib_handle* current= handle_set;
    l_grib_handle* previous= handle_set;
    l_grib_handle* the_new= NULL;
    int myindex= 1;

    /*
  if (*gid > 0 ) {
    while(current) {
      if(current->id == *gid) break;
      current = current->next;
    }
    if (current) {
      grib_handle_delete(current->h);
      current->h=h;
      return;
    }
  }
     */

    if(!handle_set){
        handle_set = (l_grib_handle*)malloc(sizeof(l_grib_handle));
        Assert(handle_set);
        handle_set->id   = myindex;
        handle_set->h    = h;
        handle_set->next = NULL;
        *gid=myindex;
        return;
    }

    current= handle_set;

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->h  = h;
            *gid=current->id;
            return;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_handle*)malloc(sizeof(l_grib_handle));
    Assert(the_new);
    the_new->id   = myindex;
    the_new->h    = h;
    the_new->next = current;
    previous->next = the_new;

    *gid=myindex;
    return;
}

static void _push_index(grib_index *h,int *gid)
{
    l_grib_index* current= index_set;
    l_grib_index* previous= index_set;
    l_grib_index* the_new= NULL;
    int myindex= 1;

    /*
  if (*gid > 0 ) {
    while(current) {
      if(current->id == *gid) break;
      current = current->next;
    }
    if (current) {
      grib_index_delete(current->h);
      current->h=h;
      return;
    }
  }
     */

    if(!index_set){
        index_set = (l_grib_index*)malloc(sizeof(l_grib_index));
        Assert(index_set);
        index_set->id   = myindex;
        index_set->h    = h;
        index_set->next = NULL;
        *gid=myindex;
        return;
    }

    current= index_set;

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->h  = h;
            *gid=current->id;
            return;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_index*)malloc(sizeof(l_grib_index));
    Assert(the_new);
    the_new->id   = myindex;
    the_new->h    = h;
    the_new->next = current;
    previous->next = the_new;

    *gid=myindex;
    return;
}

static void _push_multi_handle(grib_multi_handle *h,int *gid)
{
    l_grib_multi_handle* current= multi_handle_set;
    l_grib_multi_handle* previous= multi_handle_set;
    l_grib_multi_handle* the_new= NULL;
    int myindex= 1;

    /*
  if (*gid > 0 ) {
    while(current) {
      if(current->id == *gid) break;
      current = current->next;
    }
    if (current) {
      grib_multi_handle_delete(current->h);
      current->h=h;
      return;
    }
  }
     */

    if(!multi_handle_set){
        multi_handle_set = (l_grib_multi_handle*)malloc(sizeof(l_grib_multi_handle));
        Assert(multi_handle_set);
        multi_handle_set->id   = myindex;
        multi_handle_set->h    = h;
        multi_handle_set->next = NULL;
        *gid=myindex;
        return;
    }

    current= multi_handle_set;

    while(current){
        if(current->id < 0){
            current->id = -(current->id);
            current->h  = h;
            *gid=current->id;
            return;
        }
        else{
            myindex++;
            previous = current;
            current = current->next;
        }
    }

    the_new = (l_grib_multi_handle*)malloc(sizeof(l_grib_multi_handle));
    Assert(the_new);
    the_new->id   = myindex;
    the_new->h    = h;
    the_new->next = current;
    previous->next = the_new;

    *gid=myindex;
    return;
}

static void push_handle(grib_handle *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&handle_mutex);
    _push_handle(h,gid);
    GRIB_MUTEX_UNLOCK(&handle_mutex);
    return;
}

static void push_index(grib_index *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&index_mutex);
    _push_index(h,gid);
    GRIB_MUTEX_UNLOCK(&index_mutex);
    return;
}

static void push_multi_handle(grib_multi_handle *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&multi_handle_mutex);
    _push_multi_handle(h,gid);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex);
    return;
}

static int _push_iterator(grib_iterator *i)
{
    l_grib_iterator* current  = iterator_set;
    l_grib_iterator* previous = iterator_set;
    l_grib_iterator* the_new      = NULL;
    int myindex = 1;

    if(!iterator_set){
        iterator_set = (l_grib_iterator*)malloc(sizeof(l_grib_iterator));
        Assert(iterator_set);
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

    the_new = (l_grib_iterator*)malloc(sizeof(l_grib_iterator));
    Assert(the_new);
    the_new->id   = myindex;
    the_new->i    = i;
    the_new->next = current;
    previous->next = the_new;

    return myindex;
}

static int push_iterator(grib_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&iterator_mutex);
    ret=_push_iterator(i);
    GRIB_MUTEX_UNLOCK(&iterator_mutex);
    return ret;
}

static int _push_keys_iterator(grib_keys_iterator *i)
{
    l_grib_keys_iterator* current  = keys_iterator_set;
    l_grib_keys_iterator* previous = keys_iterator_set;
    l_grib_keys_iterator* the_new      = NULL;
    int myindex = 1;

    if(!keys_iterator_set){
        keys_iterator_set = (l_grib_keys_iterator*)malloc(sizeof(l_grib_keys_iterator));
        Assert(keys_iterator_set);
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

    the_new = (l_grib_keys_iterator*)malloc(sizeof(l_grib_keys_iterator));
    Assert(the_new);
    the_new->id   = myindex;
    the_new->i    = i;
    the_new->next = current;
    previous->next = the_new;

    return myindex;
}

static int push_keys_iterator(grib_keys_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    ret=_push_keys_iterator(i);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return ret;
}

static grib_handle* _get_handle(int handle_id)
{
    l_grib_handle* current= handle_set;

    while(current){
        if(current->id == handle_id) return current->h;
        current = current->next;
    }
    return NULL;
}

static grib_index* _get_index(int index_id)
{
    l_grib_index* current= index_set;

    while(current){
        if(current->id == index_id) return current->h;
        current = current->next;
    }
    return NULL;
}

static grib_multi_handle* _get_multi_handle(int multi_handle_id)
{
    l_grib_multi_handle* current= multi_handle_set;

    while(current){
        if(current->id == multi_handle_id) return current->h;
        current = current->next;
    }
    return NULL;
}

static grib_handle* get_handle(int handle_id)
{
    grib_handle* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&handle_mutex);
    h=_get_handle(handle_id);
    GRIB_MUTEX_UNLOCK(&handle_mutex);
    return h;
}

static grib_index* get_index(int index_id)
{
    grib_index* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&index_mutex);
    h=_get_index(index_id);
    GRIB_MUTEX_UNLOCK(&index_mutex);
    return h;
}

static grib_multi_handle* get_multi_handle(int multi_handle_id)
{
    grib_multi_handle* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&multi_handle_mutex);
    h=_get_multi_handle(multi_handle_id);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex);
    return h;
}

static FILE* get_file(int file_id)
{
    l_grib_file* current  = file_set;

    if ( file_id < MIN_FILE_ID ) return NULL;

    while(current){
        if(current->id == file_id) return current->f;
        current = current->next;
    }
    return NULL;
}

static grib_iterator* _get_iterator(int iterator_id)
{
    l_grib_iterator* current  = iterator_set;

    while(current){
        if(current->id == iterator_id) return current->i;
        current = current->next;
    }
    return NULL;
}

static grib_iterator* get_iterator(int iterator_id)
{
    grib_iterator* i=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&iterator_mutex);
    i=_get_iterator(iterator_id);
    GRIB_MUTEX_UNLOCK(&iterator_mutex);
    return i;
}

static grib_keys_iterator* _get_keys_iterator(int keys_iterator_id)
{
    l_grib_keys_iterator* current  = keys_iterator_set;

    while(current){
        if(current->id == keys_iterator_id) return current->i;
        current = current->next;
    }
    return NULL;
}

static grib_keys_iterator* get_keys_iterator(int keys_iterator_id)
{
    grib_keys_iterator* i=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    i=_get_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return i;
}

static int clear_file(int file_id)
{
    l_grib_file* current = file_set;
    while(current){
        if(current->id == file_id){
            current->id = -(current->id);
            if (current->f) fclose(current->f);
            if (current->buffer) free(current->buffer);
            return GRIB_SUCCESS;
        }
        current = current->next;
    }
    return GRIB_INVALID_FILE;
}

static int _clear_handle(int handle_id)
{
    l_grib_handle* current  = handle_set;
    if (handle_id<0) return 0;
    while(current){
        if(current->id == handle_id){
            current->id = -(current->id);
            if(current->h) return grib_handle_delete(current->h);
        }
        current = current->next;
    }
    return GRIB_SUCCESS;
}

static int _clear_index(int index_id)
{
    l_grib_index* current  = index_set;

    while(current){
        if(current->id == index_id){
            current->id = -(current->id);
            if (current->h) {
                grib_index_delete(current->h);
                return GRIB_SUCCESS;
            }
        }
        current = current->next;
    }
    return GRIB_SUCCESS;
}

static int _clear_multi_handle(int multi_handle_id)
{
    l_grib_multi_handle* current  = multi_handle_set;

    while(current){
        if(current->id == multi_handle_id){
            current->id = -(current->id);
            if(current->h) return grib_multi_handle_delete(current->h);
        }
        current = current->next;
    }
    return GRIB_SUCCESS;
}

static int clear_handle(int handle_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&handle_mutex);
    ret=_clear_handle(handle_id);
    GRIB_MUTEX_UNLOCK(&handle_mutex);
    return ret;
}

static int clear_index(int index_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&index_mutex);
    ret=_clear_index(index_id);
    GRIB_MUTEX_UNLOCK(&index_mutex);
    return ret;
}

static int clear_multi_handle(int multi_handle_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&multi_handle_mutex);
    ret=_clear_multi_handle(multi_handle_id);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex);
    return ret;
}

static int _clear_iterator(int iterator_id)
{
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

static int clear_iterator(int iterator_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&iterator_mutex);
    ret=_clear_iterator(iterator_id);
    GRIB_MUTEX_UNLOCK(&iterator_mutex);
    return ret;
}

static int _clear_keys_iterator(int keys_iterator_id)
{

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

static int clear_keys_iterator(int keys_iterator_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&keys_iterator_mutex);
    ret=_clear_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex);
    return ret;
}

/*****************************************************************************/
int grib_f_read_any_headers_only_from_file_(int* fid, char* buffer, size_t* nbytes) {
    grib_context* c;
    int err=0;
    FILE* f=get_file(*fid);

    if (f) {
        c=grib_context_get_default( );
        err=grib_read_any_headers_only_from_file(c,f,buffer,nbytes);
        return err;
    } else {
        return GRIB_INVALID_FILE;
    }
}
int grib_f_read_any_headers_only_from_file__(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_read_any_headers_only_from_file_(fid,buffer,nbytes);
}
int grib_f_read_any_headers_only_from_file(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_read_any_headers_only_from_file_(fid,buffer,nbytes);
}

/*****************************************************************************/
int grib_f_read_any_from_file_(int* fid, char* buffer, size_t* nbytes) {
    grib_context* c;
    int err=0;
    FILE* f=get_file(*fid);

    if (f) {
        c=grib_context_get_default( );
        err=grib_read_any_from_file(c,f,buffer,nbytes);
        return err;
    } else {
        return GRIB_INVALID_FILE;
    }
}
int grib_f_read_any_from_file__(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_read_any_from_file_(fid,buffer,nbytes);
}
int grib_f_read_any_from_file(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_read_any_from_file_(fid,buffer,nbytes);
}

/*****************************************************************************/
int grib_f_write_file_(int* fid, char* buffer, size_t* nbytes) {
    grib_context* c;
    FILE* f=get_file(*fid);

    if (f) {
        int ioerr;
        c=grib_context_get_default( );
        if( fwrite(buffer, 1, *nbytes, f)  != *nbytes) {
            ioerr=errno;
            grib_context_log(c,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s",strerror(ioerr));
            return GRIB_IO_PROBLEM;
        }
        return GRIB_SUCCESS;
    } else {
        return GRIB_INVALID_FILE;
    }
}
int grib_f_write_file__(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_write_file_(fid,buffer,nbytes);
}
int grib_f_write_file(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_write_file_(fid,buffer,nbytes);
}

/*****************************************************************************/
int grib_f_read_file_(int* fid, char* buffer, size_t* nbytes) {
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
int grib_f_read_file__(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_read_file_(fid,buffer,nbytes);
}
int grib_f_read_file(int* fid, char* buffer, size_t* nbytes) {
    return grib_f_read_file_(fid,buffer,nbytes);
}

/*****************************************************************************/
int grib_f_open_file_(int* fid, char* name , char* op, int lname, int lop) {
    FILE* f = NULL;
    int ioerr=0;
    char oper[1024]; /* GRIB-576: open mode */
    char *p;
    char fname[1024];
    int ret=GRIB_SUCCESS;
    char* iobuf=NULL;
    /*TODO Proper context passed as external parameter */
    grib_context* context=grib_context_get_default();

    cast_char(oper,op,lop);

    p=oper;

    while (*p != '\0') { *p=tolower(*p);p++;}

    f = fopen(cast_char(fname,name,lname),oper);
    if(!f) {
        ioerr=errno;
        grib_context_log(context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s: %s",strerror(ioerr),cast_char(fname,name,lname));
        *fid = -1;
        ret=GRIB_IO_PROBLEM;
    } else {
        if (context->io_buffer_size) {
            /* printf("setting vbuf = %d\n",context->io_buffer_size); */
#ifdef POSIX_MEMALIGN
            if (posix_memalign((void **)&iobuf,sysconf(_SC_PAGESIZE),context->io_buffer_size) ) {
                grib_context_log(context,GRIB_LOG_FATAL,"grib_f_open_file_: posix_memalign unable to allocate io_buffer\n");
            }
#else
            iobuf = (void*)malloc(context->io_buffer_size);
            if (!iobuf) {
                grib_context_log(context,GRIB_LOG_FATAL,"grib_f_open_file_: Unable to allocate io_buffer\n");
            }
#endif
            setvbuf(f,iobuf,_IOFBF,context->io_buffer_size);
        }
        *fid = push_file(f,iobuf);
        ret=GRIB_SUCCESS;
    }
    return ret;
}
int grib_f_open_file__(int* fid, char* name , char* op,  int lname, int lop){
    return grib_f_open_file_(       fid,       name ,       op,     lname,      lop);
}
int grib_f_open_file(int* fid, char* name , char* op,  int lname, int lop){
    return grib_f_open_file_(       fid,       name ,       op,     lname,      lop);
}

/*****************************************************************************/
int grib_f_close_file_(int* fid){
    return clear_file(*fid);
}
int grib_f_close_file__(int* fid){
    return grib_f_close_file_(fid);
}
int grib_f_close_file(int* fid){
    return grib_f_close_file_(fid);
}

/*****************************************************************************/
static int file_count=0;

void grib_f_write_on_fail(int* gid) {
    grib_context* c=grib_context_get_default();
    if (c->write_on_fail) {
        char filename[100]={0,};
        grib_handle* h=NULL;
        pid_t pid=getpid();

        GRIB_MUTEX_INIT_ONCE(&once,&init)
        GRIB_MUTEX_LOCK(&handle_mutex)
        file_count++;
        GRIB_MUTEX_UNLOCK(&handle_mutex)

        sprintf(filename,"%ld_%d_error.grib",(long)pid,file_count);

        h=get_handle(*gid);
        if (h) grib_write_message(h,filename,"w");
    }
}
void grib_f_write_on_fail_(int* gid) {
    grib_f_write_on_fail(gid);
}
void grib_f_write_on_fail__(int* gid) {
    grib_f_write_on_fail(gid);
}

/*****************************************************************************/
int grib_f_multi_support_on_(){
    grib_multi_support_on(0);
    return GRIB_SUCCESS;
}
int grib_f_multi_support_on__(){
    return grib_f_multi_support_on_();
}
int grib_f_multi_support_on(){
    return grib_f_multi_support_on_();
}

int grib_f_multi_support_off_(){
    grib_multi_support_off(0);
    return GRIB_SUCCESS;
}
int grib_f_multi_support_off__(){
    return grib_f_multi_support_off_();
}
int grib_f_multi_support_off(){
    return grib_f_multi_support_off_();
}


/*****************************************************************************/
static int _grib_f_iterator_new_(int* gid,int* iterid,int* mode) {
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
int grib_f_iterator_new_(int* gid,int* iterid,int* mode) {
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&iterator_mutex)
    ret=_grib_f_iterator_new_(gid,iterid,mode);
    GRIB_MUTEX_UNLOCK(&iterator_mutex)
    return ret;
}
int grib_f_iterator_new__(int* gid,int* iterid,int* mode) {
    return grib_f_iterator_new_(gid,iterid,mode);
}
int grib_f_iterator_new(int* gid,int* iterid,int* mode) {
    return grib_f_iterator_new_(gid,iterid,mode);
}

/*****************************************************************************/
int grib_f_iterator_next_(int* iterid,double* lat,double* lon,double* value) {
    grib_iterator* iter=get_iterator(*iterid);
    if (!iter) return GRIB_INVALID_ITERATOR;
    return grib_iterator_next(iter,lat,lon,value);
}

int grib_f_iterator_next__(int* iterid,double* lat,double* lon,double* value) {
    return grib_f_iterator_next_(iterid,lat,lon,value);
}
int grib_f_iterator_next(int* iterid,double* lat,double* lon,double* value) {
    return grib_f_iterator_next_(iterid,lat,lon,value);
}

/*****************************************************************************/
int grib_f_iterator_delete_(int* iterid) {
    return clear_iterator(*iterid);
}

int grib_f_iterator_delete__(int* iterid) {
    return grib_f_iterator_delete_(iterid);
}
int grib_f_iterator_delete(int* iterid) {
    return grib_f_iterator_delete_(iterid);
}

/*****************************************************************************/
static int _grib_f_keys_iterator_new_(int* gid,int* iterid,char* name_space,int len) {
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
int grib_f_keys_iterator_new_(int* gid,int* iterid,char* name_space,int len) {
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    ret=_grib_f_keys_iterator_new_(gid,iterid,name_space,len);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
    return ret;
}

int grib_f_keys_iterator_new__(int* gid,int* iterid,char* name_space,int len) {
    return grib_f_keys_iterator_new_(gid,iterid,name_space,len);
}
int grib_f_keys_iterator_new(int* gid,int* iterid,char* name_space,int len) {
    return grib_f_keys_iterator_new_(gid,iterid,name_space,len);
}

/*****************************************************************************/
int grib_f_keys_iterator_next_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;

    return grib_keys_iterator_next(iter);
}

int grib_f_keys_iterator_next__(int* iterid) {
    return grib_f_keys_iterator_next_(iterid);
}
int grib_f_keys_iterator_next(int* iterid) {
    return grib_f_keys_iterator_next_(iterid);
}

int grib_f_keys_iterator_delete_(int* iterid) {
    return clear_keys_iterator(*iterid);
}
int grib_f_keys_iterator_delete__(int* iterid) {
    return grib_f_keys_iterator_delete_(iterid);
}
int grib_f_keys_iterator_delete(int* iterid) {
    return grib_f_keys_iterator_delete_(iterid);
}

/*****************************************************************************/
int grib_f_gribex_mode_on_() {
    grib_gribex_mode_on(0);
    return GRIB_SUCCESS;
}
int grib_f_gribex_mode_on__() {
    grib_gribex_mode_on(0);
    return GRIB_SUCCESS;
}
int grib_f_gribex_mode_on() {
    grib_gribex_mode_on(0);
    return GRIB_SUCCESS;
}

int grib_f_gribex_mode_off_() {
    grib_gribex_mode_off(0);
    return GRIB_SUCCESS;
}
int grib_f_gribex_mode_off__() {
    grib_gribex_mode_off(0);
    return GRIB_SUCCESS;
}
int grib_f_gribex_mode_off() {
    grib_gribex_mode_off(0);
    return GRIB_SUCCESS;
}

/*****************************************************************************/
int grib_f_skip_computed_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_COMPUTED);
}
int grib_f_skip_computed__(int* iterid) {
    return grib_f_skip_computed_(iterid);
}
int grib_f_skip_computed(int* iterid) {
    return grib_f_skip_computed_(iterid);
}

int grib_f_skip_coded_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_CODED);
}
int grib_f_skip_coded__(int* iterid) {
    return grib_f_skip_coded_(iterid);
}
int grib_f_skip_coded(int* iterid) {
    return grib_f_skip_coded_(iterid);
}

int grib_f_skip_edition_specific_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC);
}
int grib_f_skip_edition_specific__(int* iterid) {
    return grib_f_skip_edition_specific_(iterid);
}
int grib_f_skip_edition_specific(int* iterid) {
    return grib_f_skip_edition_specific_(iterid);
}

int grib_f_skip_duplicates_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_DUPLICATES);
}
int grib_f_skip_duplicates__(int* iterid) {
    return grib_f_skip_duplicates_(iterid);
}
int grib_f_skip_duplicates(int* iterid) {
    return grib_f_skip_duplicates_(iterid);
}

int grib_f_skip_read_only_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_READ_ONLY);
}
int grib_f_skip_read_only__(int* iterid) {
    return grib_f_skip_read_only_(iterid);
}
int grib_f_skip_read_only(int* iterid) {
    return grib_f_skip_read_only_(iterid);
}

int grib_f_skip_function_(int* iterid) {
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_FUNCTION);
}
int grib_f_skip_function__(int* iterid) {
    return grib_f_skip_function_(iterid);
}
int grib_f_skip_function(int* iterid) {
    return grib_f_skip_function_(iterid);
}

/*****************************************************************************/
int grib_f_keys_iterator_get_name_(int* iterid,char* name,int len) {
    size_t lsize=len;
    char buf[1024]={0,};

    grib_keys_iterator* kiter=get_keys_iterator(*iterid);

    if (!kiter) return GRIB_INVALID_KEYS_ITERATOR;

    fort_char_clean(name,len);

    sprintf(buf,"%s",grib_keys_iterator_get_name(kiter));
    lsize=strlen(buf);
    if (len < lsize) return GRIB_ARRAY_TOO_SMALL;

    memcpy(name,buf,lsize);

    czstr_to_fortran(name,len);

    return 0;
}
int grib_f_keys_iterator_get_name__(int* kiter,char* name,int len) {
    return grib_f_keys_iterator_get_name_(kiter,name,len);
}
int grib_f_keys_iterator_get_name(int* kiter,char* name,int len) {
    return grib_f_keys_iterator_get_name_(kiter,name,len);
}

/*****************************************************************************/
int grib_f_keys_iterator_rewind_(int* kiter) {
    grib_keys_iterator* i=get_keys_iterator(*kiter);

    if (!i) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_rewind(i);
}
int grib_f_keys_iterator_rewind__(int* kiter) {
    return grib_f_keys_iterator_rewind_(kiter);
}
int grib_f_keys_iterator_rewind(int* kiter) {
    return grib_f_keys_iterator_rewind_(kiter);
}

/*****************************************************************************/
int grib_f_new_from_message_(int* gid, void* buffer , size_t* bufsize){
    grib_handle *h = NULL;
    h = grib_handle_new_from_message_copy(0, buffer, *bufsize);
    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}
int grib_f_new_from_message__(int* gid, void* buffer , size_t* bufsize){
    return grib_f_new_from_message_(gid,  buffer ,  bufsize);
}
int grib_f_new_from_message(int* gid, void* buffer , size_t* bufsize){
    return grib_f_new_from_message_(gid,  buffer ,  bufsize);
}

/*****************************************************************************/
int grib_f_new_from_message_copy_(int* gid, void* buffer , size_t* bufsize){
    grib_handle *h = NULL;
    h = grib_handle_new_from_message_copy(0, buffer, *bufsize);
    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}

int grib_f_new_from_message_copy__(int* gid, void* buffer , size_t* bufsize){
    return grib_f_new_from_message_copy_(gid,  buffer ,  bufsize);
}
int grib_f_new_from_message_copy(int* gid, void* buffer , size_t* bufsize){
    return grib_f_new_from_message_copy_(gid,  buffer ,  bufsize);
}

/*****************************************************************************/
int grib_f_new_from_samples_(int* gid, char* name , int lname){
    char fname[1024];
    grib_handle *h = NULL;

    h = grib_handle_new_from_samples(NULL,cast_char(fname,name,lname));
    /*   grib_context_set_debug(h->context,1);*/

    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_FILE_NOT_FOUND;
}

int grib_f_new_from_samples__(int* gid, char* name , int lname){
    return  grib_f_new_from_samples_( gid,  name ,  lname);
}
int grib_f_new_from_samples(int* gid, char* name , int lname){
    return  grib_f_new_from_samples_( gid,  name ,  lname);
}

/*****************************************************************************/
int codes_bufr_f_new_from_samples_(int* gid, char* name , int lname){
    char fname[1024];
    grib_handle *h = NULL;

    h = codes_bufr_handle_new_from_samples(NULL,cast_char(fname,name,lname));
    /*   grib_context_set_debug(h->context,1);*/

    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_FILE_NOT_FOUND;
}

int codes_bufr_f_new_from_samples__(int* gid, char* name , int lname){
    return  codes_bufr_f_new_from_samples_( gid,  name ,  lname);
}
int codes_bufr_f_new_from_samples(int* gid, char* name , int lname){
    return  codes_bufr_f_new_from_samples_( gid,  name ,  lname);
}

/*****************************************************************************/
int grib_f_clone_(int* gidsrc,int* giddest){
    grib_handle *src  = get_handle(*gidsrc);
    grib_handle *dest = NULL;

    if(src){
        dest = grib_handle_clone(src);
        if(dest){
            push_handle(dest,giddest);
            return GRIB_SUCCESS;
        }
    }

    *giddest = -1;
    return GRIB_INVALID_GRIB;
}
int grib_f_clone__(int* gidsrc,int* giddest){
    return grib_f_clone_(gidsrc, giddest);
}
int grib_f_clone(int* gidsrc,int* giddest){
    return grib_f_clone_(gidsrc, giddest);
}

/*****************************************************************************/
int grib_f_util_sections_copy_(int* gidfrom,int* gidto,int* what,int *gidout){
    int err=0;
    grib_handle *hfrom  = get_handle(*gidfrom);
    grib_handle *hto = get_handle(*gidto);
    grib_handle *out =0;

    if(hfrom && hto) out=grib_util_sections_copy(hfrom,hto,*what,&err);
    if(out){
        push_handle(out,gidout);
        return GRIB_SUCCESS;
    }

    return err;
}
int grib_f_util_sections_copy__(int* gidfrom,int* gidto,int* what,int *gidout){
    return grib_f_util_sections_copy_(gidfrom,gidto,what,gidout);
}
int grib_f_util_sections_copy(int* gidfrom,int* gidto,int* what,int *gidout){
    return grib_f_util_sections_copy_(gidfrom,gidto,what,gidout);
}

/*****************************************************************************/
int grib_f_copy_namespace_(int* gidsrc,char* name,int* giddest,int len){
    char buf[1024]={0,};
    grib_handle *src  = get_handle(*gidsrc);
    grib_handle *dest = get_handle(*giddest);

    if(src && dest)
        return grib_copy_namespace(dest,cast_char(buf,name,len),src);

    return GRIB_INVALID_GRIB;
}
int grib_f_copy_namespace__(int* gidsrc,char* name,int* giddest,int len){
    return grib_f_copy_namespace_(gidsrc,name,giddest,len);
}
int grib_f_copy_namespace(int* gidsrc,char* name,int* giddest,int len){
    return grib_f_copy_namespace_(gidsrc,name,giddest,len);
}

/*****************************************************************************/
int grib_f_count_in_file(int* fid,int* n) {
    int err = 0;
    FILE* f = get_file(*fid);
    if (f) err=grib_count_in_file(0, f,n);
    return err;
}
int grib_f_count_in_file_(int* fid,int* n) {
    return grib_f_count_in_file(fid,n);
}
int grib_f_count_in_file__(int* fid,int* n) {
    return grib_f_count_in_file(fid,n);
}

/*****************************************************************************/
int any_f_new_from_file_(int* fid, int* gid){
    int err = 0;
    FILE* f = get_file(*fid);
    grib_handle *h = NULL;

    if(f){
        h = codes_handle_new_from_file(0,f,PRODUCT_ANY,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }
    *gid=-1;
    return GRIB_INVALID_FILE;
}
int any_f_new_from_file__(int* fid, int* gid){
    return any_f_new_from_file_( fid, gid);
}
int any_f_new_from_file(int* fid, int* gid){
    return any_f_new_from_file_( fid, gid);
}

/*****************************************************************************/
int bufr_f_new_from_file_(int* fid, int* gid){
    int err = 0;
    FILE* f = get_file(*fid);

    grib_handle *h = NULL;

    if(f){
        h = codes_handle_new_from_file(0,f,PRODUCT_BUFR,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}

int bufr_f_new_from_file__(int* fid, int* gid){
    return bufr_f_new_from_file_( fid, gid);
}
int bufr_f_new_from_file(int* fid, int* gid){
    return bufr_f_new_from_file_( fid, gid);
}

/*****************************************************************************/
int grib_f_new_from_file_(int* fid, int* gid){
    int err = 0;
    FILE* f = get_file(*fid);

    grib_handle *h = NULL;

    if(f){
        h = grib_handle_new_from_file(0,f,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}
int grib_f_new_from_file__(int* fid, int* gid){
    return grib_f_new_from_file_( fid, gid);
}
int grib_f_new_from_file(int* fid, int* gid){
    return grib_f_new_from_file_( fid, gid);
}

/*****************************************************************************/
int grib_f_headers_only_new_from_file_(int* fid, int* gid){
    int err = 0;
    FILE* f = get_file(*fid);

    grib_handle *h = NULL;

    if(f){
        h=grib_new_from_file ( 0, f,1,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}
int grib_f_headers_only_new_from_file__(int* fid, int* gid){
    return grib_f_headers_only_new_from_file_( fid, gid);
}
int grib_f_headers_only_new_from_file(int* fid, int* gid){
    return grib_f_headers_only_new_from_file_( fid, gid);
}

/*****************************************************************************/
int grib_f_new_from_index_(int* iid, int* gid){
    int err = 0;
    grib_index* i = get_index(*iid);

    grib_handle *h = NULL;

    if(i){
        h = grib_handle_new_from_index(i,&err);
        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_INDEX;
        }
    }

    *gid=-1;
    return GRIB_INVALID_INDEX;
}

int grib_f_new_from_index__(int* iid, int* gid){
    return grib_f_new_from_index_(iid,gid);
}
int grib_f_new_from_index(int* iid, int* gid){
    return grib_f_new_from_index_(iid,gid);
}

/*****************************************************************************/
int grib_f_index_new_from_file_(char* file ,char* keys ,int* gid, int lfile, int lkeys){
    int err = 0;
    char fname[1024]={0,};
    char knames[1024]={0,};

    grib_index *i = NULL;

    if(*file){
        i = grib_index_new_from_file(0,cast_char(fname,file,lfile),
                cast_char(knames,keys,lkeys),&err);
        if(i){
            push_index(i,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}
int grib_f_index_new_from_file__(char* file, char* keys, int* gid, int lfile, int lkeys){
    return grib_f_index_new_from_file_(file ,keys ,gid, lfile, lkeys);
}
int grib_f_index_new_from_file(char* file, char* keys, int* gid, int lfile, int lkeys){
    return grib_f_index_new_from_file_(file ,keys ,gid, lfile, lkeys);
}

/*****************************************************************************/
int grib_f_index_add_file_(int* iid, char* file, int lfile) {
    grib_index *i = get_index(*iid);
    int err = GRIB_SUCCESS;
    char buf[1024];

    if (!i) {
        return GRIB_INVALID_INDEX;
    } else {
        err = grib_index_add_file(i,cast_char(buf,file,lfile));
        return err;
    }
}

int grib_f_index_add_file__(int* iid, char* file, int lfile) {
    return grib_f_index_add_file_(iid,file,lfile);
}

int grib_f_index_add_file(int* iid, char* file, int lfile) {
    return grib_f_index_add_file_(iid,file,lfile);
}

/*****************************************************************************/
int grib_f_index_read_(char* file, int* gid, int lfile) {
    int err = 0;
    char fname[1024]={0,};

    grib_index *i = NULL;

    if (*file) {
        i = grib_index_read(0,cast_char(fname,file,lfile),&err);
        if (i) {
            push_index(i,gid);
            return GRIB_SUCCESS;
        } else {
            *gid = -1;
            return GRIB_END_OF_FILE;
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}
int grib_f_index_read__(char* file, int* gid, int lfile) {
    return grib_f_index_read_(file,gid,lfile);
}
int grib_f_index_read(char* file, int* gid, int lfile) {
    return grib_f_index_read_(file,gid,lfile);
}

/*****************************************************************************/
int grib_f_index_write_(int* gid, char* file, int lfile) {
    grib_index *i = get_index(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];

    if (!i) {
        return GRIB_INVALID_GRIB;
    } else {
        err = grib_index_write(i,cast_char(buf,file,lfile));
        return err;
    }
}
int grib_f_index_write__(int* gid, char* file, int lfile) {
    return grib_f_index_write_(gid,file,lfile);
}
int grib_f_index_write(int* gid, char* file, int lfile) {
    return grib_f_index_write_(gid,file,lfile);
}

/*****************************************************************************/
int grib_f_index_release_(int* hid){
    return clear_index(*hid);
}

int grib_f_index_release__(int* hid){
    return grib_f_index_release_(hid);
}

int grib_f_index_release(int* hid){
    return grib_f_index_release_(hid);
}

int grib_f_multi_handle_release_(int* hid){
    return clear_multi_handle(*hid);
}

int grib_f_multi_handle_release__(int* hid){
    return grib_f_multi_handle_release_(hid);
}

int grib_f_multi_handle_release(int* hid){
    return grib_f_multi_handle_release_(hid);
}

/*****************************************************************************/
int grib_f_release_(int* hid){
    return clear_handle(*hid);
}
int grib_f_release__(int* hid){
    return grib_f_release_( hid);
}
int grib_f_release(int* hid){
    return grib_f_release_( hid);
}

/*****************************************************************************/
static void do_the_dump(grib_handle* h)
{
    if (h->product_kind == PRODUCT_GRIB)
    {
        const int dump_flags = GRIB_DUMP_FLAG_VALUES
                |  GRIB_DUMP_FLAG_READ_ONLY
                |  GRIB_DUMP_FLAG_ALIASES
                |  GRIB_DUMP_FLAG_TYPE;
        grib_dump_content(h,stdout,"debug", dump_flags, NULL);
    }
    else
    {
        grib_dump_content(h,stdout,"wmo",0,NULL);
    }
}
int grib_f_dump_(int* gid){
    grib_handle *h = get_handle(*gid);

    if(!h)
        return GRIB_INVALID_GRIB;
    else
        do_the_dump(h);

    return GRIB_SUCCESS;
}
int grib_f_dump__(int* gid){
    return grib_f_dump_( gid);
}
int grib_f_dump(int* gid){
    return grib_f_dump_( gid);
}

/*****************************************************************************/
int grib_f_print_(int* gid, char* key, int len){
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
int grib_f_print__(int* gid, char* key,  int len){
    return grib_f_print_( gid,  key, len);
}
int grib_f_print(int* gid, char* key,  int len){
    return grib_f_print_( gid,  key, len);
}

/*****************************************************************************/
int grib_f_get_error_string_(int* err, char* buf,  int len){
    const char* err_msg = grib_get_error_message(*err);
    size_t erlen = strlen(err_msg);
    if( len <  erlen) return GRIB_ARRAY_TOO_SMALL;
    strncpy(buf, err_msg,(size_t)erlen);
    return GRIB_SUCCESS;
}
int grib_f_get_error_string__(int* err, char* buf,  int len){
    return grib_f_get_error_string_(err,buf,len);
}
int grib_f_get_error_string(int* err, char* buf,  int len){
    return grib_f_get_error_string_(err,buf,len);
}

/*****************************************************************************/
int grib_f_get_size_int_(int* gid, char* key, int* val, int len){
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
int grib_f_get_size_int__(int* gid, char* key, int* val,  int len){
    return grib_f_get_size_int_( gid,  key,  val,  len);
}
int grib_f_get_size_int(int* gid, char* key, int* val,  int len){
    return grib_f_get_size_int_( gid,  key,  val,  len);
}

int grib_f_get_size_long_(int* gid, char* key, long* val, int len){
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
int grib_f_get_size_long__(int* gid, char* key, long* val,  int len){
    return grib_f_get_size_long_( gid,  key,  val,  len);
}
int grib_f_get_size_long(int* gid, char* key, long* val,  int len){
    return grib_f_get_size_long_( gid,  key,  val,  len);
}

int grib_f_index_get_size_int_(int* gid, char* key, int* val, int len){
    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t tsize = 0;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_index_get_size(h, cast_char(buf,key,len), &tsize);
        *val = tsize;
        return  err;
    }
}
int grib_f_index_get_size_int__(int* gid, char* key, int* val,  int len){
    return grib_f_index_get_size_int_( gid,  key,  val,  len);
}
int grib_f_index_get_size_int(int* gid, char* key, int* val,  int len){
    return grib_f_index_get_size_int_( gid,  key,  val,  len);
}

int grib_f_index_get_size_long_(int* gid, char* key, long* val, int len){
    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t tsize = 0;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_index_get_size(h, cast_char(buf,key,len), &tsize);
        *val = tsize;
        return  err;
    }
}
int grib_f_index_get_size_long__(int* gid, char* key, long* val,  int len){
    return grib_f_index_get_size_long_( gid,  key,  val,  len);
}
int grib_f_index_get_size_long(int* gid, char* key, long* val,  int len){
    return grib_f_index_get_size_long_( gid,  key,  val,  len);
}

int grib_f_get_int_(int* gid, char* key, int* val,  int len){
    grib_handle *h = get_handle(*gid);
    long long_val;
    int err = GRIB_SUCCESS;
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    err = grib_get_long(h, cast_char(buf,key,len),&long_val);
    *val = long_val;
    return err;
}
int grib_f_get_int__(int* gid, char* key, int* val,  int len){
    return grib_f_get_int_( gid,  key,  val,   len);
}
int grib_f_get_int(int* gid, char* key, int* val,  int len){
    return grib_f_get_int_( gid,  key,  val,   len);
}
int grib_f_get_long_(int* gid, char* key, long* val,  int len){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    err = grib_get_long(h, cast_char(buf,key,len),val);
    return err;
}
int grib_f_get_long__(int* gid, char* key, long* val,  int len){
    return grib_f_get_long_( gid,  key,  val,   len);
}
int grib_f_get_long(int* gid, char* key, long* val,  int len){
    return grib_f_get_long_( gid,  key,  val,   len);
}

/*****************************************************************************/
int grib_f_get_int_array_(int* gid, char* key, int *val, int* size,  int len){

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
        long_val = (long*)grib_context_malloc(h->context,(*size)*(sizeof(long)));
    else
        long_val = (long*)grib_context_malloc(h->context,(sizeof(long)));

    if(!long_val) return GRIB_OUT_OF_MEMORY;
    err = grib_get_long_array(h, cast_char(buf,key,len), long_val, &lsize);

    for(*size=0;*size<lsize;(*size)++)
        val[*size] = long_val[*size];

    grib_context_free(h->context,long_val);
    return  err;
}
int grib_f_get_int_array__(int* gid, char* key, int*val, int* size, int len){
    return grib_f_get_int_array_( gid,  key, val,  size,  len);
}
int grib_f_get_int_array(int* gid, char* key, int*val, int* size, int len){
    return grib_f_get_int_array_( gid,  key, val,  size,  len);
}
/*****************************************************************************/
int grib_f_get_long_array_(int* gid, char* key, long *val, int* size,  int len){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;

    err = grib_get_long_array(h, cast_char(buf,key,len), val, &lsize);
    *size=lsize;

    return  err;
}
int grib_f_get_long_array__(int* gid, char* key, long *val, int* size, int len){
    return grib_f_get_long_array_( gid,  key, val,  size,  len);
}
int grib_f_get_long_array(int* gid, char* key, long *val, int* size, int len){
    return grib_f_get_long_array_( gid,  key, val,  size,  len);
}

/*****************************************************************************/
int grib_f_get_byte_array_(int* gid, char* key, unsigned char *val, int* size, int len, int lenv){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;

    err = grib_get_bytes(h, cast_char(buf,key,len), val, &lsize);
    *size = (int) lsize;

    return  err;
}
int grib_f_get_byte_array__(int* gid, char* key, unsigned char *val, int* size, int len, int lenv){
    return grib_f_get_byte_array_( gid, key, val, size, len, lenv);
}
int grib_f_get_byte_array(int* gid, char* key,  unsigned char *val, int* size, int len, int lenv){
    return grib_f_get_byte_array_( gid, key, val, size, len, lenv);
}

/*****************************************************************************/
int grib_f_index_get_string_(int* gid, char* key, char* val, int *eachsize,int* size,  int len){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    int i;
    char buf[1024];
    size_t lsize = *size;
    char** bufval;
    char* p=val;

    if(!h)  return GRIB_INVALID_GRIB;

    bufval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)* *size);

    err = grib_index_get_string(h, cast_char(buf,key,len), bufval, &lsize);
    *size = lsize;

    if (err) return err;

    for (i=0;i<lsize;i++) {
        int l=strlen(bufval[i]);
        int j;
        if (*eachsize < l ) {
            printf("eachsize=%d strlen(bufval[i])=%ld\n",*eachsize,(long)strlen(bufval[i]));
            grib_context_free(h->context,bufval);
            return GRIB_ARRAY_TOO_SMALL;
        }
        memcpy(p,bufval[i],l);
        p+=l;
        for (j=0;j<*eachsize-l;j++) *(p++)=' ';
    }
    grib_context_free(h->context,bufval);

    return  err;
}
int grib_f_index_get_string__(int* gid, char* key, char *val, int* eachsize, int* size,  int len){
    return grib_f_index_get_string_(gid,key,val,eachsize,size,len);
}
int grib_f_index_get_string(int* gid, char* key, char* val, int* eachsize, int* size,  int len){
    return grib_f_index_get_string_(gid,key,val,eachsize,size,len);
}

/*****************************************************************************/
int grib_f_index_get_long_(int* gid, char* key, long *val, int* size,  int len){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;
    err = grib_index_get_long(h, cast_char(buf,key,len), val, &lsize);
    *size = lsize;
    return  err;
}
int grib_f_index_get_long__(int* gid, char* key, long *val, int* size,  int len){
    return grib_f_index_get_long_(gid,key,val,size,len);
}
int grib_f_index_get_long(int* gid, char* key, long *val, int* size,  int len){
    return grib_f_index_get_long_(gid,key,val,size,len);
}

/*****************************************************************************/
int grib_f_index_get_int_(int* gid, char* key, int *val, int* size,  int len){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    long* lval=0;
    int i;

    if(!h)  return GRIB_INVALID_GRIB;

    lval=(long*)grib_context_malloc(h->context,sizeof(long)* *size);
    if (!lval) return GRIB_OUT_OF_MEMORY;

    err = grib_index_get_long(h, cast_char(buf,key,len), lval, &lsize);
    for (i=0;i<lsize;i++) val[i]=lval[i];

    *size = lsize;
    return  err;
}
int grib_f_index_get_int__(int* gid, char* key, int *val, int* size,  int len){
    return grib_f_index_get_int_(gid,key,val,size,len);
}
int grib_f_index_get_int(int* gid, char* key, int *val, int* size,  int len){
    return grib_f_index_get_int_(gid,key,val,size,len);
}

/*****************************************************************************/
int grib_f_index_get_real8_(int* gid, char* key, double *val, int* size,  int len){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;
    err = grib_index_get_double(h, cast_char(buf,key,len), val, &lsize);
    *size = lsize;
    return  err;
}
int grib_f_index_get_real8__(int* gid, char* key, double *val, int* size,  int len){
    return grib_f_index_get_real8_(gid,key,val,size,len);
}
int grib_f_index_get_real8(int* gid, char* key, double *val, int* size,  int len){
    return grib_f_index_get_real8_(gid,key,val,size,len);
}

/*****************************************************************************/
int grib_f_set_int_array_(int* gid, char* key, int* val, int* size,  int len){
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
        long_val = (long*)grib_context_malloc(h->context,(lsize)*(sizeof(long)));
    else
        long_val = (long*)grib_context_malloc(h->context,(sizeof(long)));

    if(!long_val) return GRIB_OUT_OF_MEMORY;

    for(lsize=0;lsize<(*size);lsize++)
        long_val[lsize] = val[lsize];

    err = grib_set_long_array(h, cast_char(buf,key,len), long_val, lsize);

    grib_context_free(h->context,long_val);
    return err;
}
int grib_f_set_int_array__(int* gid, char* key, int* val, int* size,  int len){
    return grib_f_set_int_array_( gid,  key,  val,  size,   len);
}
int grib_f_set_int_array(int* gid, char* key, int* val, int* size,  int len){
    return grib_f_set_int_array_( gid,  key,  val,  size,   len);
}

/*****************************************************************************/
int grib_f_set_long_array_(int* gid, char* key, long* val, int* size,  int len){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    return  grib_set_long_array(h, cast_char(buf,key,len), val, lsize);

    return err;
}
int grib_f_set_long_array__(int* gid, char* key, long* val, int* size,  int len){
    return grib_f_set_long_array_( gid,  key,  val,  size,   len);
}
int grib_f_set_long_array(int* gid, char* key, long* val, int* size,  int len){
    return grib_f_set_long_array_( gid,  key,  val,  size,   len);
}

/*****************************************************************************/
int grib_f_set_byte_array_(int* gid, char* key, unsigned char* val, int* size, int len, int lenv){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    err = grib_set_bytes(h, cast_char(buf,key,len), val, &lsize);
    *size = (int) lsize;

    return err;
}
int grib_f_set_byte_array__(int* gid, char* key, unsigned char* val, int* size, int len, int lenv){
    return grib_f_set_byte_array_( gid, key, val, size, len, lenv);
}
int grib_f_set_byte_array(int* gid, char* key, unsigned char* val, int* size, int len, int lenv){
    return grib_f_set_byte_array_( gid, key, val, size, len, lenv);
}

/*****************************************************************************/
int grib_f_set_int_(int* gid, char* key, int* val, int len){
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    long long_val = *val;
    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_long(h, cast_char(buf,key,len), long_val);
}
int grib_f_set_int__(int* gid, char* key, int* val, int len){
    return  grib_f_set_int_( gid,  key,  val, len);
}
int grib_f_set_int(int* gid, char* key, int* val, int len){
    return  grib_f_set_int_( gid,  key,  val, len);
}

int grib_f_set_long_(int* gid, char* key, long* val, int len){
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_long(h, cast_char(buf,key,len), *val);
}
int grib_f_set_long__(int* gid, char* key, long* val, int len){
    return  grib_f_set_long_( gid,  key,  val, len);
}
int grib_f_set_long(int* gid, char* key, long* val, int len){
    return  grib_f_set_long_( gid,  key,  val, len);
}

/*****************************************************************************/
int grib_f_set_missing_(int* gid, char* key,int len){

    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if(!h)  return GRIB_INVALID_GRIB;

    return grib_set_missing(h, cast_char(buf,key,len));
}
int grib_f_set_missing__(int* gid, char* key,  int len){
    return grib_f_set_missing_( gid,  key,   len);
}
int grib_f_set_missing(int* gid, char* key,  int len){
    return grib_f_set_missing_( gid,  key,   len);
}

int grib_f_is_missing_(int* gid, char* key,int* isMissing,int len){
    int err=0;
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if(!h)  return GRIB_INVALID_GRIB;

    *isMissing=grib_is_missing(h, cast_char(buf,key,len),&err);
    return err;
}
int grib_f_is_missing__(int* gid, char* key,int* isMissing,int len){
    return grib_f_is_missing_(gid,key,isMissing,len);
}
int grib_f_is_missing(int* gid, char* key,int* isMissing,int len){
    return grib_f_is_missing_(gid,key,isMissing,len);
}

/*****************************************************************************/
int grib_f_is_defined_(int* gid, char* key,int* isDefined,int len){
    grib_handle *h = get_handle(*gid);
    char buf[1024];
    if(!h)  return GRIB_INVALID_GRIB;

    *isDefined=grib_is_defined(h, cast_char(buf,key,len));
    return GRIB_SUCCESS;
}
int grib_f_is_defined__(int* gid, char* key,int* isDefined,int len){
    return grib_f_is_defined_(gid,key,isDefined,len);
}
int grib_f_is_defined(int* gid, char* key,int* isDefined,int len){
    return grib_f_is_defined_(gid,key,isDefined,len);
}

/*****************************************************************************/
int grib_f_set_real4_(int* gid, char* key, float* val, int len){

    grib_handle *h = get_handle(*gid);
    char buf[1024];
    double val8 = *val;
    if(!h)  return GRIB_INVALID_GRIB;

    return grib_set_double(h, cast_char(buf,key,len), val8);
}
int grib_f_set_real4__(int* gid, char* key, float* val,  int len){
    return grib_f_set_real4_( gid,  key,  val,   len);
}
int grib_f_set_real4(int* gid, char* key, float* val,  int len){
    return grib_f_set_real4_( gid,  key,  val,   len);
}

int grib_f_get_real4_element_(int* gid, char* key, int* index,float* val, int len){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    double val8 = 0;

    if(!h) return GRIB_INVALID_GRIB;

    err = grib_get_double_element(h, cast_char(buf,key,len), *index,&val8);
    *val = val8;
    return err;
}
int grib_f_get_real4_element__(int* gid, char* key,int* index, float* val,int len){
    return grib_f_get_real4_element_( gid,  key, index, val,  len);
}
int grib_f_get_real4_element(int* gid, char* key,int* index, float* val,int len){
    return grib_f_get_real4_element_( gid,  key, index, val,  len);
}

int grib_f_get_real4_elements_(int* gid, char* key,int* index, float *val,int* size, int len){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    long i=0;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    else
        val8 = (double*)grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;


    err = grib_get_double_elements(h, cast_char(buf,key,len), index,(long)lsize,val8);

    for(i=0;i<lsize;(i)++)
        val[i] = val8[i];

    grib_context_free(h->context,val8);

    return  err;
}
int grib_f_get_real4_elements__(int* gid, char* key,int* index, float* val,int* len,int size){
    return grib_f_get_real4_elements_( gid,  key, index, val,  len,size);
}
int grib_f_get_real4_elements(int* gid, char* key,int* index, float* val,int* len,int size){
    return grib_f_get_real4_elements_( gid,  key, index, val,  len,size);
}

int grib_f_get_real4_(int* gid, char* key, float* val, int len){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    double val8 = 0;

    if(!h)  return GRIB_INVALID_GRIB;

    err = grib_get_double(h, cast_char(buf,key,len), &val8);
    *val = val8;
    return err;
}
int grib_f_get_real4__(int* gid, char* key, float* val,  int len){
    return grib_f_get_real4_( gid,  key,  val,  len);
}
int grib_f_get_real4(int* gid, char* key, float* val,  int len){
    return grib_f_get_real4_( gid,  key,  val,  len);
}

int grib_f_get_real4_array_(int* gid, char* key, float *val, int* size,  int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    double* val8 = NULL;
    size_t i;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    else
        val8 = (double*)grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    err  = grib_get_double_array(h, cast_char(buf,key,len), val8, &lsize);
    if (err) {
        grib_context_free(h->context,val8);
        return err;
    }

    for(i=0;i<lsize;i++)
        val[i] = val8[i];

    grib_context_free(h->context,val8);

    return  err;
}
int grib_f_get_real4_array__(int* gid, char* key, float* val, int* size, int len){
    return grib_f_get_real4_array_( gid,  key, val,  size,  len);
}
int grib_f_get_real4_array(int* gid, char* key, float* val, int* size, int len){
    return grib_f_get_real4_array_( gid,  key, val,  size,  len);
}

/*****************************************************************************/
int grib_f_set_force_real4_array_(int* gid, char* key, float*val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = (double*)grib_context_malloc(h->context,lsize*(sizeof(double)));
    else
        val8 = (double*)grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    for(lsize=0;lsize<*size;lsize++)
        val8[lsize] = val[lsize];

    err = grib_set_force_double_array(h, cast_char(buf,key,len), val8, lsize);
    grib_context_free(h->context,val8);
    return err;
}
int grib_f_set_force_real4_array__(int* gid, char* key, float*val, int* size, int len){
    return grib_f_set_force_real4_array_( gid,  key, val,  size, len);
}
int grib_f_set_force_real4_array(int* gid, char* key, float*val, int* size, int len){
    return grib_f_set_force_real4_array_( gid,  key, val,  size, len);
}

/*****************************************************************************/
int grib_f_set_real4_array_(int* gid, char* key, float*val, int* size, int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = (double*)grib_context_malloc(h->context,lsize*(sizeof(double)));
    else
        val8 = (double*)grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    for(lsize=0;lsize<*size;lsize++)
        val8[lsize] = val[lsize];

    err = grib_set_double_array(h, cast_char(buf,key,len), val8, lsize);
    grib_context_free(h->context,val8);
    return err;
}
int grib_f_set_real4_array__(int* gid, char* key, float*val, int* size, int len){
    return grib_f_set_real4_array_( gid,  key, val,  size, len);
}
int grib_f_set_real4_array(int* gid, char* key, float*val, int* size, int len){
    return grib_f_set_real4_array_( gid,  key, val,  size, len);
}

/*****************************************************************************/
int grib_f_index_select_real8_(int* gid, char* key, double* val, int len)
{
    grib_index *h = get_index(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_double(h, cast_char(buf,key,len), *val);
}
int grib_f_index_select_real8__(int* gid, char* key, double* val, int len){
    return grib_f_index_select_real8_(gid,key,val,len);
}
int grib_f_index_select_real8(int* gid, char* key, double* val, int len){
    return grib_f_index_select_real8_(gid,key,val,len);
}

/*****************************************************************************/
int grib_f_index_select_string_(int* gid, char* key, char* val, int len, int vallen)
{
    grib_index *h = get_index(*gid);

    char buf[1024];
    char bufval[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_string(h, cast_char(buf,key,len), cast_char(bufval,val,vallen));
}
int grib_f_index_select_string__(int* gid, char* key, char* val, int len, int vallen){
    return grib_f_index_select_string_(gid,key,val,len,vallen);
}
int grib_f_index_select_string(int* gid, char* key, char* val, int len, int vallen){
    return grib_f_index_select_string_(gid,key,val,len,vallen);
}

/*****************************************************************************/
int grib_f_index_select_int_(int* gid, char* key, int* val, int len)
{
    grib_index *h = get_index(*gid);
    long lval=*val;
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_long(h, cast_char(buf,key,len), lval);
}
int grib_f_index_select_int__(int* gid, char* key, int* val, int len){
    return grib_f_index_select_int_(gid,key,val,len);
}
int grib_f_index_select_int(int* gid, char* key, int* val, int len){
    return grib_f_index_select_int_(gid,key,val,len);
}

/*****************************************************************************/
int grib_f_index_select_long_(int* gid, char* key, long* val, int len)
{
    grib_index *h = get_index(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_long(h, cast_char(buf,key,len), *val);
}
int grib_f_index_select_long__(int* gid, char* key, long* val, int len){
    return grib_f_index_select_long_(gid,key,val,len);
}
int grib_f_index_select_long(int* gid, char* key, long* val, int len){
    return grib_f_index_select_long_(gid,key,val,len);
}

/*****************************************************************************/
int grib_f_set_real8_(int* gid, char* key, double* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_double(h, cast_char(buf,key,len), *val);
}
int grib_f_set_real8__(int* gid, char* key, double* val, int len){
    return  grib_f_set_real8_( gid,  key,  val, len);
}
int grib_f_set_real8(int* gid, char* key, double* val, int len){
    return  grib_f_set_real8_( gid,  key,  val, len);
}

int grib_f_get_real8_(int* gid, char* key, double* val, int len)
{
    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double(h, cast_char(buf,key,len), val);

}
int grib_f_get_real8__(int* gid, char* key, double* val, int len){
    return grib_f_get_real8_( gid,  key,  val, len);
}
int grib_f_get_real8(int* gid, char* key, double* val, int len){
    return grib_f_get_real8_( gid,  key,  val, len);
}

int grib_f_get_real8_element_(int* gid, char* key,int* index, double* val, int len){

    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double_element(h, cast_char(buf,key,len), *index,val);

}
int grib_f_get_real8_element__(int* gid, char* key, int* index,double* val, int len){
    return grib_f_get_real8_element_( gid,  key, index, val,len);
}
int grib_f_get_real8_element(int* gid, char* key, int* index,double* val, int len){
    return grib_f_get_real8_element_( gid,  key, index, val,len);
}

int grib_f_get_real8_elements_(int* gid, char* key,int* index, double* val, int *size, int len){

    grib_handle *h = get_handle(*gid);
    char buf[1024];

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double_elements(h, cast_char(buf,key,len), index,*size,val);

}
int grib_f_get_real8_elements__(int* gid, char* key, int* index,double* val, int* len,int size){
    return grib_f_get_real8_elements_( gid,  key, index, val,len,size);
}
int grib_f_get_real8_elements(int* gid, char* key, int* index,double* val, int* len,int size){
    return grib_f_get_real8_elements_( gid,  key, index, val,len,size);
}

/*****************************************************************************/
int grib_f_find_nearest_four_single_(int* gid,int* is_lsm,
        double* inlat,double* inlon,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes)
{
    grib_nearest* nearest=NULL;
    int err=0, result=0;
    unsigned long flags=0;
    size_t len=4;
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    nearest=grib_nearest_new(h,&err);
    if (err!=GRIB_SUCCESS) return err;

    result = grib_nearest_find(nearest,h,*inlat,*inlon,
            flags,outlats,outlons,values,distances,indexes,&len);

    grib_nearest_delete(nearest);
    return result;
}
int grib_f_find_nearest_four_single__(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes) {

    return grib_f_find_nearest_four_single_(gid,is_lsm,
            inlats,inlons,outlats,outlons,values,
            distances,indexes);
}
int grib_f_find_nearest_four_single(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes) {

    return grib_f_find_nearest_four_single_(gid,is_lsm,
            inlats,inlons,outlats,outlons,values,
            distances,indexes);
}

/*****************************************************************************/
int grib_f_find_nearest_single_(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes) {

    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_nearest_find_multiple(h,*is_lsm,
            inlats,inlons,1,outlats,outlons,
            values,distances,indexes);
}
int grib_f_find_nearest_single__(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes) {

    return grib_f_find_nearest_single_(gid,is_lsm,
            inlats,inlons,outlats,outlons,values,
            distances,indexes);
}
int grib_f_find_nearest_single(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes) {

    return grib_f_find_nearest_single_(gid,is_lsm,
            inlats,inlons,outlats,outlons,values,
            distances,indexes);
}

/*****************************************************************************/
int grib_f_find_nearest_multiple_(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes, int* npoints) {

    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_nearest_find_multiple(h,*is_lsm,
            inlats,inlons,*npoints,outlats,outlons,
            values,distances,indexes);
}
int grib_f_find_nearest_multiple__(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes, int* npoints) {

    return grib_f_find_nearest_multiple_(gid,is_lsm,
            inlats,inlons,outlats,outlons,values,
            distances,indexes,npoints);
}
int grib_f_find_nearest_multiple(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes, int* npoints) {

    return grib_f_find_nearest_multiple_(gid,is_lsm,
            inlats,inlons,outlats,outlons,values,
            distances,indexes,npoints);
}

/*****************************************************************************/
int grib_f_get_real8_array_(int* gid, char* key, double*val, int* size, int len){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = *size;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_get_double_array(h, cast_char(buf,key,len), val, &lsize);
        *size = lsize;
        return  err;
    }
}
int grib_f_get_real8_array__(int* gid, char* key, double*val, int* size, int len){
    return grib_f_get_real8_array_( gid,  key, val,  size,  len);
}
int grib_f_get_real8_array(int* gid, char* key, double*val, int* size, int len){
    return grib_f_get_real8_array_( gid,  key, val,  size,  len);
}

int grib_f_set_force_real8_array__(int* gid, char* key, double *val, int* size, int len){
    return grib_f_set_force_real8_array_( gid,  key, val,  size, len);
}
int grib_f_set_force_real8_array(int* gid, char* key, double *val, int* size, int len){
    return grib_f_set_force_real8_array_( gid,  key, val,  size, len);
}

int grib_f_set_force_real8_array_(int* gid, char* key, double*val, int* size, int len){

    grib_handle *h = get_handle(*gid);
    char buf[1024];
    size_t lsize = *size;

    if(!h)   return GRIB_INVALID_GRIB;

    return grib_set_force_double_array(h, cast_char(buf,key,len), val, lsize);

}

/*****************************************************************************/
int grib_f_set_real8_array_(int* gid, char* key, double*val, int* size, int len){

    grib_handle *h = get_handle(*gid);
    char buf[1024];
    size_t lsize = *size;

    if(!h)   return GRIB_INVALID_GRIB;

    return grib_set_double_array(h, cast_char(buf,key,len), val, lsize);
}
int grib_f_set_real8_array__(int* gid, char* key, double *val, int* size, int len){
    return grib_f_set_real8_array_( gid,  key, val,  size, len);
}
int grib_f_set_real8_array(int* gid, char* key, double *val, int* size, int len){
    return grib_f_set_real8_array_( gid,  key, val,  size, len);
}

/*****************************************************************************/
int grib_f_get_string_array_(int* gid, char* key, char* val,int* nvals,int* slen,int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t i;
    char buf[1024];
    size_t lsize = *nvals;
    char** cval=0;
    char* p=val;

    if(!h) return  GRIB_INVALID_GRIB;

    cval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)*lsize);
    err = grib_get_string_array(h, cast_char(buf,key,len), cval, &lsize);
    if (err) return err;

    if (strlen(cval[0])>*slen) err=GRIB_ARRAY_TOO_SMALL;

    for (i=0;i<lsize;i++) {
        strcpy(p,cval[i]);
        czstr_to_fortran(p,*slen);
        grib_context_free(h->context,cval[i]);
        p+= *slen;
    }
    grib_context_free(h->context,cval);

    return  err;
}

int grib_f_get_string_array__(int* gid, char* key, char* val,int* nvals,int* slen, int len){
    return  grib_f_get_string_array_( gid,  key,  val,nvals,slen,len);
}
int grib_f_get_string_array(int* gid, char* key, char* val,int* nvals,int* slen, int len){
    return  grib_f_get_string_array_( gid,  key,  val, nvals, slen, len);
}


/*****************************************************************************/
/* Strip whitespace from the end of a string */
static void rtrim(char* s)
{
    size_t len = 0;
    if (!s) return;
    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        len--;
    s[len] = '\0';
}

int grib_f_set_string_array_(int* gid, char* key, char* val,int* nvals,int* slen,int len)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t i;
    char buf[1024];
    size_t lsize = *nvals;
    char** cval=0;
    char* p=val;
    grib_context* c=h->context;

    if(!h) return  GRIB_INVALID_GRIB;

    cval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)*lsize);
    for (i=0;i<lsize;i++) {
        cval[i]=grib_context_malloc_clear(c,sizeof(char)* (*slen+1));
        cast_char_no_cut(cval[i],p,*slen);
        rtrim( cval[i] ); /* trim spaces at end of string */
        p+= *slen;
    }
    err = grib_set_string_array(h, cast_char(buf,key,len), (const char **)cval, lsize);
    if (err) return err;

    for (i=0;i<lsize;i++) {
        grib_context_free(c,cval[i]);
    }
    grib_context_free(c,cval);

    return  err;
}

int grib_f_set_string_array__(int* gid, char* key, char* val,int* nvals,int* slen, int len){
    return  grib_f_set_string_array_( gid,  key,  val,nvals,slen,len);
}
int grib_f_set_string_array(int* gid, char* key, char* val,int* nvals,int* slen, int len){
    return  grib_f_set_string_array_( gid,  key,  val, nvals, slen, len);
}

/*****************************************************************************/
int grib_f_get_string_(int* gid, char* key, char* val,int len, int len2){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    char buf[1024];
    size_t lsize = len2;

    if(!h) return  GRIB_INVALID_GRIB;

    fort_char_clean(val,len2);

    err = grib_get_string(h, cast_char(buf,key,len), val, &lsize);

    czstr_to_fortran(val,len2);

    return  err;
}

int grib_f_get_string__(int* gid, char* key, char* val,  int len, int len2){
    return  grib_f_get_string_( gid,  key,  val,   len,  len2);
}
int grib_f_get_string(int* gid, char* key, char* val,  int len, int len2){
    return  grib_f_get_string_( gid,  key,  val,   len,  len2);
}

int grib_f_set_string_(int* gid, char* key, char* val, int len, int len2){

    grib_handle *h = get_handle(*gid);
    char* val_str = NULL;

    char buf[1024]={0,};
    char buf2[1024]={0,};
    size_t lsize = len2;

    if(!h) return GRIB_INVALID_GRIB;
    
    /* For BUFR, the value may contain spaces e.g. stationOrSiteName='CAMPO NOVO' */
    val_str = cast_char_no_cut(buf2,val,len2);
    rtrim( val_str ); /* trim spaces at end of string */

    return grib_set_string(h, cast_char(buf,key,len), val_str, &lsize);
}

int grib_f_set_string__(int* gid, char* key, char* val, int len, int len2){
    return  grib_f_set_string_( gid,  key,  val,   len, len2);
}
int grib_f_set_string(int* gid, char* key, char* val, int len, int len2){
    return  grib_f_set_string_( gid,  key,  val,   len, len2);
}

/*****************************************************************************/
int grib_f_get_data_real4_(int* gid,float* lats, float* lons,float* values,size_t* size) {

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    double *lat8=NULL,*lon8=NULL,*val8 = NULL;
    size_t i=0;

    if(!h) return GRIB_INVALID_GRIB;

    val8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!val8) return GRIB_OUT_OF_MEMORY;
    lon8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!lon8) return GRIB_OUT_OF_MEMORY;
    lat8 = (double*)grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!lat8) return GRIB_OUT_OF_MEMORY;

    err=grib_get_data(h,lat8,lon8,val8);

    for(i=0;i<*size;i++) {
        values[i] = val8[i];
        lats[i] = lat8[i];
        lons[i] = lon8[i];
    }

    grib_context_free(h->context,val8);
    grib_context_free(h->context,lat8);
    grib_context_free(h->context,lon8);

    return err;

}
int grib_f_get_data_real4__(int* gid,float* lats, float* lons,float* values,size_t* size) {
    return grib_f_get_data_real4_(gid,lats,lons,values,size);
}
int grib_f_get_data_real4(int* gid,float* lats, float* lons,float* values,size_t* size) {
    return grib_f_get_data_real4_(gid,lats,lons,values,size);
}

int grib_f_get_data_real8_(int* gid,double* lats, double* lons,double* values,size_t* size) {

    grib_handle *h = get_handle(*gid);
    return grib_get_data(h,lats,lons,values);

}
int grib_f_get_data_real8__(int* gid,double* lats, double* lons,double* values,size_t* size) {
    return grib_f_get_data_real8_(gid,lats,lons,values,size);
}
int grib_f_get_data_real8(int* gid,double* lats, double* lons,double* values,size_t* size) {
    return grib_f_get_data_real8_(gid,lats,lons,values,size);
}

/*****************************************************************************/
int grib_f_get_message_size_(int* gid, size_t *len){
    grib_handle *h = get_handle(*gid);
    if(!h) return GRIB_INVALID_GRIB;

    *len = h->buffer->ulength;
    return GRIB_SUCCESS;

}
int grib_f_get_message_size__(int* gid, size_t *len){
    return  grib_f_get_message_size_( gid, len);
}
int grib_f_get_message_size(int* gid, size_t *len){
    return  grib_f_get_message_size_( gid, len);
}

/*****************************************************************************/
int grib_f_copy_message_(int* gid, void* mess,size_t* len){
    grib_handle *h = get_handle(*gid);
    if(!h)
        return GRIB_INVALID_GRIB;

    if(*len < h->buffer->ulength) {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "grib_copy_message: buffer=%ld message size=%ld",*len,h->buffer->ulength);
        return GRIB_BUFFER_TOO_SMALL;
    }


    memcpy(mess,h->buffer->data,h->buffer->ulength);
    *len=h->buffer->ulength;
    return GRIB_SUCCESS;

}

int grib_f_copy_message__(int* gid, void* mess,size_t* len){
    return grib_f_copy_message_( gid,  mess, len);
}
int grib_f_copy_message(int* gid, void* mess,size_t* len){
    return grib_f_copy_message_( gid,  mess, len);
}

/*****************************************************************************/
void grib_f_check_(int* err,char* call,char* str,int lencall,int lenstr){
    char bufstr[1024]={0,};
    char bufcall[1024]={0,};
    grib_context* c=grib_context_get_default();
    if ( *err == GRIB_SUCCESS || *err == GRIB_END_OF_FILE ) return;
    cast_char(bufcall,call,lencall);
    cast_char(bufstr,str,lenstr);
    grib_context_log(c,GRIB_LOG_ERROR,"%s: %s %s",
            bufcall,bufstr,grib_get_error_message(*err));
    exit(*err);
}

void grib_f_check__(int* err,char* call, char* key,  int lencall, int lenkey){
    grib_f_check_(err,call,key,lencall,lenkey);
}
void grib_f_check(int* err,char* call, char* key,  int lencall, int lenkey){
    grib_f_check_(err,call,key,lencall,lenkey);
}

/*****************************************************************************/
int grib_f_write_(int* gid, int* fid) {
    grib_handle *h = get_handle(*gid);
    FILE* f = get_file(*fid);
    const void* mess = NULL;
    size_t mess_len = 0;

    if(!f) return GRIB_INVALID_FILE;
    if (!h) return GRIB_INVALID_GRIB;

    grib_get_message(h,&mess,&mess_len);
    if(fwrite(mess,1, mess_len,f) != mess_len) {
        perror("grib_write");
        return GRIB_IO_PROBLEM;
    }

    return GRIB_SUCCESS;
}
int grib_f_write__(int* gid, int* fid) {
    return grib_f_write_(gid,fid);
}

int grib_f_write(int* gid, int* fid) {
    return grib_f_write_(gid,fid);
}

/*****************************************************************************/
int grib_f_multi_write_(int* gid, int* fid) {
    grib_multi_handle *h = get_multi_handle(*gid);
    FILE* f = get_file(*fid);

    if(!f) return GRIB_INVALID_FILE;
    if (!h) return GRIB_INVALID_GRIB;

    return grib_multi_handle_write(h,f);
}


int grib_f_multi_write__(int* gid, int* fid) {
    return grib_f_multi_write_(gid,fid);
}

int grib_f_multi_write(int* gid, int* fid) {
    return grib_f_multi_write_(gid,fid);
}

int grib_f_multi_append_(int* ingid, int* sec,int* mgid) {
    grib_handle *h = get_handle(*ingid);
    grib_multi_handle *mh = get_multi_handle(*mgid);

    if (!h) return GRIB_INVALID_GRIB;

    if (!mh) {
        mh=grib_multi_handle_new(h->context);
        push_multi_handle(mh,mgid);
    }

    return grib_multi_handle_append(h,*sec,mh);
}

int grib_f_multi_append(int* ingid, int* sec,int* mgid) {
    return grib_f_multi_append_(ingid, sec, mgid);
}

int grib_f_multi_append__(int* ingid, int* sec,int* mgid) {
    return grib_f_multi_append_(ingid, sec, mgid);
}

/*****************************************************************************/
int grib_f_set_definitions_path_(char* path, int len){
    grib_context* c = grib_context_get_default();
    char buf[1024];
    grib_context_set_definitions_path(c, cast_char(buf,path,len));
    return GRIB_SUCCESS;
}
int grib_f_set_definitions_path__(char* path,  int len){
    return grib_f_set_definitions_path_(path, len);
}
int grib_f_set_definitions_path(char* path,  int len){
    return grib_f_set_definitions_path_(path, len);
}


int grib_f_set_samples_path_(char* path, int len){
    grib_context* c = grib_context_get_default();
    char buf[1024];
    grib_context_set_samples_path(c, cast_char(buf,path,len));
    return GRIB_SUCCESS;
}
int grib_f_set_samples_path__(char* path,  int len){
    return grib_f_set_samples_path_(path, len);
}
int grib_f_set_samples_path(char* path,  int len){
    return grib_f_set_samples_path_(path, len);
}
