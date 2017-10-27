/*
 * Copyright 2005-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

static void usage(const char* prog)
{
    printf("usage: %s [-v] nchunks infile\n",prog);
    exit(1);
}

static int split_file(FILE* in, const char* filename, int nchunks,unsigned long *count)
{
    void* mesg=NULL;
    FILE* out;
    size_t size=0,read_size=0,insize=0,chunk_size;
    off_t offset=0;
    int err=GRIB_SUCCESS;
    int i;
    char* ofilename;
    grib_context* c=grib_context_get_default();

    if (!in) return 1;

    ofilename=(char*)calloc(1,strlen(filename)+10);

    fseeko(in, 0, SEEK_END);
    insize = ftello(in);
    fseeko(in, 0, SEEK_SET);
    chunk_size=insize/nchunks;

    i=1;
    sprintf(ofilename,"%s_%d",filename,i);
    out=fopen(ofilename,"w");
    if (!out) {
      perror(ofilename);
      return GRIB_IO_PROBLEM;
    }

    while ( err!=GRIB_END_OF_FILE ) {
        mesg=wmo_read_any_from_file_malloc(in,0,  &size,&offset,&err);
        if (mesg!=NULL && err==0) {
          if (fwrite(mesg,1,size,out)!=size) {
            perror(ofilename);
            fclose(out);
            fclose(in);
            return GRIB_IO_PROBLEM;
          }
          grib_context_free(c,mesg);
          read_size+=size;
          if (read_size>chunk_size) {
            fclose(out);
            i++;
            sprintf(ofilename,"%s_%d",filename,i);
            out=fopen(ofilename,"w");
            if (!out) {
              perror(ofilename);
              return GRIB_IO_PROBLEM;
            }
            read_size=0;
          }
          (*count)++;
        }
    }
    fclose(out);

    if (err==GRIB_END_OF_FILE) err=GRIB_SUCCESS;

    return err;
}

int main(int argc,char* argv[])
{
    FILE* infh = NULL;
    char* filename;
    int i, verbose=0;
    int err=0,nchunks=0;
    unsigned long count=0;
    int message_type = 0; /* GRIB, BUFR etc */

    if (argc <3) usage(argv[0]);

    i=1;
    if (strcmp(argv[i], "-v")==0) {
      i++;
      verbose = 1;
      if (argc !=4) usage(argv[0]);
    }

    /* add some error checking */
    nchunks=atoi(argv[i]);

    i++;
    filename=argv[i];
    infh=fopen(filename,"r");
    if (!infh) {
      perror(filename);
      exit(1);
    }

    count=0;
    err=split_file(infh, filename,nchunks, &count);
    if (err) {
      fprintf(stderr,"Invalid message(s) found in %s", filename);
      fprintf(stderr,"\n");
    }
    if (verbose) printf ("%7lu %s\n", count, filename);

    fclose(infh);

    return 0;
}
