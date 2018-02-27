/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * test: reading multi fields messages from memory
 */

#include "grib_api.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static void usage(const char* prog) {
    printf("usage: %s [-m] file.grib\n",prog);
    exit(1);
}

int main(int argc,char* argv[]) {
    struct stat finfo;
    char shortName[20]={0,};
    size_t len=20;
    grib_handle* h=NULL;
    size_t fsize;
    unsigned char* data=NULL;
    unsigned char* p=NULL;
    void* pdata=NULL;
    int error=0;
    int count=0;
    char* filename=NULL;
    FILE* f=NULL;
    long level=0;
    grib_context* c=grib_context_get_default();

    if (argc==3 && !strcmp(argv[1],"-m")) {
        grib_multi_support_on(0);
        filename=argv[2];
    }
    else if (argc==2) filename=argv[1];
    else usage(argv[0]);

    f=fopen(filename,"r");
    if (!f) {perror(filename);exit(1);}

    fstat(fileno((FILE*)f),&finfo);
    fsize=finfo.st_size;

    data=(unsigned char*)malloc(fsize);
    p=data;

    if (!data) {
        fprintf(stderr,"unable to allocate %ld bytes\n",(long)fsize);
        exit(1);
    }

    if( fread(data, 1, fsize, f)  != fsize) {
        perror(filename);
        exit(1);
    }
    fclose(f);
    pdata=&data;

    while ((h=grib_handle_new_from_multi_message(c,(void**)pdata,&fsize,&error))!=NULL) {
        count++;
        len=20;
        GRIB_CHECK(grib_get_string(h,"shortName",shortName,&len),"shortName");
        GRIB_CHECK(grib_get_long(h,"level",&level),"level");
        printf("%d %s %ld\n",count,shortName,level);
        grib_handle_delete(h);
    }

    free(p);

    return 0;
}
