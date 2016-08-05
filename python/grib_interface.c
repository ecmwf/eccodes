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
#include "assert.h"

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
#include <errno.h>

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

static void _push_handle(grib_handle *h,int *gid)
{
    l_grib_handle* current= handle_set;
    l_grib_handle* previous= handle_set;
    l_grib_handle* new= NULL;
    int myindex= 1;

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

    current= handle_set;

    if(!handle_set){
        handle_set = malloc(sizeof(l_grib_handle));
        handle_set->id   = myindex;
        handle_set->h    = h;
        handle_set->next = NULL;
        *gid=myindex;
        return;
    }

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

    new = malloc(sizeof(l_grib_handle));
    new->id   = myindex;
    new->h    = h;
    new->next = current;
    previous->next = new;

    *gid=myindex;
    return;
}

static void _push_index(grib_index *h,int *gid)
{
    l_grib_index* current= index_set;
    l_grib_index* previous= index_set;
    l_grib_index* new= NULL;
    int myindex= 1;

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

    current= index_set;

    if(!index_set){
        index_set = malloc(sizeof(l_grib_index));
        index_set->id   = myindex;
        index_set->h    = h;
        index_set->next = NULL;
        *gid=myindex;
        return;
    }

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

    new = malloc(sizeof(l_grib_index));
    new->id   = myindex;
    new->h    = h;
    new->next = current;
    previous->next = new;

    *gid=myindex;
    return;
}

static void _push_multi_handle(grib_multi_handle *h,int *gid)
{
    l_grib_multi_handle* current= multi_handle_set;
    l_grib_multi_handle* previous= multi_handle_set;
    l_grib_multi_handle* new= NULL;
    int myindex= 1;

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

    current= multi_handle_set;

    if(!multi_handle_set){
        multi_handle_set = malloc(sizeof(l_grib_multi_handle));
        multi_handle_set->id   = myindex;
        multi_handle_set->h    = h;
        multi_handle_set->next = NULL;
        *gid=myindex;
        return;
    }

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

    new = malloc(sizeof(l_grib_multi_handle));
    new->id   = myindex;
    new->h    = h;
    new->next = current;
    previous->next = new;

    *gid=myindex;
    return;
}

static void push_handle(grib_handle *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&handle_mutex)
    _push_handle(h,gid);
    GRIB_MUTEX_UNLOCK(&handle_mutex)
    return;
}

static void push_index(grib_index *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&index_mutex)
    _push_index(h,gid);
    GRIB_MUTEX_UNLOCK(&index_mutex)
    return;
}

static void push_multi_handle(grib_multi_handle *h,int *gid)
{
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&multi_handle_mutex)
    _push_multi_handle(h,gid);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex)
    return;
}

static int _push_iterator(grib_iterator *i)
{
    l_grib_iterator* current  = iterator_set;
    l_grib_iterator* previous = iterator_set;
    l_grib_iterator* new      = NULL;
    int myindex = 1;

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

static int push_iterator(grib_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&iterator_mutex)
    ret=_push_iterator(i);
    GRIB_MUTEX_UNLOCK(&iterator_mutex)
    return ret;
}

static int _push_keys_iterator(grib_keys_iterator *i)
{
    l_grib_keys_iterator* current  = keys_iterator_set;
    l_grib_keys_iterator* previous = keys_iterator_set;
    l_grib_keys_iterator* new      = NULL;
    int myindex = 1;

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

static int push_keys_iterator(grib_keys_iterator *i)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    ret=_push_keys_iterator(i);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
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
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&handle_mutex)
    h=_get_handle(handle_id);
    GRIB_MUTEX_UNLOCK(&handle_mutex)
    return h;
}

static grib_index* get_index(int index_id)
{
    grib_index* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&index_mutex)
    h=_get_index(index_id);
    GRIB_MUTEX_UNLOCK(&index_mutex)
    return h;
}

static grib_multi_handle* get_multi_handle(int multi_handle_id)
{
    grib_multi_handle* h=NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&multi_handle_mutex)
    h=_get_multi_handle(multi_handle_id);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex)
    return h;
}

static FILE* get_file(int file_id)
{
    l_grib_file* current  = file_set;
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
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&iterator_mutex)
    i=_get_iterator(iterator_id);
    GRIB_MUTEX_UNLOCK(&iterator_mutex)
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
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    i=_get_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
    return i;
}

static int clear_file(int file_id)
{
    l_grib_file* current = file_set;
    int ret = 0;
    while(current){
        if(current->id == file_id){
            current->id = -(current->id);
            if (current->f) {
                ret = fclose(current->f);
                if (0 == ret) {
                    return GRIB_SUCCESS;
                } else {
                    return GRIB_IO_PROBLEM;
                }
            }
            return GRIB_SUCCESS;
        }
        current = current->next;
    }
    return GRIB_INVALID_FILE;
}

static int _clear_handle(int handle_id)
{
    l_grib_handle* current  = handle_set;

    /* look for the input grib id to release it */
    while(current){
        if(current->id == handle_id){
            current->id = -(current->id);
            if(current->h) return grib_handle_delete(current->h);
        }
        current = current->next;
    }

    /* fail with invalid grib id if not found */
    return GRIB_INVALID_GRIB;
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
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&handle_mutex)
    ret=_clear_handle(handle_id);
    GRIB_MUTEX_UNLOCK(&handle_mutex)
    return ret;
}

static int clear_index(int index_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&index_mutex)
    ret=_clear_index(index_id);
    GRIB_MUTEX_UNLOCK(&index_mutex)
    return ret;
}

static int clear_multi_handle(int multi_handle_id)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&multi_handle_mutex)
    ret=_clear_multi_handle(multi_handle_id);
    GRIB_MUTEX_UNLOCK(&multi_handle_mutex)
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
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&iterator_mutex)
    ret=_clear_iterator(iterator_id);
    GRIB_MUTEX_UNLOCK(&iterator_mutex)
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
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    ret=_clear_keys_iterator(keys_iterator_id);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
    return ret;
}

int grib_c_read_any_from_file(int* fid, char* buffer, int* nbytes)
{
    grib_context* c;
    int err=0;
    size_t size=(size_t)nbytes;
    FILE* f=get_file(*fid);

    if (f) {
        c=grib_context_get_default( );
        err=grib_read_any_from_file(c,f,buffer,&size);
        *nbytes=size;
        return err;
    } else {
        return GRIB_INVALID_FILE;
    }
}

int grib_c_write_file(int* fid, char* buffer, int* nbytes)
{
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

int grib_c_read_file(int* fid, char* buffer, int* nbytes)
{
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

int grib_c_open_file(int* fid, char* name , char* op)
{
    FILE* f = NULL;
    int ioerr=0;
    int ret=GRIB_SUCCESS;

    /*TODO Proper context passed as external parameter */
    grib_context* context=grib_context_get_default();

    f = fopen(name,op);

    if(!f) {
        ioerr=errno;
        grib_context_log(context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),"IO ERROR: %s: %s",strerror(ioerr),name);
        *fid = -1;
        ret=GRIB_IO_PROBLEM;
    } else {
        *fid = push_file(f);
        ret=GRIB_SUCCESS;
    }
    return ret;
}

int grib_c_close_file(int* fid){
    return clear_file(*fid);
}

int grib_c_multi_support_on(void) {
    grib_multi_support_on(0);
    return GRIB_SUCCESS;
}

int grib_c_multi_support_off(void) {
    grib_multi_support_off(0);
    return GRIB_SUCCESS;
}

static int _grib_c_iterator_new_(int* gid,int* iterid,int* mode)
{
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

int grib_c_iterator_new(int* gid,int* iterid,int* mode)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&iterator_mutex)
    ret=_grib_c_iterator_new_(gid,iterid,mode);
    GRIB_MUTEX_UNLOCK(&iterator_mutex)
    return ret;
}

int grib_c_iterator_next(int* iterid,double* lat,double* lon,double* value)
{
    grib_iterator* iter=get_iterator(*iterid);
    if (!iter) return GRIB_INVALID_ITERATOR;
    return grib_iterator_next(iter,lat,lon,value);
}

int grib_c_iterator_delete(int* iterid)
{
    return clear_iterator(*iterid);
}

static int _grib_c_keys_iterator_new_(int* gid,int* iterid,char* name_space)
{
    int err=0;
    grib_handle* h;
    grib_keys_iterator* iter;

    h=get_handle(*gid);
    if (!h) {
        *iterid=-1;
        return GRIB_NULL_HANDLE;
    }
    iter=grib_keys_iterator_new(h,0,name_space);
    if (iter)
        *iterid=push_keys_iterator(iter);
    else
        *iterid=-1;
    return err;
}

int grib_c_keys_iterator_new(int* gid,int* iterid,char* name_space)
{
    int ret=0;
    GRIB_MUTEX_INIT_ONCE(&once,&init)
    GRIB_MUTEX_LOCK(&keys_iterator_mutex)
    ret=_grib_c_keys_iterator_new_(gid,iterid,name_space);
    GRIB_MUTEX_UNLOCK(&keys_iterator_mutex)
    return ret;
}

int grib_c_keys_iterator_next(int* iterid)
{
    int ret = 0;

    grib_keys_iterator* iter= get_keys_iterator(*iterid);

    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;

    ret = grib_keys_iterator_next(iter);

    return ret;
}

int grib_c_keys_iterator_delete(int* iterid)
{
    return clear_keys_iterator(*iterid);
}

int grib_c_gribex_mode_on(void)
{
    grib_gribex_mode_on(0);
    return GRIB_SUCCESS;
}

int grib_c_gribex_mode_off(void)
{
    grib_gribex_mode_off(0);
    return GRIB_SUCCESS;
}

int grib_c_skip_computed(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_COMPUTED);
}

int grib_c_skip_coded(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_CODED);
}

int grib_c_skip_edition_specific(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC);
}

int grib_c_skip_duplicates(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_DUPLICATES);
}

int grib_c_skip_read_only(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_READ_ONLY);
}

int grib_c_skip_function(int* iterid)
{
    grib_keys_iterator* iter=get_keys_iterator(*iterid);
    if (!iter) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_set_flags(iter,GRIB_KEYS_ITERATOR_SKIP_FUNCTION);
}

int grib_c_keys_iterator_get_name(int* iterid,char* name,int len)
{
    size_t lsize=len;
    char buf[1024]={0,};

    grib_keys_iterator* kiter=get_keys_iterator(*iterid);

    if (!kiter) return GRIB_INVALID_KEYS_ITERATOR;
  if (grib_keys_iterator_get_accessor(kiter)==NULL)
      return GRIB_INVALID_KEYS_ITERATOR;

    sprintf(buf,"%s",grib_keys_iterator_get_name(kiter));
    lsize=strlen(buf);

    if (len < lsize) return GRIB_ARRAY_TOO_SMALL;

    memcpy(name,buf,lsize);
    name[lsize] = '\0';

    return 0;
}

int grib_c_keys_iterator_rewind(int* kiter)
{
    grib_keys_iterator* i=get_keys_iterator(*kiter);

    if (!i) return GRIB_INVALID_KEYS_ITERATOR;
    return grib_keys_iterator_rewind(i);
}

int grib_c_new_from_message(int* gid, void* buffer , size_t* bufsize)
{
    grib_handle *h = NULL;
    h = grib_handle_new_from_message_copy(0, buffer, *bufsize);
    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}

int grib_c_new_from_message_copy(int* gid, void* buffer , size_t* bufsize)
{
    grib_handle *h = NULL;
    h = grib_handle_new_from_message_copy(0, buffer, *bufsize);
    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_INTERNAL_ERROR;
}

int grib_c_grib_new_from_samples(int* gid, char* name)
{
    grib_handle *h = NULL;

    h = grib_handle_new_from_samples(NULL,name);
    /*   grib_context_set_debug(h->context,1);*/

    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_FILE_NOT_FOUND;
}

int grib_c_bufr_new_from_samples(int* gid, char* name)
{
    grib_handle *h = NULL;

    h = codes_bufr_handle_new_from_samples(NULL,name);
    /*   grib_context_set_debug(h->context,1);*/

    if(h){
        push_handle(h,gid);
        return GRIB_SUCCESS;
    }

    *gid = -1;
    return  GRIB_FILE_NOT_FOUND;
}

int grib_c_clone(int* gidsrc,int* giddest)
{
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

int grib_c_copy_namespace(int* gidsrc,char* name,int* giddest)
{
    grib_handle *src  = get_handle(*gidsrc);
    grib_handle *dest = get_handle(*giddest);

    if(src && dest)
        return grib_copy_namespace(dest,name,src);

    return GRIB_INVALID_GRIB;
}

int grib_c_count_in_file(FILE* f,int* n)
{
    int err = 0;
    if (f) err=grib_count_in_file(0, f,n);
    return err;
}

int grib_c_new_gts_from_file(FILE* f,int headers_only, int* gid)
{
    grib_handle *h = NULL;
    int err = 0;

    if(f){
        h = gts_new_from_file(0,f,&err);

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

int grib_c_new_metar_from_file(FILE* f,int headers_only, int* gid)
{
    grib_handle *h = NULL;
    int err = 0;

    if(f){
        h = metar_new_from_file(0,f,&err);

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

int grib_c_new_any_from_file(FILE* f,int headers_only,int* gid)
{
    grib_handle *h = NULL;
    int err = 0;
    if(f){
        h = codes_handle_new_from_file(0,f,PRODUCT_ANY, &err);
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

int grib_c_new_bufr_from_file(FILE* f,int headers_only,int* gid)
{
    grib_handle *h = NULL;
    int err = 0;

    if(f){
        /* h = bufr_new_from_file(0,f,headers_only,&err); */
        h = codes_handle_new_from_file(0,f,PRODUCT_BUFR, &err);

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

int grib_c_new_from_file(FILE* f, int* gid, int headers_only)
{
    grib_handle *h = NULL;
    int err = 0;

    if(f){
        h=grib_new_from_file(0,f,headers_only,&err);

        if(h){
            push_handle(h,gid);
            return GRIB_SUCCESS;
        } else {
            *gid=-1;
            if (err == GRIB_SUCCESS) {
                return GRIB_END_OF_FILE;
            } else {
                /* A real error occurred */
                return err;
            }
        }
    }

    *gid=-1;
    return GRIB_INVALID_FILE;
}

int grib_c_new_from_index(int* iid, int* gid){
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

int grib_c_index_new_from_file(char* file ,char* keys ,int* gid){
    int err = 0;
    grib_index *i = NULL;

    if(*file){
        i = grib_index_new_from_file(0,file,keys,&err);
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

int grib_c_index_add_file(int* iid, char* file) {
    grib_index *i = get_index(*iid);
    int err = GRIB_SUCCESS;

    if (!i) {
        return GRIB_INVALID_INDEX;
    } else {
        err = grib_index_add_file(i,file);
        return err;
    }
}

int grib_c_index_release(int* hid){
    return clear_index(*hid);
}

int grib_c_multi_release(int* hid){
    return clear_multi_handle(*hid);
}

int grib_c_release(int* hid){
    return clear_handle(*hid);
}

int grib_c_dump(int* gid){
    grib_handle *h = get_handle(*gid);

    if(!h)
        return GRIB_INVALID_GRIB;
    else
        grib_dump_content(h,stdout,"wmo",0,NULL);

    return GRIB_SUCCESS;
}

int grib_c_print(int* gid, char* key){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    grib_dumper* d = NULL;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        d = grib_dumper_factory("serialize",h,stdout,0,0);
        err = grib_print(h, key, d);
        grib_dumper_delete(d);
        return  err;
    }
}

int grib_c_get_error_string(int* err, char* buf,  int len){
    const char* err_msg = grib_get_error_message(*err);
    size_t erlen = strlen(err_msg);
    if( len < erlen) return GRIB_ARRAY_TOO_SMALL;

    strncpy(buf, err_msg,(size_t)erlen);
    buf[erlen] = '\0';

    return GRIB_SUCCESS;
}

int grib_c_get_size_int(int* gid, char* key, int* val){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t tsize = 0;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_get_size(h, key, &tsize);
        *val = tsize;
        return  err;
    }
}

int grib_c_get_message_offset(int* gid, size_t* offset){
    int err = GRIB_SUCCESS;
    off_t myoffset;
    grib_handle *h = get_handle(*gid);

    if(!h)
        return GRIB_INVALID_GRIB;
    else {
        err=grib_get_message_offset(h, &myoffset);
        *offset=myoffset;
        return err;
    }
}

int grib_c_get_message_size(int* gid, size_t* size){
    grib_handle *h = get_handle(*gid);

    if(!h)
        return GRIB_INVALID_GRIB;
    else
        return grib_get_message_size(h, size);
}

int grib_c_get_string_length(int* gid, char* key, size_t* val){
    grib_handle *h = get_handle(*gid);

    if(!h)
        return GRIB_INVALID_GRIB;
    else
        return grib_get_string_length(h, key, val);

}

int grib_c_get_size_long(int* gid, char* key, long* val){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t tsize = 0;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_get_size(h, key, &tsize);
        *val = tsize;
        return  err;
    }
}

int grib_c_index_get_size_int(int* gid, char* key, int* val){
    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    size_t tsize = 0;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_index_get_size(h, key, &tsize);
        *val = tsize;
        return  err;
    }
}

int grib_c_index_get_size_long(int* gid, char* key, long* val){
    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    size_t tsize = 0;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_index_get_size(h, key, &tsize);
        *val = tsize;
        return  err;
    }
}

int grib_c_get_int(int* gid, char* key, int* val){
    grib_handle *h = get_handle(*gid);
    long long_val;
    int err = GRIB_SUCCESS;

    if(!h) return GRIB_INVALID_GRIB;
    err = grib_get_long(h, key,&long_val);
    *val = long_val;
    return err;
}

int grib_c_get_long(int* gid, char* key, long* val){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;

    if(!h) return GRIB_INVALID_GRIB;
    err = grib_get_long(h, key, val);
    return err;
}

int grib_c_get_double(int* gid, char* key, double* val){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;

    if(!h) return GRIB_INVALID_GRIB;
    err = grib_get_double(h, key, val);
    return err;
}

int grib_c_get_int_array(int* gid, char* key, int *val, int* size){

    grib_handle *h = get_handle(*gid);
    long* long_val = NULL;
    int err = GRIB_SUCCESS;
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;

    if(sizeof(long) == sizeof(int)){
        long_val = (long*)val;
        err = grib_get_long_array(h, key, long_val, &lsize);
        *size = lsize;
        return  err;
    }
    if(*size)
        long_val = grib_context_malloc(h->context,(*size)*(sizeof(long)));
    else
        long_val = grib_context_malloc(h->context,(sizeof(long)));

    if(!long_val) return GRIB_OUT_OF_MEMORY;
    err = grib_get_long_array(h, key, long_val, &lsize);

    for(*size=0;*size<lsize;(*size)++)
        val[*size] = long_val[*size];

    grib_context_free(h->context,long_val);
    return  err;
}

int grib_c_get_long_array(int* gid, char* key, long *val, int* size){

    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;

    err = grib_get_long_array(h, key, val, &lsize);
    *size=lsize;

    return  err;
}

int grib_c_index_get_string(int* gid, char* key, char* val, int *eachsize,int* size){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    int i;
    size_t lsize = *size;
    char** bufval;
    char* p=val;

    if(!h)  return GRIB_INVALID_GRIB;

    bufval=(char**)grib_context_malloc_clear(h->context,sizeof(char*)* *size);

    err = grib_index_get_string(h, key, bufval, &lsize);
    *size = lsize;

    if (err) return err;

    for (i=0;i<lsize;i++) {
        int l=strlen(bufval[i]);
        int j;
        if (*eachsize < l ) {
            grib_context_free(h->context,bufval);
            printf("eachsize=%d strlen(bufval[i])=%d\n",*eachsize,(unsigned int)strlen(bufval[i]));
            return GRIB_ARRAY_TOO_SMALL;
        }
        memcpy(p,bufval[i],l);
        p+=l;
        for (j=0;j<*eachsize-l;j++) *(p++)=' ';
    }
    grib_context_free(h->context,bufval);

    return  err;
}

int grib_c_index_get_long(int* gid, char* key, long *val, int* size){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;
    err = grib_index_get_long(h, key, val, &lsize);
    *size = lsize;
    return  err;
}

int grib_c_index_get_int(int* gid, char* key, int *val, int* size){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;
    long* lval=0;
    int i;

    if(!h)  return GRIB_INVALID_GRIB;

    lval=grib_context_malloc(h->context,sizeof(long)* *size);
    if (!lval) return GRIB_OUT_OF_MEMORY;

    err = grib_index_get_long(h, key, lval, &lsize);
    for (i=0;i<lsize;i++) val[i]=lval[i];

    *size = lsize;
    return  err;
}

int grib_c_index_get_real8(int* gid, char* key, double *val, int* size){

    grib_index *h = get_index(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;

    if(!h)  return GRIB_INVALID_GRIB;
    err = grib_index_get_double(h, key, val, &lsize);
    *size = lsize;
    return  err;
}

int grib_c_set_int_array(int* gid, char* key, int* val, int* size){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    long* long_val = NULL;
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    if(sizeof(long) == sizeof(int)){
        long_val = (long*)val;
        return  grib_set_long_array(h, key, long_val, lsize);
    }

    if(lsize)
        long_val = grib_context_malloc(h->context,(lsize)*(sizeof(long)));
    else
        long_val = grib_context_malloc(h->context,(sizeof(long)));

    if(!long_val) return GRIB_OUT_OF_MEMORY;

    for(lsize=0;lsize<(*size);lsize++)
        long_val[lsize] = val[lsize];

    err = grib_set_long_array(h, key, long_val, lsize);

    grib_context_free(h->context,long_val);
    return err;
}

int grib_c_set_long_array(int* gid, char* key, long* val, int* size){
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;

    if(!h) return GRIB_INVALID_GRIB;

    return  grib_set_long_array(h, key, val, lsize);

    return err;
}

int grib_c_set_int(int* gid, char* key, int* val){
    grib_handle *h = get_handle(*gid);
    long long_val = *val;
    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_long(h, key, long_val);
}

int grib_c_set_long(int* gid, char* key, long* val){
    grib_handle *h = get_handle(*gid);
    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_long(h, key, *val);
}

int grib_c_set_missing(int* gid, char* key){

    grib_handle *h = get_handle(*gid);
    if(!h)  return GRIB_INVALID_GRIB;

    return grib_set_missing(h, key);
}

/*
 * Submits all keys with the given values in one go.
 * 'keyvals' is a string of the form:
 *    key1=val1,key2=val2,key3=val3
 */
int grib_c_set_key_vals(int* gid, char* keyvals)
{
    grib_handle* h = get_handle(*gid);
    if (!h) return GRIB_INVALID_GRIB;
    {
        int err = GRIB_SUCCESS;
        grib_values values[1024]={{0,},};
        int count = 1000; /* max. num key/val pairs */
        if ((err = parse_keyval_string(NULL, keyvals, 1, GRIB_TYPE_UNDEFINED, values, &count)) != GRIB_SUCCESS) {
            return err;
        }
        if ((err = grib_set_values(h,values,count)) != GRIB_SUCCESS) {
            return err;
        }
    }
    return GRIB_SUCCESS;
}

int grib_c_is_missing(int* gid, char* key,int* isMissing)
{
    int err=0;
    grib_handle *h = get_handle(*gid);
    if(!h)  return GRIB_INVALID_GRIB;

    *isMissing=grib_is_missing(h, key,&err);
    return err;
}

int grib_c_is_defined(int* gid, char* key,int* isDefined)
{
    grib_handle *h = get_handle(*gid);
    if(!h) return GRIB_INVALID_GRIB;

    *isDefined=grib_is_defined(h, key);
    return GRIB_SUCCESS;
}

int grib_c_set_real4(int* gid, char* key, float* val)
{
    grib_handle *h = get_handle(*gid);
    double val8 = *val;
    if(!h)  return GRIB_INVALID_GRIB;

    return grib_set_double(h, key, val8);
}

int grib_c_get_real4_element(int* gid, char* key, int* index,float* val)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    double val8 = 0;

    if(!h) return GRIB_INVALID_GRIB;

    err = grib_get_double_element(h, key, *index,&val8);
    *val = val8;
    return err;
}

int grib_c_get_real4_elements(int* gid, char* key,int* index, float *val,int* size)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;
    long i=0;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = grib_context_malloc(h->context,(*size)*(sizeof(double)));
    else
        val8 = grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;


    err = grib_get_double_elements(h, key, index,(long)lsize,val8);

    for(i=0;i<lsize;(i)++)
        val[i] = val8[i];

    grib_context_free(h->context,val8);

    return  err;
}

int grib_c_get_real4(int* gid, char* key, float* val)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    double val8 = 0;

    if(!h)  return GRIB_INVALID_GRIB;

    err = grib_get_double(h, key, &val8);
    *val = val8;
    return err;
}

int grib_c_get_real4_array(int* gid, char* key, float *val, int* size)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = grib_context_malloc(h->context,(*size)*(sizeof(double)));
    else
        val8 = grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    err  = grib_get_double_array(h, key, val8, &lsize);

    for(*size=0;*size<lsize;(*size)++)
        val[*size] = val8[*size];

    grib_context_free(h->context,val8);

    return  err;
}

int grib_c_set_real4_array(int* gid, char* key, float*val, int* size)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;
    double* val8 = NULL;

    if(!h) return GRIB_INVALID_GRIB;

    if(*size)
        val8 = grib_context_malloc(h->context,lsize*(sizeof(double)));
    else
        val8 = grib_context_malloc(h->context,sizeof(double));

    if(!val8) return GRIB_OUT_OF_MEMORY;

    for(lsize=0;lsize<*size;lsize++)
        val8[lsize] = val[lsize];

    err = grib_set_double_array(h, key, val8, lsize);
    grib_context_free(h->context,val8);
    return err;
}

int grib_c_index_select_real8(int* gid, char* key, double* val)
{
    grib_index *h = get_index(*gid);

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_double(h, key, *val);
}

int grib_c_index_select_string(int* gid, char* key, char* val)
{
    grib_index *h = get_index(*gid);

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_string(h, key, val);
}

int grib_c_index_select_int(int* gid, char* key, int* val)
{
    grib_index *h = get_index(*gid);
    long lval=*val;

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_long(h, key, lval);
}

int grib_c_index_select_long(int* gid, char* key, long* val)
{
    grib_index *h = get_index(*gid);

    if(!h) return GRIB_INVALID_GRIB;
    return grib_index_select_long(h, key, *val);
}

int grib_c_set_real8(int* gid, char* key, double* val)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;
    return grib_set_double(h, key, *val);
}

int grib_c_get_real8(int* gid, char* key, double* val)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double(h, key, val);
}

int grib_c_set_double(int *gid, char *key, double *val)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_set_double(h, key, *val);
}

int grib_c_get_real8_element(int* gid, char* key,int* index, double* val)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double_element(h, key, *index,val);
}

int grib_c_get_real8_elements(int* gid, char* key,int* index, double* val, int *size)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_get_double_elements(h, key, index,*size,val);
}

int grib_c_find_nearest_four_single(int* gid,int* is_lsm,
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

int grib_c_find_nearest_single(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_nearest_find_multiple(h,*is_lsm,
            inlats,inlons,1,outlats,outlons,
            values,distances,indexes);
}

int grib_c_find_nearest_multiple(int* gid,int* is_lsm,
        double* inlats,double* inlons,
        double* outlats,double* outlons,
        double* values,double* distances,
        int* indexes, int* npoints)
{
    grib_handle *h = get_handle(*gid);

    if(!h) return GRIB_INVALID_GRIB;

    return grib_nearest_find_multiple(h,*is_lsm,
            inlats,inlons,*npoints,outlats,outlons,
            values,distances,indexes);
}

int grib_c_get_real8_array(int* gid, char* key, double*val, int* size)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = *size;

    if(!h){
        return GRIB_INVALID_GRIB;
    }else{
        err = grib_get_double_array(h, key, val, &lsize);
        *size = lsize;
        return  err;
    }
}

int grib_c_set_real8_array(int* gid, char* key, double*val, int* size)
{
    grib_handle *h = get_handle(*gid);

    size_t lsize = *size;

    if(!h)   return GRIB_INVALID_GRIB;

    return grib_set_double_array(h, key, val, lsize);
}

int grib_c_set_double_array(int* gid, char* key, double*val, int* size)
{
    grib_handle *h = get_handle(*gid);

    size_t lsize = *size;

    if(!h)   return GRIB_INVALID_GRIB;

    return grib_set_double_array(h, key, val, lsize);
}

int grib_c_get_string(int* gid, char* key, char* val, size_t *lsize)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;

    if(!h) return  GRIB_INVALID_GRIB;

    err = grib_get_string(h, key, val, lsize);

    return  err;
}

int grib_c_get_string_array(int* gid, char* key, char** val, size_t *lsize)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;

    if(!h) return  GRIB_INVALID_GRIB;

    err = grib_get_string_array(h, key, val, lsize);

    return  err;
}

int grib_c_set_string(int* gid, char* key, char* val, int len2)
{
    grib_handle *h = get_handle(*gid);

    size_t lsize = len2;

    if(!h) return GRIB_INVALID_GRIB;

    return grib_set_string(h, key, val, &lsize);
}

int grib_c_set_string_array(int *gid, char *key, const char **val)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    size_t lsize = 0;

    if(!h) return GRIB_INVALID_GRIB;

    /* Note: The array passed in will have its final entry as NULL */
    /* so to find its size we just iterate thru it. */
    /* See typemap for char** in swig interface file */
    while(val[lsize]) {
        ++lsize;
    }
    err = grib_set_string_array(h, key, val, lsize);
    return err;
}

int grib_c_get_data_real4(int* gid,float* lats, float* lons,float* values,size_t* size)
{
    grib_handle *h = get_handle(*gid);
    int err = GRIB_SUCCESS;
    double *lat8=NULL,*lon8=NULL,*val8 = NULL;
    size_t i=0;

    if(!h) return GRIB_INVALID_GRIB;

    val8 = grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!val8) return GRIB_OUT_OF_MEMORY;
    lon8 = grib_context_malloc(h->context,(*size)*(sizeof(double)));
    if(!lon8) return GRIB_OUT_OF_MEMORY;
    lat8 = grib_context_malloc(h->context,(*size)*(sizeof(double)));
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
/*
int grib_c_get_data_real8(int* gid,double* lats, double* lons,double* values,size_t* size)
{
    grib_handle *h = get_handle(*gid);
    return grib_get_data(h,lats,lons,values);
}
*/
int grib_c_copy_message(int* gid, void* mess,size_t* len)
{
    grib_handle *h = get_handle(*gid);
    if(!h)
        return GRIB_INVALID_GRIB;

    if(*len < h->buffer->ulength) {
        grib_context_log(h->context,GRIB_LOG_ERROR,"grib_copy_message: buffer=%ld message size=%ld",*len,h->buffer->ulength);
        return GRIB_BUFFER_TOO_SMALL;
    }

    memcpy(mess,h->buffer->data,h->buffer->ulength);
    *len=h->buffer->ulength;
    return GRIB_SUCCESS;
}

void grib_c_check(int* err,char* call,char* str)
{
    grib_context* c=grib_context_get_default();
    if ( *err == GRIB_SUCCESS || *err == GRIB_END_OF_FILE ) return;
    grib_context_log(c,GRIB_LOG_ERROR,"%s: %s %s",
            call,str,grib_get_error_message(*err));
    exit(*err);
}

int grib_c_write(int* gid, FILE* f)
{
    grib_handle *h = get_handle(*gid);
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

int grib_c_multi_new(int* mgid)
{
    grib_multi_handle *mh = grib_multi_handle_new(0);
    if (!mh) return GRIB_INVALID_GRIB;
    push_multi_handle(mh,mgid);
    return GRIB_SUCCESS;
}

int grib_c_multi_write(int* gid, FILE* f)
{
    grib_multi_handle *h = get_multi_handle(*gid);

    if(!f) return GRIB_INVALID_FILE;
    if (!h) return GRIB_INVALID_GRIB;

    return grib_multi_handle_write(h,f);
}

int grib_c_multi_append(int* ingid, int* sec,int* mgid)
{
    grib_handle *h = get_handle(*ingid);
    grib_multi_handle *mh = get_multi_handle(*mgid);

    if (!h) return GRIB_INVALID_GRIB;

    if (!mh) {
        mh=grib_multi_handle_new(h->context);
        push_multi_handle(mh,mgid);
    }

    return grib_multi_handle_append(h,*sec,mh);
}

int grib_c_get_native_type(int* gid, char* key, int* type)
{
    grib_handle *h = get_handle(*gid);

    if (!h) return GRIB_INVALID_GRIB;

    return grib_get_native_type(h, key, type);
}

int grib_c_index_write(int* gid, char* file)
{
    grib_index *i = get_index(*gid);
    int err = GRIB_SUCCESS;

    if (!i) {
        return GRIB_INVALID_GRIB;
    } else {
        err = grib_index_write(i,file);
        return err;
    }
}

int grib_c_index_read(char* file, int* gid)
{
    int err = 0;
    grib_index *i = NULL;

    if (*file) {
        i = grib_index_read(0,file,&err);
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

void no_fail_on_wrong_length(int flag)
{
    grib_context *c = grib_context_get_default();
    int value;
    assert(c != NULL);
    value = (flag != 0) ? 1 : 0;
    c->no_fail_on_wrong_length = value;
}

void grib_c_gts_header_on(void)
{
    grib_context *c = grib_context_get_default();
    assert(c != NULL);
    grib_gts_header_on(c);
}

void grib_c_gts_header_off(void)
{
    grib_context *c = grib_context_get_default();
    assert(c != NULL);
    grib_gts_header_off(c);
}

long grib_c_get_api_version(void)
{
    return grib_get_api_version();
}

int grib_c_get_message(int *gid, const void **msg, size_t *size)
{
    grib_handle *h = get_handle(*gid);
    return grib_get_message(h,msg,size);
}

void grib_c_set_definitions_path(const char* path)
{
    grib_context *c = grib_context_get_default();
    grib_context_set_definitions_path(c, path);
}

void grib_c_set_samples_path(const char* path)
{
    grib_context *c = grib_context_get_default();
    grib_context_set_samples_path(c, path);
}
