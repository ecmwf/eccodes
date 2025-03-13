/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = action
   IMPLEMENTS = destroy;execute
   MEMBERS    = char *name
   MEMBERS    = int append
   MEMBERS    = int padtomultiple
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "action.class" and rerun ./make_class.pl

*/

static void init_class      (grib_action_class*);
static void destroy         (grib_context*,grib_action*);
static int execute(grib_action* a,grib_handle* h);


typedef struct grib_action_write {
    grib_action          act;
    /* Members defined in write */
    char *name;
    int append;
    int padtomultiple;
} grib_action_write;


static grib_action_class _grib_action_class_write = {
    0,                              /* super                     */
    "action_class_write",                              /* name                      */
    sizeof(grib_action_write),            /* size                      */
    0,                                   /* inited */
    &init_class,                         /* init_class */
    0,                               /* init                      */
    &destroy,                            /* destroy */

    0,                               /* dump                      */
    0,                               /* xref                      */

    0,             /* create_accessor*/

    0,                            /* notify_change */
    0,                            /* reparse */
    &execute,                            /* execute */
};

grib_action_class* grib_action_class_write = &_grib_action_class_write;

static void init_class(grib_action_class* c)
{
}
/* END_CLASS_IMP */

/*extern int errno;*/

grib_action* grib_action_create_write(grib_context* context, const char* name, int append, int padtomultiple)
{
    char buf[1024];
    grib_action_write* a = NULL;
    grib_action_class* c = grib_action_class_write;
    grib_action* act     = (grib_action*)grib_context_malloc_clear_persistent(context, c->size);
    act->op              = grib_context_strdup_persistent(context, "section");

    act->cclass  = c;
    a            = (grib_action_write*)act;
    act->context = context;

    a->name = grib_context_strdup_persistent(context, name);

    snprintf(buf, 1024, "write%p", (void*)a->name);

    act->name        = grib_context_strdup_persistent(context, buf);
    a->append        = append;
    a->padtomultiple = padtomultiple;

    return act;
}

static int execute(grib_action* act, grib_handle* h)
{
    grib_action_write* a = (grib_action_write*)act;
    int err              = GRIB_SUCCESS;
    size_t size          = 0;
    const void* buffer   = NULL;
    const char* filename = NULL;
    char string[1024]    = {0,};

    grib_file* of = NULL;

    if ((err = grib_get_message(h, &buffer, &size)) != GRIB_SUCCESS) {
        grib_context_log(act->context, GRIB_LOG_ERROR, "unable to get message");
        return err;
    }

    if (strlen(a->name) != 0) {
        err = grib_recompose_name(h, NULL, a->name, string, 0);
        filename = string;
    }
    else {
        if (act->context->outfilename) {
            filename = act->context->outfilename;
            err = grib_recompose_name(h, NULL, act->context->outfilename, string, 0);
            if (!err)
                filename = string;
        }
        else {
            filename = "filter.out";
        }
    }

    Assert(filename);
    if (a->append)
        of = grib_file_open(filename, "a", &err);
    else
        of = grib_file_open(filename, "w", &err);

    if (!of || !of->handle) {
        grib_context_log(act->context, GRIB_LOG_ERROR, "Unable to open file '%s' for %s",
                        filename, (a->append ? "appending":"writing"));
        return GRIB_IO_PROBLEM;
    }

    if (h->gts_header) {
        if (fwrite(h->gts_header, 1, h->gts_header_len, of->handle) != h->gts_header_len) {
            grib_context_log(act->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                             "Error writing GTS header to '%s'", filename);
            return GRIB_IO_PROBLEM;
        }
    }

    if (fwrite(buffer, 1, size, of->handle) != size) {
        grib_context_log(act->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                         "Error writing to '%s'", filename);
        return GRIB_IO_PROBLEM;
    }

    if (a->padtomultiple) {
        char* zeros = NULL;
        if (a->padtomultiple < 0)
            return GRIB_INVALID_ARGUMENT;
        size_t padding = a->padtomultiple - size % a->padtomultiple;
        /* printf("XXX padding=%zu size=%zu padtomultiple=%d\n", padding, size,a->padtomultiple); */
        zeros = (char*)calloc(padding, 1);
        if (!zeros)
            return GRIB_OUT_OF_MEMORY;
        if (fwrite(zeros, 1, padding, of->handle) != padding) {
            grib_context_log(act->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                             "Error writing to '%s'", filename);
            free(zeros);
            return GRIB_IO_PROBLEM;
        }
        free(zeros);
    }

    if (h->gts_header) {
        const char gts_trailer[4] = { '\x0D', '\x0D', '\x0A', '\x03' };
        if (fwrite(gts_trailer, 1, 4, of->handle) != 4) {
            grib_context_log(act->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                             "Error writing GTS trailer to '%s'", filename);
            return GRIB_IO_PROBLEM;
        }
    }

    grib_file_close(filename, 0, &err);
    if (err != GRIB_SUCCESS) {
        grib_context_log(act->context, GRIB_LOG_ERROR, "Unable to write message");
        return err;
    }

    return err;
}

static void destroy(grib_context* context, grib_action* act)
{
    grib_action_write* a = (grib_action_write*)act;

    grib_context_free_persistent(context, a->name);
    grib_context_free_persistent(context, act->name);
    grib_context_free_persistent(context, act->op);
}
