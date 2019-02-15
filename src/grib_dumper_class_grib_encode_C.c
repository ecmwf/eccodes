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
   IMPLEMENTS = header;footer
   MEMBERS    =  int cr
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
static void header         (grib_dumper*,grib_handle*);
static void footer         (grib_dumper*,grib_handle*);

typedef struct grib_dumper_grib_encode_C {
    grib_dumper          dumper;  
/* Members defined in grib_encode_C */
	int cr;
} grib_dumper_grib_encode_C;


static grib_dumper_class _grib_dumper_class_grib_encode_C = {
    0,                              /* super                     */
    "grib_encode_C",                              /* name                      */
    sizeof(grib_dumper_grib_encode_C),     /* size                      */
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
    &header,                             /* header   */
    &footer,                             /* footer   */
};

grib_dumper_class* grib_dumper_class_grib_encode_C = &_grib_dumper_class_grib_encode_C;

/* END_CLASS_IMP */
static void init_class      (grib_dumper_class* c){}

static int  init(grib_dumper* d)
{
  /* grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d; */
  return GRIB_SUCCESS;
}

static int  destroy  (grib_dumper* d){
  return GRIB_SUCCESS;
}

static void pcomment(FILE* f,long value,const char* p)
{
  int cr = 0;
  fprintf(f,"\n    /* %ld = ",value);

  while(*p)
  {
    switch(*p)
    {
      case ';':
        fprintf(f,"\n    ");
        cr = 1;
        break;

      case ':':
        if(cr)
          fprintf(f,"\n    See ");
        else
          fprintf(f,". See ");
        break;

      default:
        fputc(*p,f);
        break;
    }

    p++;
  }

  fprintf(f," */\n");

}

static void dump_long(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  long value; size_t size = 1;
  int err = grib_unpack_long(a,&value,&size);

  if((a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY))
    return;

#if 1
  if(comment) pcomment(self->dumper.out,value,comment);
#endif

  if( ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0) && (value == GRIB_MISSING_LONG))
    fprintf(self->dumper.out,"    GRIB_CHECK(grib_set_missing(h,\"%s\"),%d);\n",a->name,0);
  else
    fprintf(self->dumper.out,"    GRIB_CHECK(grib_set_long(h,\"%s\",%ld),%d);\n",a->name,value,0);

  if(err)
    fprintf(self->dumper.out," /*  Error accessing %s (%s) */",a->name,grib_get_error_message(err));

  if(comment) fprintf(self->dumper.out,"\n");

}

static int test_bit(long a, long b) {return a&(1<<b);}


static void dump_bits(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  long value; size_t size = 1;
  int err = grib_unpack_long(a,&value,&size);
  int i;

  char buf[1024];

  if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
    return;

  if(a->length == 0)
    return;

  buf[0] = 0;

  for(i=0;i<(a->length*8);i++) {
    if(test_bit(value,a->length*8-i-1))
      strcat(buf,"1");
    else
      strcat(buf,"0");
  }

  if(comment) {
    strcat(buf,";");
    strcat(buf,comment);
  }

  pcomment(self->dumper.out,value,buf);

  if(err)
    fprintf(self->dumper.out," /*  Error accessing %s (%s) */",a->name,grib_get_error_message(err));
  else
    fprintf(self->dumper.out,"    GRIB_CHECK(grib_set_long(h,\"%s\",%ld),%d);\n",a->name,value,0);

  fprintf(self->dumper.out,"\n");
}

static void dump_double(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  double value; size_t size = 1;
  int err = grib_unpack_double(a,&value,&size);
  if(a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
    return;

  if(a->length == 0)
    return;

#if 0
  if(comment) fprintf(self->dumper.out,"/* %s */\n",comment);
#endif

  fprintf(self->dumper.out,"    GRIB_CHECK(grib_set_double(h,\"%s\",%g),%d);\n",a->name,value,0);

  if(err)
    fprintf(self->dumper.out," /*  Error accessing %s (%s) */",a->name,grib_get_error_message(err));
}

static void dump_string(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  char value[1024]; size_t size = sizeof(value);
  int err = grib_unpack_string(a,value,&size);

  if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
    return;

  if(a->length == 0)
    return;

#if 1
  if(comment) fprintf(self->dumper.out,"/* %s */\n",comment);
#endif

  fprintf(self->dumper.out,"    p    = \"%s\";\n",value);
  fprintf(self->dumper.out,"    size = strlen(p);\n");
  fprintf(self->dumper.out,"    GRIB_CHECK(grib_set_string(h,\"%s\",p,&size),%d);\n",a->name,0);

  if(err)
    fprintf(self->dumper.out," /*  Error accessing %s (%s) */",a->name,grib_get_error_message(err));
}

static void dump_bytes(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  int err =0;
  size_t size = a->length;
  unsigned char* buf;


  if (a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
    return;

  if(size == 0)
    return;

  buf = (unsigned char*)grib_context_malloc(d->handle->context,size);

  if(!buf)
  {
    fprintf(self->dumper.out,"/* %s: cannot malloc(%ld) */\n",a->name,(long)size);
    return;
  }


  err = grib_unpack_bytes(a,buf,&size);
  if(err){
    grib_context_free(d->handle->context,buf);
    fprintf(self->dumper.out," *** ERR=%d (%s) [grib_dumper_grib_encode_C::dump_bytes]\n}",err,grib_get_error_message(err));
    return ;
  }

#if 0
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
#endif
  grib_context_free(d->handle->context,buf);
}

static void dump_values(grib_dumper* d,grib_accessor* a)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  int k,err =0;
  double*  buf = NULL;
  int type=0;
  char stype[10];
  size_t size=0;
  long count=0;

  stype[0]='\0';

  if((a->flags & GRIB_ACCESSOR_FLAG_READ_ONLY)
      || ((a->flags & GRIB_ACCESSOR_FLAG_DATA)
          && (d->option_flags & GRIB_DUMP_FLAG_NO_DATA)   ) )
    return;

  grib_value_count(a,&count);
  size=count;

  if(size == 1){
    dump_double(d,a,NULL);
    return ;
  }

  type=grib_accessor_get_native_type(a);
  switch (type) {
     case GRIB_TYPE_LONG:
       sprintf(stype,"%s","long");
       break;
     case GRIB_TYPE_DOUBLE:
       sprintf(stype,"%s","double");
       break;
     default:
       return;
  }

  buf = (double*)grib_context_malloc(d->handle->context,size * sizeof(double));
  if(!buf)
  {
    fprintf(self->dumper.out,"/* %s: cannot malloc(%ld) */\n",a->name,(long)size);
    return;
  }

  err =  grib_unpack_double(a,buf,&size);

  if(err){
    grib_context_free(d->handle->context,buf);
    fprintf(self->dumper.out," /*  Error accessing %s (%s) */",a->name,grib_get_error_message(err));
    return ;
  }

  fprintf(self->dumper.out,"    size = %ld;\n",(long)size);
  fprintf(self->dumper.out,"    v%s    = (%s*)calloc(size,sizeof(%s));\n",stype,stype,stype);
  fprintf(self->dumper.out,"    if(!v%s) {\n",stype);
  fprintf(self->dumper.out,"        fprintf(stderr,\"failed to allocate %%d bytes\\n\",size*sizeof(%s));\n",stype);
  fprintf(self->dumper.out,"        exit(1);\n");
  fprintf(self->dumper.out,"    }\n");


  fprintf(self->dumper.out,"\n   ");
  k = 0;
  while(k < size)
  {
    fprintf(self->dumper.out," v%s[%4d] = %7g;",stype,k,buf[k]);
    k++;
    if(k%4 == 0) fprintf(self->dumper.out,"\n   ");

  }
  if(size%4) fprintf(self->dumper.out,"\n");
  fprintf(self->dumper.out,"\n");
  fprintf(self->dumper.out,"    GRIB_CHECK(grib_set_%s_array(h,\"%s\",v%s,size),%d);\n",stype,a->name,stype,0);
  fprintf(self->dumper.out,"    free(v%s);\n",stype);

  grib_context_free(d->handle->context,buf);
}

static void dump_label(grib_dumper* d,grib_accessor* a,const char* comment)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
  fprintf(self->dumper.out,"\n    /* %s */\n\n",a->name);
}

static void dump_section(grib_dumper* d,grib_accessor* a,grib_block_of_accessors* block)
{
  /*grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;*/
  grib_dump_accessors_block(d,block);
}

static void header(grib_dumper* d,grib_handle* h)
{
  long edition=0;
  int ret=0;
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;
    ret=grib_get_long(h,"editionNumber",&edition);
  if (ret != GRIB_SUCCESS) {
    grib_context_log(h->context,GRIB_LOG_ERROR,"Unable to get edition number.");
    Assert(0);
  }

  fprintf(self->dumper.out,
      "#include <grib_api.h>\n"
      "\n"
      "/* This code was generated automatically */\n"
      "\n");

  fprintf(self->dumper.out,
      "\n"
      "int main(int argc,const char** argv)\n"
      "{\n"
      "    grib_handle *h     = NULL;\n"
      "    size_t size        = 0;\n"
      "    double* vdouble    = NULL;\n"
      "    long* vlong        = NULL;\n"
      "    FILE* f            = NULL;\n"
      "    const char* p      = NULL;\n"
      "    const void* buffer = NULL;\n"
      "\n"
      "    if(argc != 2) {\n"
      "       fprintf(stderr,\"usage: %%s out\\n\",argv[0]);\n"
      "        exit(1);\n"
      "    }\n"
      "\n"
      "    h = grib_handle_new_from_samples(NULL,\"GRIB%ld\");\n"
      "    if(!h) {\n"
      "        fprintf(stderr,\"Cannot create grib handle\\n\");\n"
      "        exit(1);\n"
      "    }\n"
      "\n",(long)edition
    );
}

static void footer(grib_dumper* d,grib_handle* h)
{
  grib_dumper_grib_encode_C *self = (grib_dumper_grib_encode_C*)d;

  fprintf(self->dumper.out,

      "/* Save the message */\n"
      "\n"
      "    f = fopen(argv[1],\"w\");\n"
      "    if(!f) {\n"
      "        perror(argv[1]);\n"
      "        exit(1);\n"
      "    }\n"
      "\n"
      "    GRIB_CHECK(grib_get_message(h,&buffer,&size),0);\n"
      "\n"
      "    if(fwrite(buffer,1,size,f) != size) {\n"
      "        perror(argv[1]);\n"
      "        exit(1);\n"
      "    }\n"
      "\n"
      "    if(fclose(f)) {\n"
      "        perror(argv[1]);\n"
      "        exit(1);\n"
      "    }\n"
      "\n"
    "    grib_handle_delete(h);\n"
    "    return 0;\n"
    "}\n"
    );
}
