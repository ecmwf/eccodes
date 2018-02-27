/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>

#include "grib_api_internal.h"

typedef struct list {
    char path[1024];
    char name[1024];
    struct list* next;
} list;

int main(int argc, char *argv[])
{
    int i = 0;
    int j = 0;
    int fail = 0;
    grib_compiler compiler = {0,};

    grib_context* c = grib_context_get_default();
    grib_action *a;

    list *first = NULL;
    list *l = NULL;

    compiler.out = stdout;
    compiler.max = 4096;

    printf("#include \"grib_api_internal.h\"\n\n");
    for(i = 1; i < argc; i++){

        char name[1024];
        const char* p = argv[i];
        const char* path;
        while(*p && strncmp(p,"definitions",strlen("definitions"))) p++;
        path = p;

        if(!p) {
            fprintf(stderr,"Invalid name %s\n",argv[i]);
            exit(1);
        }

        strcpy(name,"grib_");
        j = strlen(name);
        while(*p) {
            if(isalnum(*p)) {
                name[j++] = *p;
            }
            else {
                name[j++] = '_';
            }
            p++;
        }
        name[j++] = 0;

        printf("static grib_action* %s(grib_context *ctx) {\n",name);
        printf("grib_action* a[%d] = {0,};\n\n",compiler.max);
        printf("grib_action* b[%d] = {0,};\n\n",compiler.max);

        compiler.cnt = 0;

        printf("/* %s */\n", path);
        if(!(a = grib_parse_file(c,argv[i])))
        {
            fail++;
            exit(1);
        }
        else
        {
            grib_compile_action_branch(a,&compiler,NULL);

        }
        printf("return a[0];\n");
        printf("}\n");

        l = calloc(sizeof(list),1);
        l->next = first;

        strcpy(l->name,name);
        strcpy(l->path,path);
        first = l;


    }

    printf("void grib_load_compiled_definitions(grib_context* ctx){\n");
    printf("grib_action_file* af;\n");
    printf("ctx = ctx ? ctx : grib_context_get_default();\n\n");
	printf("if(!ctx->grib_reader)\n");
	printf("  ctx->grib_reader =(grib_action_file_list*)grib_context_malloc_clear_persistent(ctx,sizeof(grib_action_file_list));");

    while(l) {
        printf("af =(grib_action_file*)grib_context_malloc_clear_persistent(ctx,sizeof(grib_action_file));\n");
        printf("af->root     = %s(ctx);\n",l->name);
        printf("af->filename = grib_context_strdup_persistent(ctx,\"COMPILED/%s\");\n",l->path);
        printf("grib_push_action_file(af,ctx->grib_reader);\n\n");

        l = l->next;
    }
    printf("}\n");

	printf("int main(int argc,char* argv[]) {grib_load_compiled_definitions(0);return 0 ;}\n");

    return fail;
}
