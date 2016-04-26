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

typedef struct grib_dumper_keys {
    grib_dumper          dumper;  
/* Members defined in keys */
	long section_offset;
	long begin;
	long theEnd;
} grib_dumper_keys;


static grib_dumper_class _grib_dumper_class_keys = {
    0,                              /* super                     */
    "keys",                              /* name                      */
    sizeof(grib_dumper_keys),     /* size                      */
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

grib_dumper_class* grib_dumper_class_keys = &_grib_dumper_class_keys;

/* END_CLASS_IMP */

static void print_offset(FILE* out,grib_dumper* d,grib_accessor* a);

static void init_class      (grib_dumper_class* c){}

static int  init(grib_dumper* d)
{
  grib_dumper_keys *self = (grib_dumper_keys*)d;
  self->section_offset=0;

  return GRIB_SUCCESS;
}

static int  destroy  (grib_dumper* d){
  return GRIB_SUCCESS;
}

static void aliases(grib_dumper* d,grib_accessor* a)
{
int i;
  grib_dumper_keys *self = (grib_dumper_keys*)d;

  if( (d->option_flags & GRIB_DUMP_FLAG_ALIASES) == 0)
    return;

  if(a->all_names[1])
  {
    char *sep = "";
    fprintf(self->dumper.out," ( ALIASES: ");

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
    printf(") ");
  }
}

static void dump_name_only(grib_dumper* d,grib_accessor* a,const char* comment) {
  grib_dumper_keys *self = (grib_dumper_keys*)d;

  print_offset(self->dumper.out,d,a);


  if (a->flags & GRIB_ACCESSOR_FLAG_HIDDEN) {
    return;
  }

  if( a->length == 0  &&
      (d->option_flags & GRIB_DUMP_FLAG_CODED) != 0)
    return;

  if( (a->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0 &&
      (d->option_flags & GRIB_DUMP_FLAG_DUMP_OK) != 0)
    return;

  fprintf(self->dumper.out,"%s",a->name);

  if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY) {
    fprintf(self->dumper.out," (read only)");
  }
  if ((d->option_flags & GRIB_DUMP_FLAG_TYPE) != 0){
    fprintf(self->dumper.out," (type %s) ",a->creator->op);
  }

  aliases(d,a);

  fprintf(self->dumper.out,"\n");
}

static void dump_long(grib_dumper* d,grib_accessor* a,const char* comment)
{
  dump_name_only(d,a,comment);
}

static void dump_bits(grib_dumper* d,grib_accessor* a,const char* comment)
{
  dump_name_only(d,a,comment);
}

static void dump_double(grib_dumper* d,grib_accessor* a,const char* comment)
{
  dump_name_only(d,a,comment);
}

static void dump_string(grib_dumper* d,grib_accessor* a,const char* comment)
{
  dump_name_only(d,a,comment);
}

static void dump_bytes(grib_dumper* d,grib_accessor* a,const char* comment)
{

#if 0
  grib_dumper_keys *self = (grib_dumper_keys*)d;
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
    fprintf(self->dumper.out," *** ERR=%d (%s) \n}",err,grib_get_error_message(err));
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
  dump_name_only(d,a,0);
}

static void dump_label(grib_dumper* d,grib_accessor* a,const char* comment)
{
  /*grib_dumper_keys *self = (grib_dumper_keys*)d;

  for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");
  fprintf(self->dumper.out,"----> %s %s %s\n",a->creator->op, a->name,comment?comment:"");*/
}

static void dump_section(grib_dumper* d,grib_accessor* a,grib_block_of_accessors* block)
{
  grib_dumper_keys *self = (grib_dumper_keys*)d;
  /*grib_section* s = grib_get_sub_section(a);*/
  int is_default_section=0;
  char* upper=NULL;
  char *p=NULL,*q=NULL;
  if (!strncmp(a->name,"section",7)) is_default_section=1;

  /*for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");*/
  if (is_default_section) {
    upper=(char*)malloc(strlen(a->name)+1);
    Assert(upper);
    p=(char*)a->name;
    q=upper;
    while (*p != '\0') {
      *q=toupper(*p);
      if (*q == '_' ) *q=' ';
      q++;
      p++;
    }
    *q='\0';

    /*sprintf(tmp,"%s ",upper,(long)s->length,(long)s->padding);*/

    fprintf(self->dumper.out,"====> %s <==== \n",upper);

    free(upper);
    self->section_offset=a->offset;
  } else {

  }

  /*printf("------------- section_offset = %ld\n",self->section_offset);*/
  d->depth += 3;
  grib_dump_accessors_block(d,block);
  d->depth -= 3;

  /*for(i = 0; i < d->depth ; i++) fprintf(self->dumper.out," ");*/
  /*fprintf(self->dumper.out,"<===== %s %s\n",a->creator->op, a->name);*/
}

static void print_offset(FILE* out,grib_dumper* d,grib_accessor* a) {

}

