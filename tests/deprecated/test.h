#include <stdio.h>
#include <unistd.h>
#include "grib_api.h"
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>


double bucket_size(grib_handle* g);
int compare_values(double *v1, double *v2, int count,const char *what);
int compare_with_gribex(grib_handle *g);
int compare_two_gribex(grib_handle *g);
int compare_with_handle(grib_handle *g, grib_handle *h);
int save_message(grib_handle* h,const char* path);
const char* problem(const char* path,const char*name, int);
void get_max_relative_error(double* v1,double* v2,int count,double* maxError,int* id);

extern long readany(FILE*,char*,long*);
extern   char *get_current_dir_name(void);

