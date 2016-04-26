/*
 * Copyright 2005-2016 ECMWF.
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
static void dump_string_array     (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_bytes      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_values     (grib_dumper* d, grib_accessor* a);
static void dump_label      (grib_dumper* d, grib_accessor* a,const char* comment);
static void dump_section    (grib_dumper* d, grib_accessor* a,grib_block_of_accessors* block);

typedef struct grib_dumper_default {
    grib_dumper          dumper;  
/* Members defined in default */
	long section_offset;
	long begin;
	long theEnd;
} grib_dumper_default;


static grib_dumper_class _grib_dumper_class_default = {
    0,                              /* super                     */
    "default",                              /* name                      */
    sizeof(grib_dumper_default),     /* size                      */
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
    0,                             /* header   */
    0,                             /* footer   */
};

grib_dumper_class* grib_dumper_class_default = &_grib_dumper_class_default;

/* END_CLASS_IMP */

static void print_offset(FILE* out,grib_dumper* d,grib_accessor* a);

static void init_class      (grib_dumper_class* c){}

static int  init(grib_dumper* d)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    self->section_offset=0;

    return GRIB_SUCCESS;
}

static int  destroy  (grib_dumper* d){
    return GRIB_SUCCESS;
}


static void aliases(grib_dumper* d,grib_accessor* a)
{
    int i;
    grib_dumper_default *self = (grib_dumper_default*)d;

    if( (d->option_flags & GRIB_DUMP_FLAG_ALIASES) == 0)
        return;

    if(a->all_names[1])
    {
        char *sep = "";
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# ALIASES: ");

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
        fprintf(self->dumper.out,"\n");
    }
}

static void dump_long(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    long value; size_t size = 1;
    long *values=NULL;
    int err = 0;
    int i;
    long count=0;

    grib_value_count(a,&count);
    size=count;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    print_offset(self->dumper.out,d,a);

    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0){
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# type %s \n",a->creator->op);
    }

    if (size>1) {
        values=(long *)grib_context_malloc_clear(a->context,sizeof(long)*size);
        err=grib_unpack_long(a,values,&size);
    } else {
        err=grib_unpack_long(a,&value,&size);
    }

    aliases(d,a);
    if(comment) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# %s \n",comment);
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"#-READ ONLY- ");
    } else
        fprintf(self->dumper.out,"  ");

    if (size>1) {
        int cols=19;
        int icount=0;
        fprintf(self->dumper.out,"%s = { \t",a->name);
        for (i=0;i<size;i++) {
            if (icount>cols) {fprintf(self->dumper.out,"\n\t\t\t\t");icount=0;}
            fprintf(self->dumper.out,"%ld ",values[i]);
            icount++;
        }
        fprintf(self->dumper.out,"}\n");
        grib_context_free(a->context,values);
    } else {
        if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && grib_is_missing_internal(a) )
            fprintf(self->dumper.out,"%s = MISSING;",a->name);
        else
            fprintf(self->dumper.out,"%s = %ld;",a->name,value);
    }

    if(err) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# *** ERR=%d (%s) [grib_dumper_default::dump_long]",err,grib_get_error_message(err));
    }

    fprintf(self->dumper.out,"\n");
}

static int test_bit(long a, long b) {return a&(1<<b);}

static void dump_bits(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    int i;
    long lvalue;
    double dvalue;
    size_t size = 1;
    int err = 0;
    int isDouble=0;

    switch (grib_accessor_get_native_type(a)) {
    case GRIB_TYPE_LONG:
        grib_unpack_long(a,&lvalue,&size);
        break;
    case GRIB_TYPE_DOUBLE:
        grib_unpack_double(a,&dvalue,&size);
        isDouble=1;
        break;
    default:
        break;
    }

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    print_offset(self->dumper.out,d,a);

    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# type %s \n",a->creator->op);
    }

    aliases(d,a);
    if(comment) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# %s \n",comment);
    }

    fprintf(self->dumper.out,"  ");
    fprintf(self->dumper.out,"# flags: ");
    for(i=0;i<(a->length*8);i++) {
        if(test_bit(lvalue,a->length*8-i-1))
            fprintf(self->dumper.out,"1");
        else
            fprintf(self->dumper.out,"0");
    }
    fprintf(self->dumper.out,"\n");

    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"#-READ ONLY- ");
    } else
        fprintf(self->dumper.out,"  ");

    if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && grib_is_missing_internal(a) )
        fprintf(self->dumper.out,"%s = MISSING;",a->name);
    else {
        if (isDouble)
            fprintf(self->dumper.out,"%s = %g;",a->name,dvalue);
        else
            fprintf(self->dumper.out,"%s = %ld;",a->name,lvalue);
    }


    if(err) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# *** ERR=%d (%s) [grib_dumper_default::dump_bits]",err,grib_get_error_message(err));
    }

    fprintf(self->dumper.out,"\n");
}

static void dump_double(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    double value; size_t size = 1;
    int err = grib_unpack_double(a,&value,&size);


    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    print_offset(self->dumper.out,d,a);

    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# type %s \n",a->creator->op);
    }

    aliases(d,a);
    if(comment) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# %s \n",comment);
    }


    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"#-READ ONLY- ");
    } else
        fprintf(self->dumper.out,"  ");

    if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && grib_is_missing_internal(a) )
        fprintf(self->dumper.out,"%s = MISSING;",a->name);
    else
        fprintf(self->dumper.out,"%s = %g;",a->name,value);


    if(err) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# *** ERR=%d (%s) [grib_dumper_default::dump_double]",err,grib_get_error_message(err));
    }

    fprintf(self->dumper.out,"\n");
}

static void dump_string_array(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    char **values;
    size_t size = 0,i=0;
    grib_context* c=NULL;
    int err = 0;
    int tab=0;
    long count=0;

    c=a->context;

    grib_value_count(a,&count);
    size=count;
    if (size==1) {
        dump_string(d,a,comment);
        return;
    }

    values=(char**)grib_context_malloc_clear(c,size*sizeof(char*));
    if (!values) {
        grib_context_log(c,GRIB_LOG_FATAL,"unable to allocate %d bytes",(int)size);
        return;
    }

    err = grib_unpack_string_array(a,values,&size);

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;


    print_offset(self->dumper.out,d,a);

    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# type %s \n",a->creator->op);
    }

    aliases(d,a);
    if(comment) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# %s \n",comment);
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"#-READ ONLY- ");
        tab=13;
    } else
        fprintf(self->dumper.out,"  ");

    tab++;
    fprintf(self->dumper.out,"%s = {\n",a->name);
    for  (i=0;i<size;i++) {
        fprintf(self->dumper.out,"%-*s\"%s\",\n",(int)(tab+strlen(a->name)+4)," ",values[i]);
    }
    fprintf(self->dumper.out,"  }");

    if(err) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# *** ERR=%d (%s)",err,grib_get_error_message(err));
    }

    fprintf(self->dumper.out,"\n");
    grib_context_free(c,values);
}

static void dump_string(grib_dumper* d,grib_accessor* a,const char* comment)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    char *value=NULL;
    char *p = NULL;
    size_t size = 0;
    grib_context* c=NULL;
    int err = _grib_get_string_length(a,&size);

    c=a->context;
    if (size==0) return;

    value=(char*)grib_context_malloc_clear(c,size);
    if (!value) {
        grib_context_log(c,GRIB_LOG_FATAL,"unable to allocate %d bytes",(int)size);
        return;
    }

    err = grib_unpack_string(a,value,&size);
    p=value;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    while(*p) { if(!isprint(*p)) *p = '.'; p++; }

    print_offset(self->dumper.out,d,a);

    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# type %s \n",a->creator->op);
    }

    aliases(d,a);
    if(comment) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# %s \n",comment);
    }

    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"#-READ ONLY- ");
    } else
        fprintf(self->dumper.out,"  ");

    if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && grib_is_missing_internal(a) )
        fprintf(self->dumper.out,"%s = MISSING;",a->name);
    else
        fprintf(self->dumper.out,"%s = %s;",a->name,value);


    if(err) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# *** ERR=%d (%s) [grib_dumper_default::dump_string]",err,grib_get_error_message(err));
    }

    fprintf(self->dumper.out,"\n");
    grib_context_free(c,value);
}

static void dump_bytes(grib_dumper* d,grib_accessor* a,const char* comment)
{

#if 0
    grib_dumper_default *self = (grib_dumper_default*)d;
    int i,k,err =0;
    int more = 0;
    size_t size = a->length;
    unsigned char* buf = grib_context_malloc(d->handle->context,size);

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;


    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
        fprintf(self->dumper.out,"-READ ONLY- ");

    /*for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");*/
    /*print_offset(self->dumper.out,self->begin,self->theEnd);*/
    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0)
        fprintf(self->dumper.out,"%s ",a->creator->op);

    fprintf(self->dumper.out,"%s = %ld",a->name,a->length);
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
        fprintf(self->dumper.out," *** ERR=%d (%s)  [grib_dumper_default::dump_bytes]\n}",err,grib_get_error_message(err));
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
#endif
}

static void dump_values(grib_dumper* d,grib_accessor* a)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    int k,err =0;
    int more = 0;
    double*  buf = NULL;
    size_t size=0;
    long count=0;

    if ( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
        return;

    grib_value_count(a,&count);
    size=count;
    if(size == 1){
        dump_double(d,a,NULL);
        return ;
    }
    buf = (double*)grib_context_malloc(d->handle->context,size * sizeof(double));

    print_offset(self->dumper.out,d,a);

    if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"# type %s \n",a->creator->op);
    }

    aliases(d,a);

    if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"#-READ ONLY- ");
    } else
        fprintf(self->dumper.out,"  ");

    fprintf(self->dumper.out,"%s(%ld) = ",a->name,(long)size);
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

    err =  grib_unpack_double(a,buf,&size);

    if(err){
        grib_context_free(d->handle->context,buf);
        fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_default::dump_values]\n}",err,grib_get_error_message(err));
        return ;
    }

    if(!(d->option_flags & GRIB_DUMP_FLAG_ALL_DATA) && size > 100) {
        more = size - 100;
        size = 100;
    }

    k = 0;
    while(k < size)  {
#if 1
        int j;
        fprintf(self->dumper.out,"  ");
        for(j = 0; j < 5 && k < size; j++, k++)  {
            fprintf(self->dumper.out,"%g",buf[k]);
            if(k != size-1)
                fprintf(self->dumper.out,", ");
        }
        fprintf(self->dumper.out,"\n");
#else

        fprintf(self->dumper.out,"%d %g\n",k,buf[k]);

#endif

    }  if(more)  {
        fprintf(self->dumper.out,"  ");
        fprintf(self->dumper.out,"... %d more values\n",more);
    }

    fprintf(self->dumper.out,"  ");
    fprintf(self->dumper.out,"} \n");
    grib_context_free(d->handle->context,buf);
}

static void dump_label(grib_dumper* d,grib_accessor* a,const char* comment)
{
    /*grib_dumper_default *self = (grib_dumper_default*)d;

  for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
  fprintf(self->dumper.out,"----> %s %s %s\n",a->creator->op, a->name,comment?comment:"");*/
}

static void dump_section(grib_dumper* d,grib_accessor* a,grib_block_of_accessors* block)
{
    grib_dumper_default *self = (grib_dumper_default*)d;
    grib_section* s = a->sub_section;
    int is_default_section=0;
    char* upper=NULL;
    char tmp[512];
    char *p=NULL,*q=NULL;
    if (!strncmp(a->name,"section",7)) is_default_section=1;
    if (!strcmp(a->creator->op,"bufr_group")) { dump_long(d,a,NULL); }

    /*for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");*/
    if (is_default_section) {
        upper=(char*)malloc(strlen(a->name)+1);
        Assert(upper);
        p=(char*)a->name;
        q=upper;
        while (*p != '\0') {
            *q=toupper(*p);
            q++;
            p++;
        }
        *q='\0';

        sprintf(tmp,"%s ( length=%ld, padding=%ld )",upper,(long)s->length,(long)s->padding);
        /*
    fprintf(self->dumper.out,"#==============   %-38s   ==============\n",tmp);
         */
        free(upper);
        self->section_offset=a->offset;
    }

    /*printf("------------- section_offset = %ld\n",self->section_offset);*/
    d->depth += 3;
    grib_dump_accessors_block(d,block);
    d->depth -= 3;

    /*for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");*/
    /*fprintf(self->dumper.out,"<===== %s %s\n",a->creator->op, a->name);*/
}

static void print_offset(FILE* out,grib_dumper* d,grib_accessor* a) {
    int i,k;
    long offset;
    long theBegin=0,theEnd=0;
    size_t size=0,more=0;
    grib_dumper_default *self = (grib_dumper_default*)d;
    grib_handle* h=grib_handle_of_accessor(a);

    theBegin=a->offset-self->section_offset+1;;
    theEnd =grib_get_next_position_offset(a)-self->section_offset;

    if ((d->option_flags & GRIB_DUMP_FLAG_HEXADECIMAL) != 0 && a->length != 0) {
        if (theBegin == theEnd) {
            fprintf(self->dumper.out,"  ");
            fprintf(out,"# Octet: ");
            fprintf(out,"%ld" ,theBegin);
        }
        else {
            fprintf(self->dumper.out,"  ");
            fprintf(out,"# Octets: ");
            fprintf(out,"%ld-%ld" ,theBegin,theEnd);
        }
        fprintf(out,"  = ");
        size=a->length;

        if(!(d->option_flags & GRIB_DUMP_FLAG_ALL_DATA) && size > 112) {
            more = size - 112;
            size = 112;
        }

        k = 0;
        while(k < size)  {
            offset=a->offset;
            for (i=0;i<14 && k<size;i++,k++) {
                fprintf(out," 0x%.2X",h->buffer->data[offset]);
                offset++;
            }
            if (k<size) fprintf(self->dumper.out,"\n  #");
        }  if(more)  {
            fprintf(self->dumper.out,"\n  #... %d more values\n",(int)more);
        }
        fprintf(self->dumper.out,"\n");

    }
}
