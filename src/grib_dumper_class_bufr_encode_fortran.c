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

typedef struct grib_dumper_bufr_encode_fortran {
    grib_dumper          dumper;  
/* Members defined in bufr_encode_fortran */
	long section_offset;
	long empty;
	long end;
	long isLeaf;
	long isAttribute;
	grib_string_list* keys;
} grib_dumper_bufr_encode_fortran;


static grib_dumper_class _grib_dumper_class_bufr_encode_fortran = {
    0,                              /* super                     */
    "bufr_encode_fortran",                              /* name                      */
    sizeof(grib_dumper_bufr_encode_fortran),     /* size                      */
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

grib_dumper_class* grib_dumper_class_bufr_encode_fortran = &_grib_dumper_class_bufr_encode_fortran;

/* END_CLASS_IMP */
static void dump_attributes(grib_dumper* d, grib_accessor* a, const char* prefix);

/* Note: A fast cut-down version of strcmp which does NOT return -1 */
/* 0 means input strings are equal and 1 means not equal */
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    grib_context* c=d->handle->context;
    self->section_offset=0;
    self->empty=1;
    d->count=1;
    self->isLeaf=0;
    self->isAttribute=0;
    self->keys=(grib_string_list*)grib_context_malloc_clear(c,sizeof(grib_string_list));

    return GRIB_SUCCESS;
}

static int destroy(grib_dumper* d)
{
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
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

static char* lval_to_string(grib_context* c, long v)
{
    char* sval=(char*)grib_context_malloc_clear(c,sizeof(char)*40);
    if (v == GRIB_MISSING_LONG) sprintf(sval,"CODES_MISSING_LONG");
    else                        sprintf(sval,"%ld",v);
    return sval;
}
static char* dval_to_string(grib_context* c, double v)
{
    char* sval=(char*)grib_context_malloc_clear(c,sizeof(char)*40);
    if (v == GRIB_MISSING_DOUBLE) {
        sprintf(sval,"CODES_MISSING_DOUBLE");
    } else {
        char* p;
        sprintf(sval,"%.18e",v);
        p=sval;
        while (*p !=0 ) {
            if (*p == 'e') *p='d';
            p++;
        }
    }
    return sval;
}

/* Some lines can grow longer than Fortran compilers allow (=132). */
/* This is mainly due to long key names with attributes. */
/* The resturn value of this function must be freed by the caller */
static char* break_line(grib_context* c, const char* input)
{
    /* Break a long line using Fortran continuation characters */
    char* result=NULL;
    char* a_token = NULL;
    int first = 1;
    const size_t len = strlen(input);
    /* Add a bit more for inserted newlines and continuation characters */
    result = (char*)grib_context_malloc_clear(c,sizeof(char)*len+100);

    /* No need to alter input which is already too short or has newlines */
    if (len < 70 || strchr(input, '\n')) {
        strcpy(result, input);
        return result;
    }

    /* A Fortran multi-line string has two ampersands. E.g. */
    /* 'hello &
     * &world'  is the same as 'hello world'
     */
    a_token = strtok((char*)input, "->");
    while (a_token) {
        if (first) {
            first = 0;
            strcat(result, a_token);
        } else {
            char tmp[256] = {0,};
            sprintf(tmp, "->&\n    &%s", a_token);
            strcat(result, tmp);
        }
        a_token = strtok(NULL, "->");
    }

    return result;
}

static void dump_values(grib_dumper* d, grib_accessor* a)
{
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    double value=0; size_t size = 0;
    double *values=NULL;
    int err = 0;
    int i,r,icount;
    int cols=2;
    long count=0;
    char* sval;
    grib_context* c=a->context;
    grib_handle* h=grib_handle_of_accessor(a);

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) !=0)
        return;

    grib_value_count(a,&count);
    size=count;

    if (size>1) {
        values=(double*)grib_context_malloc_clear(c,sizeof(double)*size);
        err=grib_unpack_double(a,values,&size);
    } else {
        err=grib_unpack_double(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {

        fprintf(self->dumper.out,"  if(allocated(rvalues)) deallocate(rvalues)\n");
        fprintf(self->dumper.out,"  allocate(rvalues(%lu))\n", (unsigned long)size);

        fprintf(self->dumper.out,"  rvalues=(/");

        icount=0;
        for (i=0; i<size-1; ++i) {
            if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
            sval=dval_to_string(c,values[i]);
            fprintf(self->dumper.out,"%s, ", sval);
            grib_context_free(c,sval);
            icount++;
        }
        if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
        sval=dval_to_string(c,values[i]);
        fprintf(self->dumper.out,"%s", sval);
        grib_context_free(c,sval);

        depth-=2;
        fprintf(self->dumper.out,"/)\n");
        grib_context_free(c,values);

        if ((r=compute_bufr_key_rank(h,self->keys,a->name))!=0)
            fprintf(self->dumper.out,"  call codes_set(ibufr,'#%d#%s',rvalues)\n",r,a->name);
        else
            fprintf(self->dumper.out,"  call codes_set(ibufr,'%s',rvalues)\n",a->name);
    } else {
        r=compute_bufr_key_rank(h,self->keys,a->name);
        sval=dval_to_string(c,value);
        if (r!=0)
            fprintf(self->dumper.out,"  call codes_set(ibufr,'#%d#%s',%s)\n",r,a->name,sval);
        else
            fprintf(self->dumper.out,"  call codes_set(ibufr,'%s',%s)\n",a->name,sval);
        grib_context_free(c,sval);
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    double value=0; size_t size = 0;
    double *values=NULL;
    int err = 0;
    int i,icount;
    int cols=2;
    long count=0;
    char* sval;
    grib_context* c=a->context;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) !=0)
        return;

    grib_value_count(a,&count);
    size=count;

    if (size>1) {
        values=(double*)grib_context_malloc_clear(c,sizeof(double)*size);
        err=grib_unpack_double(a,values,&size);
    } else {
        err=grib_unpack_double(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {

        fprintf(self->dumper.out,"  if(allocated(rvalues)) deallocate(rvalues)\n");
        fprintf(self->dumper.out,"  allocate(rvalues(%lu))\n", (unsigned long)size);

        fprintf(self->dumper.out,"  rvalues=(/");

        icount=0;
        for (i=0; i<size-1; ++i) {
            if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
            sval=dval_to_string(c,values[i]);
            fprintf(self->dumper.out,"%s, ", sval);
            grib_context_free(c,sval);
            icount++;
        }
        if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
        sval=dval_to_string(c,values[i]);
        fprintf(self->dumper.out,"%s", sval);
        grib_context_free(c,sval);

        depth-=2;
        fprintf(self->dumper.out,"/)\n");
        grib_context_free(c,values);

        fprintf(self->dumper.out,"  call codes_set(ibufr,'%s->%s' &\n,rvalues)\n",prefix,a->name);
    } else {
        sval=dval_to_string(c,value);
        fprintf(self->dumper.out,"  call codes_set(ibufr,'%s->%s' &\n,%s)\n",prefix,a->name,sval);
        grib_context_free(c,sval);
    }

    if (self->isLeaf==0) {
        char* prefix1;

        prefix1=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+strlen(prefix)+5));
        sprintf(prefix1,"%s->%s",prefix,a->name);

        dump_attributes(d,a,prefix1);

        grib_context_free(c,prefix1);
        depth-=2;
    }

    (void)err; /* TODO */
}

static void dump_long(grib_dumper* d,grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    long value=0; size_t size = 0;
    long *values=NULL;
    int err = 0;
    int i,r,icount;
    int cols=4;
    long count=0;
    char* sval = NULL;
    grib_context* c=a->context;
    grib_handle* h=grib_handle_of_accessor(a);

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0  ) return;

    grib_value_count(a,&count);
    size=count;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0) {
        if (self->isLeaf==0) {
            char* prefix;
            int dofree=0;

            r=compute_bufr_key_rank(h,self->keys,a->name);
            if (r!=0) {
                prefix=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
                dofree=1;
                sprintf(prefix,"#%d#%s",r,a->name);
            } else prefix=(char*)a->name;

            dump_attributes(d,a,prefix);
            if (dofree) grib_context_free(c,prefix);
            depth-=2;
        }
        return;
    }

    if (size>1) {
        values=(long*)grib_context_malloc_clear(a->context,sizeof(long)*size);
        err=grib_unpack_long(a,values,&size);
    } else {
        err=grib_unpack_long(a,&value,&size);
    }

    self->empty=0;

    if (size>1) {
        fprintf(self->dumper.out,"  if(allocated(ivalues)) deallocate(ivalues)\n");
        fprintf(self->dumper.out,"  allocate(ivalues(%lu))\n", (unsigned long)size);

        fprintf(self->dumper.out,"  ivalues=(/");
        icount=0;
        for (i=0;i<size-1;i++) {
            if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
            fprintf(self->dumper.out,"%ld, ",values[i]);
            icount++;
        }
        if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
        fprintf(self->dumper.out,"%ld ",values[i]);

        depth-=2;
        fprintf(self->dumper.out,"/)\n");
        grib_context_free(a->context,values);

        if ((r=compute_bufr_key_rank(h,self->keys,a->name))!=0)
            fprintf(self->dumper.out,"  call codes_set(ibufr,'#%d#%s',ivalues)\n",r,a->name);
        else
            fprintf(self->dumper.out,"  call codes_set(ibufr,'%s',ivalues)\n",a->name);

    } else {
        int doing_unexpandedDescriptors=0;
        r=compute_bufr_key_rank(h,self->keys,a->name);
        sval=lval_to_string(c,value);
        if (r!=0) {
            fprintf(self->dumper.out,"  call codes_set(ibufr,'#%d#%s',",r,a->name);
        } else {
            if (strcmp(a->name, "unexpandedDescriptors")==0) {
                doing_unexpandedDescriptors=1;
                fprintf(self->dumper.out,"\n  ! Create the structure of the data section\n");
            }
            fprintf(self->dumper.out,"  call codes_set(ibufr,'%s',",a->name);
        }

        fprintf(self->dumper.out,"%s)\n",sval);
        grib_context_free(c,sval);
        if (doing_unexpandedDescriptors)
            fprintf(self->dumper.out,"\n");
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    long value=0; size_t size = 0;
    long *values=NULL;
    int err = 0;
    int i,icount;
    int cols=4;
    long count=0;
    char* pref = NULL;
    grib_context* c=a->context;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_value_count(a,&count);
    size=count;

    if (size>1) {
        values=(long*)grib_context_malloc_clear(a->context,sizeof(long)*size);
        err=grib_unpack_long(a,values,&size);
    } else {
        err=grib_unpack_long(a,&value,&size);
    }

    self->empty=0;

    /* Fortran standard specifies the maximum length of a free-form source line is 132 characters */
    /* Break long prefix string into multiple lines to avoid compiler error */
    pref = break_line(c, prefix);

    if (size>1) {
        fprintf(self->dumper.out,"  if(allocated(ivalues)) deallocate(ivalues)\n");
        fprintf(self->dumper.out,"  allocate(ivalues(%lu))\n", (unsigned long)size);

        fprintf(self->dumper.out,"  ivalues=(/");
        icount=0;
        for (i=0;i<size-1;i++) {
            if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
            fprintf(self->dumper.out,"%ld, ",values[i]);
            icount++;
        }
        if (icount>cols || i==0) {fprintf(self->dumper.out,"  &\n      ");icount=0;}
        fprintf(self->dumper.out,"%ld ",values[i]);

        depth-=2;
        fprintf(self->dumper.out,"/)\n");
        grib_context_free(a->context,values);

        fprintf(self->dumper.out,"  call codes_set(ibufr,'%s->%s' &\n,ivalues)\n",pref,a->name);

    } else {
        char* sval=lval_to_string(c,value);
        fprintf(self->dumper.out,"  call codes_set(ibufr,'%s->%s'&\n,",pref,a->name);
        fprintf(self->dumper.out,"%s)\n",sval);
        grib_context_free(c,sval);
    }

    if (self->isLeaf==0) {
        char* prefix1;

        prefix1=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+strlen(pref)+5));
        sprintf(prefix1,"%s->%s",pref,a->name);

        dump_attributes(d,a,prefix1);

        grib_context_free(c,prefix1);
        depth-=2;
    }
    grib_context_free(c,pref);
    (void)err; /* TODO */
}

static void dump_bits(grib_dumper* d, grib_accessor* a, const char* comment)
{
}

static void dump_double(grib_dumper* d, grib_accessor* a, const char* comment)
{
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    double value=0; size_t size = 1;
    int r;
    char* sval;
    grib_handle* h=grib_handle_of_accessor(a);
    grib_context* c=h->context;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 || (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0)
        return;

    grib_unpack_double(a,&value,&size);
    self->empty=0;

    r=compute_bufr_key_rank(h,self->keys,a->name);

    sval=dval_to_string(c,value);
    if (r!=0)
        fprintf(self->dumper.out,"  call codes_set(ibufr,'#%d#%s',%s)\n",r,a->name,sval);
    else
        fprintf(self->dumper.out,"  call codes_set(ibufr,'%s',%s)\n",a->name,sval);
    grib_context_free(c,sval);

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    char **values;
    size_t size = 0,i=0;
    grib_context* c=NULL;
    int err = 0;
    long count=0;
    int r = 0;
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

    fprintf(self->dumper.out,"  if(allocated(svalues)) deallocate(svalues)\n");
    fprintf(self->dumper.out,"  allocate(svalues(%lu))\n", (unsigned long)size);

    fprintf(self->dumper.out,"  svalues=(/");

    self->empty=0;

    values=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    if (!values) {
        grib_context_log(c,GRIB_LOG_FATAL,"unable to allocate %d bytes",(int)size);
        return;
    }

    err = grib_unpack_string_array(a,values,&size);

    for  (i=0;i<size-1;i++) {
        fprintf(self->dumper.out,"    \"%s\", &\n",values[i]);
    }
    fprintf(self->dumper.out,"    \"%s\" /)\n",values[i]);

    if (self->isLeaf==0) {
        if ((r=compute_bufr_key_rank(h,self->keys,a->name))!=0)
            fprintf(self->dumper.out,"  call codes_set_string_array(ibufr,'#%d#%s',svalues)\n",r,a->name);
        else
            fprintf(self->dumper.out,"  call codes_set_string_array(ibufr,'%s',svalues)\n",a->name);
    }

    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
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
    r=compute_bufr_key_rank(h,self->keys,a->name);
    if (grib_is_missing_string(a,(unsigned char *)value,size)) {
        strcpy(value, ""); /* Empty string means MISSING string */
    }

    while(*p) { if(!isprint(*p)) *p = '.'; p++; }

    if (self->isLeaf==0) {
        depth+=2;
        if (r!=0)
            fprintf(self->dumper.out,"  call codes_set(ibufr,'#%d#%s',",r,a->name);
        else
            fprintf(self->dumper.out,"  call codes_set(ibufr,'%s',",a->name);
    }
    fprintf(self->dumper.out,"\'%s\')\n",value);


    if (self->isLeaf==0) {
        char* prefix;
        int dofree=0;

        if (r!=0) {
            prefix=(char*)grib_context_malloc_clear(c,sizeof(char)*(strlen(a->name)+10));
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
    long* val;
    size_t size=0,i;
    int cols=9,icount=0;

    if (grib_get_size(h,key,&size)==GRIB_NOT_FOUND) return;
    if (size==0) return;

    fprintf(f,"  if(allocated(ivalues)) deallocate(ivalues)\n");
    fprintf(f,"  allocate(ivalues(%lu))\n", (unsigned long)size);

    fprintf(f,"  ivalues=(/ ");

    val=(long*)grib_context_malloc_clear(h->context,sizeof(long)*size);
    grib_get_long_array(h,key,val,&size);
    for (i=0;i<size-1;i++) {
        if (icount>cols || i==0) {fprintf(f,"  &\n      ");icount=0;}
        fprintf(f,"%ld, ",val[i]);
        icount++;
    }
    if (icount>cols) {fprintf(f,"  &\n      ");}
    fprintf(f,"%ld /)\n",val[size-1]);

    grib_context_free(h->context,val);
    fprintf(f,"  call codes_set(ibufr,'%s',ivalues)\n",print_key);
}

static void dump_section(grib_dumper* d, grib_accessor* a, grib_block_of_accessors* block)
{
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
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
        _dump_long_array(h,self->dumper.out,"inputOverriddenReferenceValues","inputOverriddenReferenceValues");
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
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
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    char sampleName[200]={0};
    long localSectionPresent,edition,bufrHeaderCentre,isSatellite;

    grib_get_long(h,"localSectionPresent",&localSectionPresent);
    grib_get_long(h,"bufrHeaderCentre",&bufrHeaderCentre);
    grib_get_long(h,"edition",&edition);

    if (localSectionPresent && bufrHeaderCentre==98 ) {
        grib_get_long(h,"isSatellite",&isSatellite);
        if (isSatellite)
            sprintf(sampleName,"BUFR%ld_local_satellite",edition);
        else
            sprintf(sampleName,"BUFR%ld_local",edition);
    } else {
        sprintf(sampleName,"BUFR%ld",edition);
    }

    if (d->count<2) {
        fprintf(self->dumper.out,"!  This program was automatically generated with bufr_dump -Efortran\n");
        fprintf(self->dumper.out,"!  Using ecCodes version: ");
        grib_print_api_version(self->dumper.out);
        fprintf(self->dumper.out, "\n\n");
        fprintf(self->dumper.out,"program bufr_encode\n");
        fprintf(self->dumper.out,"  use eccodes\n");
        fprintf(self->dumper.out,"  implicit none\n");
        fprintf(self->dumper.out,"  integer                                       :: iret\n");
        fprintf(self->dumper.out,"  integer                                       :: outfile\n");
        fprintf(self->dumper.out,"  integer                                       :: ibufr\n");
        fprintf(self->dumper.out,"  integer(kind=4), dimension(:), allocatable    :: ivalues\n");
        fprintf(self->dumper.out,"  integer, parameter  :: max_strsize = 100\n");
        fprintf(self->dumper.out,"  character(len=max_strsize) , dimension(:),allocatable   :: svalues\n");
        fprintf(self->dumper.out,"  real(kind=8), dimension(:), allocatable       :: rvalues\n");
    }
    fprintf(self->dumper.out,"  call codes_bufr_new_from_samples(ibufr,'%s',iret)\n",sampleName);
    fprintf(self->dumper.out,"  if (iret/=CODES_SUCCESS) then\n");
    fprintf(self->dumper.out,"    print *,'ERROR creating BUFR from %s'\n",sampleName);
    fprintf(self->dumper.out,"    stop 1\n");
    fprintf(self->dumper.out,"  endif\n");
}

static void footer(grib_dumper* d, grib_handle* h)
{
    grib_dumper_bufr_encode_fortran *self = (grib_dumper_bufr_encode_fortran*)d;
    fprintf(self->dumper.out,"\n  ! Encode the keys back in the data section\n");
    fprintf(self->dumper.out,"  call codes_set(ibufr,'pack',1)\n\n");
    if (d->count==1)
        fprintf(self->dumper.out,"  call codes_open_file(outfile,'outfile.bufr','w')\n");
    else
        fprintf(self->dumper.out,"  call codes_open_file(outfile,'outfile.bufr','a')\n");

    fprintf(self->dumper.out,"  call codes_write(ibufr,outfile)\n");
    fprintf(self->dumper.out,"  call codes_close_file(outfile)\n");
    fprintf(self->dumper.out,"  call codes_release(ibufr)\n");
    fprintf(self->dumper.out,"  print *, \"Created output BUFR file 'outfile.bufr'\"\n");
    fprintf(self->dumper.out,"  if(allocated(ivalues)) deallocate(ivalues)\n");
    fprintf(self->dumper.out,"  if(allocated(rvalues)) deallocate(rvalues)\n");
    fprintf(self->dumper.out,"  if(allocated(svalues)) deallocate(svalues)\n");
}
