/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*****************************************
 *  Enrico Fucile
 ****************************************/

#include "grib_api_internal.h"
#include <ctype.h>

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

static void thread_init() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex1,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex1;

static void thread_init()
{
    GRIB_OMP_CRITICAL(lock_grib_accessor_class_codetable_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex1);
            once = 1;
        }
    }
}
#endif

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = init;dump;unpack_string;pack_expression;unpack_long
   IMPLEMENTS = value_count;pack_string; destroy; get_native_type;
   MEMBERS    =  const char* tablename
   MEMBERS    =  const char* masterDir
   MEMBERS    =  const char* localDir
   MEMBERS    =  grib_codetable* table
   MEMBERS    =  int table_loaded
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int pack_string(grib_accessor*, const char*, size_t *len);
static int pack_expression(grib_accessor*, grib_expression*);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static int value_count(grib_accessor*,long*);
static void destroy(grib_context*,grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_codetable {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in unsigned */
	long nbytes;
	grib_arguments* arg;
/* Members defined in codetable */
	const char* tablename;
	const char* masterDir;
	const char* localDir;
	grib_codetable* table;
	int table_loaded;
} grib_accessor_codetable;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_codetable = {
    &grib_accessor_class_unsigned,                      /* super                     */
    "codetable",                      /* name                      */
    sizeof(grib_accessor_codetable),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    &destroy,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    &pack_string,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    &pack_expression,            /* pack_expression */
    0,              /* notify_change   */
    0,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_codetable = &_grib_accessor_class_codetable;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->notify_change	=	(*(c->super))->notify_change;
	c->update_size	=	(*(c->super))->update_size;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->next	=	(*(c->super))->next;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static int grib_load_codetable(grib_context* c,const char* filename,
        const char* recomposed_name,size_t size,grib_codetable* t);

static void init(grib_accessor* a, const long len, grib_arguments* params)
{
    int n=0;
    long new_len = len;
    grib_handle* hand = grib_handle_of_accessor(a);
    grib_accessor_codetable* self  = (grib_accessor_codetable*)a;
    grib_action* act=(grib_action*)(a->creator);
    DebugAssert(len == self->nbytes);

    if (new_len == 0) {
        /* ECC-485: When the codetable length is 0, it means we are passing
         * its length as an identifier not an integer. This identifier is
         * added to the argument list (at the beginning)
         */
        new_len = grib_arguments_get_long(hand,params,n++);
        if ( new_len <= 0 ) {
            grib_context_log(a->context,GRIB_LOG_FATAL,"%s: codetable length must be a positive integer",a->name);
        }
        self->nbytes = new_len;
    }

    self->tablename = grib_arguments_get_string(hand,params,n++);
    if (self->tablename == NULL) {
        grib_context_log(a->context,GRIB_LOG_FATAL,"%s: codetable table is invalid",a->name);
    }
    self->masterDir = grib_arguments_get_name(hand,params,n++); /* can be NULL */
    self->localDir  = grib_arguments_get_name(hand,params,n++); /* can be NULL */

    /*if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
    printf("-------- %s type string (%ld)\n",a->name,a->flags);*/

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        a->length = 0;
        if (!a->vvalue)
            a->vvalue = (grib_virtual_value*)grib_context_malloc_clear(a->context,sizeof(grib_virtual_value));
        a->vvalue->type=grib_accessor_get_native_type(a);
        a->vvalue->length = new_len;
        if (act->default_value!=NULL) {
            const char* p = 0;
            size_t s_len = 1;
            long l;
            int ret=0;
            double d;
            char tmp[1024];
            grib_expression* expression=grib_arguments_get_expression(hand,act->default_value,0);
            int type = grib_expression_native_type(hand,expression);
            switch(type) {
            case GRIB_TYPE_DOUBLE:
                grib_expression_evaluate_double(hand,expression,&d);
                grib_pack_double(a,&d,&s_len);
                break;

            case GRIB_TYPE_LONG:
                grib_expression_evaluate_long(grib_handle_of_accessor(a),expression,&l);
                grib_pack_long(a,&l,&s_len);
                break;

            default:
                s_len = sizeof(tmp);
                p = grib_expression_evaluate_string(grib_handle_of_accessor(a),expression,tmp,&s_len,&ret);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(a->context,GRIB_LOG_FATAL,
                            "unable to evaluate %s as string",a->name);
                }
                s_len = strlen(p)+1;
                pack_string(a,p,&s_len);
                break;
            }
        }
    } else {
        a->length = new_len;
    }
}

/* Note: A fast cut-down version of strcmp which does NOT return -1 */
/* 0 means input strings are equal and 1 means not equal */
GRIB_INLINE static int grib_inline_strcmp(const char* a,const char* b) {
    if (*a != *b) return 1;
    while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
    return (*a==0 && *b==0) ? 0 : 1;
}

static int str_eq(const char* a, const char* b)
{
    if ( a && b && (grib_inline_strcmp(a,b)==0) )
        return 1;
    return 0;
}

#ifdef DEBUGGING
static void dump_codetable(grib_codetable* atable)
{
    grib_codetable* next = NULL;
    int count = 0;

    next=atable;
    while(next) {
        printf("[%.2d] CodeTable Dump: f0=%s f1=%s\n", count, next->filename[0], next->filename[1]);
        count++;
        next = next->next;
    }
}
#endif
static grib_codetable* load_table(grib_accessor_codetable* self)
{
    size_t size = 0;
    grib_handle*    h = ((grib_accessor*)self)->parent->h;
    grib_context*   c = h->context;
    grib_codetable* t = NULL;
    grib_codetable* next=NULL ;
    grib_accessor* a=(grib_accessor*)self;
    char *filename=0;
    char recomposed[1024]={0,};
    char localRecomposed[1024]={0,};
    char *localFilename=0;
    char masterDir[1024]={0,};
    char localDir[1024]={0,};
    size_t len=1024;

    if (self->masterDir != NULL)
        grib_get_string(h,self->masterDir,masterDir,&len);

    len=1024;
    if (self->localDir != NULL)
        grib_get_string(h,self->localDir,localDir,&len);

    if (*masterDir!=0) {
        char name[1024]={0,};
        sprintf(name,"%s/%s",masterDir,self->tablename);
        grib_recompose_name(h, NULL,name, recomposed,0);
        filename=grib_context_full_defs_path(c,recomposed);
    } else {
        grib_recompose_name(h, NULL,self->tablename, recomposed,0);
        filename=grib_context_full_defs_path(c,recomposed);
    }

    if (*localDir!=0) {
        char localName[1024]={0,};
        sprintf(localName,"%s/%s",localDir,self->tablename);
        grib_recompose_name(h, NULL,localName, localRecomposed,0);
        localFilename=grib_context_full_defs_path(c,localRecomposed);
    }

    GRIB_MUTEX_INIT_ONCE(&once,&thread_init);
    GRIB_MUTEX_LOCK(&mutex1); /* GRIB-930 */

    /*printf("DBG %s: Look in cache: f=%s lf=%s (recomposed=%s)\n", self->att.name, filename, localFilename,recomposed);*/
    if (filename == NULL && localFilename == NULL) {
        t = NULL;
        goto the_end;
    }
    next=c->codetable;
    while(next) {
        if ((filename && next->filename[0] && grib_inline_strcmp(filename,next->filename[0]) == 0) &&
                ((localFilename==0 && next->filename[1]==NULL) ||
                        ((localFilename!=0 && next->filename[1]!=NULL)
                                && grib_inline_strcmp(localFilename,next->filename[1]) ==0)) )
        {
            t = next;
            goto the_end;
        }
        /* Special case: see GRIB-735 */
        if (filename==NULL && localFilename!=NULL)
        {
            if ( str_eq(localFilename, next->filename[0]) ||
                 str_eq(localFilename, next->filename[1]) )
            {
                t = next;
                goto the_end;
            }
        }
        next = next->next;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        Assert(a->vvalue!=NULL);
        size=a->vvalue->length*8;
    } else {
        size = grib_byte_count((grib_accessor*)self) * 8;
    }
    size = grib_power(size,2);

    t = (grib_codetable*)grib_context_malloc_clear_persistent(c,sizeof(grib_codetable) +
            (size-1)*sizeof(code_table_entry));

    if (filename!=0)
        grib_load_codetable(c,filename,recomposed,size,t);

    if (localFilename!=0)
        grib_load_codetable(c,localFilename,localRecomposed,size,t);

    /*dump_codetable(c->codetable);*/

    if (t->filename[0]==NULL && t->filename[1]==NULL) {
        grib_context_free_persistent(c,t);
        t = NULL;
        goto the_end;
    }

the_end:
    GRIB_MUTEX_UNLOCK(&mutex1);

    return t;
}

static int grib_load_codetable(grib_context* c,const char* filename,
        const char* recomposed_name,size_t size,grib_codetable* t)
{
    char line[1024];
    FILE *f = NULL;
    int lineNumber = 0;
    grib_context_log(c,GRIB_LOG_DEBUG,"Loading code table from %s",filename);

    f = codes_fopen(filename, "r");
    if (!f) return GRIB_IO_PROBLEM;

    Assert(t!=NULL);

    if (t->filename[0] == NULL ){
        t->filename[0]  = grib_context_strdup_persistent(c,filename);
        t->recomposed_name[0]  = grib_context_strdup_persistent(c,recomposed_name);
        t->next      = c->codetable;
        t->size      = size;
        c->codetable = t;
    } else {
        t->filename[1]  = grib_context_strdup_persistent(c,filename);
        t->recomposed_name[1]  = grib_context_strdup_persistent(c,recomposed_name);
    }

    while(fgets(line,sizeof(line)-1,f))
    {
        char* p = line;
        int code    = 0;
        char abbreviation[1024] = {0,};
        char title[1024]={0,};
        char* q = abbreviation;
        char* r = title;
        char* units=0;
        char unknown[]="unknown";
        ++lineNumber;

        line[strlen(line)-1] = 0;

        while(*p != '\0' && isspace(*p)) p++;

        if(*p == '#')
            continue;

        while(*p != '\0' && isspace(*p)) p++;

        if( *p =='\0' ) continue;

        if (!isdigit(*p))
        {
            grib_context_log(c,GRIB_LOG_ERROR, "Invalid entry in file %s: line %d", filename, lineNumber);
            continue; /* skip this line */
        }
        Assert(isdigit(*p));

        while(*p != '\0')
        {
            if(isspace(*p)) break;
            code *= 10;
            code += *p - '0';
            p++;
        }

        if(code <0 || code >= size)
        {
            grib_context_log(c,GRIB_LOG_WARNING,"code_table_entry: invalid code in %s: %d (table size=%d)",filename,code,size);
            continue;
        }

        while(*p != '\0' && isspace(*p)) p++;

        while(*p != '\0')
        {
            if(isspace(*p)) break;
            *q++ = *p++;
        }
        *q = 0;
        while(*p != '\0' && isspace(*p)) p++;

        while(*p != '\0')
        {
            if(*p == '(' ) break;
            *r++ = *p++;
        }
        *r = 0;

        while(*p != '\0' && isspace(*p)) p++;
        if (*p != '\0') {
            units=++p;
            while(*p != '\0' && *p != ')' ) p++;
            *p='\0';
        } else {
            units=unknown;
        }

        Assert(*abbreviation);
        Assert(*title);

        if(t->entries[code].abbreviation != NULL)
        {
            grib_context_log(c,GRIB_LOG_WARNING,"code_table_entry: duplicate code in %s: %d (table size=%d)",filename,code,size);
            continue;
        }

        Assert(t->entries[code].abbreviation == NULL);
        Assert(t->entries[code].title        == NULL);

        t->entries[code].abbreviation = grib_context_strdup_persistent(c,abbreviation);
        t->entries[code].title        = grib_context_strdup_persistent(c,title);
        t->entries[code].units        = grib_context_strdup_persistent(c,units);
    }

    fclose(f);

    return 0;
}

void grib_codetable_delete(grib_context* c)
{
    grib_codetable* t = c->codetable;

    while(t)
    {
        grib_codetable* s = t->next;
        int i;

        for(i = 0; i < t->size; i++)
        {
            grib_context_free_persistent(c,t->entries[i].abbreviation);
            grib_context_free_persistent(c,t->entries[i].title);
        }
        grib_context_free_persistent(c,t->filename[0]);
        if(t->filename[1])
            grib_context_free_persistent(c,t->filename[1]);
        grib_context_free_persistent(c,t->recomposed_name[0]);
        if (t->recomposed_name[1])
            grib_context_free_persistent(c,t->recomposed_name[1]);
        grib_context_free_persistent(c,t);
        t = s;
    }
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_accessor_codetable* self  = (grib_accessor_codetable*)a;
    char comment[2048];
    grib_codetable* table;

    size_t llen = 1;
    long value;

    if (!self->table_loaded) {
        self->table = load_table(self); /* may return NULL */
        self->table_loaded = 1;
    }
    table=self->table;

    grib_unpack_long(a, &value,&llen);

    if(value == GRIB_MISSING_LONG)
    {
        if(a->length < 4)
        {
            value = (1L << a->length) - 1;
        }
    }

    if(table && value >= 0 && value < table->size)
    {
        if(table->entries[value].abbreviation)
        {
            int b = atol(table->entries[value].abbreviation);
            if(b == value)
                strcpy(comment,table->entries[value].title);
            else
                sprintf(comment,"%s", table->entries[value].title);

            if (table->entries[value].units!=NULL && grib_inline_strcmp(table->entries[value].units,"unknown")) {
                strcat(comment," (");
                strcat(comment,table->entries[value].units);
                strcat(comment,") ");
            }
        }
        else
        {
            strcpy(comment,"Unknown code table entry");
        }

    }
    else
    {
        strcpy(comment,"Unknown code table entry");
    }

    strcat(comment," (");
    if (table) {
        strcat(comment,table->recomposed_name[0]);
        if (table->recomposed_name[1]!=NULL) {
            strcat(comment," , ");
            strcat(comment,table->recomposed_name[1]);
        }
    }
    strcat(comment,") ");

    grib_dump_long(dumper,a,comment);
}

static int unpack_string (grib_accessor* a, char* buffer, size_t *len)
{
    grib_accessor_codetable* self = (grib_accessor_codetable*)a;
    grib_codetable*          table = NULL;

    size_t size = 1;
    long   value;
    int err = GRIB_SUCCESS;
    char tmp[1024];
    size_t l = 0;

    if( (err = grib_unpack_long(a,&value,&size)) != GRIB_SUCCESS)
        return err;

    if (!self->table_loaded) {
        self->table = load_table(self); /* may return NULL */
        self->table_loaded=1;
    }
    table=self->table;

    if(table && (value >= 0) && (value < table->size) && table->entries[value].abbreviation)
    {
        strcpy(tmp,table->entries[value].abbreviation);
    }
    else
    {

#if 1
        sprintf(tmp,"%d",(int)value);
#else
        return GRIB_DECODING_ERROR;
#endif
    }

    l = strlen(tmp) + 1;

    if(*len < l)
    {
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }

    strcpy(buffer,tmp);
    *len = l;

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

static int pack_string(grib_accessor* a, const char* buffer, size_t *len)
{
    grib_accessor_codetable* self = (grib_accessor_codetable*)a;
    grib_codetable*          table ;

    long i;
    size_t size = 1;

    typedef int (*cmpproc)(const char*, const char*);
#ifndef ECCODES_ON_WINDOWS
    cmpproc cmp = (a->flags & GRIB_ACCESSOR_FLAG_LOWERCASE) ? strcmp_nocase : strcmp;
#else
    /* Microsoft Windows Visual Studio support */
    cmpproc cmp = (a->flags & GRIB_ACCESSOR_FLAG_LOWERCASE) ? stricmp : strcmp;
#endif

    if (!self->table_loaded) {
        self->table = load_table(self); /* may return NULL */
        self->table_loaded=1;
    }
    table=self->table;

    if(!table)
        return GRIB_ENCODING_ERROR;

    if (a->set) {
        int err=grib_set_string(grib_handle_of_accessor(a),a->set,buffer,len);
        if (err!=0) return err;
    }

    for(i = 0 ; i < table->size; i++)
        if(table->entries[i].abbreviation)
            if(cmp(table->entries[i].abbreviation,buffer) == 0)
                return grib_pack_long(a,&i,&size);

    if (a->flags & GRIB_ACCESSOR_FLAG_NO_FAIL) {
        grib_action* act=(grib_action*)(a->creator);
        if (act->default_value!=NULL) {
            const char* p = 0;
            size_t s_len = 1;
            long l;
            int ret=0;
            double d;
            char tmp[1024];
            grib_expression* expression=grib_arguments_get_expression(grib_handle_of_accessor(a),act->default_value,0);
            int type = grib_expression_native_type(grib_handle_of_accessor(a),expression);
            switch(type) {
            case GRIB_TYPE_DOUBLE:
                grib_expression_evaluate_double(grib_handle_of_accessor(a),expression,&d);
                grib_pack_double(a,&d,&s_len);
                break;

            case GRIB_TYPE_LONG:
                grib_expression_evaluate_long(grib_handle_of_accessor(a),expression,&l);
                grib_pack_long(a,&l,&s_len);
                break;

            default:
                s_len = sizeof(tmp);
                p = grib_expression_evaluate_string(grib_handle_of_accessor(a),expression,tmp,&s_len,&ret);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(a->context,GRIB_LOG_FATAL,
                            "unable to evaluate %s as string",a->name);
                    return ret;
                }
                s_len = strlen(p)+1;
                pack_string(a,p,&s_len);
                break;
            }
            return GRIB_SUCCESS;
        }

    }
    return GRIB_ENCODING_ERROR;
}

static int pack_expression(grib_accessor* a, grib_expression *e)
{
    const char* cval=NULL;
    int ret=0;
    long lval=0;
    size_t len = 1;
    grib_handle* hand = grib_handle_of_accessor(a);

    if (strcmp(e->cclass->name,"long")==0) {
        ret=grib_expression_evaluate_long(hand,e,&lval);
        /*if (hand->context->debug)
            printf("ECCODES DEBUG grib_accessor_class_codetable::pack_expression %s %ld\n", a->name,lval);*/

        ret = grib_pack_long(a,&lval,&len);
    } else {
        char tmp[1024];
        len = sizeof(tmp);
        cval = grib_expression_evaluate_string(hand,e,tmp,&len,&ret);
        if (ret!=GRIB_SUCCESS) {
            grib_context_log(a->context,GRIB_LOG_ERROR,"grib_accessor_codetable.pack_expression: unable to evaluate string %s to be set in %s\n",grib_expression_get_name(e),a->name);
            return ret;
        }
        len = strlen(cval) + 1;
        /*if (hand->context->debug)
            printf("ECCODES DEBUG grib_accessor_class_codetable::pack_expression %s %s\n", a->name, cval);*/
        ret = grib_pack_string(a,cval,&len);
    }
    return ret;
}

static void destroy(grib_context* context,grib_accessor* a)
{
    if (a->vvalue != NULL) {
        grib_context_free(context, a->vvalue);
        a->vvalue=NULL;
    }
}

static int get_native_type(grib_accessor* a)
{
    int type=GRIB_TYPE_LONG;
    /*printf("---------- %s flags=%ld GRIB_ACCESSOR_FLAG_STRING_TYPE=%d\n",
         a->name,a->flags,GRIB_ACCESSOR_FLAG_STRING_TYPE);*/
    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type=GRIB_TYPE_STRING;
    return type;
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_codetable* self = (grib_accessor_codetable*)a;
    long rlen = 0;

    unsigned long i = 0;
    long pos = a->offset*8;
    grib_handle* hand = NULL;

#ifdef DEBUG
    {
        int err = grib_value_count(a,&rlen);
        Assert(!err);
        Assert(rlen == 1);
    }
#endif
    rlen = 1; /* ECC-480 Performance: avoid func call overhead of grib_value_count */

    if (!self->table_loaded) {
        self->table = load_table(self); /* may return NULL */
        self->table_loaded=1;
    }

    if(*len < rlen)
    {
        grib_context_log(a->context, GRIB_LOG_ERROR, " wrong size (%ld) for %s it contains %d values ",*len, a->name , rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_TRANSIENT) {
        *val=a->vvalue->lval;
        *len=1;
        return GRIB_SUCCESS;
    }

    /* ECC-480 Performance: inline the grib_handle_of_accessor here to reduce func call overhead */
    if (a->parent==NULL) hand = a->h;
    else                 hand = a->parent->h;

    for(i=0; i< rlen;i++){
        val[i] = (long)grib_decode_unsigned_long(hand->buffer->data , &pos, self->nbytes*8);
    }

    *len = rlen;
    return GRIB_SUCCESS;
}
