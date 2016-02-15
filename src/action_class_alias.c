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
 *   Enrico Fucile
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   IMPLEMENTS = create_accessor
   IMPLEMENTS = dump;xref
   IMPLEMENTS = destroy
   IMPLEMENTS = compile
   MEMBERS    = char* target
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "action.class" and rerun ./make_class.pl

*/

static void init_class      (grib_action_class*);
static void dump            (grib_action* d, FILE*,int);
static void xref            (grib_action* d, FILE* f,const char* path);
static void compile         (grib_action* a, grib_compiler* compiler);
static void destroy         (grib_context*,grib_action*);
static int create_accessor(grib_section*,grib_action*,grib_loader*);


typedef struct grib_action_alias {
    grib_action          act;  
/* Members defined in alias */
	char* target;
} grib_action_alias;


static grib_action_class _grib_action_class_alias = {
    0,                              /* super                     */
    "action_class_alias",                              /* name                      */
    sizeof(grib_action_alias),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    &xref,                               /* xref                      */

    &create_accessor,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    0,                            /* execute */
    &compile,                            /* compile */
};

grib_action_class* grib_action_class_alias = &_grib_action_class_alias;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */


grib_action* grib_action_create_alias(grib_context* context, const char* name, const char* arg1,const char* name_space,int flags)
{
    grib_action_alias* a                            ;
    grib_action_class* c   =  grib_action_class_alias;
    grib_action* act       = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);

    act->context = context;

    act->op           = NULL;
    act->name         = grib_context_strdup_persistent(context, name);
    if(name_space)
        act->name_space =  grib_context_strdup_persistent(context, name_space);

    act->cclass       = c;
    act->flags        = flags;
    a                 = (grib_action_alias*)act;
    a->target         = arg1 ? grib_context_strdup_persistent(context, arg1) : NULL;

    return act;
}

static void compile(grib_action* act, grib_compiler *compiler)
{
    grib_action_alias* a  = (grib_action_alias*)act;
    fprintf(compiler->out,"%s = grib_action_create_alias(ctx,",compiler->var);
    fprintf(compiler->out,"\"%s\",",act->name);
    if(a->target) {
        fprintf(compiler->out,"\"%s\",",a->target);
    }
    else
    {
        fprintf(compiler->out,"NULL,");
    }
    if(act->name_space) {
        fprintf(compiler->out,"\"%s\",",act->name_space);
    }
    else
    {
        fprintf(compiler->out,"NULL,");
    }
    grib_compile_flags(compiler, act->flags);
    fprintf(compiler->out,");");
    fprintf(compiler->out,"\n");
}

static int same(const char* a,const char* b) 
{
    if(a == b) return 1;
    if(a && b) return (strcmp(a,b) == 0);
    return 0;
}

static int create_accessor( grib_section* p, grib_action* act,grib_loader *h)
{
    int i,j,id;
    grib_action_alias* self = (grib_action_alias*)act;
    grib_accessor *x=NULL ;
    grib_accessor *y=NULL ;

    /*if alias and target have the same name add only the namespace */
    if (self->target && !strcmp(act->name,self->target) && act->name_space!=NULL) {
        x = grib_find_accessor_fast(p->h,self->target);
        if(x == NULL)
        {
            grib_context_log(p->h->context,GRIB_LOG_DEBUG,"alias %s: cannot find %s (part 1)",
                    act->name,self->target);
            grib_context_log(p->h->context,GRIB_LOG_WARNING,"alias %s: cannot find %s",
                    act->name,self->target);
            return GRIB_SUCCESS;
        }

        if (x->name_space==NULL)
            x->name_space=act->name_space;

        grib_context_log(p->h->context, GRIB_LOG_DEBUG, "alias: add only namespace: %s.%s",
                act->name_space, act->name);
        i = 0;
        while(i < MAX_ACCESSOR_NAMES) {
            if(x->all_names[i] != NULL && !strcmp(x->all_names[i],act->name) ) {
                if (x->all_name_spaces[i]==NULL) {
                    x->all_name_spaces[i] =  act->name_space;
                    return GRIB_SUCCESS;
                } else if (!strcmp(x->all_name_spaces[i],act->name_space) ) {
                    return GRIB_SUCCESS;
                }
            }
            i++;
        }
        i=0;
        while(i < MAX_ACCESSOR_NAMES) {
            if (x->all_names[i]==NULL) {
                x->all_names[i]=act->name;
                x->all_name_spaces[i] =  act->name_space;
                return GRIB_SUCCESS;
            }
            i++;
        }
        grib_context_log(p->h->context,GRIB_LOG_FATAL,
                "unable to alias %s : increase MAX_ACCESSOR_NAMES",act->name);

        return GRIB_INTERNAL_ERROR;
    }

    /* if(self->target == NULL || (act->flags & GRIB_ACCESSOR_FLAG_OVERRIDE)) */
    y = grib_find_accessor_fast(p->h,act->name);

    /* delete old alias if already defined */
    if ( y != NULL )
    {
        i=0;
        while ( i < MAX_ACCESSOR_NAMES && y->all_names[i] )
        {
            if ( same ( y->all_names[i],act->name ) && same ( y->all_name_spaces[i],act->name_space ) )
            {
                grib_context_log ( p->h->context,GRIB_LOG_DEBUG,"alias %s.%s already defined for %s. Deleting old alias",
                        act->name_space,act->name,y->name );
                /* printf("[%s %s]\n",y->all_names[i], y->all_name_spaces[i]); */

                while ( i < MAX_ACCESSOR_NAMES-1 )
                {
                    y->all_names[i]       = y->all_names[i+1];
                    y->all_name_spaces[i] = y->all_name_spaces[i+1];
                    i++;
                }

                y->all_names[MAX_ACCESSOR_NAMES-1]       =  NULL;
                y->all_name_spaces[MAX_ACCESSOR_NAMES-1] =  NULL;

                break;
            }
            i++;
        }

        if ( self->target == NULL )
            return GRIB_SUCCESS;
    }

    if (!self->target) return GRIB_SUCCESS;

    x = grib_find_accessor_fast(p->h,self->target);
    if(x == NULL)
    {
        grib_context_log(p->h->context,GRIB_LOG_DEBUG,"alias %s: cannot find %s (part 2)",
                act->name,self->target);
        grib_context_log(p->h->context,GRIB_LOG_WARNING,"alias %s: cannot find %s",
                act->name,self->target);
        return GRIB_SUCCESS;
    }

    if (grib_handle_of_accessor(x)->use_trie) {
        id=grib_hash_keys_get_id(x->context->keys,act->name);

        if (grib_handle_of_accessor(x)->accessors[id] != x) {
            /*x->same=grib_handle_of_accessor(x)->accessors[id];*/
            grib_handle_of_accessor(x)->accessors[id]=x;
        }
    }

    i = 0;
    while(i < MAX_ACCESSOR_NAMES) {
        if(x->all_names[i] == NULL)
        {
            /* Only add entries if not already there */
            int found = 0;
            for(j=0; j<i && !found; ++j) {
                int nameSame      = same(x->all_names[j], act->name);
                int namespaceSame = same(x->all_name_spaces[j], act->name_space);
                if (nameSame && namespaceSame) {
                    found = 1;
                }
            }
            if (!found) {  /* Not there. So add them */
                x->all_names[i]       =  act->name;
                x->all_name_spaces[i] =  act->name_space;
                grib_context_log(p->h->context, GRIB_LOG_DEBUG, "alias %s.%s added (%s)",
                        act->name_space, act->name, self->target );
            }
            return GRIB_SUCCESS;
        }
        i++;
    }

    for(i = 0 ; i < MAX_ACCESSOR_NAMES; i++)
        grib_context_log(p->h->context,GRIB_LOG_ERROR,"alias %s= ( %s already bound to %s )",
                act->name,self->target,x->all_names[i]);

    return GRIB_SUCCESS;
}

static void dump( grib_action* act, FILE* f, int lvl)
{
    grib_action_alias* a = (grib_action_alias*)act;
    int i =0;
    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");
    if(a->target)
        grib_context_print(act->context,f," alias %s  %s \n", act->name, a->target );
    else
        grib_context_print(act->context,f," unalias %s  \n", act->name);
}

static void xref( grib_action* act, FILE* f,const char* path)
{
    grib_action_alias* a = (grib_action_alias*)act;
    if(a->target) {
        fprintf(f,"bless({name=>'%s', target=>'%s', path=>'%s'},'xref::alias'),\n", act->name, a->target,path );
        if(act->name_space)
            fprintf(f,"bless({name=>'%s.%s', target=>'%s', path=>'%s'},'xref::alias'),\n", act->name_space,act->name, a->target,path );
    }
    else
    {
        fprintf(f,"bless({name=>'%s',  path=>'%s'},'xref::unalias'),\n", act->name, path );
        if(act->name_space)
            fprintf(f,"bless({name=>'%s.%s', path=>'%s'},'xref::unalias'),\n", act->name_space,act->name, path );
    }
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_alias* a = (grib_action_alias*)act;

    if(a->target)
        grib_context_free_persistent(context, a->target);

    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
    grib_context_free_persistent(context, act->name_space);
}
