/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void f_sizeof_(void *x, void *y, int *size) {
  *size=((char*)y)-((char*)x);
}

void check_double_(double *x, double* y, char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}

void check_float_(float* x, float* y, char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}

void check_int_(int* x, int* y, char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}

void check_long_(long* x, long * y, char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}

void check_size_t_(size_t* x, size_t* y, char* ret) {
  *ret = ((char*)y)-((char*)x) == sizeof(*x) ?  't' : 'f';
}

#ifdef __cplusplus
}
#endif
