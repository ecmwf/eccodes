/*
 * Copyright 2005-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *   Enrico  Fucile
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   SUPER      = action_class_gen
   IMPLEMENTS = dump
   IMPLEMENTS = destroy
   MEMBERS    = grib_concept_value* concept
   MEMBERS    = char* basename
   MEMBERS    = char* masterDir
   MEMBERS    = char* localDir
   MEMBERS    = int nofail
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
static void destroy         (grib_context*,grib_action*);


typedef struct grib_action_concept {
    grib_action          act;  
/* Members defined in gen */
	long            len;
	grib_arguments* params;
/* Members defined in concept */
	grib_concept_value* concept;
	char* basename;
	char* masterDir;
	char* localDir;
	int nofail;
} grib_action_concept;

extern grib_action_class* grib_action_class_gen;

static grib_action_class _grib_action_class_concept = {
    &grib_action_class_gen,                              /* super                     */
    "action_class_concept",                              /* name                      */
    sizeof(grib_action_concept),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    &dump,                               /* dump                      */
    0,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    0,                            /* execute */
    0,                            /* compile */
};

grib_action_class* grib_action_class_concept = &_grib_action_class_concept;

static void init_class(grib_action_class* c)
{
	c->xref	=	(*(c->super))->xref;
	c->create_accessor	=	(*(c->super))->create_accessor;
	c->notify_change	=	(*(c->super))->notify_change;
	c->reparse	=	(*(c->super))->reparse;
	c->execute	=	(*(c->super))->execute;
	c->compile	=	(*(c->super))->compile;
}
/* END_CLASS_IMP */

static grib_concept_value* get_concept(grib_handle* h,grib_action_concept* self);

grib_action* grib_action_create_concept( grib_context* context,
        const char* name,
        grib_concept_value* concept,
        const char* basename,const char* name_space,const char* defaultkey,
        const char* masterDir,const char* localDir,const char* ecmfDir,int flags,int nofail )
{
    grib_action_concept* a=NULL ;
    grib_action_class* c = grib_action_class_concept;
    grib_action* act     = (grib_action*)grib_context_malloc_clear_persistent(context,c->size);
    act->op              = grib_context_strdup_persistent(context,"concept");

    act->cclass       = c;
    a=(grib_action_concept*)act ;
    act->context      = context;
    act->flags        = flags;

    if (name_space)
        act->name_space  = grib_context_strdup_persistent(context,name_space);

    if (basename)
        a->basename= grib_context_strdup_persistent(context,basename);
    else a->basename=NULL;

    if (masterDir)
        a->masterDir= grib_context_strdup_persistent(context,masterDir);
    else a->masterDir=NULL;

    if (localDir)
        a->localDir= grib_context_strdup_persistent(context,localDir);
    else a->localDir=NULL;

    if (defaultkey)
        act->defaultkey = grib_context_strdup_persistent(context,defaultkey);

    a->concept     = concept;
    if (concept) {
        grib_concept_value* c=concept;
        grib_trie* index=grib_trie_new(context);
        while (c) {
            c->index=index;
            grib_trie_insert_no_replace(index,c->name,c);
            c=c->next;
        }
    }
    act->name = grib_context_strdup_persistent(context,name);

    a->nofail=nofail;

    return act;
}

static void dump(grib_action* act, FILE* f, int lvl)
{
    int i = 0;

    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");

    printf("concept(%s) { ",act->name);
    printf("\n");

    for (i=0;i<lvl;i++)
        grib_context_print(act->context,f,"     ");
    printf("}\n");
}

static void destroy(grib_context* context,grib_action* act)
{
    grib_action_concept* self = (grib_action_concept*) act;

    grib_concept_value * v = self->concept;
    if (v) grib_trie_delete(v->index);
    while(v)
    {
        grib_concept_value* n = v->next;
        grib_concept_value_delete(context,v);
        v = n;
    }
    grib_context_free_persistent(context, self->masterDir);
    grib_context_free_persistent(context, self->localDir);
    grib_context_free_persistent(context, self->basename);
}

static grib_concept_value* get_concept(grib_handle* h,grib_action_concept* self)
{
    char buf[1024]={0,};
    char master[1024]={0,};
    char local[1024]={0,};
    char masterDir[1024]={0,};
    size_t lenMasterDir=1024;
    char key[1024]={0,};
    char* full=0;
    int id;

    grib_context* context=((grib_action*)self)->context;
    grib_concept_value* c=NULL;

    if (self->concept != NULL)
        return self->concept;

    Assert(self->masterDir);
    grib_get_string(h,self->masterDir,masterDir,&lenMasterDir);

    sprintf(buf,"%s/%s",masterDir,self->basename);

    grib_recompose_name(h,NULL, buf, master,1);

    if (self->localDir) {
        char localDir[1024]={0,};
        size_t lenLocalDir=1024;
        grib_get_string(h,self->localDir,localDir,&lenLocalDir);
        sprintf(buf,"%s/%s",localDir,self->basename);
        grib_recompose_name(h,NULL, buf, local,1);
    }

    sprintf(key,"%s%s",master,local);

    id=grib_itrie_get_id(h->context->concepts_index,key);
    if ((c=h->context->concepts[id])!=NULL) return c;

    if (*local && (full=grib_context_full_defs_path(context,local))!=NULL) {
        c=grib_parse_concept_file(context,full);
        grib_context_log(h->context,GRIB_LOG_DEBUG,
                "Loading concept %s from %s",((grib_action*)self)->name,full);
    }

    full=grib_context_full_defs_path(context,master);

    if(c) {
        grib_concept_value* last=c;
        while (last->next) last=last->next;
        if (full) {
            last->next=grib_parse_concept_file(context,full);
        }
    } else if (full) {
        c=grib_parse_concept_file(context,full);
    } else {
        grib_context_log(context,GRIB_LOG_FATAL,
                "unable to find definition file %s in %s:%s\nDefinition files path=\"%s\"",
                self->basename,master,local,context->grib_definition_files_path);
        return NULL;
    }

    if (full) {
        grib_context_log(h->context,GRIB_LOG_DEBUG,
                "Loading concept %s from %s",((grib_action*)self)->name,full);
    }

    h->context->concepts[id]=c;
    if (c) {
        grib_trie* index=grib_trie_new(context);
        while (c) {
            c->index=index;
            grib_trie_insert_no_replace(index,c->name,c);
            c=c->next;
        }
    }

    return h->context->concepts[id];
}

const char* grib_concept_evaluate(grib_handle* h,grib_action* act)
{
    grib_action_concept* self = (grib_action_concept*) act;
    grib_concept_value*  c = get_concept(h,self);
    int match = 0;
    const char* best = 0;
    const char* prev = 0;

    while(c)
    {
        grib_concept_condition* e = c->conditions;
        int cnt = 0;
        while(e)
        {
            long lval;
            double dval;
            long lres=0;
            double dres=0.0;
            const char *cval;
            char buf[80];
            char tmp[80];
            size_t len = sizeof(buf);
            size_t size=sizeof(tmp);
            int err=0;
            int ok = 0;
            int type       = grib_expression_native_type(h,e->expression);

            switch(type)
            {
            case GRIB_TYPE_LONG:
                grib_expression_evaluate_long(h,e->expression,&lres);
                ok =  (grib_get_long(h,e->name,&lval) == GRIB_SUCCESS) &&
                        (lval == lres);
                break;

            case GRIB_TYPE_DOUBLE:
                grib_expression_evaluate_double(h,e->expression,&dres);
                ok = (grib_get_double(h,e->name,&dval) == GRIB_SUCCESS) &&
                        (dval == dres);
                break;

            case GRIB_TYPE_STRING:
                ok = (grib_get_string(h,e->name,buf,&len) == GRIB_SUCCESS) &&
                ((cval = grib_expression_evaluate_string(h,e->expression,tmp,&size,&err)) != NULL) &&
                (err==0) && (strcmp(buf,cval) == 0);
                break;

            default:
                /* TODO: */
                break;
            }

            if(!ok)
                break;

            e = e->next;
            cnt++;
        }

        if(e == NULL)
        {
            if(cnt >= match) {
                prev  = (cnt > match) ? NULL : best;
                match = cnt;
                best  = c->name;
            }
        }

        c = c->next;
    }

    return best;
}

static int
cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

#define MAX_NUM_CONCEPT_VALUES 40
int grib_concept_apply(grib_handle* h,grib_action* act,const char* name)
{
    long lres=0;
    double dres=0.0;
    int err=0;
    size_t count = 0;
    size_t size;
    grib_concept_condition* e=NULL;
    grib_values values[1024];
    char tmp[80][1024];
    grib_action_concept* self = (grib_action_concept*) act;
    grib_concept_value*  concepts = get_concept(h,self);
    grib_concept_value* c=NULL;

    Assert(concepts!=NULL);

    c=grib_trie_get(concepts->index,name);

    if (!c) c=grib_trie_get(concepts->index,"default");

    if (!c){
        err= self->nofail ? GRIB_SUCCESS : GRIB_CONCEPT_NO_MATCH;
        if (err) {
            size_t i = 0, count = 0;
            char* all_concept_vals[MAX_NUM_CONCEPT_VALUES] = {NULL,}; /* sorted array containing concept values */
            grib_concept_value* pCon = concepts;

            grib_context_log(h->context,GRIB_LOG_ERROR, "concept: no match for %s=%s", act->name,name);

            /* Create a list of all possible values for this concept and sort it */
            while (pCon) {
                if (i >= MAX_NUM_CONCEPT_VALUES)
                    break;
                all_concept_vals[i++] = pCon->name;
                pCon = pCon->next;
            }
            count = i;
            /* Printing out all values for concepts like paramId will be silly! */
            if (count < MAX_NUM_CONCEPT_VALUES) {
                fprintf(stderr, "Here are the possible values for concept %s:\n", act->name);
                qsort(&all_concept_vals, count, sizeof(char*), cmpstringp);
                for(i=0; i<count; ++i) {
                    if (all_concept_vals[i]) {
                        fprintf(stderr, "\t%s\n", all_concept_vals[i]);
                    }
                }
            }
        }
        return err;
    }

    e = c->conditions;
    while(e)
    {
        Assert(count<1024);
        values[count].name = e->name;

        values[count].type = grib_expression_native_type(h,e->expression);
        switch(values[count].type)
        {
        case GRIB_TYPE_LONG:
            grib_expression_evaluate_long(h,e->expression,&lres);
            values[count].long_value = lres;
            break;
        case GRIB_TYPE_DOUBLE:
            grib_expression_evaluate_double(h,e->expression,&dres);
            values[count].double_value = dres;
            break;
        case GRIB_TYPE_STRING:
            size = sizeof(tmp[count]);
            values[count].string_value = grib_expression_evaluate_string(h,e->expression,tmp[count],&size,&err);
            break;
        default:
            return GRIB_NOT_IMPLEMENTED;
            break;
        }

        count++;
        e = e->next;
    }

    return grib_set_values(h,values,count);
}
