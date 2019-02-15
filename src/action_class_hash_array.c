/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Enrico  Fucile 2012                                                   *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   SUPER      = action_class_gen
   IMPLEMENTS = dump
   IMPLEMENTS = destroy
   MEMBERS    = grib_hash_array_value* hash_array
   MEMBERS    = char* basename
   MEMBERS    = char* masterDir
   MEMBERS    = char* localDir
   MEMBERS    = char* ecmfDir
   MEMBERS    = int nofail
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "action.class" and rerun ./make_class.pl

*/

static void init_class      (grib_action_class*);
static void dump            (grib_action* d, FILE*,int);
static void destroy         (grib_context*,grib_action*);


typedef struct grib_action_hash_array {
    grib_action          act;  
/* Members defined in gen */
	long            len;
	grib_arguments* params;
/* Members defined in hash_array */
	grib_hash_array_value* hash_array;
	char* basename;
	char* masterDir;
	char* localDir;
	char* ecmfDir;
	int nofail;
} grib_action_hash_array;

extern grib_action_class* grib_action_class_gen;

static grib_action_class _grib_action_class_hash_array = {
    &grib_action_class_gen,                              /* super                     */
    "action_class_hash_array",                              /* name                      */
    sizeof(grib_action_hash_array),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    0,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    0,                            /* execute */
};

grib_action_class* grib_action_class_hash_array = &_grib_action_class_hash_array;

static void init_class(grib_action_class* c)
{
	c->xref	=	(*(c->super))->xref;
	c->create_accessor	=	(*(c->super))->create_accessor;
	c->notify_change	=	(*(c->super))->notify_change;
	c->reparse	=	(*(c->super))->reparse;
	c->execute	=	(*(c->super))->execute;
}
/* END_CLASS_IMP */

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void init() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void init()
{
    GRIB_OMP_CRITICAL(lock_action_class_hash_array_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif

grib_action* grib_action_create_hash_array( grib_context* context,
        const char* name,
        grib_hash_array_value* hash_array,
        const char* basename,const char* name_space,const char* defaultkey,
        const char* masterDir,const char* localDir,const char* ecmfDir,int flags,int nofail )
{
    grib_action_hash_array* a=NULL ;
    grib_action_class* c = grib_action_class_hash_array;
    grib_action* act     = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op              = grib_context_strdup_persistent(context,"hash_array");

    act->cclass       = c;
    a=(grib_action_hash_array*)act ;
    act->context      = context;
    act->flags        = flags;

    if (name_space)
        act->name_space  = grib_context_strdup_persistent(context,name_space);

    if (basename)
        a->basename= grib_context_strdup_persistent(context,basename);
    else a->basename=NULL;

    if (masterDir)
        a->masterDir= grib_context_strdup_persistent(context,masterDir);
    else a->masterDir=NULL;

    if (localDir)
        a->localDir= grib_context_strdup_persistent(context,localDir);
    else a->localDir=NULL;

    if (ecmfDir)
        a->ecmfDir= grib_context_strdup_persistent(context,ecmfDir);
    else a->ecmfDir=NULL;

    if (defaultkey)
        act->defaultkey = grib_context_strdup_persistent(context,defaultkey);

    a->hash_array = hash_array;
    if (hash_array) {
        grib_hash_array_value* ha=hash_array;
        grib_trie* index=grib_trie_new(context);
        while (ha) {
            ha->index=index;
            grib_trie_insert_no_replace(index,ha->name,ha);
            ha=ha->next;
        }
    }
    act->name = grib_context_strdup_persistent(context,name);

    a->nofail=nofail;

    return act;
}

static void dump(grib_action* act, FILE* f, int lvl)
{
    int i = 0;

    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");

    printf("hash_array(%s) { ",act->name);
    printf("\n");

    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");
    printf("}\n");
}


static void destroy(grib_context* context,grib_action* act)
{
    grib_action_hash_array* self = (grib_action_hash_array*) act;

    grib_hash_array_value * v = self->hash_array;
    if (v) grib_trie_delete(v->index);
    while(v)
    {
        grib_hash_array_value* n = v->next;
        grib_hash_array_value_delete(context,v);
        v = n;
    }

    grib_context_free_persistent(context, self->masterDir);
    grib_context_free_persistent(context, self->localDir);
    grib_context_free_persistent(context, self->ecmfDir);
    grib_context_free_persistent(context, self->basename);
}

static grib_hash_array_value* get_hash_array_impl(grib_handle* h,grib_action* a)
{
    char buf[1024]={0,};
    char master[1024]={0,};
    char local[1024]={0,};
    char ecmf[1024]={0,};
    char masterDir[1024]={0,};
    size_t lenMasterDir=1024;
    char localDir[1024]={0,};
    size_t lenLocalDir=1024;
    char ecmfDir[1024]={0,};
    size_t lenEcmfDir=1024;
    char key[1024]={0,};
    char* full=0;
    int id;
    int err;
    grib_action_hash_array* self=(grib_action_hash_array*)a;

    grib_context* context=((grib_action*)self)->context;
    grib_hash_array_value* c=NULL;

    if (self->hash_array != NULL)
        return self->hash_array;

    Assert(self->masterDir);
    grib_get_string(h,self->masterDir,masterDir,&lenMasterDir);

    sprintf(buf,"%s/%s",masterDir,self->basename);

    err=grib_recompose_name(h,NULL, buf, master,1);
    if (err) {
        grib_context_log(context,GRIB_LOG_ERROR,
                "unable to build name of directory %s",self->masterDir);
        return NULL;
    }

    if (self->localDir) {
        grib_get_string(h,self->localDir,localDir,&lenLocalDir);
        sprintf(buf,"%s/%s",localDir,self->basename);
        grib_recompose_name(h,NULL, buf, local,1);
    }

    if (self->ecmfDir) {
        grib_get_string(h,self->ecmfDir,ecmfDir,&lenEcmfDir);
        sprintf(buf,"%s/%s",ecmfDir,self->basename);
        grib_recompose_name(h,NULL, buf, ecmf,1);
    }

    sprintf(key,"%s%s%s",master,local,ecmf);

    id=grib_itrie_get_id(h->context->hash_array_index,key);
    if ((c=h->context->hash_array[id])!=NULL) return c;

    if (*local && (full=grib_context_full_defs_path(context,local))!=NULL) {
        c=grib_parse_hash_array_file(context,full);
        grib_context_log(h->context,GRIB_LOG_DEBUG,
                "Loading hash_array %s from %s",((grib_action*)self)->name,full);
    } else if (*ecmf && (full=grib_context_full_defs_path(context,ecmf))!=NULL) {
        c=grib_parse_hash_array_file(context,full);
        grib_context_log(h->context,GRIB_LOG_DEBUG,
                "Loading hash_array %s from %s",((grib_action*)self)->name,full);
    }

    full=grib_context_full_defs_path(context,master);

    if(c) {
        grib_hash_array_value* last=c;
        while (last->next) last=last->next;
        last->next=grib_parse_hash_array_file(context,full);
    } else if (full) {
        c=grib_parse_hash_array_file(context,full);
    } else {
        grib_context_log(context,GRIB_LOG_ERROR,
                "unable to find definition file %s in %s:%s:%s\nDefinition files path=\"%s\"",
                self->basename,master,ecmf,local,context->grib_definition_files_path);
        return NULL;
    }

    grib_context_log(h->context,GRIB_LOG_DEBUG,
            "Loading hash_array %s from %s",((grib_action*)self)->name,full);

    h->context->hash_array[id]=c;
    if (c) {
        grib_trie* index=grib_trie_new(context);
        while (c) {
            c->index=index;
            grib_trie_insert_no_replace(index,c->name,c);
            c=c->next;
        }

    }

    return h->context->hash_array[id];
}

grib_hash_array_value* get_hash_array(grib_handle* h,grib_action* a)
{
    grib_hash_array_value* result = NULL;
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&mutex);

    result = get_hash_array_impl(h, a);

    GRIB_MUTEX_UNLOCK(&mutex);
    return result;
}
