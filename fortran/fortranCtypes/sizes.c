#include <stddef.h>

void grib_check_fortran_char(char* a) {
  if (*a != 't') exit(1);
  *a='f';
}
void grib_check_fortran_char_(char* a) {grib_check_fortran_char(a);}


void check_double(double *x,double *y,char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}
void check_double_(double *x,double *y,char* ret) {check_double(x,y,ret);}


void check_float(float *x,float *y,char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}
void check_float_(float *x,float *y,char* ret) { check_float(x,y,ret); }


void check_int(int *x,int *y,char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}
void check_int_(int *x,int *y,char* ret) { check_int(x,y,ret); }


void check_long(long *x,long *y,char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}
void check_long_(long *x,long *y,char* ret) {check_long(x,y,ret);}


void check_size_t(size_t *x,size_t *y,char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}
void check_size_t_(size_t *x,size_t *y,char* ret) {check_size_t(x,y,ret);}


void check_long_long(long long *x,long long *y,char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}
void check_long_long_(long long *x,long long *y,char* ret) {check_long_long(x,y,ret);}

