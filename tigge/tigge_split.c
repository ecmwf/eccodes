/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/* cmake config header */
#ifdef HAVE_ECCODES_CONFIG_H
#include "eccodes_config.h"
#endif

/* autoconf config header */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <grib_api.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>


#define CHECK(a) check(#a,a)

const char* file = 0;
int field = 0;
const char* param = "unknown";

int print_names = 0;
int unique      = 0;

static void check(const char* name,int a)
{
    if(!a) {
        fprintf(stderr,"%s, field %d [%s]: %s failed\n",file,field,param,name);
        exit(1);
    }
}

static long get(grib_handle *h,const char* what)
{
    int e; long val;
    if((e = grib_get_long(h,what,&val)) != GRIB_SUCCESS)
    {
        fprintf(stderr,"%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        exit(1);
        val = -1;
    }
    return val;
}

static char* sget(grib_handle *h,const char* what,char* val,size_t size)
{
    int e;
    if((e = grib_get_string(h,what,val,&size)) != GRIB_SUCCESS)
    {
        fprintf(stderr,"%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        exit(1);
    }
    return val;
}

/*
static double dget(grib_handle *h,const char* what)
{
    int e; double val;
    if((e = grib_get_double(h,what,&val)) != GRIB_SUCCESS)
    {
        fprintf(stderr,"%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        exit(1);
        val = -1;
    }
    return val;
}

static int missing(grib_handle *h,const char* what)
{
    int err=0;
    return grib_is_missing(h,what,&err);
}

static int eq(grib_handle *h,const char* what,long value)
{
    return get(h,what) == value;
}

static int ne(grib_handle *h,const char* what,long value)
{
    return get(h,what) != value;
}

static int ge(grib_handle *h,const char* what,long value)
{
    return get(h,what) >= value;
}
*/

static void split(grib_handle *h)
{
    char wmo_name[1024];
    char origin[80];
    char model[80];
    char expver[80];
    char levtype[80];
    char type[80];
    char tigge_name[80];

    long level = 0;
    long number = 0;

    size_t size;
    const void* buffer;
    FILE *f;

    sget(h,"type",type,sizeof(type));
    sget(h,"levtype",levtype,sizeof(levtype));

    if(strcmp(type,"fc") != 0)
        number = get(h,"number");

    if(strcmp(levtype,"sfc") == 0)
        strcpy(levtype,"sl");
    else
        level = get(h,"level");

    sprintf(wmo_name,"z_tigge_c_%s_%08ld%04ld00_%s_%s_%s_%s_%04ld_%03ld_%04ld_%s.grib",
            sget(h,"origin",origin,sizeof(origin)),
            get(h,"date"),
            get(h,"time"),
            sget(h,"model",model,sizeof(model)),
            sget(h,"expver",expver,sizeof(expver)),
            type,
            levtype,
            get(h,"endStep"),
            number,
            level,
            sget(h,"tigge_short_name",tigge_name,sizeof(tigge_name))

    );

    if(print_names) printf("%s\n",wmo_name);

    if(unique)
    {
        if(access(wmo_name,F_OK) == 0)
        {
            fprintf(stderr,"tigge_split: %s already exists\n",wmo_name);
            exit(1);
        }
    }

    f = fopen(wmo_name,"w");
    if(!f) { perror(wmo_name); exit(1); }

    CHECK(grib_get_message(h,&buffer,&size) == 0);

    if(fwrite(buffer,1,size,f) != size) {
        perror(wmo_name);
        exit(1);
    }

    if(fclose(f)) { perror(wmo_name); exit(1); }
}

static void validate(const char* path)
{
    FILE *f = fopen(path,"r");
    grib_handle *h = 0;
    int err;
    int count = 0;

    if(!f) {
        fprintf(stderr,"%s: %s\n",path,strerror(errno));
        exit(1);
    }

    while( (h= grib_handle_new_from_file(0,f,&err)) != NULL)
    {
        ++field;
        split(h);
        grib_handle_delete(h);
        count++;
        param = "unknown";
    }
    fclose(f);

    if(err) {
        fprintf(stderr,"%s: grib_handle_new_from_file: %s\n",path,grib_get_error_message(err));
        exit(1);
    }

    if(count == 0) {
        fprintf(stderr,"%s does not contain any GRIBs\n",path);
        exit(1);
    }
}

static void usage()
{
    printf("tigge_split [-p] files ....\n");
    printf("   -p: print names of files created\n");
    exit(1);
}

int main(int argc, char** argv)
{
    int i = 1;

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
            case 'p':
                print_names++;
                break;

            case 'u':
                unique++;
                break;

            default:
                usage();
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(i == argc)
        usage();

    for(; i < argc; i++)
        validate(argv[i]);

    return 0;
}
