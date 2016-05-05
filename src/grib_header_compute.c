/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


/*
  Author: B.Raoult
  Modified by Enrico Fucile
*/

#include "grib_api_internal.h"
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>

#if 0
GRIB_INLINE static int strcmp(const char* a,const char* b) {
  if (*a != *b) return 1;
  while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
  return (*a==0 && *b==0) ? 0 : 1;
}
#endif

#ifndef NUMBER
#define NUMBER(a)    (sizeof(a)/sizeof(a[0])) /* number of elem. of an array */
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#define ABS(a)       ((a)<0?-(a):(a))
#define ROUND(a)     ((long)((a)+0.5))
#define EQ(a,b)      ((*(a) == *(b)) && (strcmp(a,b) == 0))
#define LT(a,b)      ((*(a) <  *(b)) || (strcmp(a,b) < 0))


static grib_math *readpower(grib_context* c,char** form,int *err);
static grib_math *readatom(grib_context* c,char** form,int *err);
static grib_math *readfactor(grib_context* c,char** form,int *err);
static grib_math *readterm(grib_context* c,char** form,int *err);
static grib_math *readtest(grib_context* c,char** form,int *err);
static grib_math *readlist(grib_context* c,char** form,int* n,int *err);
static grib_math *readand(grib_context* c,char** form,int* err);
static grib_math *reador(grib_context* c,char** form,int* err);
static void advance(char** form);
static char *opname(char *p,int n);
/*
static long op_eq(long a, long b)     {return a == b;}
static long op_ne(long a, long b)     {return a != b;}
static long op_lt(long a, long b)     {return a <  b;}
static long op_gt(long a, long b)     {return a >  b;}
static long op_and(long a, long b)    {return a && b;}
static long op_or(long a, long b)     {return a || b;}
static long op_ge(long a, long b)     {return a >= b;}
static long op_le(long a, long b)     {return a <= b;}

static long op_bit(long a, long b)    {return a&(1<<b);}
static long op_bitoff(long a, long b) {return !op_bit(a,b);}

static long op_not(long a) {return !a;}
static long op_neg(long a) {return -a;}

static double op_neg_d(double a) {return -a;}

static long op_pow(long a, long b) {return grib_power(a,b);}
static long op_add(long a, long b) {return a+b;}
static long op_sub(long a, long b) {return a-b;}
static long op_div(long a, long b) {return a/b;}
static long op_mul(long a, long b) {return a*b;}
static long op_modulo(long a, long b) {return a%b;}

static double op_mul_d(double a, double b) {return a*b;}
static double op_div_d(double a, double b) {return a/b;}
static double op_add_d(double a, double b) {return a+b;}
static double op_sub_d(double a, double b) {return a-b;}

static double op_eq_d(double a, double b)     {return a == b;}
static double op_ne_d(double a, double b)     {return a != b;}
static double op_lt_d(double a, double b)     {return a <  b;}
static double op_gt_d(double a, double b)     {return a >  b;}
static double op_ge_d(double a, double b)     {return a >= b;}
static double op_le_d(double a, double b)     {return a <= b;}

static int op_eq_string(char* a,char* b) {
  if (a && b) return strcmp(a,b) == 0 ? 1 : 0;
  return 0;
}
static int op_ne_string(char* a,char* b) {
  if (a && b) return strcmp(a,b) == 0 ? 0 : 1;
  return 0;
}
static int op_lt_string(char* a,char* b) {
  if (a && b) return strcmp(a,b) < 0 ? 1 : 0;
  return 0;
}
static int op_gt_string(char* a,char* b) {
  if (a && b) return strcmp(a,b) > 0 ? 1 : 0;
  return 0;
}
static int op_ge_string(char* a,char* b) {
  if (a && b) return strcmp(a,b) >= 0 ? 1 : 0;
  return 0;
}
static int op_le_string(char* a,char* b) {
  if (a && b) return strcmp(a,b) <= 0 ? 1 : 0;
  return 0;
}
*/

typedef double real;
typedef int boolean;
typedef int err;
typedef real (*fop1)(real);
typedef real (*fop2)(real,real);

static void advance(char** form)
{
    (*form)++;
    while(isspace(**form)) (*form)++;
}

static grib_math *readatom(grib_context* c,char** form,int *err)
{
    grib_math *p;
    int i;
    char buf[1024];

    switch(**form)
    {
    case '(':
        advance(form);
        p = reador(c,form,err);
        if(**form != ')') {
            grib_context_log(c, GRIB_LOG_ERROR,"Formula: missing )");
            *err = GRIB_INTERNAL_ERROR;
        }
        advance(form);
        break;

    case '-':
        p        = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        p->arity = 1;
        p->name  = strdup("neg");
        Assert(p->name);
        advance(form);
        p->left  = readatom(c,form,err);
        break;

    case '!':
        p        = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        p->arity = 1;
        p->name  = strdup("neg");
        Assert(p->name);
        advance(form);
        p->left  = readatom(c,form,err);
        break;

    case '\0':
        grib_context_log(c, GRIB_LOG_ERROR,"Formula: syntax error");
        *err = GRIB_INTERNAL_ERROR;
        return NULL;
        /*NOTREACHED*/
        break;

    default:
        i = 0;

        if(**form == '\'' || **form == '"')
        {
            char achar = *((*form)++);
            while(**form && **form != achar)
                buf[i++] = *((*form)++);
            if(**form) (*form)++;
        } else
            while(isalpha(**form)||isdigit(**form)
                    || **form =='.' || **form == '_')
                buf[i++] = *((*form)++);

        buf[i] = 0;
        if(isspace(**form)) advance(form);

        p = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        p->name  = strdup(buf);
        Assert(p->name);
        p->left=0;

        switch(**form)
        {
        case '(':
            advance(form);
            p->arity = 0;
            p->left = readlist(c,form,&p->arity,err);
            if(**form != ')')
            {
                grib_context_log(c, GRIB_LOG_ERROR,"Formula: missing )");
                *err = GRIB_INTERNAL_ERROR;
            }
            advance(form);
            break;

        case '[':
            advance(form);
            p->arity = 0;
            p->left = readlist(c,form,&p->arity,err);
            if(**form != ']') {
                grib_context_log(c, GRIB_LOG_ERROR,"Formula: missing ]");
                *err = GRIB_INTERNAL_ERROR;
            }
            p->arity = -p->arity;
            advance(form);
            break;

        default:
            p->arity = 0;
            break;
        }

        break;
    }

    return p;
}

static char *opname(char *p,int n)
{
    char buf[5];
    strncpy(buf,p,n);
    buf[n] = 0;
    return strdup(buf);
}

void print_math(grib_math *m)
{
    if(m)
    {
        putchar('(');
        print_math(m->left);
        printf("%s",m->name);
        print_math(m->right);
        putchar(')');
    }
}

static grib_math *readpower(grib_context* c,char** form,int *err)
{
    grib_math *p = readatom(c,form,err);


    while(**form == '^' || (**form == '*' && *(*form + 1) == '*') )
    {
        grib_math *q  = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        q->left  = p;
        q->arity = 2;

        if(**form == '*') { advance(form); **form = '^'; }

        q->name     = opname(*form,1);
        advance(form);
        q->right    = readatom(c,form,err);
        p = q;
    }
    return p;
}

static grib_math *readlist(grib_context* c,char** form,int *n,int * err)
{
    grib_math *p;

    if(**form == ')') return NULL;

    p  = readtest(c,form,err);
    *n = 1;

    while(**form == ',')  {
        grib_math *q = (grib_math*)grib_context_malloc(c,sizeof(grib_math));

        (*n)++;

        q->left  = p;

        advance(form);

        q->right    = readtest(c,form,err);

        p = q;
    }
    return p;
}

static grib_math *readfactor(grib_context* c,char** form,int* err)
{
    grib_math *p = readpower(c,form,err);

    while(**form == '*' || **form == '/')  {
        grib_math *q = (grib_math*)grib_context_malloc(c,sizeof(grib_math));

        q->arity    = 2;
        q->left     = p;
        q->name     = opname(*form,1);

        advance(form);

        q->right    = readpower(c,form,err);

        p = q;
    }
    return p;
}

static grib_math *readterm(grib_context* c,char** form,int* err)
{
    grib_math *p = readfactor(c,form,err);
    while(**form == '+' || **form == '-' )  {
        grib_math *q = (grib_math*)grib_context_malloc(c,sizeof(grib_math));

        q->arity    = 2;
        q->left     = p;
        q->name     = opname(*form,1);

        advance(form);

        q->right    = readfactor(c,form,err);

        p = q;

    }
    return p;
}

static grib_math *readtest(grib_context* c,char** form,int* err)
{
    grib_math *p = readterm(c,form,err);
    while(**form == '<' || **form == '>' || **form == '=')  {
        grib_math *q = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        char *x = *form;
        int   n = 1;

        q->arity    = 2;
        q->left     = p;

        advance(form);
        if(**form == '=' || **form == '>') {
            n = 2;
            advance(form);
        }

        q->name = opname(x,n);

        q->right    = readterm(c,form,err);

        p = q;

    }
    return p;
}

static grib_math *readand(grib_context* c,char** form,int* err)
{
    grib_math *p = readtest(c,form,err);
    while(**form == '&' )  {
        grib_math *q = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        char *x = *form;
        int   n = 1;

        q->arity    = 2;
        q->left     = p;

        advance(form);
        if(**form == '&') {
            n = 2;
            advance(form);
        }

        q->name = opname(x,n);

        q->right    = readtest(c,form,err);

        p = q;

    }
    return p;
}

static grib_math *reador(grib_context* c,char** form,int* err)
{
    grib_math *p = readand(c,form,err);
    while(**form == '|' )  {
        grib_math *q = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        char *x = *form;
        int   n = 1;

        q->arity    = 2;
        q->left     = p;

        advance(form);
        if(**form == '|' ) {
            n = 2;
            advance(form);
        }

        q->name = opname(x,n);

        q->right    = readand(c,form,err);

        p = q;

    }
    return p;
}

grib_math *grib_math_clone(grib_context* c,grib_math *m)
{
    grib_math *n = NULL;
    if(m)  {
        n = (grib_math*)grib_context_malloc(c,sizeof(grib_math));
        n->arity = m->arity;
        n->name  = strdup(m->name);
        Assert(n->name);
        n->left  = grib_math_clone(c,m->left);
        n->right = grib_math_clone(c,m->right);
    }
    return n;
}

void grib_math_delete (grib_context* c,grib_math* m)
{
    grib_math *left=0,*right=0;
    left=m->left;
    right=m->right;
    if (m->name) free(m->name);
    grib_context_free(c,m);
    if (left) grib_math_delete(c,left);
    if (right) grib_math_delete(c,right);
}

grib_math *grib_math_new(grib_context* c,const char* formula,int *err)
{
    grib_math *x;
    char* f=0;
    char* fsave=0;

    *err=0;

    if (!formula) {*err=GRIB_INVALID_ARGUMENT;return NULL;}

    f=strdup(formula);
    Assert(f);
    fsave=f;

    x = reador(c,&f,err);
    if(*err!=GRIB_SUCCESS) return NULL;

    if(*f) {
        grib_context_log(c, GRIB_LOG_ERROR,
                "grib_math_new : Part of the formula was not processed: '%s'",f);
        return NULL;
    }

    free(fsave);

    return x;
}
