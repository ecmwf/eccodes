/*
 * (C) Copyright 2005- ECMWF.
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
#include <sys/types.h>
#include "tigge_tools.h"

#ifdef ECCODES_ON_WINDOWS
  #include <direct.h>
  #include <io.h>
#endif

#define NUMBER(a) (sizeof(a)/sizeof(a[0]))

int error = 0;
const char* file = 0;
int field = 0;
const char* param = "unknown";
int list_mode = 0;
int compare_mode = 0;

static long get(grib_handle *h,const char* what)
{
    int e; long val;
    if((e = grib_get_long(h,what,&val)) != GRIB_SUCCESS)
    {
        printf("%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        error++;
        val = -1;
    }
    return val;
}

static char* sget(grib_handle *h,const char* what,char* val,size_t size)
{
    int e; 
    if((e = grib_get_string(h,what,val,&size)) != GRIB_SUCCESS)
    {
        printf("%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        error++;
    }
    return val;
}

static void verify(grib_handle *h,const char* full,const char* base)
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
            compare_mode?0:get(h,"date"),
            compare_mode?0:get(h,"time"),
            sget(h,"model",model,sizeof(model)),
            compare_mode?"xxxx":sget(h,"expver",expver,sizeof(expver)),
            type,
            levtype,
            get(h,"step"),
            number,
            level,
            sget(h,"tigge_short_name",tigge_name,sizeof(tigge_name))

           );


    if(list_mode)
    {
        printf("%s\n",wmo_name);
    }
    else if(strcmp(base,wmo_name) != 0) {
        printf("WRONG FILE NAME:   %s\nCORRECT FILE NAME: %s\n\n",base,wmo_name);
        error++;
    }
}

void validate(const char* path)
{
    FILE *f = fopen(path,"rb");
    grib_handle *h = 0;
    int err;
    int count = 0;
    const char* base = path;
    const char* p    = path;

    file  = path;
    field = 0;

    while(*p) {
        if(*p == '/') base = p+1;
        p++;
    }

    if(!f) {
        printf("%s: %s\n",path,strerror(errno));
        error++;
        return;
    }

    while( (h= grib_handle_new_from_file(0,f,&err)) != NULL)
    {
        ++field;
        verify(h,path,base);
        grib_handle_delete(h);
        count++;
        param = "unknown";
    }
    fclose(f);

    if(err) {
        printf("%s: grib_handle_new_from_file: %s\n",path,grib_get_error_message(err));
        error++;
        return;
    }

    if(count == 0) {
        printf("%s does not contain any GRIBs\n",path);
        error++;
        return;
    }
}

static void usage()
{
    printf("tigge_name [-l] [-c] files ....\n");
    exit(1);
}

int main(int argc, char** argv)
{
    int i = 1;
    int err = 0;

    if(argc == 1)
        usage();

    for(; i < argc; i++)
    {
        error = 0;

        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'l':
                    list_mode = 1;
                    break;

                case 'c':
                    list_mode = 1;
                    compare_mode = 1;
                    break;

                default:
                    usage();
                    break;
            }
        }
        else scan(argv[i]);

        if(error) err = 1;
    }

    return err;
}
