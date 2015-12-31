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

void usage(const char *prog)
{
  fprintf(stderr,"%s: [-b blacklist][-e] [-p err] file1 file2\n",prog);
  exit(1);
}

typedef struct blacklist {
  struct blacklist *next;
  char* key;
} blacklist;

const char* prog;

double maxAbsoluteError = 0;
double maxRelativeError = 0;


static double err(double A, double B)
{
  double relativeError;

  if(fabs(A) <= maxAbsoluteError || fabs(B) <= maxAbsoluteError)
    relativeError = fabs(A-B);
  else if (fabs(B) > fabs(A))
    relativeError = fabs((A - B) / B);
  else
    relativeError = fabs((A - B) / A);

  return relativeError;
}


static int same(double a,double b)
{
  return err(a,b) <= maxRelativeError;
}


static int compare_values(grib_handle* h1,grib_handle *h2,const char *name)
{
  size_t len1 = 0;
  size_t len2 = 0;
  int err;
  int err1;
  int err2;
  int type1,type2;

  char *sval1 = NULL,*sval2 = NULL;
  unsigned char *uval1 = NULL,*uval2 = NULL;
  double *dval1 = NULL, *dval2 = NULL;
  long *lval1 = NULL, *lval2 = NULL;

  if((err = grib_get_native_type(h1,name,&type1)) != GRIB_SUCCESS)
  {
    printf("Oops... cannot get type of [%s] in 1st field: %s\n",name,grib_get_error_message(err));
    return err;
  }

  if((err = grib_get_native_type(h2,name,&type2)) != GRIB_SUCCESS)
  {
    if(err == GRIB_NOT_FOUND)
    {
      printf("[%s] not found in 2nd field\n",name);
      return err;
    }

    printf("Oops... cannot get type of [%s] in 2nd field: %s\n",name,grib_get_error_message(err));
    return err;
  }

  if(type1 != type2)
  {
    printf("Warning, [%s] has different types: 1st field: [%s], 2nd field: [%s]\n",
        name,grib_get_type_name(type1),grib_get_type_name(type2));
    /* return GRIB_TYPE_MISMATCH; */
  }

  if(type1 == GRIB_TYPE_LABEL)
    return err;

  if(type1 == GRIB_TYPE_SECTION)
    return err;


  if((err = grib_get_size(h1,name,&len1)) != GRIB_SUCCESS)
  {
    printf("Oops... cannot get size of [%s] in 1st field: %s\n",name,grib_get_error_message(err));
    return err;
  }

  if((err = grib_get_size(h2,name,&len2)) != GRIB_SUCCESS)
  {
    if(err == GRIB_NOT_FOUND)
    {
      printf("[%s] not found in 2nd field\n",name);
      return err;
    }

    printf("Oops... cannot get size of [%s] in 2nd field: %s\n",name,grib_get_error_message(err));
    return err;
  }

  if(len1 != len2)
  {
    printf("[%s] has different size: 1st field: %ld, 2nd field: %ld\n",name,(long)len1,(long)len2);
    return GRIB_COUNT_MISMATCH;
  }

  switch(type1)
  {
    case GRIB_TYPE_STRING:

      sval1 = (char*)grib_context_malloc(h1->context,len1*sizeof(char));
      sval2 = (char*)grib_context_malloc(h2->context,len2*sizeof(char));

      if((err1 = grib_get_string(h1,name,sval1,&len1)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get string value of [%s] in 1st field: %s\n",
          name,grib_get_error_message(err1));
      }

      if((err2 = grib_get_string(h2,name,sval2,&len2)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get string value of [%s] in 2nd field: %s\n",
          name,grib_get_error_message(err2));
      }

      if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS)
      {
        if(strcmp(sval1,sval2) != 0)
        {
          printf("[%s] string values are different: [%s] and [%s]\n",
            name,sval1,sval2);
          err1 = GRIB_VALUE_MISMATCH;
        }
      }

      grib_context_free(h1->context,sval1);
      grib_context_free(h2->context,sval2);

      if(err1) return err1;
      if(err2) return err2;

      break;

    case GRIB_TYPE_LONG:

      lval1 = (long*)grib_context_malloc(h1->context,len1*sizeof(long));
      lval2 = (long*)grib_context_malloc(h2->context,len2*sizeof(long));

      if((err1 = grib_get_long_array(h1,name,lval1,&len1)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get long value of [%s] in 1st field: %s\n",
          name,grib_get_error_message(err1));
      }

      if((err2 = grib_get_long_array(h2,name,lval2,&len2)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get long value of [%s] in 2nd field: %s\n",
          name,grib_get_error_message(err2));
      }

      if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS)
      {
        int i;
        for(i = 0; i < len1; i++)
          if(lval1[i] != lval2[i])
          {
              if(len1 == 1)
                printf("[%s] long  values are different: [%ld] and [%ld]\n",
                  name,lval1[i],lval2[i]);
              else
                printf("[%s] long value %d of %ld are different: [%ld] and [%ld]\n",
                  name,i,(long)len1,lval1[i],lval2[i]);

            err1 = GRIB_VALUE_MISMATCH;
            break;
          }
      }

      grib_context_free(h1->context,lval1);
      grib_context_free(h2->context,lval2);

      if(err1) return err1;
      if(err2) return err2;
      break;

    case GRIB_TYPE_DOUBLE:
      dval1 = (double*)grib_context_malloc(h1->context,len1*sizeof(double));
      dval2 = (double*)grib_context_malloc(h2->context,len2*sizeof(double));

      if((err1 = grib_get_double_array(h1,name,dval1,&len1)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get double value of [%s] in 1st field: %s\n",
          name,grib_get_error_message(err1));
      }

      if((err2 = grib_get_double_array(h2,name,dval2,&len2)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get double value of [%s] in 2nd field: %s\n",
          name,grib_get_error_message(err2));
      }

      if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS)
      {
        int i;
        for(i = 0; i < len1; i++)
          if(!same(dval1[i],dval2[i]))
          {
              if(len1 == 1)
                printf("[%s] double values are different: [%g] and [%g], diff: %g\n",
                  name,dval1[i],dval2[i],dval1[i] - dval2[i]);
              else
                printf("[%s] double value %d of %ld are different: [%g] and [%g], diff: %g\n",
                  name,i,(long)len1,dval1[i],dval2[i],dval1[i] - dval2[i]);

            err1 = GRIB_VALUE_MISMATCH;
            break;
          }
      }

      grib_context_free(h1->context,dval1);
      grib_context_free(h2->context,dval2);

      if(err1) return err1;
      if(err2) return err2;
      break;

    case GRIB_TYPE_BYTES:

      uval1 = (unsigned char*)grib_context_malloc(h1->context,len1*sizeof(unsigned char));
      uval2 = (unsigned char*)grib_context_malloc(h2->context,len2*sizeof(unsigned char));

      if((err1 = grib_get_bytes(h1,name,uval1,&len1)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get bytes value of [%s] in 1st field: %s\n",
          name,grib_get_error_message(err1));
      }

      if((err2 = grib_get_bytes(h2,name,uval2,&len2)) != GRIB_SUCCESS)
      {
        printf("Oops... cannot get bytes value of [%s] in 2nd field: %s\n",
          name,grib_get_error_message(err2));
      }

      if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS)
      {
        if(memcmp(uval1,uval2,len1) != 0)
        {
        int i;
        for(i = 0; i < len1; i++)
          if(uval1[i] != uval2[i])
          {
              if(len1 == 1)
                printf("[%s] byte values are different: [%02x] and [%02x]\n",
                  name,uval1[i],uval2[i]);
              else
                printf("[%s] byte value %d of %ld are different: [%02x] and [%02x]\n",
                  name,i,(long)len1,uval1[i],uval2[i]);

            err1 = GRIB_VALUE_MISMATCH;
            break;
          }
          err1 = GRIB_VALUE_MISMATCH;
        }
      }

      grib_context_free(h1->context,uval1);
      grib_context_free(h2->context,uval2);

      if(err1) return err1;
      if(err2) return err2;
      break;

    case GRIB_TYPE_LABEL:
      break;

    default:
      printf("Cannot compare [%s], unsupported type %d\n",name,type1);
      return GRIB_UNABLE_TO_COMPARE_ACCESSORS;
      break;
  }

  return GRIB_SUCCESS;

}

static int in_blacklist(const char* name,blacklist* bl) {
    blacklist* p=bl;
    while (p) {
      if (!strcmp(name,p->key)) return 1;
      p=p->next;
    }
    return 0;
}

static int compare_handles(grib_handle* h1,grib_handle* h2,blacklist* bl)
{
  int err = 0;
  const char* name=NULL;
  grib_keys_iterator* iter  = grib_keys_iterator_new(h1,
    GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC|GRIB_KEYS_ITERATOR_SKIP_DUPLICATES,NULL);

  if (!iter) {
    printf("ERROR: unable to get iterator begin\n");
    exit(1);
  }

  while(grib_keys_iterator_next(iter))
  {
    /* printf("--------------- %s -----------\n",iter->current->name); */
  /* We can also black list with the class*/
/* grib_get_accessor_class_name' */

    name=grib_keys_iterator_get_name(iter);

    if( !in_blacklist(name,bl) && compare_values(h1,h2,name))
      err++;

  }

  grib_keys_iterator_delete(iter);

  return err;
}

static void compare_files(const char* file1,const char* file2, blacklist* bl)
{
  FILE *f1 = fopen(file1,"r");
  FILE *f2 = fopen(file2,"r");
  int e1 = 0;
  int e2 = 0;
  grib_handle *h1;
  grib_handle *h2;
  int count = 0;
  int err = 0;
  grib_context* c=grib_context_get_default( );

  grib_multi_support_off(c);

  if(!f1) {
    perror(file1);
    exit(1);
  }

  if(!f2) {
    perror(file2);
    exit(1);
  }

  h1 = grib_handle_new_from_file(c,f1,&e1);
  h2 = grib_handle_new_from_file(c,f2,&e2);

  while(h1 && h2)
  {
    ++count;

    printf("..............................\n");
    if(compare_handles(h1,h2,bl)) err++;
    printf("..............................\n");
    if(compare_handles(h2,h1,bl)) err++;

    grib_handle_delete(h1);
    grib_handle_delete(h2);

    e1 = e2 = 0;

    h1 = grib_handle_new_from_file(c,f1,&e1);
    h2 = grib_handle_new_from_file(c,f2,&e2);
  }

  GRIB_CHECK(e1,file1);
  GRIB_CHECK(e2,file2);

  if(h1 != h2)
  {
    fprintf(stderr,"%s: Premature eof on %s\n",prog,h1 == 0 ? file1 : file2);
    exit(1);
  }

  if(err) exit(1);

}



int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  char *tok=NULL;
  blacklist *bl=NULL,*p=NULL;
  int c;

  prog = argv[0];

  while((c = getopt(argc, argv,"ep:b:")) != EOF) {
    switch(c) {

      case 'p':
        maxRelativeError = atof(optarg);
        break;

      case 'e':
        maxRelativeError = 1e-5;
        break;

      case 'b':
        tok=strtok(optarg,", ");
        while (tok) {
          p=(blacklist*)calloc(1,sizeof(blacklist));
          p->key=strdup(tok);
          if (!bl) bl=p;
          else {
            p->next=bl;
            bl=p;
          }
          tok=strtok(NULL,", ");
        }
        break;

      default:
        usage(prog);
        break;
    }
  }

  if(argc-optind < 2) usage(prog);

  compare_files(argv[argc-2],argv[argc-1],bl);

  return 0;

}

