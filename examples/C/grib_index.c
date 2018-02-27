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
 * C Implementation: grib_index
 *
 * Description: How to create and use an index to access GRIB messages from a file
 *
 */

#include "eccodes.h"

static void usage(const char* prog) {
    printf("usage: %s infile\n",prog);
    exit(1);
}

int main(int argc,char* argv[])
{
    codes_index* index=NULL;
    codes_handle* h=NULL;
    char* infile=NULL;
    long *steps,*levels,*numbers; /* arrays */
    char** shortName=NULL;
    int i,j,k,l;
    size_t stepSize,levelSize,shortNameSize,numberSize;
    long ostep,olevel,onumber;
    char oshortName[200];
    size_t lenshortName=200;
    int ret=0,count=0;

    if (argc != 2) usage(argv[0]);
    infile=argv[1];

    printf("indexing...\n");

    /* Create an index given set of keys*/
    index=codes_index_new(0,"shortName,level,number,step",&ret);
    if (ret) {printf("error: %s\n",codes_get_error_message(ret)); exit(ret);}

    /* Indexes a file */
    ret=codes_index_add_file(index,infile);
    if (ret) {printf("error: %s\n",codes_get_error_message(ret)); exit(ret);}
    printf("end indexing...\n");

    /* get the number of distinct values of "step" in the index */
    CODES_CHECK(codes_index_get_size(index,"step",&stepSize),0);
    steps=(long*)malloc(sizeof(long)*stepSize);
    if (!steps) exit(1);
    /* get the list of distinct steps from the index */
    /* the list is in ascending order */
    CODES_CHECK(codes_index_get_long(index,"step",steps,&stepSize),0);
    printf("stepSize=%ld\n",(long)stepSize);
    for (i=0;i<stepSize;i++) printf("%ld ",steps[i]);
    printf("\n");

    /*same as for "step"*/
    CODES_CHECK(codes_index_get_size(index,"level",&levelSize),0);
    levels=(long*)malloc(sizeof(long)*levelSize);
    if (!levels) exit(1);
    /*same as for "step"*/
    CODES_CHECK(codes_index_get_long(index,"level",levels,&levelSize),0);
    printf("levelSize=%ld\n",(long)levelSize);
    for (i=0;i<levelSize;i++) printf("%ld ",levels[i]);
    printf("\n");

    /*same as for "step"*/
    CODES_CHECK(codes_index_get_size(index,"number",&numberSize),0);
    numbers=(long*)malloc(sizeof(long)*numberSize);
    if (!numbers) exit(1);
    /*same as for "step"*/
    CODES_CHECK(codes_index_get_long(index,"number",numbers,&numberSize),0);
    printf("numberSize=%ld\n",(long)numberSize);
    for (i=0;i<numberSize;i++) printf("%ld ",numbers[i]);
    printf("\n");

    /*same as for "step"*/
    CODES_CHECK(codes_index_get_size(index,"shortName",&shortNameSize),0);
    shortName=(char**)malloc(sizeof(char*)*shortNameSize);
    if (!shortName) exit(1);
    /*same as for "step"*/
    CODES_CHECK(codes_index_get_string(index,"shortName",shortName,&shortNameSize),0);
    printf("shortNameSize=%ld\n",(long)shortNameSize);
    for (i=0;i<shortNameSize;i++) printf("%s ",shortName[i]);
    printf("\n");

    count=0;
    /* nested loops on the keys values of the index */
    /* different order of the nested loops doesn't affect performance*/
    for (i=0;i<shortNameSize;i++) {
        /* select the GRIB with shortName=shortName[i] */
        codes_index_select_string(index,"shortName",shortName[i]);

        for (l=0;l<levelSize;l++) {
            /* select the GRIB with level=levels[i] */
            codes_index_select_long(index,"level",levels[l]);

            for (j=0;j<numberSize;j++) {
                /* select the GRIB with number=numbers[i] */
                codes_index_select_long(index,"number",numbers[j]);

                for (k=0;k<stepSize;k++) {
                    /* select the GRIB with step=steps[i] */
                    codes_index_select_long(index,"step",steps[k]);

                    /* create a new codes_handle from the index with the constraints
                           imposed by the select statements. It is a loop because
                           in the index there could be more than one GRIB with those
                           constraints */
                    while ((h=codes_handle_new_from_index(index,&ret))!=NULL){
                        count++;
                        if (ret) {printf("error: %d\n",ret); exit(ret);}
                        lenshortName=200;
                        codes_get_string(h,"shortName",oshortName,&lenshortName);
                        codes_get_long(h,"level",&olevel);
                        codes_get_long(h,"number",&onumber);
                        codes_get_long(h,"step",&ostep);
                        printf("shortName=%s ",oshortName);
                        printf("level=%ld ",olevel);
                        printf("number=%ld ",onumber);
                        printf("step=%ld \n",ostep);
                        codes_handle_delete(h);
                    }
                    if (ret && ret!=GRIB_END_OF_INDEX ) {printf("error: %s\n",codes_get_error_message(ret)); exit(ret);}
                }
            }
        }
    }
    printf("  %d messages selected\n",count);

    codes_index_write(index,"out.gribidx");
    codes_index_delete(index);

    return 0;
}
