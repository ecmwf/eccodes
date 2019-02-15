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
   IMPLEMENTS = dump_double;dump_string
   IMPLEMENTS = dump_bytes;dump_values
   IMPLEMENTS = dump_label;dump_section
   IMPLEMENTS = init;destroy
   MEMBERS = long section_offset
   MEMBERS = long begin
   MEMBERS = long theEnd
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
static void dump_bytes      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_values     (grib_dumper* d, grib_accessor* a);
static void dump_label      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_section    (grib_dumper* d, grib_accessor* a,grib_block_of_accessors* block);

typedef struct grib_dumper_debug {
    grib_dumper          dumper;  
/* Members defined in debug */
	long section_offset;
	long begin;
	long theEnd;
} grib_dumper_debug;


static grib_dumper_class _grib_dumper_class_debug = {
    0,                              /* super                     */
    "debug",                              /* name                      */
    sizeof(grib_dumper_debug),     /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    &init,                               /* init                      */
    &destroy,                            /* free mem                       */
    &dump_long,                          /* dump long         */
    &dump_double,                        /* dump double    */
    &dump_string,                        /* dump string    */
    0,                        /* dump string array   */
    &dump_label,                         /* dump labels  */
    &dump_bytes,                         /* dump bytes  */
    &dump_bits,                          /* dump bits   */
    &dump_section,                       /* dump section      */
    &dump_values,                        /* dump values   */
    0,                             /* header   */
    0,                             /* footer   */
};

grib_dumper_class* grib_dumper_class_debug = &_grib_dumper_class_debug;

/* END_CLASS_IMP */
static void set_begin_end(grib_dumper* d,grib_accessor* a);

static void init_class      (grib_dumper_class* c){}

static int  init(grib_dumper* d)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    self->section_offset=0;

    return GRIB_SUCCESS;
}

static int  destroy  (grib_dumper* d)
{
    return GRIB_SUCCESS;
}

static void aliases(grib_dumper* d,grib_accessor* a)
{
    int i;
    grib_dumper_debug *self = (grib_dumper_debug*)d;

    if(a->all_names[1])
    {
        char *sep = "";
        fprintf(self->dumper.out," [");

        for(i = 1; i < MAX_ACCESSOR_NAMES; i++)
        {
            if(a->all_names[i])
            {
                if(a->all_name_spaces[i])
                    fprintf(self->dumper.out,"%s%s.%s", sep,a->all_name_spaces[i],a->all_names[i]);
                else
                    fprintf(self->dumper.out,"%s%s", sep,a->all_names[i]);
            }
            sep = ", ";
        }
        fprintf(self->dumper.out,"]");
    }
}

static void dump_long(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    long value=0; size_t size = 0;
    long *values=NULL; /* array of long */
    long count = 0;
    int err = 0, i = 0, more = 0;

    grib_value_count(a,&count);size=count;

    if (a->length == 0 && (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
        return;

    if ((a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 &&
            (d->option_flags & GRIB_DUMP_FLAG_READ_ONLY) == 0)
        return;

    if (size>1) {
        values=(long*)grib_context_malloc_clear(a->context,sizeof(long)*size);
        err=grib_unpack_long(a,values,&size);
    } else {
        err=grib_unpack_long(a,&value,&size);
    }

    set_begin_end(d,a);

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");

    if (size>1) {
        fprintf(self->dumper.out,"%ld-%ld %s %s = {\n",self->begin,self->theEnd,a->creator->op,a->name);
        if (values) {
            int k = 0;
            if(size > 100) {
                more = size - 100;
                size = 100;
            }
            while(k < size) {
                int j;
                for(i = 0; i < d->depth + 3 ; i++) fprintf(self->dumper.out," ");
                for(j = 0; j < 8 && k < size; j++, k++) {
                    fprintf(self->dumper.out,"%ld",values[k]);
                    if(k != size-1) fprintf(self->dumper.out,", ");
                }
                fprintf(self->dumper.out,"\n");
            }
            if(more) {
                for(i = 0; i < d->depth + 3 ; i++) fprintf(self->dumper.out," ");
                fprintf(self->dumper.out,"... %d more values\n",more);
            }
            for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
            fprintf(self->dumper.out,"} # %s %s \n",a->creator->op, a->name);
            grib_context_free(a->context,values);
        }
    } else {
        if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && grib_is_missing_internal(a))
            fprintf(self->dumper.out,"%ld-%ld %s %s = MISSING",self->begin,self->theEnd,a->creator->op, a->name);
        else
            fprintf(self->dumper.out,"%ld-%ld %s %s = %ld",self->begin,self->theEnd,a->creator->op, a->name,value);
        if(comment) fprintf(self->dumper.out," [%s]",comment);
    }
    if(err)
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_debug::dump_long]",err,grib_get_error_message(err));

    aliases(d,a);

    fprintf(self->dumper.out,"\n");
}

static int test_bit(long a, long b) {return a&(1<<b);}

static void dump_bits(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    long value=0; size_t size = 1;
    int err = grib_unpack_long(a,&value,&size);
    int i;

    if( a->length == 0  &&
            (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
        return;

    set_begin_end(d,a);

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"%ld-%ld %s %s = %ld [",self->begin,self->theEnd, a->creator->op,a->name,value);

    for(i=0;i<(a->length*8);i++) {
        if(test_bit(value,a->length*8-i-1))
            fprintf(self->dumper.out,"1");
        else
            fprintf(self->dumper.out,"0");
    }

    if(comment)
        fprintf(self->dumper.out,":%s]",comment);
    else
        fprintf(self->dumper.out,"]");

    if(err)
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_debug::dump_bits]",err,grib_get_error_message(err));

    aliases(d,a);
    fprintf(self->dumper.out,"\n");
}

static void dump_double(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    double value=0; size_t size = 1;
    int err = grib_unpack_double(a,&value,&size);
    int i;

    if( a->length == 0  &&
            (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
        return;

    set_begin_end(d,a);

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");

    if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && grib_is_missing_internal(a))
        fprintf(self->dumper.out,"%ld-%ld %s %s = MISSING",self->begin,self->theEnd,a->creator->op, a->name);
    else
        fprintf(self->dumper.out,"%ld-%ld %s %s = %g",self->begin,self->theEnd,a->creator->op, a->name,value);
    if(comment) fprintf(self->dumper.out," [%s]",comment);
    if(err)
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_debug::dump_double]",err,grib_get_error_message(err));
    aliases(d,a);
    fprintf(self->dumper.out,"\n");
}

static void dump_string(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    int err=0;
    int i;
    size_t size=0;
    char *value=NULL;
    char *p=NULL ;

    err = _grib_get_string_length(a,&size);
    if ( (size < 2) && grib_is_missing_internal(a) ) {
        /* GRIB-302: transients and missing keys. Need to re-adjust the size */
        size = 10; /* big enough to hold the string "missing" */
    }

    value=(char*)grib_context_malloc_clear(a->context,size);
    if (!value) return;
    err=grib_unpack_string(a,value,&size);

    if(err) strcpy(value,"<error>");

    p=value;

    if( a->length == 0 && (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
        return;

    set_begin_end(d,a);

    while(*p) { if(!isprint(*p)) *p = '.'; p++; }

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"%ld-%ld %s %s = %s",self->begin,self->theEnd,a->creator->op, a->name,value);

    if(comment) fprintf(self->dumper.out," [%s]",comment);
    if(err)
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_debug::dump_string]",err,grib_get_error_message(err));
    aliases(d,a);
    fprintf(self->dumper.out,"\n");

    grib_context_free(a->context,value);
}

static void dump_bytes(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    int i,k,err =0;
    int more = 0;
    size_t size = a->length;
    unsigned char* buf = (unsigned char*)grib_context_malloc(d->handle->context,size);

    if( a->length == 0  &&
            (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
        return;

    set_begin_end(d,a);

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"%ld-%ld %s %s = %ld",self->begin,self->theEnd,a->creator->op, a->name,a->length);
    aliases(d,a);
    fprintf(self->dumper.out," {");

    if(!buf)
    {
        if(size == 0)
            fprintf(self->dumper.out,"}\n");
        else
            fprintf(self->dumper.out," *** ERR cannot malloc(%ld) }\n",(long)size);
        return;
    }

    fprintf(self->dumper.out,"\n");

    err = grib_unpack_bytes(a,buf,&size);
    if(err){
        grib_context_free(d->handle->context,buf);
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_debug::dump_bytes]\n}",err,grib_get_error_message(err));
        return ;
    }

    if(size > 100) {
        more = size - 100;
        size = 100;
    }

    k = 0;
    /* if(size > 100) size = 100;  */
    while(k < size)
    {
        int j;
        for(i = 0; i < d->depth + 3 ; i++) fprintf(self->dumper.out," ");
        for(j = 0; j < 16 && k < size; j++, k++)
        {
            fprintf(self->dumper.out,"%02x",buf[k]);
            if(k != size-1)
                fprintf(self->dumper.out,", ");
        }
        fprintf(self->dumper.out,"\n");
    }

    if(more)
    {
        for(i = 0; i < d->depth + 3 ; i++) fprintf(self->dumper.out," ");
        fprintf(self->dumper.out,"... %d more values\n",more);
    }

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"} # %s %s \n",a->creator->op, a->name);
    grib_context_free(d->handle->context,buf);
}

static void dump_values(grib_dumper* d,grib_accessor* a)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    int i,k,err =0;
    int more = 0;
    double*  buf = NULL;
    size_t size=0;
    long count=0;

    if( a->length == 0  &&
            (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
        return;

    grib_value_count(a,&count);
    size=count;
    if(size == 1){
        dump_double(d,a,NULL);
        return ;
    }
    buf = (double*)grib_context_malloc_clear(d->handle->context,size * sizeof(double));

    set_begin_end(d,a);

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"%ld-%ld %s %s = (%ld,%ld)",self->begin,self->theEnd,a->creator->op, a->name,(long)size,a->length);
    aliases(d,a);
    fprintf(self->dumper.out," {");

    if(!buf) {
        if(size == 0)
            fprintf(self->dumper.out,"}\n");
        else
            fprintf(self->dumper.out," *** ERR cannot malloc(%ld) }\n",(long)size);
        return;
    }

    fprintf(self->dumper.out,"\n");

    err =  grib_unpack_double(a,buf,&size);
    if(err){
        grib_context_free(d->handle->context,buf);
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_debug::dump_values]\n}",err,grib_get_error_message(err));
        return ;
    }

    if(size > 100) {
        more = size - 100;
        size = 100;
    }

    k = 0;
    while(k < size)
    {
#if 1
        int j;
        for(i = 0; i < d->depth + 3 ; i++) fprintf(self->dumper.out," ");
        for(j = 0; j < 8 && k < size; j++, k++)
        {
            fprintf(self->dumper.out,"%10g",buf[k]);
            if(k != size-1)
                fprintf(self->dumper.out,", ");
        }
        fprintf(self->dumper.out,"\n");
#else

        fprintf(self->dumper.out,"%d %g\n",k,buf[k]);

#endif
    }
    if(more) {
        for(i = 0; i < d->depth + 3 ; i++) fprintf(self->dumper.out," ");
        fprintf(self->dumper.out,"... %d more values\n",more);
    }

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"} # %s %s \n",a->creator->op, a->name);
    grib_context_free(d->handle->context,buf);
}

static void dump_label(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    int i;
    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"----> %s %s %s\n",a->creator->op, a->name,comment?comment:"");
}

static void dump_section(grib_dumper* d,grib_accessor* a,grib_block_of_accessors* block)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    int i;
    /* grib_section* s = grib_get_sub_section(a); */
    grib_section* s = a->sub_section;


#if 1
    if(a->name[0] == '_'){
        grib_dump_accessors_block(d,block);
        return;
    }
#endif
    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"======> %s %s (%ld,%ld,%ld)\n",a->creator->op,
            a->name,a->length,(long)s->length,(long)s->padding);
    if (!strncmp(a->name,"section",7)) self->section_offset=a->offset;
    /*printf("------------- section_offset = %ld\n",self->section_offset);*/
    d->depth += 3;
    grib_dump_accessors_block(d,block);
    d->depth -= 3;

    for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
    fprintf(self->dumper.out,"<===== %s %s\n",a->creator->op, a->name);
}

static void set_begin_end(grib_dumper* d,grib_accessor* a)
{
    grib_dumper_debug *self = (grib_dumper_debug*)d;
    if ((d->option_flags & GRIB_DUMP_FLAG_OCTECT) != 0) {

        self->begin=a->offset-self->section_offset+1;
        self->theEnd=grib_get_next_position_offset(a)-self->section_offset;
    } else {
        self->begin=a->offset;
        self->theEnd=grib_get_next_position_offset(a);
    }
}
