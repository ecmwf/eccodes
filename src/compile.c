/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *                                                                         *
 ***************************************************************************/

#include "grib_api_internal.h"


#define COMPILE_FLAGS(z) if(flags&z) { flags ^= ~z; fprintf(c->out,"%s%s",sep,#z);sep="|";  }

void grib_compile_flags(grib_compiler* c,long flags)
{
    char *sep = "";

    fprintf(c->out,"0x%ld",flags);
    return;

    if(flags == 0) {
        fprintf(c->out,"0");
        return;
    }

    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_READ_ONLY);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_DUMP);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_CAN_BE_MISSING);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_HIDDEN);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_CONSTRAINT);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_OVERRIDE);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_NO_COPY);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_COPY_OK);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_FUNCTION);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_DATA);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_NO_FAIL);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_TRANSIENT);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_STRING_TYPE);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_LONG_TYPE);
    COMPILE_FLAGS(GRIB_ACCESSOR_FLAG_LOWERCASE);

    if(flags) {
        fprintf(stderr,"Unknown flags %lx\n", flags);
        Assert(0);
    }
}

void grib_compile_action_branch(grib_action* a,grib_compiler* c, char* name)
{
    int first = 1;
    char var[80];
    char tmp[80];
    const char* save = c->var;
    int n = c->cnt++;
    c->var = tmp;
    if(c->cnt >= c->max) {
        fprintf(stderr,"Not enough variables %d\n",c->max);
        Assert(0);
    }
    sprintf(var,"a[%d]", n);
    while(a)
    {
        if(first) sprintf(tmp,"%s",var); else sprintf(tmp,"b[%d]->next",n); 
        grib_compile(a,c);
        fprintf(c->out,"b[%d] = %s;\n",n, c->var);
        a=a->next;
        first = 0;
    }
    c->var = save;

    if(name) strcpy(name, var);
}

void grib_compile_expression(grib_expression* e, grib_compiler *compiler)
{
    if(!e)
    {
        fprintf(compiler->out,"NULL");
    }
    else
    {
        grib_expression_compile(e,compiler);
    }
}

void grib_compile_arguments(grib_arguments* a, grib_compiler *compiler)
{
    if(!a)
        fprintf(compiler->out,"NULL");
    else {
        fprintf(compiler->out, "grib_arguments_new(ctx,");
        grib_compile_expression(a->expression, compiler);
        fprintf(compiler->out, ",");
        grib_compile_arguments(a->next,compiler);
        fprintf(compiler->out, ")");
    }
}
