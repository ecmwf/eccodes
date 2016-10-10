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
#include <ctype.h>
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = dumper
   IMPLEMENTS = dump_long;dump_bits
   IMPLEMENTS = dump_double;dump_string;dump_string_array
   IMPLEMENTS = dump_bytes;dump_values
   IMPLEMENTS = dump_label;dump_section
   IMPLEMENTS = init;destroy
   IMPLEMENTS = header;footer
   MEMBERS = long section_offset
   MEMBERS = long empty
   MEMBERS = long end
   MEMBERS = long isLeaf
   MEMBERS = long isAttribute
   MEMBERS = grib_string_list* keys
   END_CLASS_DEF

 */


/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "dumper.class" and rerun ./make_class.pl

*/

static void init_class      (grib_dumper_class*);
static int init            (grib_dumper* d);
static int destroy         (grib_dumper*);
static void dump_long       (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_bits       (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_double     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_string     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_string_array     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_bytes      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_values     (grib_dumper* d, grib_accessor* a);
static void dump_label      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_section    (grib_dumper* d, grib_accessor* a,grib_block_of_accessors* block);
static void header         (grib_dumper*,grib_handle*);
static void footer         (grib_dumper*,grib_handle*);

typedef struct grib_dumper_bufr_decode_C {
    grib_dumper          dumper;  
/* Members defined in bufr_decode_C */
	long section_offset;
	long empty;
	long end;
	long isLeaf;
	long isAttribute;
	grib_string_list* keys;
} grib_dumper_bufr_decode_C;


static grib_dumper_class _grib_dumper_class_bufr_decode_C = {
    0,                              /* super                     */
    "bufr_decode_C",                              /* name                      */
    sizeof(grib_dumper_bufr_decode_C),     /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    &init,                               /* init                      */
    &destroy,                            /* free mem                       */
    &dump_long,                          /* dump long         */
    &dump_double,                        /* dump double    */
    &dump_string,                        /* dump string    */
    &dump_string_array,                        /* dump string array   */
    &dump_label,                         /* dump labels  */
    &dump_bytes,                         /* dump bytes  */
    &dump_bits,                          /* dump bits   */
    &dump_section,                       /* dump section      */
    &dump_values,                        /* dump values   */
    &header,                             /* header   */
    &footer,                             /* footer   */
};

grib_dumper_class* grib_dumper_class_bufr_decode_C = &_grib_dumper_class_bufr_decode_C;

/* END_CLASS_IMP */
static void dump_attributes(grib_dumper* d, grib_accessor* a, const char* prefix);

GRIB_INLINE static int grib_inline_strcmp(const char* a, const char* b)
{
    if (*a != *b) return 1;
    while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
    return (*a==0 && *b==0) ? 0 : 1;
}

typedef struct string_count string_count;
struct string_count {
    char* value;
    int count;
    string_count* next;
};

static int depth=0;

static void init_class      (grib_dumper_class* c){}

static int init(grib_dumper* d)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    grib_context* c=d->handle->context;
    self->section_offset=0;
    self->empty=1;
    d->count=1;
    self->isLeaf=0;
    self->isAttribute=0;
    self->keys=grib_context_malloc_clear(c,sizeof(grib_string_list));

    return GRIB_SUCCESS;
}

static int destroy(grib_dumper* d)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    grib_string_list* next=self->keys;
    grib_string_list* cur=self->keys;
    grib_context* c=d->handle->context;
    while(next) {
        cur=next;
        next=next->next;
        grib_context_free(c,cur->value);
        grib_context_free(c,cur);
    }
    return GRIB_SUCCESS;
}

static char* dval_to_string(grib_context* c, double v)
{
    char* sval=grib_context_malloc_clear(c,sizeof(char)*40);
    sprintf(sval,"%.18e",v);
    return sval;
}

static void dump_values(grib_dumper* d, grib_accessor* a)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    double value=0; size_t size = 0;
    int err = 0;
    int r=0;
    long count=0;
    char* sval;
    grib_context* c=a->context;
    grib_handle* h=grib_handle_of_accessor(a);

    grib_value_count(a,&count);
    size=count;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) !=0)
        return;

    if (size <= 1) {
        err=grib_unpack_double(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {
        fprintf(self->dumper.out,"\n");
        fprintf(self->dumper.out,"  free(dValues);\n");
        fprintf(self->dumper.out,"  dValues = (double*)malloc(%lu*sizeof(double));\n", (unsigned long)size);
        fprintf(self->dumper.out,"  if (!dValues) { fprintf(stderr, \"Failed to allocate memory (dValues).\\n\"); return 1; }\n");
        fprintf(self->dumper.out,"  size = %lu;\n", size);

        depth-=2;

        if ((r=compute_key_rank(h,self->keys,a->name))!=0)
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double_array(h, \"#%d#%s\",dValues, &size), 0);\n", r, a->name);
        else
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double_array(h, \"%s\", dValues, &size), 0);\n",a->name);
    } else {
        r=compute_key_rank(h,self->keys,a->name);
        if( !grib_is_missing_double(a,value) ) {

            sval=dval_to_string(c,value);
            if (r!=0)
                fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double(h, \"#%d#%s\", &dVal), 0);\n", r, a->name);
            else
                fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double(h, \"%s\", &dVal), 0);\n", a->name);

            grib_context_free(c,sval);
        }
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
            dofree=1;
            sprintf(prefix,"#%d#%s",r,a->name);
        } else prefix=(char*)a->name;

        dump_attributes(d,a,prefix);
        if (dofree) grib_context_free(c,prefix);
        depth-=2;
    }

    (void)err; /* TODO */
}

static void dump_values_attribute(grib_dumper* d, grib_accessor* a, const char* prefix)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    double value; size_t size = 0;
    int err = 0;
    long count=0;
    char* sval;
    grib_context* c=a->context;

    grib_value_count(a,&count);
    size=count;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) !=0)
        return;

    if (size <= 1) {
        err=grib_unpack_double(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {
        fprintf(self->dumper.out,"\n");
        fprintf(self->dumper.out,"  free(dValues);\n");
        fprintf(self->dumper.out,"  dValues = (double*)malloc(%lu*sizeof(double));\n", (unsigned long)size);
        fprintf(self->dumper.out,"  if (!dValues) { fprintf(stderr, \"Failed to allocate memory (dValues).\\n\"); return 1; }\n");
        fprintf(self->dumper.out,"  size = %lu\n;", size);

        depth-=2;

        fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double_array(h, \"%s->%s\", dValues, &size), 0);\n", prefix,a->name);
    } else {
        /* int r=compute_key_rank(h,self->keys,a->name); */
        if( !grib_is_missing_double(a,value) ) {

            sval=dval_to_string(c,value);
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double(h, \"%s->%s\", &dVal), 0);\n", prefix,a->name);

            grib_context_free(c,sval);

        }
    }

    if (self->isLeaf==0) {
        char* prefix1;

        prefix1=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+strlen(prefix)+5));
        sprintf(prefix1,"%s->%s",prefix,a->name);

        dump_attributes(d,a,prefix1);

        grib_context_free(c,prefix1);
        depth-=2;
    }

    (void)err; /* TODO */
}

static void dump_long(grib_dumper* d,grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    long value=0; size_t size = 0;
    int err = 0;
    int r=0;
    long count=0;
    grib_context* c=a->context;
    grib_handle* h=grib_handle_of_accessor(a);

    grib_value_count(a,&count);
    size=count;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0  ) return;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0) {
        if (self->isLeaf==0) {
            char* prefix;
            int dofree=0;

            r=compute_key_rank(h,self->keys,a->name);
            if (r!=0) {
                prefix=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
                dofree=1;
                sprintf(prefix,"#%d#%s",r,a->name);
            } else prefix=(char*)a->name;

            dump_attributes(d,a,prefix);
            if (dofree) grib_context_free(c,prefix);
            depth-=2;
        }
        return;
    }

    if (size <= 1) {
        err=grib_unpack_long(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {
        fprintf(self->dumper.out,"\n");
        fprintf(self->dumper.out,"  free(iValues);\n");
        fprintf(self->dumper.out,"  iValues = (long*)malloc(%lu*sizeof(long));\n", (unsigned long)size);
        fprintf(self->dumper.out,"  if (!iValues) { fprintf(stderr, \"Failed to allocate memory (iValues).\\n\"); return 1; }\n");
        fprintf(self->dumper.out,"  size = %lu;\n", size);

        depth-=2;

        if ((r=compute_key_rank(h,self->keys,a->name))!=0)
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_long_array(h, \"#%d#%s\", iValues, &size), 0);\n",r,a->name);
        else
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_long_array(h, \"%s\", iValues, &size), 0);\n",a->name);

    } else {
        r=compute_key_rank(h,self->keys,a->name);
        if( !grib_is_missing_long(a,value) ) {
            if (r!=0)
                fprintf(self->dumper.out,"  CODES_CHECK(codes_get_long(h, \"#%d#%s\", &iVal), 0);\n", r,a->name);
            else
                fprintf(self->dumper.out,"  CODES_CHECK(codes_get_long(h, \"%s\", &iVal), 0);\n", a->name);
        }
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
            dofree=1;
            sprintf(prefix,"#%d#%s",r,a->name);
        } else prefix=(char*)a->name;

        dump_attributes(d,a,prefix);
        if (dofree) grib_context_free(c,prefix);
        depth-=2;
    }
    (void)err; /* TODO */
}

static void dump_long_attribute(grib_dumper* d, grib_accessor* a, const char* prefix)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    long value=0; size_t size = 0;
    int err = 0;
    long count=0;
    grib_context* c=a->context;

    grib_value_count(a,&count);
    size=count;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    if (size <= 1) {
        err=grib_unpack_long(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {
        fprintf(self->dumper.out,"\n");
        fprintf(self->dumper.out,"  free(iValues);\n");
        fprintf(self->dumper.out,"  iValues = (long*)malloc(%lu*sizeof(long));\n", (unsigned long)size);
        fprintf(self->dumper.out,"  if (!iValues) { fprintf(stderr, \"Failed to allocate memory (iValues).\\n\"); return 1; }\n");
        fprintf(self->dumper.out,"  size = %lu;\n", size);

        depth-=2;

        fprintf(self->dumper.out,"  CODES_CHECK(codes_get_long_array(h, \"%s->%s\", iValues, &size), 0);\n", prefix,a->name);

    } else {
        /* int r=compute_key_rank(h,self->keys,a->name); */
        if( !grib_is_missing_long(a,value) ) {
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_long(h, \"%s->%s\", &iVal), 0);\n", prefix,a->name);
        }
    }

    if (self->isLeaf==0) {
        char* prefix1;

        prefix1=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+strlen(prefix)+5));
        sprintf(prefix1,"%s->%s",prefix,a->name);

        dump_attributes(d,a,prefix1);

        grib_context_free(c,prefix1);
        depth-=2;
    }
    (void)err; /* TODO */
}

static void dump_bits(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_double(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    double value=0; size_t size = 1;
    int r;
    char* sval;
    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=h->context;

    grib_unpack_double(a,&value,&size);
    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    self->empty=0;

    r=compute_key_rank(h,self->keys,a->name);
    if( !grib_is_missing_double(a,value) ) {
        sval=dval_to_string(c,value);
        if (r!=0)
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double(h, \"#%d#%s\", &dVal), 0);\n", r,a->name);
        else
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_double(h, \"%s\", &dVal), 0);\n", a->name);

        grib_context_free(c,sval);
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
            dofree=1;
            sprintf(prefix,"#%d#%s",r,a->name);
        } else prefix=(char*)a->name;

        dump_attributes(d,a,prefix);
        if (dofree) grib_context_free(c,prefix);
        depth-=2;
    }
}

static void dump_string_array(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    char **values;
    size_t size = 0;
    grib_context* c=NULL;
    int err = 0;
    long count=0;
    int r;
    grib_handle* h=grib_handle_of_accessor(a);

    c=a->context;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a,&count);
    size=count;
    if (size==1) {
        dump_string(d,a,comment);
        return;
    }

    fprintf(self->dumper.out,"\n");
    fprintf(self->dumper.out,"  free(sValues);\n");
    fprintf(self->dumper.out,"  sValues = (char**)malloc(%lu * sizeof(char*));\n", (unsigned long)size);
    fprintf(self->dumper.out,"  if (!sValues) { fprintf(stderr, \"Failed to allocate memory (sValues).\\n\"); return 1; }\n");
    fprintf(self->dumper.out,"  size = %lu;\n", size);

    self->empty=0;
    values=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    if (!values) {
        grib_context_log(c,GRIB_LOG_FATAL,"unable to allocate %d bytes",(int)size);
        return;
    }

    err = grib_unpack_string_array(a,values,&size);

    if (self->isLeaf==0) {
        if ((r=compute_key_rank(h,self->keys,a->name))!=0)
            fprintf(self->dumper.out,"  codes_get_string_array(h, \"#%d#%s\", sValues, &size);\n",r,a->name);
        else
            fprintf(self->dumper.out,"  codes_get_string_array(h, \"%s\", sValues, &size);\n",a->name);
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
            dofree=1;
            sprintf(prefix,"#%d#%s",r,a->name);
        } else prefix=(char*)a->name;

        dump_attributes(d,a,prefix);
        if (dofree) grib_context_free(c,prefix);
        depth-=2;
    }

    grib_context_free(c,values);
    (void)err; /* TODO */
}

static void dump_string(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    char *value=NULL;
    char *p = NULL;
    size_t size = 0;
    grib_context* c=NULL;
    int r;
    int err = _grib_get_string_length(a,&size);
    grib_handle* h=grib_handle_of_accessor(a);

    c=a->context;
    if (size==0) return;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    value=(char*)grib_context_malloc_clear(c,size);
    if (!value) {
        grib_context_log(c,GRIB_LOG_FATAL,"unable to allocate %d bytes",(int)size);
        return;
    }

    self->empty=0;

    err = grib_unpack_string(a,value,&size);
    p=value;
    r=compute_key_rank(h,self->keys,a->name);
    if (grib_is_missing_string(a,(unsigned char *)value,size))
        return;

    while(*p) { if(!isprint(*p)) *p = '.'; p++; }

    fprintf(self->dumper.out,"  size = 1024;\n"); /* TODO */
    if (self->isLeaf==0) {
        depth+=2;
        if (r!=0)
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_string(h, \"#%d#%s\", sVal, &size), 0);\n", r, a->name);
        else
            fprintf(self->dumper.out,"  CODES_CHECK(codes_get_string(h, \"%s\", sVal, &size), 0);\n", a->name);
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
            dofree=1;
            sprintf(prefix,"#%d#%s",r,a->name);
        } else prefix=(char*)a->name;

        dump_attributes(d,a,prefix);
        if (dofree) grib_context_free(c,prefix);
        depth-=2;
    }

    grib_context_free(c,value);
    (void)err; /* TODO */
}

static void dump_bytes(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_label(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void _dump_long_array(grib_handle* h, FILE* f, const char* key, const char* print_key)
{
    size_t size=0;
    if (grib_get_size(h,key,&size)==GRIB_NOT_FOUND) return;

    fprintf(f,"  free(iValues);\n");
    fprintf(f,"  iValues = (long*)malloc(%lu*sizeof(long));\n", (unsigned long)size);
    fprintf(f,"  if (!iValues) { fprintf(stderr, \"Failed to allocate memory (iValues).\\n\"); return 1; }\n");
    fprintf(f,"  size = %lu;", size);

    fprintf(f,"  CODES_CHECK(codes_get_long_array(h, \"%s\", iValues, &size), 0);\n",print_key);
}

static void dump_section(grib_dumper* d, grib_accessor* a, grib_block_of_accessors* block)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    if (!grib_inline_strcmp(a->name,"BUFR") ||
            !grib_inline_strcmp(a->name,"GRIB") ||
            !grib_inline_strcmp(a->name,"META")
    ) {
        grib_handle* h=grib_handle_of_accessor(a);
        depth=2;
        self->empty=1;
        depth+=2;
        _dump_long_array(h,self->dumper.out,"dataPresentIndicator","inputDataPresentIndicator");
        _dump_long_array(h,self->dumper.out,"delayedDescriptorReplicationFactor","inputDelayedDescriptorReplicationFactor");
        _dump_long_array(h,self->dumper.out,"shortDelayedDescriptorReplicationFactor","inputShortDelayedDescriptorReplicationFactor");
        _dump_long_array(h,self->dumper.out,"extendedDelayedDescriptorReplicationFactor","inputExtendedDelayedDescriptorReplicationFactor");
        grib_dump_accessors_block(d,block);
        depth-=2;
    } else if (!grib_inline_strcmp(a->name,"groupNumber")) {
        if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
            return;
        self->empty=1;
        depth+=2;
        grib_dump_accessors_block(d,block);
        depth-=2;
    } else {
        grib_dump_accessors_block(d,block);
    }
}

static void dump_attributes(grib_dumper* d,grib_accessor* a, const char* prefix)
{
    int i=0;
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    unsigned long flags;
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes[i]) {
        self->isAttribute=1;
        if (  (d->option_flags & GRIB_DUMP_FLAG_ALL_ATTRIBUTES ) == 0
                && (a->attributes[i]->flags & GRIB_ACCESSOR_FLAG_DUMP)== 0 )
        {
            i++;
            continue;
        }
        self->isLeaf=a->attributes[i]->attributes[0]==NULL ? 1 : 0;
        flags=a->attributes[i]->flags;
        a->attributes[i]->flags |= GRIB_ACCESSOR_FLAG_DUMP;
        switch (grib_accessor_get_native_type(a->attributes[i])) {
        case GRIB_TYPE_LONG:
            dump_long_attribute(d,a->attributes[i],prefix);
            break;
        case GRIB_TYPE_DOUBLE:
            dump_values_attribute(d,a->attributes[i],prefix);
            break;
        case GRIB_TYPE_STRING:
            break;
        }
        a->attributes[i]->flags=flags;
        i++;
    }
    self->isLeaf=0;
    self->isAttribute=0;
}

static void header(grib_dumper* d, grib_handle* h)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;
    Assert(h->product_kind == PRODUCT_BUFR);

    if (d->count<2) {
        /* This is the first message being processed */
        fprintf(self->dumper.out,"/* This program was automatically generated with bufr_dump -DC */\n");
        fprintf(self->dumper.out,"/* Using ecCodes version: ");
        grib_print_api_version(self->dumper.out);
        fprintf(self->dumper.out, " */\n\n");
        fprintf(self->dumper.out,"#include \"eccodes.h\"\n");
        fprintf(self->dumper.out,"int main(int argc, char* argv[])\n");
        fprintf(self->dumper.out,"{\n");
        fprintf(self->dumper.out,"  size_t         size = 0;\n");
        fprintf(self->dumper.out,"  int            err = 0;\n");
        fprintf(self->dumper.out,"  FILE*          fin = NULL;\n");
        fprintf(self->dumper.out,"  codes_handle*  h = NULL;\n");
        fprintf(self->dumper.out,"  long           iVal = 0;\n");
        fprintf(self->dumper.out,"  double         dVal = 0.0;\n");
        fprintf(self->dumper.out,"  char           sVal[1024] = {0,};\n");
        fprintf(self->dumper.out,"  long*          iValues = NULL;\n");
        fprintf(self->dumper.out,"  char**         sValues = NULL;\n");
        fprintf(self->dumper.out,"  double*        dValues = NULL;\n");
        fprintf(self->dumper.out,"  const char*    infile_name = NULL;\n\n");

        fprintf(self->dumper.out,"  if (argc != 2) {\n");
        fprintf(self->dumper.out,"    fprintf(stderr, \"Usage: %%s BUFR_file\\n\", argv[0]);\n");
        fprintf(self->dumper.out,"    return 1;\n");
        fprintf(self->dumper.out,"  }\n");
        fprintf(self->dumper.out,"  infile_name = argv[1];\n");
        fprintf(self->dumper.out,"  fin = fopen(infile_name, \"r\");\n");
        fprintf(self->dumper.out,"  if (!fin) {\n");
        fprintf(self->dumper.out,"    fprintf(stderr,\"ERROR: Unable to open input BUFR file %%s\\n\", infile_name);\n");
        fprintf(self->dumper.out,"    return 1;\n");
        fprintf(self->dumper.out,"  }\n\n");
    }

    fprintf(self->dumper.out,"  h = codes_handle_new_from_file(NULL, fin, PRODUCT_BUFR, &err);\n");
    fprintf(self->dumper.out,"  if (h == NULL) {\n");
    fprintf(self->dumper.out,"    fprintf(stderr, \"ERROR: cannot create BUFR handle\\n\");\n");
    fprintf(self->dumper.out,"    return 1;\n");
    fprintf(self->dumper.out,"  }\n");
    fprintf(self->dumper.out,"  CODES_CHECK(codes_set_long(h, \"unpack\", 1),0);\n\n");
}

static void footer(grib_dumper* d, grib_handle* h)
{
    grib_dumper_bufr_decode_C *self = (grib_dumper_bufr_decode_C*)d;

    /*fprintf(self->dumper.out,"  fout = fopen(\"outfile.bufr\", \"w\");");*/
    /*fprintf(self->dumper.out,"  if (fclose(fout)) {\n");
    fprintf(self->dumper.out,"    fprintf(stderr, \"Failed to close file handle.\\n\");\n");
    fprintf(self->dumper.out,"    return 1;\n");
    fprintf(self->dumper.out,"  }\n");
    */
    fprintf(self->dumper.out,"\n");
    fprintf(self->dumper.out,"  codes_handle_delete(h);\n");
    fprintf(self->dumper.out,"  free(iValues); iValues = NULL;\n");
    fprintf(self->dumper.out,"  free(dValues); dValues = NULL;\n");
    fprintf(self->dumper.out,"  free(sValues); sValues = NULL;\n\n");
}
