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

void usage(const char *prog)
{
  fprintf(stderr,"%s: [-d] [-e err] [-o out] file1 file2\n",prog);
  exit(1);
}

const char* prog;

double maxAbsoluteError = 0;
double maxRelativeError = 0;
int absolute = 0;
const char* outname;


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

static int location(grib_handle* h,int n,double *lat,double* lon)
{

  int err=0;
  grib_iterator*  iter =     grib_iterator_new(h,0,&err);
  int i = 0;
  double val;

  if(!iter)
    return 0;


  while(grib_iterator_next(iter,lat,lon,&val))
  {
    if(i++ == n)
    {
      grib_iterator_delete(iter);
      return 1;
    }
  }

  grib_iterator_delete(iter);
  return 0;

}

/**
* Get the next value from an iterator.
*
* @param i           : the iterator
* @param lat         : on exit latitude in degree
* @param lon         : on exit longitude in degree
* @param value       : on exit value of the point
* @return            : positive value if successfull, 0 if no more data are available
*/


static void compare_handles(int n,grib_handle* h1,grib_handle* h2,FILE* out)
{
  size_t len1 = 0;
  size_t len2 = 0;

  double *dval1 = NULL, *dval2 = NULL, *dval3 = NULL;
  double maxe = 0,mine = 0;
  double maxa = 0,mina = 0;
  int i,maxi = 0,maxai = 0;
  double lat,lon;

  GRIB_CHECK(grib_get_size(h1,"values",&len1),NULL);
  GRIB_CHECK(grib_get_size(h2,"values",&len2),NULL);

  if(len1 != len2)
  {
    printf("Field size mismatch %ld != %ld\n",(long)len1,(long)len2);
    exit(1);
  }

  dval1 = (double*)grib_context_malloc(h1->context,len1*sizeof(double)); Assert(dval1);
  dval2 = (double*)grib_context_malloc(h2->context,len2*sizeof(double)); Assert(dval2);
  if(out)
    dval3 = (double*)grib_context_malloc(h2->context,len2*sizeof(double)); Assert(dval2);

  GRIB_CHECK(grib_get_double_array(h1,"values",dval1,&len1),NULL);
  GRIB_CHECK(grib_get_double_array(h2,"values",dval2,&len2),NULL);


  for(i = 0; i < len1; i++)
  {
    double e = err(dval1[i],dval2[i]);
    double a = fabs(dval1[i]-dval2[i]);
    if(i == 0)  maxe = mine = e;
    if(i == 0)  maxa = mina = a;
    if(out) dval3[i] = absolute ? a : e;
    if(e < mine) mine = e;
    if(e > maxe) { maxe = e; maxi = i; }
    if(a < mina) mina = a;
    if(a > maxa) { maxa = a; maxai = i; }
  }

  if(out)
  {
    const void *buffer;
    size_t size;
    GRIB_CHECK(grib_set_long(h1,"generatingProcessIdentifier",255),NULL); /* To prevent Magics to scale the field */
    GRIB_CHECK(grib_set_long(h1,"numberOfBitsContainingEachPackedValue",24),NULL);
    GRIB_CHECK(grib_set_double_array(h1,"values",dval3,len1),NULL);
    GRIB_CHECK(grib_get_message(h1,&buffer,&size),NULL);

    if(fwrite(buffer,1,size,out) != size)
    {
      perror(outname);
      exit(1);
    }

  }

  printf("Field %ld: min relative error: %g%%, max relative error: %g%% [%g,%g,%g]\n",(long)n,mine*100,maxe*100,
    dval1[maxi],dval2[maxi],fabs(dval1[maxi]-dval2[maxi]));

    if(location(h1,maxi,&lat,&lon))
      printf("   latitude = %g, longitude = %g\n",lat,lon);

  printf("Field %ld: min absolute error: %g, max absolute error: %g [%g,%g]\n",(long)n,mina,maxa,
    dval1[maxai],dval2[maxai]);
    if(location(h1,maxai,&lat,&lon))
      printf("   latitude = %g, longitude = %g\n",lat,lon);


  grib_context_free(h1->context,dval1);
  grib_context_free(h2->context,dval2);
  grib_context_free(h2->context,dval3);
}

static void compare_files(const char* file1,const char* file2,FILE* out)
{
  FILE *f1 = fopen(file1,"r");
  FILE *f2 = fopen(file2,"r");
  int e1 = 0;
  int e2 = 0;
  grib_handle *h1;
  grib_handle *h2;
  const void* b1;
  const void* b2;
  size_t s1;
  size_t s2;
  int count = 0;

  if(!f1) {
    perror(file1);
    exit(1);
  }

  if(!f2) {
    perror(file2);
    exit(1);
  }

  h1 = grib_handle_new_from_file(NULL,f1,&e1);
  h2 = grib_handle_new_from_file(NULL,f2,&e2);

  while(h1 && h2)
  {
    ++count;

    GRIB_CHECK(grib_get_message(h1,&b1,&s1),file1);
    GRIB_CHECK(grib_get_message(h2,&b2,&s2),file2);

    compare_handles(count,h1,h2,out);

    grib_handle_delete(h1);
    grib_handle_delete(h2);

    e1 = e2 = 0;

    h1 = grib_handle_new_from_file(NULL,f1,&e1);
    h2 = grib_handle_new_from_file(NULL,f2,&e2);
  }


  GRIB_CHECK(e1,file1);
  GRIB_CHECK(e2,file2);

  if(h1 != h2)
  {
    fprintf(stderr,"%s: Premature eof on %s\n",prog,h1 == 0 ? file1 : file2);
    exit(1);
  }


}

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  FILE* out = 0;

  prog = argv[0];

  while((c = getopt(argc, argv,"ae:o:")) != EOF) {
    switch(c) {

      case 'o':
        outname = optarg;
        out     = fopen(outname,"w");
        if(!out) {
          perror(outname);
          exit(1);
        }
        break;

      case 'a':
        absolute = 1;
        break;

      case 'e':
        maxAbsoluteError = atof(optarg);
        break;

      default:
        usage(prog);
        break;
    }
  }

  if(argc-optind < 2) usage(prog);


  compare_files(argv[argc-2],argv[argc-1],out);

  if(out) {
    if(fclose(out))
    {
      perror(outname);
      exit(1);
    }
  }

  return 0;

}

