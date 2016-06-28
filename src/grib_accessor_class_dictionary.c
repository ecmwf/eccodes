/*
 * Copyright 2005-2016 ECMWF.
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

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init;dump;unpack_string;unpack_long;unpack_double
   IMPLEMENTS = value_count; get_native_type;
   MEMBERS    =  const char* dictionary
   MEMBERS    =  const char* key
   MEMBERS    =  long column
   MEMBERS    =  const char* masterDir
   MEMBERS    =  const char* localDir
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int  get_native_type(grib_accessor*);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int unpack_string (grib_accessor*, char*, size_t *len);
static int value_count(grib_accessor*,long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_dictionary {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in dictionary */
	const char* dictionary;
	const char* key;
	long column;
	const char* masterDir;
	const char* localDir;
} grib_accessor_dictionary;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_dictionary = {
    &grib_accessor_class_gen,                      /* super                     */
    "dictionary",                      /* name                      */
    sizeof(grib_accessor_dictionary),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
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
    &unpack_double,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
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


grib_accessor_class* grib_accessor_class_dictionary = &_grib_accessor_class_dictionary;


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
	c->pack_string	=	(*(c->super))->pack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
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

static void init(grib_accessor* a, const long len, grib_arguments* params)
{
    int n=0;
    grib_accessor_dictionary* self  = (grib_accessor_dictionary*)a;

    self->dictionary = grib_arguments_get_string(grib_handle_of_accessor(a),params,n++);
    self->key = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->column = grib_arguments_get_long(grib_handle_of_accessor(a),params,n++);
    self->masterDir = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);
    self->localDir = grib_arguments_get_name(grib_handle_of_accessor(a),params,n++);

    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;

}

static grib_trie* load_dictionary(grib_context* c,grib_accessor* a, int* err)
{
    grib_accessor_dictionary* self = (grib_accessor_dictionary*)a;

    char* filename=NULL;
    char line[1024]={0,};
    char key[1024]={0,};
    char masterDir[1024]={0,};
    char localDir[1024]={0,};
    char name[1024]={0,};
    char localName[1024]={0,};
    char recomposed[1024]={0,};
    char localRecomposed[1024]={0,};
    char dictName[1024]={0,};
    char *localFilename=0;
    char* list=0;
    size_t len=1024;
    grib_trie* dictionary=NULL;
    FILE* f=NULL;
    int i=0;
    grib_handle* h=grib_handle_of_accessor(a);

    *err=GRIB_SUCCESS;

    len=1024;
    if (self->masterDir != NULL) grib_get_string(h,self->masterDir,masterDir,&len);
    len=1024;
    if (self->localDir != NULL) grib_get_string(h,self->localDir,localDir,&len);

    if (*masterDir!=0) {
        sprintf(name,"%s/%s",masterDir,self->dictionary);
        grib_recompose_name(h, NULL,name, recomposed,0);
        filename=grib_context_full_defs_path(c,recomposed);
    } else {
        filename=grib_context_full_defs_path(c,self->dictionary);
    }

    if (*localDir!=0) {
        sprintf(localName,"%s/%s",localDir,self->dictionary);
        grib_recompose_name(h, NULL,localName, localRecomposed,0);
        localFilename=grib_context_full_defs_path(c,localRecomposed);
        sprintf(dictName,"%s:%s",localFilename,filename);
    } else {
        sprintf(dictName,"%s",filename);
    }

    if (!filename) {
        grib_context_log(c,GRIB_LOG_ERROR,"unable to find def file %s",self->dictionary);
        *err=GRIB_FILE_NOT_FOUND;
        return NULL;
    } else {
        grib_context_log(c,GRIB_LOG_DEBUG,"found def file %s",filename);
    }
    dictionary=(grib_trie*)grib_trie_get(c->lists,dictName);
    if (dictionary) {
        grib_context_log(c,GRIB_LOG_DEBUG,"using dictionary %s from cache",self->dictionary);
        return dictionary;
    } else {
        grib_context_log(c,GRIB_LOG_DEBUG,"using dictionary %s from file %s",self->dictionary,filename);
    }

    f=codes_fopen(filename,"r");
    if (!f) {*err=GRIB_IO_PROBLEM; return NULL;}

    dictionary=grib_trie_new(c);

    while(fgets(line,sizeof(line)-1,f)) {
        i=0;
        while (line[i] != '|' && line[i] != 0)  {
            key[i]=line[i];
            i++;
        }
        key[i]=0;
        list=(char*)grib_context_malloc_clear(c,strlen(line)+1);
        memcpy(list,line,strlen(line));
        grib_trie_insert(dictionary,key,list);
    }

    fclose(f);

    if (localFilename!=0) {
        f=codes_fopen(localFilename,"r");
        if (!f) {*err=GRIB_IO_PROBLEM; return NULL;}

        while(fgets(line,sizeof(line)-1,f)) {
            i=0;
            while (line[i] != '|' && line[i] != 0)  {
                key[i]=line[i];
                i++;
            }
            key[i]=0;
            list=(char*)grib_context_malloc_clear(c,strlen(line)+1);
            memcpy(list,line,strlen(line));
            grib_trie_insert(dictionary,key,list);
        }


        fclose(f);
    }
    grib_trie_insert(c->lists,filename,dictionary);
    return dictionary;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    switch (get_native_type(a)) {
    case GRIB_TYPE_STRING:
        grib_dump_string(dumper,a,NULL);
        break;
    case GRIB_TYPE_LONG:
        grib_dump_long(dumper,a,NULL);
        break;
    case GRIB_TYPE_DOUBLE:
        grib_dump_double(dumper,a,NULL);
        break;
    }
}

static int unpack_string (grib_accessor* a, char* buffer, size_t *len)
{
    grib_accessor_dictionary* self = (grib_accessor_dictionary*)a;
    int err = GRIB_SUCCESS;
    char key[1024]={0,};
    size_t size=1024;
    char* list=NULL;
    char* start=NULL;
    char* end=NULL;
    size_t rsize=0;
    int i=0;

    grib_trie* dictionary=load_dictionary(a->context,a,&err);
    if (err) return err;

    if((err=grib_get_string_internal(grib_handle_of_accessor(a),self->key,key,&size)) != GRIB_SUCCESS) {
        /* grib_trie_delete(dictionary); */
        return err;
    }

    list=(char*)grib_trie_get(dictionary,key);
    if (!list) {
        /* grib_trie_delete(dictionary); */
        return GRIB_NOT_FOUND;
    }

    end=list;
    for (i=0;i<=self->column;i++) {
        start=end;
        while (*end != '|' && *end!=0) end++;
        if (! *end ) {
            break;
        }
        end++;
    }
    end--;
    rsize=end-start;
    if (*len < rsize) {
        /* grib_trie_delete(dictionary); */
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len=rsize;
    memcpy(buffer,start,rsize);
    buffer[rsize]=0;

    /* grib_trie_delete(dictionary); */

    return err;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

static int get_native_type(grib_accessor* a)
{
    int type=GRIB_TYPE_DOUBLE;
    if (a->flags & GRIB_ACCESSOR_FLAG_LONG_TYPE)
        type=GRIB_TYPE_LONG;
    if (a->flags & GRIB_ACCESSOR_FLAG_STRING_TYPE)
        type=GRIB_TYPE_STRING;
    return type;
}

static int unpack_long   (grib_accessor* a, long* val, size_t *len)
{
    int err=0;
    char buffer[1024]={0,};
    size_t size=1024;

    err=unpack_string (a,buffer,&size);
    if (err) return err;

    *val=atol(buffer);
    *len=1;

    return err;
}

static int unpack_double   (grib_accessor* a, double* val, size_t *len)
{
    int err=0;
    char buffer[1024]={0,};
    size_t size=1024;

    err=unpack_string (a,buffer,&size);
    if (err) return err;

    *val=atof(buffer);
    *len=1;

    return err;
}
