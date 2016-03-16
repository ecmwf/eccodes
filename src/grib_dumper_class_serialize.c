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
   IMPLEMENTS = dump_double;dump_string
   IMPLEMENTS = dump_bytes;dump_values
   IMPLEMENTS = dump_label;dump_section
   IMPLEMENTS = init;destroy
   MEMBERS    =  char* format
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

typedef struct grib_dumper_serialize {
    grib_dumper          dumper;  
/* Members defined in serialize */
	char* format;
} grib_dumper_serialize;


static grib_dumper_class _grib_dumper_class_serialize = {
    0,                              /* super                     */
    "serialize",                              /* name                      */
    sizeof(grib_dumper_serialize),     /* size                      */
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

grib_dumper_class* grib_dumper_class_serialize = &_grib_dumper_class_serialize;

/* END_CLASS_IMP */
static void init_class      (grib_dumper_class* c){}

static int  init(grib_dumper* d)
{
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  self->format = (char*)d->arg;
  return GRIB_SUCCESS;
}

static int  destroy  (grib_dumper* d){
  return GRIB_SUCCESS;
}

static void dump_long(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  long value=0; size_t size = 1;
  int err = grib_unpack_long(a,&value,&size);

  if(  (a->flags & GRIB_ACCESSOR_FLAG_HIDDEN) != 0 )
    return;

  if( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 &&
      (d->option_flags & GRIB_DUMP_FLAG_READ_ONLY) == 0 &&
      (strcmp(a->cclass->name,"lookup") != 0) )
    return;

  if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && (value == GRIB_MISSING_LONG))
    fprintf(self->dumper.out,"%s = MISSING", a->name);
  else
    fprintf(self->dumper.out,"%s = %ld", a->name,value);

  if ( ((a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0) &&
      (strcmp(a->cclass->name,"lookup") != 0) )
    fprintf(self->dumper.out," (read_only)");

#if 0
  if(comment) fprintf(self->dumper.out," [%s]",comment);
#endif
  if(err)
    fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_serialize::dump_long]",err,grib_get_error_message(err));

  fprintf(self->dumper.out,"\n");

}

#if 0
static int test_bit(long a, long b) {return a&(1<<b);}
#endif

static void dump_bits(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  long value; size_t size = 1;
  int err = grib_unpack_long(a,&value,&size);

  if(  (a->flags & GRIB_ACCESSOR_FLAG_HIDDEN) != 0 )
    return;

  if( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 &&
      (d->option_flags & GRIB_DUMP_FLAG_READ_ONLY) == 0)
    return;

  fprintf(self->dumper.out,"%s = %ld ", a->name,value);

#if 0

  fprintf(self->dumper.out,"[");
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
#endif
  if(err)
    fprintf(self->dumper.out," *** ERR=%d (%s)",err,grib_get_error_message(err));

  fprintf(self->dumper.out,"\n");

}

static void dump_double(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  double value; size_t size = 1;
  int err = grib_unpack_double(a,&value,&size);

  if(  (a->flags & GRIB_ACCESSOR_FLAG_HIDDEN) != 0 )
    return;

  if( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 &&
      (d->option_flags & GRIB_DUMP_FLAG_READ_ONLY) == 0)
    return;


  if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && (value == GRIB_MISSING_DOUBLE))
    fprintf(self->dumper.out,"%s = MISSING", a->name);
  else
    fprintf(self->dumper.out,"%s = %g",a->name,value);

  if ( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 )  fprintf(self->dumper.out," (read_only)");

#if 0
  if(comment) fprintf(self->dumper.out," [%s]",comment);
#endif
  if(err)
    fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_serialize::dump_double]",err,grib_get_error_message(err));
  fprintf(self->dumper.out,"\n");

}

static void dump_string(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  char value[1024]={0,}; size_t size = sizeof(value);
  int err = grib_unpack_string(a,value,&size);
  int i;

  char *p = value;

  if(  (a->flags & GRIB_ACCESSOR_FLAG_HIDDEN) != 0 )
    return;

  if( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 &&
      (d->option_flags & GRIB_DUMP_FLAG_READ_ONLY) == 0)
    return;

  while(*p) { if(!isprint(*p)) *p = '.'; p++; }

  for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");



  fprintf(self->dumper.out,"%s = %s", a->name,value);
  if ( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 )  fprintf(self->dumper.out," (read_only)");

#if 0
  if(comment) fprintf(self->dumper.out," [%s]",comment);
#endif
  if(err)
    fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_serialize::dump_string]",err,grib_get_error_message(err));
  fprintf(self->dumper.out,"\n");

}

static void dump_bytes(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  int i,k,err =0;
  int more = 0;
  size_t size = a->length;
  unsigned char* buf = (unsigned char*)grib_context_malloc(d->handle->context,size);

  if(  (a->flags & GRIB_ACCESSOR_FLAG_HIDDEN) != 0 )
    return;

  if( (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) != 0 &&
      (d->option_flags & GRIB_DUMP_FLAG_READ_ONLY) == 0)
    return;

  for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
  fprintf(self->dumper.out,"%s = (%ld) {", a->name,a->length);

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
    fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_serialize::dump_bytes]\n}",err,grib_get_error_message(err));
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
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  int k,err =0;
  double*  buf = NULL;
  int last=0;
  int columns=4;
  char* values_format=NULL;
  char* default_format="%.16e";
  char* columns_str=NULL;
  size_t len=0;
  char* pc=NULL;
  char* pcf=NULL;
  size_t size=0;
  long count=0;
  values_format=default_format;

  if((a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY))
    return;

  grib_value_count(a,&count);
  size=count;

  if (self->format) {
    if (self->format[0]=='\"') values_format=self->format+1;
    else values_format=self->format;
    last=strlen(values_format)-1;
    if (values_format[last]=='\"') values_format[last]='\0';
  }


  pc = values_format;
  pcf = values_format;
  while( *pc!='\0' && *pc != '%') pc++;
  if (strlen(pc) > 1 ) {
    values_format=pc;
    len=pc-pcf;
  } else {
    values_format=default_format;
    len=0;
  }

  if (len>0) {
    columns_str=(char*)malloc((len+1)*sizeof(char));
    Assert(columns_str);
    columns_str=(char*)memcpy(columns_str,pcf,len);
    columns_str[len]='\0';
    columns=atoi(columns_str);
    free(columns_str);
  }

  if(size == 1){
    dump_double(d,a,NULL);
    return ;
  }

  if ((d->option_flags & GRIB_DUMP_FLAG_VALUES) == 0 ) return;

  buf = (double*)grib_context_malloc(d->handle->context,size * sizeof(double));

  fprintf(self->dumper.out,"%s (%ld) {",a->name,(long)size);

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
    fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_serialize::dump_values]\n}",err,grib_get_error_message(err));
    return ;
  }

  k = 0;
  while(k < size)
  {
    int j;
    for(j = 0; j < columns && k < size; j++, k++)
    {
      fprintf(self->dumper.out,values_format,buf[k]);
      if(k != size-1)
        fprintf(self->dumper.out,", ");
    }
    fprintf(self->dumper.out,"\n");
  }
  fprintf(self->dumper.out,"}\n");
  grib_context_free(d->handle->context,buf);
}

static void dump_label(grib_dumper* d,grib_accessor* a,const char* comment)
{
#if 0
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;
  int i;
  for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
  fprintf(self->dumper.out,"----> %s %s %s\n",a->creator->op, a->name,comment?comment:"");
#endif
}

static void dump_section(grib_dumper* d,grib_accessor* a,grib_block_of_accessors* block)
{
  const char* secstr="section";
  int len=0;
  grib_dumper_serialize *self = (grib_dumper_serialize*)d;

  len=strlen(secstr);

  if(a->name[0] == '_'){
    grib_dump_accessors_block(d,block);
    return;
  }

  if (strncmp(secstr,a->name,len)==0)
    fprintf(self->dumper.out,"#------ %s -------\n",a->name);


  grib_dump_accessors_block(d,block);

#if 0
  fprintf(self->dumper.out,"<------ %s %s\n",a->creator->op, a->name);
#endif
}
