/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#ifdef HAVE_NETCDF

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include <time.h>
#include <limits.h>
#include <float.h>
#include <netcdf.h>

#include "grib_tools.h"

char* grib_tool_description = "Convert a GRIB file to netCDF format.";
char* grib_tool_name = "grib_to_netcdf";
char* grib_tool_usage = "[options] grib_file grib_file ... ";
static char argvString[2048];

/*=====================================================================*/

static grib_context* ctx = NULL;
static double global_missing_value = 9.9692099683868690e+36; /* See GRIB-953 */

/*===============================================================================*/
/* request from mars client */
/*===============================================================================*/

#define NUMBER(x) (sizeof(x)/sizeof(x[0]))

typedef int boolean;
typedef int err;
typedef off_t file_offset;

static int FALSE = 0;
static int TRUE = 1;

static int files = 0;

struct value {
    struct value *next;
    char* name;
};
typedef struct value value;

struct request {
    struct request *next;
    struct parameter *params;
    char* name;
    int order;
};
typedef struct request request;
/*  request part */
/* language part */

struct parameter {
    struct parameter *next;
    struct value *values;
    char* name;
    int count;
};
typedef struct parameter parameter;

static const char* get_value(const request*, const char* name, int n);
static boolean parsedate(const char *name, long* julian, long *second, boolean* isjul);

static boolean eq_string(const char *l, const char *r)
{
    if(l && r)
        return strcmp(l, r) == 0;
    return FALSE;
}

static boolean eq_integer(const char *l, const char *r)
{
    if(l && r)
        return atol(l) == atol(r);
    return FALSE;
}

static boolean eq_null(const char *l, const char *r)
{
    return TRUE;
}

static boolean eq_coord(const char *l, const char *r)
{
    if(l && r)
        return strcmp(l, r) == 0;
    return FALSE;
}

static boolean eq_range(const char *l, const char *r)
{
    if(l && r)
        return strcmp(l, r) == 0;
    return FALSE;
}

static boolean eq_date(const char *l, const char *r)
{
    if(l && r)
        return strcmp(l, r) == 0;
    return FALSE;
}

static boolean eq_param(const char *l, const char *r)
{
    if(l && r)
        return strcmp(l, r) == 0;
    return FALSE;
}

static boolean eq_time(const char *l, const char *r)
{
    if(l && r)
        return strcmp(l, r) == 0;
    return FALSE;
}

static value *new_value(const char *name)
{
    value *v = (value*)calloc(sizeof(value), 1);
    Assert(v);
    v->name = grib_context_strdup(ctx, name);
    return v;
}

static parameter *find_parameter(const request *r, const char *parname)
{
    if(!parname)
        return 0;
    if(r)
    {
        parameter *p = r->params;
        while(p)
        {
            if(strcmp(p->name, parname) == 0)
                return p;
            p = p->next;
        }
    }
    return NULL;
}

static void _reqmerge(parameter *pa, const parameter *pb, request *a)
{
    const value *vb = pb->values;

    if(strcmp(pa->name, pb->name) != 0)
        return;

    while(vb)
    {
        value *va = pa->values;
        value *last = 0;
        const char *nb = vb->name;
        boolean add = TRUE;

        while(va)
        {
            if(strcmp(va->name, nb) == 0)
            {
                add = FALSE;
                break;
            }

            last = va;
            va = va->next;
        }

        if(add)
        {
            value* v = new_value(nb);
            if(last)
                last->next = v;
            else
                pa->values = v;
            pa->count = 0;
        }

        vb = vb->next;
    }

}

/* Fast version if a && b same */
static boolean _reqmerge1(request *a, const request *b)
{
    parameter *pa = a->params;
    const parameter *pb = b->params;

    while(pa && pb)
    {
        if(strcmp(pa->name, pb->name) != 0)
            return FALSE;

        _reqmerge(pa, pb, a);

        pa = pa->next;
        pb = pb->next;
    }

    return (pa == NULL && pb == NULL);
}

static void free_one_value(value *p)
{
    grib_context_free(ctx, p->name);
    grib_context_free(ctx, p);
}

static void free_all_values(value *p)
{
    while(p)
    {
        value *q = p->next;
        free_one_value(p);
        p = q;
    }
}

/* Convert the first part of the string 'p' to a number (x) and set n to its length. */
/* Return the rest of the string */
static const char *parse1(const char *p, int* x, int *n)
{
    *x = *n = 0;
    while(*p && isdigit(*p))
    {
        (*x) *= 10;
        (*x) += *p - '0';
        (*n)++;
        p++;
    }
    return p;
}

static boolean is_number(const char *name)
{
    const char *p = name;
    int x, n;

    if(p == 0 || *p == 0)
        return FALSE;

    if(*p == '-')
        p++;
    else if(*p == '+')
        p++;

    p = parse1(p, &x, &n);
    if(n == 0 && *p != '.')
        return FALSE;

    if(*p == '.')
    {
        p++;
        p = parse1(p, &x, &n);
    }

    if(*p == 'e' || *p == 'E')
    {
        p++;
        if(*p == '-')
            p++;
        else if(*p == '+')
            p++;
        p = parse1(p, &x, &n);
        if(n == 0)
            return FALSE;
    }

    return *p == 0 ? TRUE : FALSE;
}
static parameter *new_parameter(char *name, value *v)
{
    parameter *p = (parameter*)calloc(sizeof(parameter), 1);
    Assert(p);
    p->name = grib_context_strdup(ctx, name);
    p->values = v;
    return p;
}

static void put_value(request *r, const char *parname, const char *valname, boolean append, boolean unique, boolean ascending)
{
    parameter *p;
    value *v;

    if(!r)
        return;

    if((p = find_parameter(r, parname)) != NULL)
    {
        if(append)
        {
            value *a = p->values, *b = NULL, *c = NULL;
            while(a)
            {
                b = a;
                if(unique)
                {
                    if(is_number(a->name) && is_number(valname))
                    {
                        if(atof(a->name) == atof(valname))
                            return;
                    }
                    else if(strcmp(a->name, valname) == 0)
                        return;
                }

                if(ascending)
                {
                    if(is_number(a->name))
                    {
                        if(atof(valname) < atof(a->name))
                            break;
                    }
                    else if(strcmp(valname, a->name) < 0)
                        break;
                }
                c = b;
                a = a->next;
            }
            v = new_value(grib_context_strdup(ctx, valname));
            if(ascending)
            {
                if(c)
                {
                    if(b && b != c)
                        v->next = b;
                    c->next = v;
                }
                else
                {
                    if(a)
                        v->next = a;
                    p->values = v;
                }
            }
            else
            {
                if(b)
                    b->next = v;
                else
                    p->values = v;
            }
            /* p->count++; */
            p->count = 0;
        }
        else
        {
            if(p->values)
            {
                free_all_values(p->values->next);
                p->values->next = NULL;
                /* p->count = 1; */
                p->count = 0;
                if(strcmp(p->values->name, valname) == 0)
                    return;
                else
                {
                    grib_context_free(ctx, p->values->name);
                    p->values->name = grib_context_strdup(ctx, valname);
                }
            }
            else
            {
                v = new_value(grib_context_strdup(ctx, valname));
                p->values = v;
                /* p->count = 1; */
                p->count = 0;
            }
        }
    }
    else
    {
        parameter *q = NULL;
        parameter *s = r->params;
        v = new_value(grib_context_strdup(ctx, valname));
        p = new_parameter(grib_context_strdup(ctx, parname), v);
        while(s)
        {
            q = s;
            s = s->next;
        }
        if(q)
            q->next = p;
        else
            r->params = p;
    }
}

static void add_value(request *r, const char *parname, const char *fmt, ...)
{
    char buffer[1024];
    va_list list;

    va_start(list, fmt);
    vsprintf(buffer, fmt, list);
    va_end(list);

    put_value(r, parname, buffer, TRUE, FALSE, FALSE);
    va_end(list);
}

static void _reqmerge2(request *a, const request *b)
{
    const parameter *pb = b->params;

    while(pb)
    {
        parameter* pa = find_parameter(a, pb->name);

        if(pa == NULL)
        {
            value *v = pb->values;
            while(v)
            {
                put_value(a, pb->name, v->name, TRUE, TRUE, FALSE);
                v = v->next;
            }

        }
        else
        {
            _reqmerge(pa, pb, a);
        }

        pb = pb->next;
    }
}

static void reqmerge(request *a, const request *b)
{
    if(a && b)
    {
        if(!_reqmerge1(a, b))
            _reqmerge2(a, b);
    }
}

static void save_name(FILE *f, const char* name, int n)
{

    int i = 0, cnt = 0;

    if(name == NULL)
        name = "(null)";

    cnt = fprintf(f, "%s", name);

    for(i = cnt; i < n; i++)
        putc(' ', f);
}

static void save_one_value(FILE *f, value *r)
{
    save_name(f, r->name, 0);
}

static void save_all_values(FILE *f, value *r)
{
    while(r)
    {
        save_one_value(f, r);
        if(r->next)
            putc('/', f);
        r = r->next;
    }
}

static void save_all_parameters(FILE *f, parameter *r)
{
    while(r)
    {

        if(r->values)
        {
            fprintf(f, ",\n    ");
            save_name(f, r->name, 10);
            fprintf(f, " = ");
            save_all_values(f, r->values);
        }

        r = r->next;
    }
    putc('\n', f);
}

static void save_one_request(FILE *f, const request *r)
{
    if(r)
    {
        save_name(f, r->name, 0);
        save_all_parameters(f, r->params);
        putc('\n', f);
    }
}

static void save_all_requests(FILE *f, const request *r)
{
    while(r)
    {
        save_one_request(f, r);
        r = r->next;
    }
}

/* Not used for the moment
static void print_one_request(const request *r)
{
    save_one_request(stdout, r);
}
 */

static void print_all_requests(const request *r)
{
    save_all_requests(stdout, r);
}

static void free_one_parameter(parameter *p)
{
    grib_context_free(ctx, p->name);
    free_all_values(p->values);
    /*free_all_requests(p->interface);*/
    grib_context_free(ctx, p);
}

static void free_all_parameters(parameter *p)
{
    while(p)
    {
        parameter *q = p->next;
        free_one_parameter(p);
        p = q;
    }
}

static void free_one_request(request *r)
{
    grib_context_free(ctx, r->name);
    free_all_parameters(r->params);
    grib_context_free(ctx, r);
}

static void free_all_requests(request *p)
{
    while(p)
    {
        request *q = p->next;
        free_one_request(p);
        p = q;
    }
}

static void set_value(request *r, const char *parname, const char *fmt, ...)
{
    char buffer[10240];
    va_list list;

    va_start(list, fmt);
    vsprintf(buffer, fmt, list);
    va_end(list);

    put_value(r, parname, buffer, FALSE, FALSE, FALSE);
}

static err handle_to_request(request *r, grib_handle* g)
{
    grib_keys_iterator* ks;
    char name[256];
    char value[256];
    size_t len = sizeof(value);
    int e = 0;

    if(!g)
        return -1;

    /* printf("------------\n"); */
    ks = grib_keys_iterator_new(g, GRIB_KEYS_ITERATOR_ALL_KEYS, "mars");

    while(grib_keys_iterator_next(ks))
    {

        strcpy(name, grib_keys_iterator_get_name(ks));

        if((e = grib_keys_iterator_get_string(ks, value, &len)) != GRIB_SUCCESS)
            grib_context_log(ctx, GRIB_LOG_ERROR, "Cannot get %s as string %d (%s)", name, e, grib_get_error_message(e));

        set_value(r, name, "%s", value);
        len = sizeof(value);
    }

    /*
     Assert(grib_get_long(g, "validityDate", &l ) == 0);
     set_value(r, "validityDate", "%ld", l);

     Assert(grib_get_long(g, "validityTime", &l ) == 0);
     set_value(r, "validityTime", "%ld", l);
     */

    len = sizeof(value);
    if(grib_get_string(g, "cfVarName", name, &len) == 0)
    {
        if(strcmp(name,"unknown") != 0)
        {
            set_value(r, "param", "%s", name);
        }
        else
        {
            len = sizeof(value);
            if(grib_get_string(g, "shortName", name, &len) == 0)
            {
                set_value(r, "param", "%s", name);
            }
        }
    }

    len = sizeof(value);
    if(grib_get_string(g, "name", name, &len) == 0)
    {
        if(strcmp(name,"unknown") != 0)
        {
            set_value(r, "_long_name", "%s", name);
        }
    }

    len = sizeof(value);
    if(grib_get_string(g, "units", name, &len) == 0)
    {
        if(strcmp(name,"unknown") != 0)
        {
            set_value(r, "_units", "%s", name);
        }
    }

    len = sizeof(value);
    if(grib_get_string(g, "cfName", name, &len) == 0)
    {
        if(strcmp(name,"unknown") != 0)
        {
            set_value(r, "_cf_name", "%s", name);
        }
    }

    grib_keys_iterator_delete(ks);
    return e;
}

/*===============================================================================*/
/*===============================================================================*/

typedef boolean (*namecmp)(const char *, const char *);

typedef struct hypercube {
    request *cube;
    request *r;
    request *iterator;
    char *set;
    int count;
    int size;
    int max;
    int* index_cache;
    int index_cache_size;
    namecmp *compare;
} hypercube;

typedef struct axis_t {
    const char *name;
    namecmp compare;
} axis_t;

/* This should be c++ ... */

typedef enum field_state {
    unknown, packed_mem, packed_file, expand_mem
} field_state;

typedef struct {
    int refcnt;
    request* r;
} field_request;

/* One field .. */

typedef struct field {
    int refcnt;

    field_state shape;

    grib_handle* handle;
    double* values;
    size_t value_count;

    /* if on file */

    file_offset offset;
    size_t length;
    grib_file *file;

    /* missing fields/values */

    /*boolean is_missing;*/ /* field is missing */
    boolean has_bitmap; /* field has missing values (= bitmap) */

    field_request *r;

} field;

typedef struct fieldset {

    int refcnt;

    /* if fields */

    int max;
    int count;

    field **fields;

} fieldset;

/*
 #define MISSING_VALUE(n)      ((n) == missing_value)
 #define MISSING_FIELD(f)      ((f)->missing)
 #define FIELD_HAS_BITMAP(f)   ((f)->bitmap)

 #define FASTNEW(type)         (type*)calloc(sizeof(type),1)
 #define grib_context_free(ctx,x)           grib_context_free(ctx,x)
 */

static field *get_field(fieldset *v, int n, field_state shape);
static hypercube *new_hypercube_from_mars_request(const request *r);
static void release_field(field *g);
static int count_axis(const hypercube *h);
static const char* get_axis(const hypercube *h, int pos);
static const char* get_axis(const hypercube *h, int pos);
static int cube_order(const hypercube *h, const request *r);
static void free_hypercube(hypercube *h);
static int _cube_position(const hypercube *h, const request *r, boolean remove_holes);

static value *clone_one_value(const value *p)
{
    value *q = (value*)calloc(sizeof(value), 1);
    Assert(q);
    q->next = NULL;
    q->name = grib_context_strdup(ctx, p->name);
    return q;
}

static value *clone_all_values(const value *p)
{
    if(p)
    {
        value *q = clone_one_value(p);
        q->next = clone_all_values(p->next);
        /* q->alias = cone_value(p->alias); */
        return q;
    }
    return NULL;
}

static parameter *clone_one_parameter(const parameter *p)
{
    parameter *q = (parameter*)calloc(sizeof(parameter), 1);
    Assert(q);
    q->next = NULL;
    q->name = grib_context_strdup(ctx, p->name);
    q->values = clone_all_values(p->values);
    return q;
}

static parameter *clone_all_parameters(const parameter *p)
{
    if(p)
    {
        parameter *q = clone_one_parameter(p);
        q->next = clone_all_parameters(p->next);
        return q;
    }
    return NULL;
}

static request *clone_one_request(const request *r)
{
    if(r)
    {
        request *p = (request*)calloc(sizeof(request), 1);
        Assert(p);
        p->name = grib_context_strdup(ctx, r->name);

        p->params = clone_all_parameters(r->params);
        p->next = NULL;
        return p;
    }
    return NULL;
}

static request *new_request(const char *name, parameter *p)
{
    request *r = (request *)calloc(sizeof(request), 1);
    Assert(r);
    r->name = grib_context_strdup(ctx, name);
    r->params = p;
    return r;
}

static request *empty_request(const char *name)
{
    return new_request(name ? name : "", NULL);
}

static field_request* new_field_request(request *r)
{
    field_request* g = (field_request*)grib_context_malloc_clear(ctx, sizeof(field_request));
    g->r = clone_one_request(r);
    return g;
}

static void free_field_request(field_request* g)
{
    if(!g)
        return;
    g->refcnt--;
    if(g->refcnt <= 0)
    {
        free_all_requests(g->r);
        grib_context_free(ctx, g);
    }
}

static void free_field(field *g)
{
    if(!g)
        return;
    g->refcnt--;
    if(g->refcnt <= 0)
    {
        /*free_gribfile(g->file);*/
        free_field_request(g->r);
        if(g->values)
            grib_context_free(ctx, g->values);
        grib_handle_delete(g->handle);
        grib_context_free(ctx, g);
    }
}

static void free_fieldset(fieldset *v)
{
    if(!v)
        return;
    v->refcnt--;
    if(v->refcnt <= 0)
    {
        int i;
        grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: free_fieldset (%d fields) : ", v->count);

        for(i = 0; i < v->count; i++)
            free_field(v->fields[i]);

        grib_context_free(ctx, v->fields);
        grib_context_free(ctx, v);
    }
}

static field *new_field()
{
    return (field*)grib_context_malloc_clear(ctx, sizeof(field));
}

#define INIT_SIZE 1024
static void grow_fieldset(fieldset *v, int n)
{
    int m = v->count;
    int x = v->max;

    if(n < v->count)
        return;

    v->count = n;

    while(v->count >= v->max)
        if(v->max < INIT_SIZE)
            v->max = INIT_SIZE;
        else
            v->max += v->max / 2 + 1;

    if(v->max != x)
    {
        int i;
        if(v->fields == NULL)
        {
            v->fields = (field**) grib_context_malloc(ctx, sizeof(field*) * v->max);
            Assert(v->fields);
        }
        else
        {
            field** f = (field**) grib_context_malloc(ctx, sizeof(field*) * v->max);
            Assert(f);
            for(i = 0; i < m; i++)
                f[i] = v->fields[i];
            grib_context_free(ctx, v->fields);
            v->fields = f;
        }

        for(i = m; i < v->max; i++)
            v->fields[i] = NULL;
    }
}

static fieldset *new_fieldset(int n)
{
    fieldset *f = (fieldset*)calloc(sizeof(fieldset), 1);
    Assert(f);
    grow_fieldset(f, n);
    return f;
}

static field* read_field(grib_file *file, file_offset pos, long length)
{
    field *g = new_field();

    g->file = file;
    g->offset = pos;
    g->length = length;
    g->shape = packed_file;

    return g;
}

static err set_field(fieldset *v, field *g, int pos)
{
    if(pos >= 0)
    {
        field *h;
        grow_fieldset(v, pos + 1);
        h = v->fields[pos];
        v->fields[pos] = g;
        g->refcnt++;
        if(h)
            free_field(h);
    }
    return 0;
}

static void count_parval(parameter *p)
{
    int n = 0;
    value *v = p->values;

    while(v)
    {
        n++;
        v = v->next;
    }

    p->count = n;

}

static int count_values(const request *r, const char *parname)
{
    parameter *p = find_parameter(r, parname);

    if(p == NULL)
        return 0;
    if(p->count)
        return p->count;

    count_parval(p);

    return p->count;
}
static const char *get_value(const request *r, const char *parname, int nth)
{
    parameter *p = find_parameter(r, parname);
    value *v;
    int i = 0;

    if(p == NULL)
        return NULL;

    if(!p->count)
        count_values(r, parname);

    v = p->values;

    while(v)
    {
        if(nth == i++)
            return v->name;
        v = v->next;
    }

    return NULL;
}

static err to_packed_mem(field *g)
{

    if(g->shape == packed_mem)
        return 0;

    if(g->shape == expand_mem)
    {
        if(g->values)
            grib_context_free(ctx, g->values);
        g->values = NULL;
        g->value_count = 0;
        g->shape = packed_mem;

        return 0;
    }

    if(g->shape == packed_file)
    {

    }
    return 0;
}

static err to_expand_mem(field *g)
{
    err e = 0;

    Assert(g);
    if(g->shape == expand_mem)
        return 0;

    if(g->shape == packed_file)
    {
        const void* dummy = NULL;

        grib_file* file = grib_file_open(g->file->name, "r", &e);
        if(!file || !file->handle)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR | GRIB_LOG_PERROR, "%s", g->file->name);
            return -1;
        }

        fseeko(file->handle, g->offset, SEEK_SET);

        g->handle = grib_handle_new_from_file(ctx, file->handle, &e);
        Assert(g->handle);

        if(g->handle)
            grib_get_message(g->handle, &dummy, &g->length);

        grib_file_close(file->name, 0, &e);
        if(!g->handle)
            return -1;

        if(g->values)
            grib_context_free(ctx, g->values);
        g->values = NULL;
    }

    if(g->values == NULL)
    {
        size_t count = 0;
        long bitmap = 0;

        if((e = grib_get_size(g->handle, "values", &g->value_count)))
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get number of values %s", grib_get_error_message(e));
            return e;
        }

        count = g->value_count;

        if((e = grib_set_double(g->handle, "missingValue", global_missing_value)))
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot set missingValue %s", grib_get_error_message(e));
            return e;
        }

        g->values = (double*) grib_context_malloc(ctx, sizeof(double) * g->value_count);
        if((e = grib_get_double_array(g->handle, "values", g->values, &count)))
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get decode values %s", grib_get_error_message(e));
            return e;
        }

        if(count != g->value_count)
            grib_context_log(ctx, GRIB_LOG_FATAL, "ecCodes: value count mismatch %d %d", count, g->value_count);

        if((e = grib_get_long(g->handle, "bitmapPresent", &bitmap)))
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get bitmapPresent %s", grib_get_error_message(e));
            return e;
        }

        g->has_bitmap = (bitmap != 0);

#ifdef COMEBACK
        set g->missing
#endif
    }

    g->shape = expand_mem;

    return e;
}

static void set_field_state(field *g, field_state shape)
{
    switch(shape)
    {
    case expand_mem:
        to_expand_mem(g);
        break;

    case packed_mem:
        to_packed_mem(g);
        break;

    case packed_file:
        release_field(g);
        break;

    default:
        grib_context_log(ctx, GRIB_LOG_FATAL, "Internal error %s %d", __FILE__, __LINE__);
        break;

    }
}

static field *get_field(fieldset *v, int n, field_state shape)
{
    field *g = v->fields[n];
    set_field_state(g, shape);
    return g;
}

static void release_field(field *g)
{
    if(g->file)
    {
        if(g->values)
            grib_context_free(ctx, g->values);
        g->values = NULL;
        g->shape = packed_file;
        grib_handle_delete(g->handle);
        g->handle = NULL;
    }
}

static request* field_to_request(field* f)
{
    if(f->r == 0)
    {
        field_state state = f->shape;
        request *r = empty_request(
#ifdef COMEBACK
                ((f->ksec1 == NULL) || (f->ksec1[2] != mars.computeflg)) ? "GRIB" : "COMPUTED");
#else
        "GRIB");
#endif

        set_field_state(f, packed_mem);
        handle_to_request(r, f->handle);
        set_field_state(f, state);

        f->r = new_field_request(r);
        free_all_requests(r);
    }
    return f->r->r;
}

static request* fieldset_to_request(fieldset *fs)
{
    int i;
    request* r = empty_request("GRIB");

    if(!fs)
        return 0;

    for(i = 0; i < fs->count; i++)
    {
        request *s = field_to_request(fs->fields[i]);
        reqmerge(r,s);
    }

    return r;
}

/*===============================================================================*/
/* hypercube from mars client */
/*===============================================================================*/

static boolean eq_string(const char *, const char *);
static boolean eq_integer(const char *, const char *);
static boolean eq_range(const char *, const char *);
static boolean eq_param(const char *, const char *);
static boolean eq_coord(const char *, const char *);
static boolean eq_date(const char *, const char *);
static boolean eq_time(const char *, const char *);
static boolean eq_null(const char *, const char *);

static axis_t global_axis[] =
{

        /* From dhsbase.c 'check_grib' */
        { "class", eq_string, },
        { "type", eq_string, },
        { "stream", eq_string, },
        { "levtype", eq_string, },

        { "origin", eq_string, },
        { "product", eq_string, },
        { "section", eq_string, },
        { "method", eq_integer, },
        { "system", eq_integer, },

        /* testing */
        /* {"repres",      eq_null, }, */

        /* from field order */
        { "date", eq_date, },
        { "refdate", eq_date, },
        { "hdate", eq_date, },
        { "time", eq_time, },

        { "reference", eq_range, },
        { "step", eq_range, },
        { "fcmonth", eq_integer, },
        { "fcperiod", eq_range, },

        { "leadtime", eq_range, },
        { "opttime", eq_range, },

        { "expver", eq_string, },
        { "domain", eq_string, },

        { "diagnostic", eq_integer, },
        { "iteration", eq_integer, },

        { "quantile", eq_range, },
        { "number", eq_integer, },

        { "levelist", eq_coord, },
        { "latitude", eq_coord, },
        { "longitude", eq_coord, },
        { "range", eq_range, },

        { "param", eq_param, },

        { "ident", eq_integer, },
        { "obstype", eq_integer, },
        { "instrument", eq_integer, },

        { "frequency", eq_integer, },
        { "direction", eq_integer, },

        { "channel", eq_integer, }, };

static int axisindex(const char *name)
{
    int i = 0;
    for(i = 0; i < NUMBER(global_axis); i++)
    {
        if(strcmp(name, global_axis[i].name) == 0)
            return i;
    }
    return -1;
}

static namecmp comparator(const char *name)
{
    static char *dontcompare = NULL;
    static boolean first = 1;
    int i = 0;

    if(first)
    {
        dontcompare = getenv("MARS_DONT_CHECK");
        first = FALSE;
    }

    if(dontcompare != NULL)
    {
        if(strcmp(dontcompare, name) == 0)
            return eq_null;
    }

    if((i = axisindex(name)) != -1)
        return global_axis[i].compare;
    grib_context_log(ctx, GRIB_LOG_ERROR, "No comparator for %s", name);
    return eq_string;
}

/********************/
/* index accessors  */
/********************/

static int count_index(const hypercube *h)
{
    int i = 0, n = 0;

    for(i = 0; i < h->size; ++i)
        n += h->set[i];
    return n;
}

static int count_holes(const hypercube *h, int cnt)
{
    int i = 0, n = 0;

    for(i = 0; i < cnt; ++i)
        n += h->set[i];

    return (n == cnt) ? 0 : (cnt - n);
}

static void reset_index(hypercube *h, int v)
{
    memset(h->set, v, h->size);
}

static void set_index(hypercube *h, int index, int value)
{
    if(index < 0 || index >= h->count)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error, bad hypercube index %d", index);
        exit(1);
    }

    if(index >= h->max)
    {
        int old = h->max;
        while(index >= h->max)
            h->max += 4096;

        h->set = h->set ? (char*)grib_context_realloc(ctx, h->set, h->max) : (char*)grib_context_malloc(ctx, h->max);
        Assert(h->set);
        memset(h->set + old, 0, h->max - old);
    }

    if(index >= h->size)
        h->size = index + 1;

    h->set[index] = value;
}

/**************************/
/* End of index accessors */
/**************************/

/*******************/
/* axis accessors  */
/*******************/

static int count_axis(const hypercube *h)
{
    if(h && h->cube)
        return count_values(h->cube, "axis");

    return -1;
}

static const char* get_axis(const hypercube *h, int pos)
{
    const char *axis = NULL;
    if(pos < count_axis(h))
    {
        axis = get_value(h->cube, "axis", pos);
    }
    return axis;
}

static void add_axis(hypercube *h, const char *axis)
{
    add_value(h->cube, "axis", "%s", axis);
}

static void unset_value(request *r, const char *parname)
{
    parameter *p, *q = NULL;
    if(!r)
        return;

    p = r->params;
    while(p)
    {
        if(strcmp(parname, p->name) == 0)
        {
            if(q)
                q->next = p->next;
            else
                r->params = p->next;
            free_one_parameter(p);
            return;
        }
        q = p;
        p = p->next;
    }
}

static void reset_axis(hypercube *h)
{
    unset_value(h->cube, "axis");
}

static void valcpy(request *a, request *b, char *aname, char *bname)
{
    parameter *p;
    if(a && b && (p = find_parameter(b, bname)))
    {
        boolean z = FALSE;
        value *v = p->values;
        while(v)
        {
            put_value(a, aname, v->name, z, FALSE, FALSE);
            z = TRUE;
            v = v->next;
        }
    }
}

static void cube_values(hypercube *h, const char *p)
{
    valcpy(h->cube, h->r, (char *) p, (char *) p);
}

static int count_dimensions(const hypercube *, const char *);

static int set_axis(hypercube *h)
{
    int i = 0;
    int count = (h && h->r) ? 1 : -1;

    reset_axis(h);
    for(i = (NUMBER(global_axis) - 1); i >= 0; --i)
    {
        int n = count_dimensions(h, global_axis[i].name);
        if(n > 1)
        {
            add_axis(h, global_axis[i].name);
            cube_values(h, global_axis[i].name);
            count *= n;
        }
    }

    return count;
}

/*************************/
/* End of axis accessors */
/*************************/

/*******************/
/* Cube dimensions */
/*******************/

static int count_dimensions(const hypercube *h, const char *axis)
{
    int dims = -1;
    if(h && h->r)
        dims = count_values(h->r, axis);
    return dims;
}

/**************************/
/* End of cube dimensions */
/**************************/

/**************************/
/* Auxiliary functions    */
/**************************/

static int count_hypercube(const request *r)
{
    int i = 0, count = 1;
    for(i = 0; i < NUMBER(global_axis); ++i)
    {
        int c = count_values(r, global_axis[i].name);
        count *= c ? c : 1;
    }

    return count;
}

static int cube_order(const hypercube *h, const request *r)
{
    return _cube_position(h, r, TRUE);
}

static int cube_position(const hypercube *h, const request *r)
{
    return _cube_position(h, r, FALSE);
}

static void reserve_index_cache(hypercube *h, int size)
{
    if(size == 0)
        return;

    if(h->index_cache != 0)
        grib_context_free(ctx, h->index_cache);
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: Allocating hypercube index_cache: %d entries", size);
    h->index_cache = (int*)calloc(sizeof(int), size);
    Assert(h->index_cache);
    h->index_cache_size = size;
}

static int _cube_position(const hypercube *h, const request *r, boolean remove_holes)
{
    request *cube = h->cube;
    int c = count_axis(h);
    int index = 0;
    int i = 0;
    int n = 1;
    int ok = 0;

    if(h->index_cache == 0 || h->index_cache_size != c)
        reserve_index_cache((hypercube*) h, c);

    for(i = 0; i < c; ++i)
    {
        const char *axis = get_axis(h, i);
        const char *v = get_value(r, axis, 0);
        const char *w = NULL;
        int dims = count_dimensions(h, axis);
        int k = 0;
        int count = count_values(cube, axis);
        int last = h->index_cache[i];

        for(k = 0; k < count; k++)
        {
            int j = (k + last) % count;
            w = get_value(cube, axis, j);
            if(h->compare ? h->compare[i](w, v) : (strcmp(w, v) == 0))
            {
                index += j * n;
                n *= dims;
                ok++;
                ((hypercube*) h)->index_cache[i] = j;
                break;
            }
            else
                grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: _cube_position, %s, %s != %s [%scompare function available]", axis, w, v, h->compare ? "" : "no ");
        }
    }

    if(remove_holes)
    {
        int holes = 0;
        if(count_index(h) != h->size)
            holes = count_holes(h, index);
        index -= holes;
    }

    return (ok == c) ? index : -1;
}

static void cube_indexes(
        const hypercube *h, request *r,
        char** times_array, size_t times_array_size,
        int *indexes, int size)
{
    request *cube = h->cube;
    int c = count_axis(h);
    int i = 0;
    int index = 0;
    int n = 1;
    int ok = 0;

    if(size < c)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error in cube_indexes. size=%d < axis=%d", size, c);
    }

    if(h->index_cache == 0 || h->index_cache_size != c)
        reserve_index_cache((hypercube*) h, c);

    for(i = 0; i < c; ++i)
    {
        const char *axis = get_axis(h, i);
        const char *v = get_value(r, axis, 0);
        const char *w = NULL;
        int dims = count_dimensions(h, axis);
        int j = 0;
        int k = 0;
        int count = count_values(cube, axis);
        int last = h->index_cache[i];
        const boolean is_time_axis = (strcmp(axis, "time")==0);
        if (is_time_axis) {
            Assert(times_array);
            Assert(times_array_size == count);
        }

        for(k = 0; k < count; k++)
        {
            j = (k + last) % count;
            if (is_time_axis) {
                /* GRIB-792: use fast lookup */
                Assert( j >=0 && j < times_array_size );
                w = times_array[j];
                /* For testing:
                 * Assert( strcmp(w, get_value(cube, axis, j))==0 );
                 * */
            }
            else {
                /* slow access method */
                w = get_value(cube, axis, j);
            }

            if(h->compare ? h->compare[i](w, v) : (w == v))
            {
                index += j * n;
                n *= dims;
                ok++;
                ((hypercube*) h)->index_cache[i] = j;
                break;

            }
        }
        indexes[i] = j;
    }
}

/*********************************/
/* End of Auxiliary functions    */
/*********************************/

static hypercube *new_hypercube(const request *r)
{
    hypercube *h = (hypercube*) calloc(sizeof(hypercube), 1);
    int total = 0, count = 0;
    int n = 0;
    const char *val = 0;

    h->r = clone_one_request(r);
    h->cube = empty_request("CUBE");

    h->count = total = count_hypercube(r);
    count = set_axis(h);

    h->compare = 0;

    if((total != count) || (count == 0))
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error while computing hypercube fields");
        grib_context_log(ctx, GRIB_LOG_ERROR, "Number of fields in request %d", total);
        grib_context_log(ctx, GRIB_LOG_ERROR, "Number of fields in hypercube %d", count);
    }
    set_index(h, count - 1, 1);
    memset(h->set, 1, count);

    /* This is expensive, but makes the iterator with only
       those parameters found as axis */
    h->iterator = empty_request(0);
    for(n = 0; n < NUMBER(global_axis); ++n)
        if((val = get_value(h->r, global_axis[n].name, 0)) != NULL)
            set_value(h->iterator, global_axis[n].name, val);

    return h;
}

static void print_hypercube(const hypercube *h)
{
    print_all_requests(h->r);
    print_all_requests(h->cube);
    grib_context_log(ctx, GRIB_LOG_INFO, "%d active out of %d fields described\n", count_index(h), h->size);
}

static void free_hypercube(hypercube *h)
{
    free_all_requests(h->r);
    free_all_requests(h->cube);
    free_all_requests(h->iterator);
    grib_context_free(ctx, h->index_cache);
    grib_context_free(ctx, h->compare);
    grib_context_free(ctx, h->set);
    grib_context_free(ctx, h);
}

struct stuff_1 {
    hypercube *c;
    request *r;
};

static void reqcb_1(const request *r, int count, axis_t *names, char *vals[], void *data)
{
    struct stuff_1 *s = (struct stuff_1*) data;
    int i;

    for(i = 0; i < count; i++)
        if(vals[i])
            set_value(s->r, names[i].name, vals[i]);

    set_index(s->c, cube_position(s->c, s->r), 1);
}

typedef void (*loopproc)(const request*, int, axis_t*, char**, void*);

static void names_loop(const request *r, loopproc proc, void *data);

static hypercube *new_hypercube_from_mars_request(const request *r)
{
    int i;
    int n;

    struct stuff_1 s;

#if 0
    const request *lang = mars_language_from_request(r);
    int count = 0;

    count = init_axis(lang);
    grib_context_log(ctx,GRIB_LOG_DEBUG,"cube %s",r->kind);
    /* print_all_requests(mars_language_from_request(r)); */
    grib_context_log(ctx,GRIB_LOG_INFO,"NUMBER(axis): %d, number axisnew: %d",NUMBER(axis),count);
#endif

    s.c = new_hypercube(r);
    s.r = clone_one_request(r);

    reset_index(s.c, 0);
    names_loop(r, reqcb_1, &s);

    free_one_request(s.r);

    /* add single parameters */

    for(i = 0; i < NUMBER(global_axis); i++)
    {
        int m = count_values(r, global_axis[i].name);
        if(m == 1)
        {
            add_value(s.c->cube, "axis", global_axis[i].name);
            set_value(s.c->cube, global_axis[i].name, get_value(r, global_axis[i].name, 0));
        }
    }

    n = count_values(s.c->cube, "axis");
    if(n)
        s.c->compare = (namecmp*)calloc(sizeof(namecmp), n);

    for(i = 0; i < n; i++)
        s.c->compare[i] = comparator(get_value(s.c->cube, "axis", i));

    return s.c;
}

/* This one doesn't have single parameters in CUBE */
static hypercube *new_simple_hypercube_from_mars_request(const request *r)
{
    int i;
    int n;

    struct stuff_1 s;
    s.c = new_hypercube(r);
    s.r = clone_one_request(r);

    reset_index(s.c, 0);

    names_loop(r, reqcb_1, &s);

    free_one_request(s.r);
    n = count_values(s.c->cube, "axis");
    if(n)
        s.c->compare = (namecmp*)calloc(sizeof(namecmp), n);

    for(i = 0; i < n; i++)
        s.c->compare[i] = comparator(get_value(s.c->cube, "axis", i));

    return s.c;
}

/*===========================================================================================*/

/*===========================================================================================*/

/* Todo:
 - BUILD A TEST SUITE !!
 - Print usage in log file
 - consider FCMONTH and Climatology
 - Build logic to create validationtime when only one of DATE or TIME or STEP have multiple values:
 for example: date=-1, time=12, step=24/48
 - parametrise the type of data for each axis (function fill_netcdf_dimensions & define_netcdf_dimensions)
 Now, all are INT but TIME could be float
 - allow user specified scale_factor
 - insert 'user input request'
 - ORIGIN
 - Monthly means seem not to ignore STEP (data server, era40, eg retrieve 39/142)
 */

typedef struct ncatt {
    char name[1024];
    char *long_name;
    char *units;
    char *short_name;
    char *standard_name;
    request *metadata;
    nc_type nctype;
} ncatt_t;

typedef struct filter {
    fieldset *fset;
    hypercube *filter;
    int count;
    double scale_factor;
    double add_offset;
    double missing;
    boolean bitmap;
    ncatt_t att;
    request *filter_request;
    boolean scale;
} dataset_t;

typedef struct ncfile {
    dataset_t *filters;
    int ncid;
} ncfile_t;

typedef struct ncoptions {
    boolean usevalidtime; /* Whether to use valid TIME only or not */
    boolean auto_refdate; /* Automatic Reference Date */
    long refdate; /* Reference date */
    const char *version;

    char *title;
    char *history;
    char *unlimited;
    boolean checkvalidtime;
    request *mars_description;
    boolean mmeans; /* Whether this dataset is Monthly Means */
    boolean climatology; /* Whether this dataset is climatology */
} ncoptions_t;

ncoptions_t setup;

#define NC_TYPES 7
struct nc_types_values {
    double nc_type_max;
    double nc_type_min;
    double nc_type_missing;
} nc_type_values[NC_TYPES] =
{
        /* In some occasions, SHRT_MIN-2 for the minimum value, makes ncview display
 missing values for -32766, while NC_FILL_SHORT=-32767, and SHRT_MIN=-32768 */
        { 0, 0, 0 },                                 /* NC_NAT,   'Not A Type' (c.f. NaN) */
        { 0x7f, NC_FILL_BYTE +1, NC_FILL_BYTE },     /* NC_BYTE,   signed 1 byte integer */
        { 0xff, NC_FILL_CHAR +1, NC_FILL_CHAR },     /* NC_CHAR,   ISO/ASCII character */
        { 0x7fff, NC_FILL_SHORT+1 , NC_FILL_SHORT }, /* NC_SHORT,  signed 2 byte integer */
        { 0x7ffffff, NC_FILL_INT + 1, NC_FILL_INT }, /* NC_INT,    signed 4 byte integer */
        { FLT_MAX, -FLT_MAX, NC_FILL_FLOAT },        /* NC_FLOAT,  single precision floating point number */
        { DBL_MAX, -DBL_MAX, NC_FILL_DOUBLE },       /* NC_DOUBLE, double precision floating point number */
};

static long fcmonth2days(long date, long months)
{
    long julianfrom = grib_date_to_julian(date);
    long years      = (long)(months/12);
    long leap       = (long)(years/4) - (long)(years/100);
    long to         = years*365 + (months%12)*32 + 1 + leap;             /* FCMONTH can't be > 28 */
    long julianto = julianfrom + to;
    long days = 0;

    long dd = date % 100;
    long dateto = grib_julian_to_date(julianto);
    long nextdate = (dateto / 100) * 100 + dd;

    julianto = grib_date_to_julian(nextdate);
    days = julianto - julianfrom;

    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: date: %ld + %ld months = %ld days", date, months, days);

    return days;
}

static long request_fields(request *r)
{
    long cnt = 1;
    parameter *p = r->params;
    while(p)
    {
        if(p->name[0] != '_')
        {
            cnt *= count_values(r,p->name);
        }
        p = p->next;
    }
    return cnt;
}

/* Transform MARS FCMONTHs into number of months from base date.
 For example, FCMONTH=1 is current month == 0 */
static void fcmonth2nbmonths(request *r)
{
    long n = count_values(r, "fcmonth");

    if(n == 0)
        return;
    n = atol(get_value(r, "fcmonth", 0));
    set_value(r, "fcmonth", "%ld", n - 1);
}

static long monthnumber(const char *m)
{
    const char *months[] =
    { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
    int i = 0;

    while(i < 12)
        if(strcmp(m, months[i++]) == 0)
            return i;
    grib_context_log(ctx, GRIB_LOG_ERROR, "Error. Translation for MONTH='%s' not found", m);
    return -1;
}

/* The argument represents 1 field */
static void validation_time(request *r)
{
    long date = 0;
    long time = 0;
    long step = 0;
    long fcmonthdays = 0;
    long fcmonth = 0;
    double v;
    long julian = 0;

    long nstep = count_values(r, "step");
    long ndate = count_values(r, "date");
    long ntime = count_values(r, "time");
    long nfcmonth = count_values(r, "fcmonth");

    static long julianrefdate = 0;

    if(nstep > 1 || ndate > 1 || ntime > 1 || nfcmonth > 1)
    {
        if(nstep > 1)
            grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error. Field has more than 1 STEP");
        if(ntime > 1)
            grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error. Field has more than 1 TIME");
        if(ndate > 1)
            grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error. Field has more than 1 DATE");
        if(nfcmonth > 1)
            grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error. Field has more than 1 FCMONTH");
        print_all_requests(r);
        exit(1);
    }

    if(nstep)
        step = atol(get_value(r, "step", 0));

    if(ndate)
    {
        const char* p = get_value(r, "date", 0);
        const char* marsClass = get_value(r, "class", 0);
        if (eq_string(marsClass, "s2")) {
            /* S2S Data. See GRIB-699 and GRIB-762 */
            const char* hdate = get_value(r, "hdate", 0);
            grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: S2S Data");
            if (hdate) {
                p = hdate; /* This is a hindcast */
            }
        }
        if(is_number(p))
            date = atol(p);
        else
        {
            long julian = 0, second = 0;
            boolean isjul, date_ok;
            date_ok = parsedate(p, &julian, &second, &isjul);
            if (!date_ok) grib_context_log(ctx, GRIB_LOG_ERROR, "Failed to parse date: '%s'", p);
            date = grib_julian_to_date(julian);
        }

        /* Climatology */
        if(strlen(p) == 3)
        {
            if(setup.usevalidtime)
                grib_context_log(ctx, GRIB_LOG_ERROR, "Climatology data. Setting usevalidtime=OFF");
            setup.auto_refdate = FALSE;
            setup.usevalidtime = FALSE;
            setup.climatology = TRUE;
        }
    }

    if(ntime)
        time = atol(get_value(r, "time", 0));

    if(nfcmonth)
    {
        fcmonth = atol(get_value(r, "fcmonth", 0));
        /* FCMONTH needs base DATE */
        if(date == 0)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error: FCMONTH needs DATE");
            exit(1);
        }
        fcmonthdays = fcmonth2days(date, fcmonth);
    }

    julian = grib_date_to_julian(date);
    v = julian * 24.0 + fcmonthdays * 24.0 + time / 100.0 + step * 1.0;
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: date=%ld, julian=%ld, fcmonthdays=%ld, time=%ld, step=%ld, validation=%ld", date, julian, fcmonthdays, time, step, v);
    set_value(r, "_validation", "%lf", v);
    set_value(r, "_juliandate", "%ld", julian);

    if(!julianrefdate)
        julianrefdate = grib_date_to_julian(setup.refdate);

    set_value(r, "_validationtime", "%lf", v - julianrefdate * 24.0);

    /* Remove minutes from TIME */
    if(ntime)
        set_value(r, "time", "%.1lf", time / 100.0);
}

static void free_nc_options()
{

    if(setup.title)
        grib_context_free(ctx, setup.title);
    if(setup.history)
        grib_context_free(ctx, setup.history);
    if(setup.unlimited)
        grib_context_free(ctx, setup.unlimited);
    if(setup.mars_description)
        free_all_requests(setup.mars_description);
}

static void get_nc_options(const request *user_r)
{
    const char *checkvalidtime_env = NULL;
    const char *validtime = get_value(user_r, "usevalidtime", 0);
    const char *refdate = get_value(user_r, "referencedate", 0);

    const char *title = get_value(user_r, "title", 0);
    const char *history = get_value(user_r, "history", 0);
    const char *unlimited = get_value(user_r, "unlimited", 0);

    setup.usevalidtime = validtime ? (strcmp(validtime, "true") == 0) : FALSE;
    setup.refdate = refdate ? atol(refdate) : 19000101;
    setup.auto_refdate = refdate ? (strcmp(get_value(user_r, "referencedate", 0), "AUTOMATIC") == 0) : FALSE;

    setup.title = title ? grib_context_strdup(ctx, (title)) : NULL;
    setup.history = history ? grib_context_strdup(ctx, (history)) : NULL;
    setup.unlimited = unlimited ? grib_context_strdup(ctx, ((unlimited))) : NULL;

    checkvalidtime_env = getenv("GRIB_TO_NETCDF_CHECKVALIDTIME");
    setup.checkvalidtime = checkvalidtime_env ? atol(checkvalidtime_env) : 1;
    setup.mars_description = empty_request("MARS");
}

static nc_type translate_nctype(const char *name)
{
    if(!name)
        return NC_SHORT;

    if(strcmp(name, "NC_BYTE") == 0)
        return NC_BYTE;

    if(strcmp(name, "NC_SHORT") == 0)
        return NC_SHORT;

    if(strcmp(name, "NC_INT") == 0)
        return NC_INT;

    if(strcmp(name, "NC_FLOAT") == 0)
        return NC_FLOAT;

    if(strcmp(name, "NC_DOUBLE") == 0)
        return NC_DOUBLE;

    grib_context_log(ctx, GRIB_LOG_ERROR, "Unknown NetCDF type '%s'. Using NC_SHORT", name);
    return NC_SHORT;
}

static void check_err(const int stat, const int line, const char *file)
{
    if(stat != NC_NOERR)
    {
        (void) fprintf(stderr, "line %d of %s: %s\n", line, file, nc_strerror(stat));
        if (stat == NC_EVARSIZE) {
            (void) fprintf(stderr, "\nCannot create netCDF classic format, dataset is too large!\n"
                    "Try splitting the input GRIB(s).\n");
        }
        exit(1);
    }
}

#define DIM_ID 1
static int set_dimension(int ncid, const char *name, int n, int xtype, const char *units, const char *long_name)
{
    int var_id = 0;
    int stat = 0;
    int dim_id = DIM_ID;
    int dim_vec[DIM_ID];

    if(setup.unlimited && (strcmp(name, setup.unlimited) == 0))
        n = NC_UNLIMITED;

    stat = nc_def_dim(ncid, name, n, &dim_id);
    check_err(stat, __LINE__, __FILE__);

    dim_vec[0] = dim_id;
    stat = nc_def_var(ncid, name, (nc_type) xtype, 1, dim_vec, &var_id);
    check_err(stat, __LINE__, __FILE__);

    if(units != NULL)
    {
        stat = nc_put_att_text(ncid, var_id, "units", strlen(units), units);
        check_err(stat, __LINE__, __FILE__);
    }

    if(long_name != NULL)
    {
        stat = nc_put_att_text(ncid, var_id, "long_name", strlen(long_name), long_name);
        check_err(stat, __LINE__, __FILE__);
    }

    return var_id;
}

static int def_latlon(int ncid, fieldset *fs)
{
    int n = 0;
    size_t l = 0;
    int var_id = 0;
    err e = 0;

    field *g = get_field(fs, 0, expand_mem);

    char grid_type[80];
    size_t size;
    size = sizeof(grid_type);
    if((e = grib_get_string(g->handle, "typeOfGrid", grid_type, &size)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get typeOfGrid %s", grib_get_error_message(e));
        return e;
    }

    if(strcmp(grid_type, "regular_ll") != 0 && (strcmp(grid_type, "regular_gg") != 0) )
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "First GRIB is not on a regular lat/lon grid or on a regular Gaussian grid. Exiting.\n");
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    /* Define longitude */
    if((e = grib_get_size(g->handle, "distinctLongitudes", &l)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get distinctLongitudes %s", grib_get_error_message(e));
        return e;
    }
    n = l;
    var_id = set_dimension(ncid, "longitude", n, NC_FLOAT, "degrees_east", "longitude");

    /* Define latitude */
    if((e = grib_get_size(g->handle, "distinctLatitudes", &l)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get distinctLatitudes %s", grib_get_error_message(e));
        return e;
    }
    n = l;
    var_id = set_dimension(ncid, "latitude", n, NC_FLOAT, "degrees_north", "latitude");

    /* g->purge_header = TRUE; */
    release_field(g);

    (void)var_id; /* suppress gcc warning */
    return e;
}

static int put_latlon(int ncid, fieldset *fs)
{
    int var_id = 0;
    int i = 0;
    size_t n = 0;
    int stat = 0;
    err e = 0;

    field *g = get_field(fs, 0, expand_mem);

    double *dvalues = NULL;
    float *fvalues = NULL;
    long nv = 0;

    size_t ni;
    size_t nj;

#if 0
    /* Get info in degrees */
    if((e = grib_get_double(g->handle, "iDirectionIncrementInDegrees", &ew_stride)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get iDirectionIncrementInDegrees %s", grib_get_error_message(e));
        return e;
    }

    if((e = grib_get_double(g->handle, "jDirectionIncrementInDegrees", &ns_stride)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get jDirectionIncrementInDegrees %s", grib_get_error_message(e));
        return e;
    }

    /* Define longitude */
    if((e = grib_get_long(g->handle, "Ni", &ni)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get Ni %s", grib_get_error_message(e));
        return e;
    }
    /* Define latitude */
    if((e = grib_get_long(g->handle, "Nj", &nj)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get Nj %s", grib_get_error_message(e));
        return e;
    }

#endif

    if((e = grib_get_size(g->handle, "distinctLatitudes", &nj)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get distinctLatitudes %s", grib_get_error_message(e));
        return e;

    }

    if((e = grib_get_size(g->handle, "distinctLongitudes", &ni)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get distinctLongitudes %s", grib_get_error_message(e));
        return e;

    }
    /* Compute max. # values and allocate */
    nv = ni;
    if(nv < nj)
        nv = nj;

    fvalues = (float *) grib_context_malloc(ctx, sizeof(float) * nv);
    dvalues = (double *) grib_context_malloc(ctx, sizeof(double) * nv);

    /* longitude */
    n = ni;
    stat = nc_inq_varid(ncid, "longitude", &var_id);
    check_err(stat, __LINE__, __FILE__);
    if((e = grib_get_double_array(g->handle, "distinctLongitudes", dvalues, &n)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get distinctLongitudes %s", grib_get_error_message(e));
        return e;
    }
    Assert(n == ni);

    for(i=0; i< n; i++) { fvalues[i] = dvalues[i]; }

    stat = nc_put_var_float(ncid, var_id, fvalues);
    check_err(stat, __LINE__, __FILE__);

    /* latitude */
    n = nj;
    stat = nc_inq_varid(ncid, "latitude", &var_id);
    check_err(stat, __LINE__, __FILE__);
    if((e = grib_get_double_array(g->handle, "distinctLatitudes", dvalues, &n)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get distinctLatitudes %s", grib_get_error_message(e));
        return e;
    }

    Assert(n == nj);

    for(i=0; i< n; i++) { fvalues[i] = dvalues[i]; }
    stat = nc_put_var_float(ncid, var_id, fvalues);
    check_err(stat, __LINE__, __FILE__);

    /* g->purge_header = TRUE; */
    release_field(g);
    grib_context_free(ctx, fvalues);
    grib_context_free(ctx, dvalues);

    return e;
}

static int compute_scale(dataset_t *subset)
{
    double max = -DBL_MAX;
    double min = DBL_MAX;
    double median = 0;
    long i = 0;
    long j = 0;
    int64_t scaled_max = 0;
    int64_t scaled_min = 0;
    int64_t scaled_median = 0;
    double ao = 0.0, sf = 0.0;
    double x;

    char test_scaled_max = 0;
    char test_scaled_min = 0;
    char test_scaled_median = 0;

    err e = 0;

    fieldset *fs = subset->fset;
    int idx = subset->att.nctype;

    for(i = 0; i < fs->count; i++)
    {
        field *g = get_field(fs, i, expand_mem);
        size_t len;
        static double *vals = NULL;
        static size_t vals_len = 0;

        if((e = grib_get_size(g->handle, "values", &len)) != GRIB_SUCCESS)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get size of values %s", grib_get_error_message(e));
            return e;
        }

        if(len > vals_len)
        {
            if(vals)
                grib_context_free(ctx, vals);
            vals = (double*) grib_context_malloc(ctx, sizeof(double) * len);
            vals_len = len;
        }
        if((e = grib_get_double_array(g->handle, "values", vals, &len)) != GRIB_SUCCESS)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get values %s", grib_get_error_message(e));
            return e;
        }

        if(g->has_bitmap)
        {
            subset->bitmap = TRUE;
            for(j = 0; j < len; ++j)
            {
                if(vals[j] != (double) global_missing_value)
                {
                    if(vals[j] > max)
                        max = vals[j];
                    if(vals[j] < min)
                        min = vals[j];
                }
            }
        }
        else
        {
            for(j = 0; j < len; ++j)
            {
                if(vals[j] > max)
                    max = vals[j];
                if(vals[j] < min)
                    min = vals[j];
            }
        }
        /* g->purge_header = TRUE; */
        release_field(g);
    }

    median = (max + min) / 2.0;

    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: max_int: %lf, min_int: %lf", nc_type_values[idx].nc_type_max, nc_type_values[idx].nc_type_min);

    sf = (double) ((max - min) / (double) (nc_type_values[idx].nc_type_max - nc_type_values[idx].nc_type_min));
    ao = ((max + min) - sf * (nc_type_values[idx].nc_type_min + nc_type_values[idx].nc_type_max)) / 2;

    if (min == max) {
        sf = 1.0; /* Prevent divide by zero later. Constant field grib has max == min */
    }

    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: idx is: %d", idx);
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: max: %lf, min: %lf, median: %lf, scale factor: %lf, add_offset: %lf", max, min, median, sf, ao);

    x = ((median - ao));
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: x=%lf", x);
    x /= sf;
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: x=%lf", x);

    scaled_max = rint((max - ao) / sf);
    scaled_min = rint((min - ao) / sf);
    scaled_median = rint((median - ao) / sf);

    test_scaled_max = (char) scaled_max;
    test_scaled_min = (char) scaled_min;
    test_scaled_median = (char) scaled_median;

    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: scaled_max: %lld, scaled_min: %lld, scaled_median: %lld, x: %lf", scaled_max, scaled_min, scaled_median, x);

    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: test_scaled_max: %x, test_scaled_min: %x, test_scaled_median: %x", test_scaled_max, test_scaled_min, test_scaled_median, x);

    max = scaled_max * sf + ao;
    min = scaled_min * sf + ao;
    median = scaled_median * sf + ao;

    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: max: %lf, min: %lf, median: %lf", max, min, median);

    subset->scale_factor = sf;
    subset->add_offset = ao;

    return 0;
}

static int nc_put_att_type(int ncid, int varid, const char *name, nc_type nctype, int n, double value)
{
    int r = 0;
    switch(nctype)
    {
    case NC_BYTE: {
        unsigned char val_uchar = (unsigned char) value;
        r = nc_put_att_uchar(ncid, varid, name, nctype, n, &val_uchar);
        break;
    }
    case NC_SHORT: {
        short int val_short = (short int) value;
        r = nc_put_att_short(ncid, varid, name, nctype, n, &val_short);
        break;
    }
    case NC_INT: {
        int val_int = (int) value;
        r = nc_put_att_int(ncid, varid, name, nctype, n, &val_int);
        break;
    }
    case NC_FLOAT: {
        float val_flt = (float) value;
        r = nc_put_att_float(ncid, varid, name, nctype, n, &val_flt);
        break;
    }
    case NC_DOUBLE: {
        double val_dbl = (double) value;
        r = nc_put_att_double(ncid, varid, name, nctype, n, &val_dbl);
        break;
    }
    default:
        grib_context_log(ctx, GRIB_LOG_ERROR, "nc_put_att_type(...): Unknown netcdf type '%d'", nctype);
        break;
    }
    return r;
}

static int nc_put_vara_type(int ncid, int varid, const size_t start[], const size_t count[], void *valuesp, nc_type nctype)
{
    int r = 0;
    switch(nctype)
    {
    case NC_BYTE:
        r = nc_put_vara_uchar(ncid, varid, start, count, (unsigned char *) valuesp);
        break;
    case NC_SHORT:
        r = nc_put_vara_short(ncid, varid, start, count, (short int *) valuesp);
        break;
    case NC_INT:
        r = nc_put_vara_int(ncid, varid, start, count, (int *) valuesp);
        break;
    case NC_FLOAT:
        r = nc_put_vara_float(ncid, varid, start, count, (float *) valuesp);
        break;
    case NC_DOUBLE:
        r = nc_put_vara_double(ncid, varid, start, count, (double *) valuesp);
        break;
    default:
        grib_context_log(ctx, GRIB_LOG_ERROR, "nc_put_vara_type(...): Unknown netcdf type '%d'", nctype);
        break;
    }
    return r;
}

static void scale_bitmap(double *vals, long n, void *data, dataset_t *subset)
{
    int i = 0;
    nc_type nctype = subset->att.nctype;

    /*
     if(!subset->bitmap)
     {
      grib_context_log(ctx,GRIB_LOG_DEBUG,"No scale of bitmap required");
      return;
     }
     */

    switch(nctype)
    {
    case NC_BYTE:
    {
        unsigned char *vscaled = (unsigned char *) data;
        for(i = 0; i < n; ++i){
            if(vals[i] == global_missing_value){
                vscaled[i] = (unsigned char) subset->missing;
            }
        }
        break;
    }

    case NC_SHORT:
    {
        short int *vscaled = (short int *) data;
        for(i = 0; i < n; ++i){
            if(vals[i] == global_missing_value){
                vscaled[i] = (short int) subset->missing;
            }
        }
        break;
    }

    case NC_INT:
    {
        int *vscaled = (int *) data;
        for(i = 0; i < n; ++i){
            if(vals[i] == global_missing_value){
                vscaled[i] = (int) subset->missing;
            }
        }
        break;
    }

    case NC_FLOAT:
    {
        float *vscaled = (float *) data;
        for(i = 0; i < n; ++i){
            if(vals[i] == global_missing_value){
                vscaled[i] = (float) subset->missing;
            }
        }
        break;
    }

    case NC_DOUBLE:
    {
        double *vscaled = (double *) data;
        for(i = 0; i < n; ++i){
            if(vals[i] == global_missing_value){
                vscaled[i] = (double) subset->missing;
            }
        }
        break;
    }

    default:
        grib_context_log(ctx, GRIB_LOG_ERROR, "scale(...): Unknown netcdf type %d", nctype);
        break;
    }
}

static void scale(double *vals, long n, void *data, dataset_t *g)
{
    int i = 0;
    nc_type nctype = g->att.nctype;
    double scale_factor = g->scale_factor;
    double add_offset = g->add_offset;

    /*
    if(!subset->scale)
    {
     grib_context_log(ctx,GRIB_LOG_DEBUG,"No scale required");
     return;
    }
     */

    switch(nctype)
    {
    case NC_BYTE:
    {
        unsigned char *vscaled = (unsigned char *) data;
        for(i = 0; i < n; ++i)
        {
            if(!g->bitmap || (vals[i] != global_missing_value))
            {
                double d = rint((vals[i] - add_offset) / scale_factor);
                Assert(d >= nc_type_values[nctype].nc_type_min && d <= nc_type_values[nctype].nc_type_max);
                vscaled[i] = d;
            }
        }
        break;
    }

    case NC_SHORT:
    {
        short int *vscaled = (short int *) data;
        for(i = 0; i < n; ++i)
        {
            if(!g->bitmap || (vals[i] != global_missing_value))
            {
                double d = 0;
                Assert(scale_factor>0);
                d = rint((vals[i] - add_offset) / scale_factor);
                Assert(d >= nc_type_values[nctype].nc_type_min && d <= nc_type_values[nctype].nc_type_max);
                vscaled[i] = d;
            }
        }
        break;
    }

    case NC_INT:
    {
        int *vscaled = (int *) data;
        for(i = 0; i < n; ++i)
        {
            if(!g->bitmap || (vals[i] != global_missing_value))
            {
                double d = rint((vals[i] - add_offset) / scale_factor);
                Assert(d >= nc_type_values[nctype].nc_type_min && d <= nc_type_values[nctype].nc_type_max);
                vscaled[i] = d;
            }
        }
        break;
    }

    case NC_FLOAT:
    {
        float *vscaled = (float *) data;
        for(i = 0; i < n; ++i)
        {
            if(!g->bitmap || (vals[i] != global_missing_value))
            {
                double d = vals[i];
                Assert(d >= nc_type_values[nctype].nc_type_min && d <= nc_type_values[nctype].nc_type_max);
                vscaled[i] = d;
            }
        }
        break;
    }

    case NC_DOUBLE:
    {
        double *vscaled = (double *) data;
        for(i = 0; i < n; ++i)
        {
            if(!g->bitmap || (vals[i] != global_missing_value))
            {
                double d = vals[i];
                Assert(d >= nc_type_values[nctype].nc_type_min && d <= nc_type_values[nctype].nc_type_max);
                vscaled[i] = d;
            }
        }
        break;
    }

    default:
        grib_context_log(ctx, GRIB_LOG_ERROR, "scale(...): Unknown netcdf type %d", nctype);
        break;
    }
}

/* Return array of strings which are the "time" values */
static char** create_times_array(const request* cube, size_t* size)
{
    char** result = NULL;
    const char *time_axis ="time"; /* special case */
    parameter *the_param = find_parameter(cube, time_axis);
    *size = 0;
    if (the_param) {
        size_t i=0, num_values=0;
        value* va = NULL;

        if(!the_param->count)
            count_values(cube, time_axis);

        /* Go thru all values to count how many there are */
        va = the_param->values;
        while(va) {
            ++num_values;
            va = va->next;
        }
        /* Create and populate array */
        result = (char**) grib_context_malloc(ctx, sizeof(char*) * num_values);
        va = the_param->values;
        while(va) {
            result[i++] = va->name;
            va = va->next;
        }
        *size = num_values;
    }
    return result;
}

static int put_data(hypercube *h, int ncid, const char *name, dataset_t *subset)
{
    int i = 0;
    int stat = 0;
    int dataid = 0;
    int naxis = count_axis(h);
    size_t start[NC_MAX_DIMS];
    size_t count[NC_MAX_DIMS];
    char** times_array = NULL;
    size_t times_array_size = 0;
    fieldset *fs = subset->fset;
    field *f = get_field(fs, 0, expand_mem);

    void *vscaled = NULL;
    long vscaled_length = 0;

    long ni;
    long nj;
    err e = 0;

    /* Define longitude */
    if((e = grib_get_long(f->handle, "Ni", &ni)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get Ni %s", grib_get_error_message(e));
        return e;
    }
    /* Define latitude */
    if((e = grib_get_long(f->handle, "Nj", &nj)) != GRIB_SUCCESS)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get Nj %s", grib_get_error_message(e));
        return e;
    }

    /* Start filling dimensions at first value */
    for(i = 0; i < 2 + naxis; ++i)
        start[i] = 0;

    /* Count dimensions per axis */
    for(i = 0; i < naxis; ++i)
        count[naxis - i - 1] = 1;

    count[naxis] = nj; /* latitude */
    count[naxis + 1] = ni; /* longitude */

    /* f->purge_header = TRUE; */
    release_field(f);

    stat = nc_inq_varid(ncid, name, &dataid);
    check_err(stat, __LINE__, __FILE__);

    /* GRIB-792: Build fast array storing values for the "time" axis. */
    /* This is for performance reasons */
    times_array = create_times_array(h->cube, &times_array_size);

    for(i = 0; i < fs->count; i++)
    {
        field *g = get_field(fs, i, expand_mem);
        size_t len;
        static double *vals = NULL;
        static size_t vals_len = 0;
        boolean missing = 0;

        request *r;
        int j = 0;
        int idx[1024];
        int idxsize = 1024;

        if((e = grib_get_size(g->handle, "values", &len)) != GRIB_SUCCESS)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get size of values %s", grib_get_error_message(e));
            return e;
        }

        if(len > vals_len)
        {
            if(vals)
                grib_context_free(ctx, vals);
            vals = (double*) grib_context_malloc(ctx, sizeof(double) * len);
            vals_len = len;
        }
        if((e = grib_get_double_array(g->handle, "values", vals, &len)) != GRIB_SUCCESS)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get values %s", grib_get_error_message(e));
            return e;
        }

#if 0
        boolean missing = (g->ksec4[0] < 0); /* If negative number of values, field is missing */
#endif

        r = field_to_request(g);
        if(!missing)
        {
            /* Reserved the maximum memory needed */
            /* This should only be done once, as all fields have the same geometry */
            if((vscaled_length == 0) || (vscaled_length < sizeof(double) * len))
            {
                if(vscaled)
                    grib_context_free(ctx, vscaled);
                vscaled = (void *) grib_context_malloc(ctx, sizeof(double) * len);
                vscaled_length = sizeof(double) * len;
            }

            scale(vals, len, vscaled, subset);
            if(subset->bitmap)
                scale_bitmap(vals, len, vscaled, subset);

            if((e = grib_get_long(g->handle, "Ni", &ni)) != GRIB_SUCCESS)
            {
                grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get Ni %s", grib_get_error_message(e));
                return e;
            }
            /* Define latitude */
            if((e = grib_get_long(g->handle, "Nj", &nj)) != GRIB_SUCCESS)
            {
                grib_context_log(ctx, GRIB_LOG_ERROR, "ecCodes: cannot get Nj %s", grib_get_error_message(e));
                return e;
            }

            if(nj != count[naxis] || ni != count[naxis + 1])
            {
                grib_context_log(ctx, GRIB_LOG_ERROR, "Grib %d has different resolution\n", i + 1);
                grib_context_log(ctx, GRIB_LOG_ERROR, "lat=%d, long=%d instead of lat=%d, long=%d\n", nj, ni, count[naxis], count[naxis + 1]);
                exit(1);
            }

            cube_indexes(h, r, times_array, times_array_size, idx, idxsize);
            for(j = 0; j < naxis; ++j)
                start[naxis - j - 1] = idx[j];

            grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: Put data from field %d", i);

            stat = nc_put_vara_type(ncid, dataid, start, count, vscaled, subset->att.nctype);
            check_err(stat, __LINE__, __FILE__);
        }

        /* g->purge_header = TRUE; */
        release_field(g);
    }

    grib_context_free(ctx, vscaled);
    grib_context_free(ctx, times_array);
    return 0;
}

static void set_always_a_time(hypercube *h, request *data_r)
{
    if (setup.usevalidtime && count_values(data_r, "time") == 1)
    {
        set_value(h->cube, "time", "%.2lf", atof(get_value(data_r, "_validationtime", 0)));
        add_value(h->cube, "axis", "time");

        {
            int i = 0;
            int n = count_values(h->cube, "axis");
            if(n)
            {
                h->compare = (namecmp*)calloc(sizeof(namecmp), n);
                Assert(h->compare);
            }

            for(i = 0; i < n; i++)
                h->compare[i] = comparator(get_value(h->cube, "axis", i));
        }
    }
}

static int define_netcdf_dimensions(hypercube *h, fieldset *fs, int ncid, dataset_t *subsets, int subsetcnt, const request *data_r)
{
    const request *cube = h->cube;
    int naxis = count_axis(h);
    int i = 0;
    int stat = 0;
    int n = 0;
    int var_id = 0; /* Variable ID */
    int dims[1024];

    /* START DEFINITIONS */

    /* Define latitude/longitude dimensions */
    err e = def_latlon(ncid, fs);
    if (e != GRIB_SUCCESS)
        return e;

    /* Define netcdf dimensions */
    for(i = 0; i < naxis; ++i)
    {
        int nctype = NC_INT;
        const char *axis = get_axis(h, i);
        char *units = NULL;
        char u[10240];
        const char *lowaxis = (axis);
        char *longname = (char *) lowaxis;
        n = count_values(cube, axis);

        if(count_values(data_r,"levtype") > 1)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "Cannot handle fields for different levtypes.\n");
            grib_context_log(ctx, GRIB_LOG_ERROR, "Please split input data into different files. Exiting!\n");
            exit(1);
        }

        if(strcmp(axis, "levelist") == 0)
        {
            const char *levtype = get_value(data_r, "levtype", 0);
            if(levtype)
            {
                if(strcmp("pl", levtype) == 0)
                {
                    units = "millibars";
                    longname = "pressure_level";
                }

                if(strcmp("ml", levtype) == 0)
                {
                    longname = "model_level_number";
                }
            }

            lowaxis = "level";
        }

        if(strcmp(axis, "number") == 0)
        {
            longname = "ensemble_member";
        }

        if(strcmp(axis, "date") == 0)
        {
            sprintf(u, "days since %ld-%02ld-%02ld 00:00:0.0", setup.refdate / 10000, (setup.refdate % 10000) / 100, (setup.refdate % 100));
            units = u;
            longname = "Base_date";
            if(setup.climatology)
            {
                sprintf(u, "months");
                units = u;
            }
        }

        if(strcmp(axis, "time") == 0)
        {
            boolean onedtime = (count_values(cube, "date") == 0 && count_values(cube, "step") == 0);
            sprintf(u, "hours since 0000-00-00 00:00:0.0");
            longname = "reference_time";
            if(setup.usevalidtime || onedtime)
            {
                sprintf(u, "hours since %ld-%02ld-%02ld 00:00:0.0", setup.refdate / 10000, (setup.refdate % 10000) / 100, (setup.refdate % 100));
                longname = "time";
            }
            if(setup.climatology)
            {
                sprintf(u, "hours");
            }
            units = u;
            /* nctype = NC_FLOAT; */
        }

        if(strcmp(axis, "step") == 0)
        {
            units = "hours";
            longname = "time_step";
            if(count_values(cube, "date") == 0 && count_values(cube, "time") == 0)
            {
                const char *d = get_value(data_r, "date", 0);
                const char *t = get_value(data_r, "time", 0);
                long date = d ? atol(d) : 0;
                long hour = t ? atol(t) : 0;
                long min = t ? 60 * (atof(t) - hour) : 0;
                sprintf(u, "hours since %ld-%02ld-%02ld %02ld:%02ld:0.0", date / 10000, (date % 10000) / 100, (date % 100), hour, min);
                units = u;
            }
        }

        if((strcmp(axis, "fcmonth") == 0))
        {
            const char *date = get_value(data_r, "date", 0);
            char ymd[32] = "";

            if(date)
            {
                strncat(ymd, date, 4);
                strcat(ymd, "-");
                strncat(ymd, date + 4, 2);
                strcat(ymd, "-");
                /* udunits is a bit tricky with month being 30.4 days */
                /* ncview doesn't display properly */
                strcat(ymd, "01");
            }
            else
            {
                sprintf(ymd, "00-00-00");
            }
            sprintf(u, "months since %s 00:00:00.0", ymd);
            units = u;
            longname = "time";
        }

        var_id = set_dimension(ncid, lowaxis, n, nctype, units, longname);

        if(strcmp(axis, "time") == 0)
        {
            if(setup.usevalidtime)
            {
                const char *cal = "gregorian";

                if(setup.mmeans)
                {
                    char *period = "0000-01-00 00:00:00";
                    stat = nc_put_att_text(ncid, var_id, "avg_period", strlen(period), period);
                    check_err(stat, __LINE__, __FILE__);
                }

                stat = nc_put_att_text(ncid, var_id, "calendar", strlen(cal), cal);
                check_err(stat, __LINE__, __FILE__);
            }
        }
    }

    /* Define data dimension */
    n = 1 + 1 + naxis; /* longitude + latitude + # axis */
    for(i = 0; i < n; ++i)
        dims[i] = n - i - 1;

    for(i = 0; i < subsetcnt; ++i)
    {

        printf("%s: Defining variable '%s'.\n", grib_tool_name, subsets[i].att.name);

        stat = nc_def_var(ncid, subsets[i].att.name, subsets[i].att.nctype, n, dims, &var_id);
        check_err(stat, __LINE__, __FILE__);

        if(subsets[i].scale)
        {
            compute_scale(&subsets[i]);
            stat = nc_put_att_double(ncid, var_id, "scale_factor", NC_DOUBLE, 1, &subsets[i].scale_factor);
            check_err(stat, __LINE__, __FILE__);

            stat = nc_put_att_double(ncid, var_id, "add_offset", NC_DOUBLE, 1, &subsets[i].add_offset);
            check_err(stat, __LINE__, __FILE__);
        }

        stat = nc_put_att_type(ncid, var_id, "_FillValue", subsets[i].att.nctype, 1, nc_type_values[subsets[i].att.nctype].nc_type_missing);
        check_err(stat, __LINE__, __FILE__);
        stat = nc_put_att_type(ncid, var_id, "missing_value", subsets[i].att.nctype, 1, nc_type_values[subsets[i].att.nctype].nc_type_missing);
        check_err(stat, __LINE__, __FILE__);

        if(subsets[i].att.units)
        {
            const char *txt = subsets[i].att.units;
            stat = nc_put_att_text(ncid, var_id, "units", strlen(txt), txt);
            check_err(stat, __LINE__, __FILE__);
        }

        if(subsets[i].att.long_name)
        {
            const char *txt = subsets[i].att.long_name;
            stat = nc_put_att_text(ncid, var_id, "long_name", strlen(txt), txt);
            check_err(stat, __LINE__, __FILE__);
        }

        if(subsets[i].att.short_name)
        {
            const char *txt = subsets[i].att.short_name;
            stat = nc_put_att_text(ncid, var_id, "short_name", strlen(txt), txt);
            check_err(stat, __LINE__, __FILE__);
        }

        if(subsets[i].att.standard_name)
        {
            const char *txt = subsets[i].att.standard_name;
            stat = nc_put_att_text(ncid, var_id, "standard_name", strlen(txt), txt);
            check_err(stat, __LINE__, __FILE__);
        }

#if 0
        if(subsets[i].att.other)
        {
            const char *txt = subsets[i].att.long_name;
            stat = nc_put_att_text(ncid, var_id, "other",strlen(txt),txt);
            check_err(stat,__LINE__,__FILE__);
        }
#endif

        if(subsets[i].att.metadata)
        {
            parameter *p = subsets[i].att.metadata->params;

            while(p)
            {
                const char *txt = p->values->name;
                stat = nc_put_att_text(ncid, var_id, p->name, strlen(txt), txt);
                check_err(stat, __LINE__, __FILE__);

                p = p->next;
            }
        }

    }

    /* Dimension-less variable for MARS request */
    if(0) /* reset when we have proper & fast mars_description */
    {
        /* parameter *p = data_r->params; */
        parameter *p = setup.mars_description->params;
        stat = nc_def_var(ncid, "MARS", NC_CHAR, 0, 0, &var_id);
        check_err(stat, __LINE__, __FILE__);

        /* Store request for those parameters with single value */
        while(p)
        {
            /* if((p->name[0] != '_') && (p->count == 1)) */
            if(p->name[0] != '_')
            {
                char par[1024];
                char val[1024000] = "";
                sprintf(par, "%s", (p->name));
#if 0
                value2string(p->values,val);
#else
                sprintf(val, "%s", (p->values->name));
#endif
                stat = nc_put_att_text(ncid, var_id, par, strlen(val), (val));
                if(stat != NC_NOERR)
                {
                    printf("Error setting request for %s = %s\n", par, val);
                }
                check_err(stat, __LINE__, __FILE__);
            }
            p = p->next;
        }
    }

    /* Global attributes */
    {
        char timestamp[80];
        time_t now;
        /* char *convention = "MARS;CF"; */
        char *convention = "CF-1.6";
        char history[10240];
        /* char *institution = "ECMWF Meteorological Archival and Retrieval System"; */

        /* Convention */
        stat = nc_put_att_text(ncid, NC_GLOBAL, "Conventions", strlen(convention), convention);
        check_err(stat, __LINE__, __FILE__);

        /* Use history provided or Timestamp */
        if(setup.history)
        {
            sprintf(history, "%s", setup.history);
        }
        else
        {
            int major = ECCODES_MAJOR_VERSION;
            int minor = ECCODES_MINOR_VERSION;
            int revision = ECCODES_REVISION_VERSION;

            time(&now);
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S GMT", gmtime(&now));
            sprintf(history, "%s by grib_to_netcdf-%d.%d.%d: %s", timestamp, major, minor, revision, argvString);
        }
        stat = nc_put_att_text(ncid, NC_GLOBAL, "history", strlen(history), history);
        check_err(stat, __LINE__, __FILE__);

#if 0
        stat = nc_put_att_text(ncid, NC_GLOBAL, "source",strlen(setup.source),setup.source);
        check_err(stat,__LINE__,__FILE__);

        stat = nc_put_att_text(ncid, NC_GLOBAL, "institution",strlen(institution),institution);
        check_err(stat,__LINE__,__FILE__);
#endif

        if(setup.title)
        {
            stat = nc_put_att_text(ncid, NC_GLOBAL, "title", strlen(setup.title), setup.title);
            check_err(stat, __LINE__, __FILE__);
        }

    }
    return e;
}

static int fill_netcdf_dimensions(hypercube *h, fieldset *fs, int ncid)
{
    const request *cube = h->cube;
    int naxis = count_axis(h);
    int i = 0;
    int var_id = 0;
    int stat = 0;

    /* Put latitude/longitude values */
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: Put latitude/longitude values");
    put_latlon(ncid, fs);

    /* Put netcdf axis values */
    grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: Put netcdf axis values");
    for(i = 0; i < naxis; ++i)
    {
        int j = 0;
        const char *axis = get_axis(h, i);
        int n = count_values(cube, axis);
        int *values = (int *) grib_context_malloc(ctx, sizeof(int) * n);
        const char *lowaxis = (axis);

        if(!values)
            grib_context_log(ctx, GRIB_LOG_ERROR, "fill_netcdf_dimensions: cannot allocate %ld bytes", sizeof(int) * n);

        if(strcmp("levelist", axis) == 0)
            lowaxis = "level";

        if(strcmp("date", axis) == 0)
        {
            if(setup.climatology)
                for(j = 0; j < n; ++j)
                    values[j] = monthnumber(get_value(cube, axis, j));
            else
                for(j = 0; j < n; ++j)
                    values[j] = grib_date_to_julian(atol(get_value(cube, axis, j))) - grib_date_to_julian(setup.refdate);
        }
        else
        {
            for(j = 0; j < n; ++j)
                values[j] = atol(get_value(cube, axis, j));
        }

        stat = nc_inq_varid(ncid, (lowaxis), &var_id);
        check_err(stat, __LINE__, __FILE__);

        /* if ( strcmp("time", axis) == 0 && setup.unlimited != NULL && strcmp(setup.unlimited, "time") == 0 && setup.usevalidtime) */
        /* GRIB-437, GRIB-625 Special treatment of RECORD (unlimited) dimension */
        /* See "The NetCDF C Interface Guide" Section 6.23 */
        if ( setup.unlimited != NULL && strcmp(setup.unlimited, axis) == 0 )
        {
            /* This is tricky. I'm not sure it works when this dimension is not outer dimension */
            size_t start[NC_MAX_DIMS];
            size_t count[NC_MAX_DIMS];
            nc_type dim_type = 0;

            start[0] = 0;
            count[0] = n;

            stat = nc_inq_vartype(ncid, var_id, &dim_type); /* get the type of this dimension */
            check_err(stat, __LINE__, __FILE__);

            stat = nc_put_vara_type(ncid, var_id, start, count, values, dim_type);
            check_err(stat, __LINE__, __FILE__);
        }
        else
        {
            stat = nc_put_var_int(ncid, var_id, values);
            check_err(stat, __LINE__, __FILE__);
        }

        grib_context_free(ctx, values);
    }

    return 0;
}

static void remove_param(request *r, void *data, const char *p)
{
    request *config = (request *) data;
    const char *ignore;
    int i = 0;

    while((ignore = get_value(config, p, i++)) != NULL) {
        unset_value(r, ignore);
    }
}

static void print_ignored_keys(FILE* f, request* data)
{
    const char *ignore = NULL;
    int i = 0;
    while ((ignore = get_value(data, "ignore", i)) != NULL) {
        if (i==0) {
            fprintf(f, "%s: Ignoring key(s): %s", grib_tool_name, ignore);
        } else {
            fprintf(f, ", %s", ignore);
        }
        ++i;
    }
    if (i>0) fprintf(f, "\n");
}

#define NO_TABLE -1
#define NO_PARAM  0

static void paramtable(const char *p, long *param, long *table, boolean paramIdMode)
{
    const char *q = p;
    int len = strlen(p);

    *param = atol(p);

    while(p && (*p != '.') && ((p - q) < len))
        ++p;

    if((*p == '.'))
        *table = atol(++p);

    /* This version is grib_api... It should rely on what grib_api returns,
       either param.table or paramId
     */
    if(paramIdMode)
    {
        /* Special case for param=228015 => 15.228 */
        if((*param != NO_PARAM) && (*table == NO_TABLE) && (len == 6))
        {
            char tbl[4];
            char par[4];
            p = q;
            strncpy(tbl, p, 3);
            tbl[3] = '\0';
            strncpy(par, p + 3, 3);
            par[3] = '\0';
            *param = atol(par);
            *table = atol(tbl);
        }
    }
}

static void find_nc_attributes(const request *subset_r, const request *user_r, ncatt_t *att, const request *config_r, const request *data_r)
{
    const char *split = NULL;
    int j = 0;
    boolean set_param_as_name = TRUE;
    long datatable = 0; /* = atol(get_value(data_r,"_CODETABLE2",0)); */

    if(count_values(user_r, "split") == 0)
        strcat(att->name, "data");

    while((split = get_value(user_r, "split", j++)) != NULL)
    {
        if(strcmp(split, "param") != 0)
        {
            if(count_values(data_r, split) > 1)
                set_param_as_name = FALSE;
        }
    }

    j = 0;
    while((split = get_value(user_r, "split", j++)) != NULL)
    {
        boolean found = FALSE;
        request *cfg = (request *) config_r;
        boolean is_param = strcmp(split, "param") == 0;
        /* Only use this parameter in the name if there is more
           than one value in the original request or if param */
        boolean setname = ((count_values(data_r, split) > 1) || (is_param && set_param_as_name));

        while(cfg)
        {
            const char *cfgname = get_value(cfg, "NAME", 0);
            const char *cfgval = get_value(cfg, "VALUE", 0);
            const char *dataval = NULL;
            int i = 0;

            if(strcmp(split, cfgname) == 0)
            {
                while((dataval = get_value(subset_r, cfgname, i++)) != NULL)
                {
                    const char *tablestr = get_value(cfg, "TABLE2", 0);
                    long cfgtable = (is_param && tablestr != NULL) ? atol(get_value(cfg, "TABLE2", 0)) : -1;

                    long cfgparam = atol(cfgval);
                    long dataparam = atol(dataval);
                    paramtable(dataval, &dataparam, &datatable, FALSE);

                    /* If it is not param and they're EXACTLY equal or
                       being param, they're the same parameter and table */
                    if((!is_param && (strcmp(dataval, cfgval) == 0)) || (is_param && (dataparam == cfgparam) && (datatable == cfgtable || (datatable == 0 && (cfgtable == 128)))))
                    {
                        const char *val = NULL;
                        const char *metafile = NULL;

                        if((val = get_value(cfg, "accuracy", 0)) != NULL)
                            att->nctype = translate_nctype(val);
                        att->long_name = grib_context_strdup(ctx, (get_value(cfg, "LONG_NAME", 0)));
                        att->short_name = grib_context_strdup(ctx, (get_value(cfg, "SHORT_NAME", 0)));
                        att->units = grib_context_strdup(ctx, (get_value(cfg, "UNITS", 0)));

                        /* Check if there is more metadata for this variable */
                        if((metafile = get_value(cfg, "METADATA", 0)) != NULL)
                        {
                            static const char *metadata_dir = NULL;
                            char metapath[1024];

                            if(!metadata_dir)
                                metadata_dir = getenv("METADATA_DIR");

                            sprintf(metapath, "%s/%s", metadata_dir ? metadata_dir : ".", metafile);
                            att->metadata = 0;/* read_request_file(metapath); */

                        }

                        if(setname)
                        {
                            const char *pname = get_value(cfg, "DATA", 0);
                            if(strlen(att->name))
                                strcat(att->name, "_");
                            strcat(att->name, pname);
                        }
                        found = TRUE;

                        grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: Var. name found: '%s'", att->name);
                    }
                }
            }
            cfg = cfg->next;
        }

        /* We have not found configuration for this attribute */
        if(!found)
        {
            const char *val = get_value(subset_r, split, 0);
            if(val && (setname))
            {
                if(strlen(att->name))
                    strcat(att->name, "_");
                strcat(att->name, val);
            }
        }
    }

    /* NetCDF does not allow variable names to start with a digit */
    if(!isalpha(att->name[0]))
    {
        char buf[1024];
        sprintf(buf,"p%s",att->name);
        strcpy(att->name,buf);
    }
}

static request *first;
static request *last;

static void reqcb(const request *r, int count, axis_t *names, char *vals[], void *data)
{
    request *w = clone_one_request(r);
    int i;
    /*request **d = (request**)data;*/

    int *n = (int*) data;

    w->order = (*n)++;
    for(i = 0; i < count; i++)
    {

        if(vals[i])
            put_value(w, names[i].name, vals[i], FALSE, FALSE, FALSE);
    }

    if(first == NULL)
        first = w;
    else
        last->next = w;
    last = w;
}

static boolean chk_152(int count, axis_t *names, char *vals[])
{
    return TRUE;
}

static void loop(const request *r, boolean ml, int index, int count, axis_t* strings, char *values[], loopproc callback, void *data)
{
    if(index < count)
    {
        parameter *p = find_parameter(r, strings[index].name);

        (void) count_values(r, strings[index].name); /* force list expansion */

        if(p)
        {
            value *v = p->values;

            while(v)
            {
                values[index] = v->name;
                loop(r, ml, index + 1, count, strings, values, callback, data);
                v = v->next;
            }
        }
        else
        {
            values[index] = NULL;
            loop(r, ml, index + 1, count, strings, values, callback, data);
        }

    }
    else if(!ml || chk_152(count, strings, values))
        callback(r, count, strings, values, data);
}

static void values_loop(const request *r, int count, axis_t *parnames, loopproc callback, void *data)
{
    char **values = (char**) grib_context_malloc(ctx, sizeof(char*) * count);
    const char * p = get_value(r, "levtype", 0);
    boolean ml = (boolean) (p && (strcmp(p, "ml") == 0));

    if(ml)
    {
        p = get_value(r, "expect", 0);
        if(p && atol(p) != 0)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "EXPECT provided, special treatment of LNSP");
            grib_context_log(ctx, GRIB_LOG_ERROR, "and other single level parameters disabled");
            ml = FALSE;
        }
    }

    loop(r, ml, 0, count, parnames, values, callback, data);

    grib_context_free(ctx, values);
}

static void names_loop(const request *r, loopproc proc, void *data)
{
    values_loop(r, NUMBER(global_axis), global_axis, proc, data);
}

static request *unwind_one_request(const request *r)
{
    int n = 0;
    first = last = NULL;
    names_loop(r, reqcb, &n);
    return first;
}

static int split_fieldset(fieldset *fs, request *data_r, dataset_t **subsets, const request *user_r, const request *config_r)
{
    const char *split = NULL;
    int count = 1;
    int i = 0;
    request *s = NULL;
    request *u = NULL;
    dataset_t *filters = NULL;
    nc_type nctype = translate_nctype(get_value(user_r, "accuracy", 0));

    s = empty_request("filter");
    while((split = get_value(user_r, "split", i++)) != NULL)
    {
        int cnt = count_values(data_r, split);
        if(cnt)
        {
            count *= count_values(data_r, split);
            valcpy(s, data_r, (char *) split, (char *) split);
        }
    }

    u = unwind_one_request(s);

    free_all_requests(s);

    filters = (dataset_t*)calloc(sizeof(dataset_t), count);
    Assert(filters);

    s = u;
    for(i = 0; i < count; ++i)
    {
        Assert(s);
        filters[i].filter = new_hypercube_from_mars_request(s);
        filters[i].fset = new_fieldset(1);
        filters[i].count = 0;
        filters[i].filter_request = clone_one_request(s);
        filters[i].bitmap = FALSE;
        /* filters[i].mmeans  = FALSE; */

        s = s->next;
    }

    for(i = 0; i < fs->count; ++i)
    {
        boolean ok = FALSE;
        field *f = get_field(fs, i, packed_mem);
        request *g = field_to_request(f);
        int j = 0;

        while(!ok && (j < count))
        {
            ok = (cube_order(filters[j].filter, g) != -1);
            if(ok)
            {
                const char *p;
                set_field(filters[j].fset, f, filters[j].count++);
                filters[j].bitmap |= f->has_bitmap;
                if((p = get_value(f->r->r, "_units", 0)) != NULL)
                {
                    filters[j].att.units = grib_context_strdup(ctx, p);
                }
                if((p = get_value(f->r->r, "_long_name", 0)) != NULL)
                {
                    filters[j].att.long_name = grib_context_strdup(ctx, p);
                }
                if((p = get_value(f->r->r, "_cf_name", 0)) != NULL)
                {
                    filters[j].att.standard_name = grib_context_strdup(ctx, p);
                }
            }
            j++;
        }
        if(!ok)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "Internal error. Lost field %d while splitting fieldset", i);
            print_all_requests(g);
            grib_context_log(ctx, GRIB_LOG_ERROR, "count is %d", count);
            grib_context_log(ctx, GRIB_LOG_ERROR, "First cube is:");
            print_hypercube(filters[0].filter);
            exit(1);
        }
        /* f->purge_header = TRUE; */
        release_field(f);
    }

    for(i = 0; i < count; ++i)
    {
        filters[i].att.nctype = nctype;
        filters[i].scale = TRUE;
        filters[i].missing = nc_type_values[nctype].nc_type_missing;
        find_nc_attributes(filters[i].filter_request, user_r, &(filters[i].att), config_r, data_r);
        grib_context_log(ctx, GRIB_LOG_DEBUG, "grib_to_netcdf: filter[%d] found.- Var. name '%s', nctype: %d, found nctype: %d", i, filters[i].att.name, nctype, filters[i].att.nctype);

        if(strlen(filters[i].att.name) == 0)
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "Variable name not found");
            exit(1);
        }

        /* Check if we need scaling */
        switch(filters[i].att.nctype)
        {
        case NC_FLOAT:
        case NC_DOUBLE:
            filters[i].scale = FALSE;
            break;
        default:
            filters[i].scale = TRUE;
            break;
        }
    }

    free_all_requests(u);
    *subsets = filters;
    return count;
}

static void free_subsets(dataset_t *subsets, int count)
{
    int i = 0;

    for(i = 0; i < count; ++i)
    {
        grib_context_free(ctx, subsets[i].att.units);
        grib_context_free(ctx, subsets[i].att.long_name);
        grib_context_free(ctx, subsets[i].att.short_name);
        grib_context_free(ctx, subsets[i].att.standard_name);
        free_all_requests(subsets[i].att.metadata);
        free_hypercube(subsets[i].filter);
        free_fieldset(subsets[i].fset);
        free_all_requests(subsets[i].filter_request);
    }
    grib_context_free(ctx, subsets);
}

/* Return the number of the given month or -1 if it fails to match */
static int convert_month(const char* pMonthString)
{
    if (strcmp(pMonthString, "jan")==0) return 1;
    if (strcmp(pMonthString, "feb")==0) return 2;
    if (strcmp(pMonthString, "mar")==0) return 3;
    if (strcmp(pMonthString, "apr")==0) return 4;
    if (strcmp(pMonthString, "may")==0) return 5;
    if (strcmp(pMonthString, "jun")==0) return 6;
    if (strcmp(pMonthString, "jul")==0) return 7;
    if (strcmp(pMonthString, "aug")==0) return 8;
    if (strcmp(pMonthString, "sep")==0) return 9;
    if (strcmp(pMonthString, "oct")==0) return 10;
    if (strcmp(pMonthString, "nov")==0) return 11;
    if (strcmp(pMonthString, "dec")==0) return 12;
    return -1; /*Failed*/
}

static boolean parsedate(const char *name, long* julian, long *second, boolean* isjul)
{
    const char *p = name;
    int n;
    int y = 0, m = 0, d = 0, H = 0, M = 0, S = 0;

    *julian = *second = 0;
    *isjul = FALSE;

    if(p == 0 || *p == 0)
        return FALSE;

    /* Special ERA Interim grib1 date format: jul-21, sep-02 etc
     * See GRIB-416
     */
    if (isalpha(*p))
    {
        char month[32];
        int day = 0;
        int n = sscanf(p, "%[^-]-%d", month, &day);
        /* Matched two items (month and day) and month is 3 letters */
        if (n == 2 && strlen(month) == 3) {
            y = 1900; /* no year specified */
            m = convert_month(month);
            if (m == -1) return FALSE;
            *julian = grib_date_to_julian(y * 10000 + m * 100 + day);
            *second = 0;
            return TRUE;
        }
    }

    /* year */
    p = parse1(p, &y, &n);
    if(n != 2 && n != 4) /* year string must be 2 or 4 characters long: 93 or 1993 */
        return FALSE;
    if(*p++ != '-')
        return FALSE;

    /* month */
    p = parse1(p, &m, &n);
    if(n == 2)
    {
        /* day */
        if(*p++ != '-')
            return FALSE;
        p = parse1(p, &d, &n);
        if(n != 2)
            return FALSE;
    }
    else if(n == 3)
    {
        long j = grib_date_to_julian(y * 10000 + 101) + m - 1;
        j = grib_julian_to_date(j);
        /* julian day */;
        d = j % 100;
        m = (j % 10000) / 100;
        *isjul = TRUE;
    }
    else
        return FALSE;

    if (m == 0 || m > 12) {
        return FALSE; /* month out of range */
    }

    while(*p && isspace(*p))
        p++;

    /* hour */
    p = parse1(p, &H, &n);
    if(n != 0)
    {
        if(n != 2)
            return FALSE;

        /* minute */
        if(*p++ != ':')
            return FALSE;

        p = parse1(p, &M, &n);
        if(n != 2)
            return FALSE;

        if(*p != 0)
        {
            /* second */
            if(*p++ != ':')
                return FALSE;
            p = parse1(p, &S, &n);
            if(n != 2)
                return FALSE;
        }
    }

    *julian = grib_date_to_julian(y * 10000 + m * 100 + d);
    *second = H * 3600 + M * 60 + S;

    return *p == 0 ? TRUE : FALSE;
}

static boolean check_dimension_name(const char* dim)
{
    /* Dimension name must begin with an alphabetic character, followed by zero
     * or more alphanumeric characters including the underscore */
    int i = 0, len = 0;
    if (!dim) return FALSE;

    len = strlen(dim);
    if (len==0)  return FALSE;
    if (!isalpha(dim[0])) return FALSE;

    for (i=1; i<len; ++i) {
        const char c = dim[i];
        const int ok = isalnum(c) || c == '_';
        if (!ok) return FALSE;
    }
    return TRUE;
}

static int get_creation_mode(int option_kind)
{
    /* Return the mode flag for nc_create based   */
    /* on the kind of netcdf user wants to create */
    int creation_mode = NC_CLOBBER;
    switch(option_kind) {
    case NC_FORMAT_CLASSIC:
        printf("%s: Creating classic file format.\n", grib_tool_name);
        break;
    case NC_FORMAT_64BIT:
        creation_mode |= NC_64BIT_OFFSET;
        printf("%s: Creating large (64 bit) file format.\n", grib_tool_name);
        break;
#ifdef NC_NETCDF4
    case NC_FORMAT_NETCDF4:
        creation_mode |= NC_NETCDF4;
        printf("%s: Creating netCDF-4/HDF5 format.\n", grib_tool_name);
        break;
    case NC_FORMAT_NETCDF4_CLASSIC:
        creation_mode |= NC_NETCDF4 | NC_CLASSIC_MODEL;
        printf("%s: Creating netCDF-4 classic model file format.\n", grib_tool_name);
        break;
#else
    case NC_FORMAT_NETCDF4:
    case NC_FORMAT_NETCDF4_CLASSIC:
        fprintf(stderr,"%s not built with netcdf4, cannot create netCDF-4 files.\n", grib_tool_name);
        exit(1);
        break;
#endif
    default:
        fprintf(stderr, "Bad value (%d) for -k option\n", option_kind);
        exit(1);
        break;
    }
    return creation_mode;
}
/*=====================================================================*/

grib_option grib_options[] = {
        { "I:", "key1,key2,...", "\n\t\tIgnore keys. Default method,type,stream,refdate,hdate\n", 0, 1, "method,type,stream,refdate,hdate" },
        { "S:", "key1,key2,...", "\n\t\tSplit according to keys. Default param,expver\n", 0, 1, "param,expver" },
        { "R:", "date",          "\n\t\tReference date in the format YYYYMMDD. Default value 19000101.\n", 0, 1, "19000101" },
        { "D:", "NC_DATATYPE",   "\n\t\tType of data. Possible values NC_BYTE, NC_SHORT, NC_INT, NC_FLOAT, NC_DOUBLE."
                "\n\t\tDefault NC_SHORT\n", 0, 1, "NC_SHORT" },
        { "T", 0, "Don't use time of validity.\n", 0, 1, 0 },
        { "f", 0, 0, 0, 1, 0 },
        { "o:", "output file",   "\n\t\tThe name of the netcdf file.\n", 1, 1, 0 },
        { "V", 0, 0, 0, 1, 0 },
        { "M", 0, 0, 0, 1, 0 },
        { "k:", "kind",  "\n\t\tSpecifies the kind of file to be created. Possible values are:"
                "\n\t\t1 -> netCDF classic file format"
                "\n\t\t2 -> netCDF 64 bit classic file format (Default)"
                "\n\t\t3 -> netCDF-4 file format"
                "\n\t\t4 -> netCDF-4 classic model file format\n"
                , 0, 1, "2" },
        { "u:", "dimension",  "\n\t\tSet dimension to be an unlimited dimension.\n", 0, 1, "time" }
};

int grib_options_count = sizeof(grib_options) / sizeof(grib_option);
static fieldset *fs = NULL;
static request* data_r = NULL;
request *user_r = NULL;
static int option_kind = 2; /* By default NetCDF3, 64-bit offset */

/* Table of formats for legal -k values. Inspired by nccopy */
struct KindValue {
    char* name;
    int kind;
} legalkinds[] = {
        {"1", NC_FORMAT_CLASSIC},
        {"classic", NC_FORMAT_CLASSIC},

        /* The 64-bit offset kind */
        {"2", NC_FORMAT_64BIT},
        {"64-bit-offset", NC_FORMAT_64BIT},
        {"64-bit offset", NC_FORMAT_64BIT},

        /* NetCDF-4 HDF5 format */
        {"3", NC_FORMAT_NETCDF4},
        {"hdf5", NC_FORMAT_NETCDF4},
        {"netCDF-4", NC_FORMAT_NETCDF4},
        {"netCDF4", NC_FORMAT_NETCDF4},
        {"enhanced", NC_FORMAT_NETCDF4},

        /* NetCDF-4 HDF5 format, but using only nc3 data model */
        {"4", NC_FORMAT_NETCDF4_CLASSIC},
        {"hdf5-nc3", NC_FORMAT_NETCDF4_CLASSIC},
        {"netCDF-4 classic model", NC_FORMAT_NETCDF4_CLASSIC},
        {"netCDF4_classic", NC_FORMAT_NETCDF4_CLASSIC},
        {"enhanced-nc3", NC_FORMAT_NETCDF4_CLASSIC},

        /* null terminate*/
        {NULL,0}
};

int main(int argc, char *argv[])
{
    int i, ret = 0;

    /* GRIB-413: Collect all program arguments into a string */
    for (i=0; i<argc; ++i) {
        strcat(argvString, argv[i]);
        if (i != argc-1) strcat(argvString, " ");
    }
    ret = grib_tool(argc, argv);

    return ret;
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    char* p = NULL;
    char* list = NULL;
    ctx = grib_context_get_default();
    options->onlyfiles = 1;
    fs = new_fieldset(0);
    data_r = empty_request(0);
    user_r = empty_request(0);

    printf("%s: Version ", grib_tool_name);
    grib_print_api_version(stdout);
    printf("\n");

    if(grib_options_on("D:"))
    {
        set_value(user_r, "accuracy", grib_options_get_option("D:"));
    }
    else
    {
        set_value(user_r, "accuracy", "NC_SHORT");
    }

    /* Option -S: Split according to keys */
    if(grib_options_on("S:"))
    {
        list = grib_options_get_option("S:");
        p = strtok(list, ",");
        set_value(user_r, "split", p);
        p = strtok(NULL, ",");
        while(p != NULL)
        {
            add_value(user_r, "split", p);
            p = strtok(NULL, ",");
        }
    }
    else
    {
        set_value(user_r, "split", "param");
        add_value(user_r, "split", "expver");
    }

    /* Option -I: Ignore keys */
    if (grib_options_on("I:"))
    {
        list = grib_options_get_option("I:");
        p = strtok(list, ",");
        set_value(user_r, "ignore", p);
        p = strtok(NULL, ",");
        while(p != NULL) {
            add_value(user_r, "ignore", p);
            p = strtok(NULL, ",");
        }
    }
    else {
        set_value(user_r, "ignore", "method");
        add_value(user_r, "ignore", "type");
        add_value(user_r, "ignore", "stream");
        add_value(user_r, "ignore", "refdate");
        add_value(user_r, "ignore", "hdate");
    }

    if(grib_options_on("T"))
        set_value(user_r, "usevalidtime", "false");
    else
        set_value(user_r, "usevalidtime", "true");

    if (grib_options_on("k:"))
    {
        struct KindValue* kvalue = NULL;
        char* kind_name = grib_options_get_option("k:");
        for (kvalue=legalkinds; kvalue->name; kvalue++) {
            if (strcmp(kind_name, kvalue->name) == 0) {
                option_kind = kvalue->kind;/* Found the right kind */
                break;
            }
        }
        if (kvalue->name == NULL) {
            fprintf(stderr, "Invalid format: %s", kind_name);
            usage();
            exit(1);
        }
    }

    if(grib_options_on("R:"))
    {
        char* theArg = grib_options_get_option("R:");
        if (!is_number(theArg)) {
            fprintf(stderr, "Invalid reference date: %s\n", theArg);
            usage();
            exit(1);
        }
        set_value(user_r, "referencedate", theArg);
    }
    else
    {
        set_value(user_r, "referencedate", "19000101");
    }

    if (grib_options_on("u:")) {
        char* theDimension = grib_options_get_option("u:");
        if ( !check_dimension_name(theDimension) ) {
            fprintf(stderr, "Invalid dimension: \"%s\"\n", theDimension);
            exit(1);
        }
        set_value(user_r, "unlimited", theDimension);
    }

    get_nc_options(user_r);

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options, const char* filename)
{
    char buf[1024] = { 0, };
    int e = 0;
    int i = 0;
    grib_handle* h = NULL;
    grib_file* file = NULL;

    printf("%s: Processing input file '%s'.\n", grib_tool_name, filename);

    file = grib_file_open(filename, "r", &e);
    if(!file || !file->handle)
        return e;

    fseeko(file->handle, 0, SEEK_SET);

    files++;

    while((h = grib_handle_new_from_file(ctx, file->handle, &e)) != NULL)
    {
        long length;
        field* g;
        request *r;

        /* process only GRIB for the moment*/
        size_t size = sizeof(buf);
        Assert(grib_get_string(h, "identifier", buf, &size) == 0);
        if(strcmp(buf, "GRIB"))
        {
            grib_handle_delete(h);
            continue;
        }
        Assert(grib_get_long(h,"totalLength",&length) == 0);

        g = read_field(file, h->offset, length);

        r = empty_request("");
        Assert(handle_to_request(r,h) == 0);

        /* Keep full MARS description */
        /* copy = clone_one_request(r); */
        /* reqmerge(setup.mars_description,copy); */
        /* reqmerge(setup.mars_description,r); */

        if(i == 1)
        {
            const char *mmeans = get_value(r, "_MONTHLY_MEANS", 0);
            setup.mmeans = mmeans ? (atol(mmeans) == 1) : FALSE;
        }
        fcmonth2nbmonths(r);

        if(!setup.refdate)
        {
            if(setup.auto_refdate)
                setup.refdate = atol(get_value(r, "date", 0));
            else
            {
                const char *p = get_value(user_r, "referencedate", 0);
                if(is_number(p))
                    setup.refdate = atol(p);
                else
                {
                    long julian = 0, second = 0;
                    boolean isjul;
                    parsedate(p, &julian, &second, &isjul);
                    setup.refdate = grib_julian_to_date(julian);
                }
            }
        }

        validation_time(r);
        if(setup.usevalidtime)
        {
            unset_value(r, "date");
            unset_value(r, "time");
            unset_value(r, "step");
            unset_value(r, "fcmonth");

            set_value(r, "time", "%.2lf", atof(get_value(r, "_validationtime", 0)));
        }

        g->r = new_field_request(r);

        set_field(fs, g, i++);

        reqmerge(data_r, r);
        free_all_requests(r);

        grib_handle_delete(h);
    }

    grib_file_close(file->name, 0, &e);

    {
        /* Now do some checks */
        request* temp_data_r = fieldset_to_request(fs);
        if(setup.checkvalidtime)
        {
            int cnt = request_fields(temp_data_r);
            if(fs->count != i || (cnt < i))
            {
                grib_context_log(ctx, GRIB_LOG_ERROR, "Wrong number of fields");
                grib_context_log(ctx, GRIB_LOG_ERROR, "File contains %d GRIBs, %d left in internal description, %d in request", i, fs->count, cnt);
                grib_context_log(ctx, GRIB_LOG_ERROR, "The fields are not considered distinct!\n");
                /*grib_context_log(ctx, GRIB_LOG_ERROR, "MARS description");*/
                /*print_all_requests(setup.mars_description);*/
                if (ctx->debug) {
                    grib_context_log(ctx, GRIB_LOG_ERROR, "Internal description");
                    print_all_requests(temp_data_r);
                }
                grib_context_log(ctx, GRIB_LOG_ERROR, "Hint: This may be due to several fields having the same validity time.");
                grib_context_log(ctx, GRIB_LOG_ERROR, "Try using the -T option (Do not use time of validity)");
                exit(1);
            }
        }
        free_all_requests(temp_data_r);
    }

    return e;
}

int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options, grib_handle* h)
{
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    request *config_r = NULL;
    hypercube *dims = NULL;
    dataset_t *subsets = NULL;
    int count;
    int i;
    int ncid;
    int stat;
    int err = 0;
    int creation_mode = NC_CLOBBER;

    if(options->outfile==NULL || options->outfile->name==NULL)
    {
        usage();
        exit(1);
    }

    if(fs->count == 0)
    {
        grib_context_log(ctx, GRIB_LOG_ERROR, "Input does not contain any field. Exiting!");
        return -1;
    }

    printf("%s: Found %d GRIB field%s in %d file%s.\n", grib_tool_name, fs->count, fs->count>1?"s":"", files, files > 1 ? "s" : "");

    /*
     grib_context_log(ctx, GRIB_LOG_INFO, "Request representing %d fields ", fs->count);
     print_all_requests(data_r);
     */

    /* Split the SOURCE from request into as many datasets as specified */
    count = split_fieldset(fs, data_r, &subsets, user_r, config_r);
    remove_param(data_r, (void *) user_r, "ignore");
    remove_param(data_r, (void *) user_r, "split");
    print_ignored_keys(stdout, user_r);

    dims = new_simple_hypercube_from_mars_request(data_r);

    /* In case there is only 1 DATE+TIME+STEP, set at least 1 time as axis */
    set_always_a_time(dims, data_r);

    /* Create netcdf file */

    printf("%s: Creating netCDF file '%s'\n", grib_tool_name, options->outfile->name);
    printf("%s: NetCDF library version: %s\n", grib_tool_name, nc_inq_libvers());

    creation_mode = get_creation_mode(option_kind);
    stat = nc_create(options->outfile->name, creation_mode, &ncid);
    check_err(stat, __LINE__, __FILE__);

    /* Define netcdf dataset */
    err = define_netcdf_dimensions(dims, fs, ncid, subsets, count, data_r);
    if (err != GRIB_SUCCESS) {
        stat = nc_close(ncid);
        check_err(stat, __LINE__, __FILE__);
        stat = nc_delete(options->outfile->name);
        check_err(stat, __LINE__, __FILE__);

        exit(1);
    }

    /* End definitions */
    stat = nc_enddef(ncid);
    check_err(stat, __LINE__, __FILE__);

    /* Fill dimensions */
    fill_netcdf_dimensions(dims, fs, ncid);

    /* Put data values */
    for(i = 0; i < count; ++i)
    {
        if(subsets[i].fset)
        {
            char dataset[100];

            sprintf(dataset, subsets[i].att.name, i + 1);
            put_data(dims, ncid, dataset, &subsets[i]);
        }
        else
        {
            grib_context_log(ctx, GRIB_LOG_ERROR, "Fieldset %d is empty!!", i + 1);
        }
    }

    stat = nc_close(ncid);
    check_err(stat, __LINE__, __FILE__);

    free_all_requests(data_r);
    free_hypercube(dims);
    free_fieldset(fs);
    free_subsets(subsets, count);

    free_nc_options();
    printf("%s: Done.\n", grib_tool_name);

    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}


#else
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv)
{
    printf("\n");
    printf("grib_to_netcdf:\n");
    printf("\n");
    printf(" ecCodes was not compiled with NETCDF enabled\n");
    printf("\n");
    exit(1);
}
#endif
