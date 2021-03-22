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

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if 0
/* This is a mechanism where we generate C code in grib_templates.h
 * from our GRIB sample files and then include the header so one
 * can instantiate samples without any disk access.
 * This is now superseded by MEMFS
 */
typedef struct grib_templates {
    const char*           name;
    const unsigned char* data;
    size_t               size;
} grib_templates;

#include "grib_templates.h"

#define NUMBER(x) (sizeof(x) / sizeof(x[0]))

grib_handle* grib_internal_sample(grib_context* c,const char* name)
{
    size_t i;
    const size_t num_samples = NUMBER(templates);
    Assert(0);
    for(i = 0; i < num_samples; i++)
        if(strcmp(name,templates[i].name) == 0)
            return grib_handle_new_from_message_copy(c,templates[i].data,templates[i].size);
    return NULL;
}
#endif

/* Windows always has a colon in pathnames e.g. C:\temp\file. It uses semi-colons as delimiter */
#ifdef ECCODES_ON_WINDOWS
#define ECC_PATH_DELIMITER_CHAR ';'
#else
#define ECC_PATH_DELIMITER_CHAR ':'
#endif

static grib_handle* try_template(grib_context* c, const char* dir, const char* name)
{
    char path[1024];
    grib_handle* g = NULL;
    int err        = 0;

    sprintf(path, "%s/%s.tmpl", dir, name);

    if (c->debug) {
        fprintf(stderr, "ECCODES DEBUG try_template path='%s'\n", path);
    }

    if (codes_access(path, F_OK) == 0) {
        FILE* f = codes_fopen(path, "r");
        if (!f) {
            grib_context_log(c, GRIB_LOG_PERROR, "cannot open %s", path);
            return NULL;
        }
        g = grib_handle_new_from_file(c, f, &err);
        if (!g) {
            grib_context_log(c, GRIB_LOG_ERROR, "cannot create GRIB handle from %s", path);
        }
        fclose(f);
    }

    return g;
}

static grib_handle* try_bufr_template(grib_context* c, const char* dir, const char* name)
{
    char path[1024];
    grib_handle* g = NULL;
    int err        = 0;

    sprintf(path, "%s/%s.tmpl", dir, name);

    if (c->debug) {
        fprintf(stderr, "ECCODES DEBUG try_template path='%s'\n", path);
    }

    if (codes_access(path, F_OK) == 0) {
        FILE* f = codes_fopen(path, "r");
        if (!f) {
            grib_context_log(c, GRIB_LOG_PERROR, "cannot open %s", path);
            return NULL;
        }
        g = codes_bufr_handle_new_from_file(c, f, &err);
        if (!g) {
            grib_context_log(c, GRIB_LOG_ERROR, "cannot create BUFR handle from %s", path);
        }
        fclose(f);
    }

    return g;
}

static char* try_template_path(grib_context* c, const char* dir, const char* name)
{
    char path[2048];

    sprintf(path, "%s/%s.tmpl", dir, name);

    if (codes_access(path, R_OK) == 0) {
        return grib_context_strdup(c, path);
    }

    return NULL;
}

grib_handle* grib_external_template(grib_context* c, const char* name)
{
    const char* base = c->grib_samples_path;
    char buffer[1024];
    char* p        = buffer;
    grib_handle* g = NULL;

    if (!base)
        return NULL;

    while (*base) {
        if (*base == ECC_PATH_DELIMITER_CHAR) {
            *p = 0;
            g  = try_template(c, buffer, name);
            if (g)
                return g;
            p = buffer;
            base++; /*advance past delimiter*/
        }
        *p++ = *base++;
    }

    *p       = 0;
    return g = try_template(c, buffer, name);
}

grib_handle* bufr_external_template(grib_context* c, const char* name)
{
    const char* base = c->grib_samples_path;
    char buffer[1024];
    char* p        = buffer;
    grib_handle* g = NULL;

    if (!base)
        return NULL;

    while (*base) {
        if (*base == ECC_PATH_DELIMITER_CHAR) {
            *p = 0;
            g  = try_bufr_template(c, buffer, name);
            if (g)
                return g;
            p = buffer;
            base++; /*advance past delimiter*/
        }
        *p++ = *base++;
    }

    *p = 0;
    g  = try_bufr_template(c, buffer, name);
    return g;
}

char* grib_external_template_path(grib_context* c, const char* name)
{
    const char* base = c->grib_samples_path;
    char buffer[1024];
    char* p = buffer;
    char* g = NULL;

    if (!base)
        return NULL;

    while (*base) {
        if (*base == ECC_PATH_DELIMITER_CHAR) {
            *p = 0;
            g  = try_template_path(c, buffer, name);
            if (g)
                return g;
            p = buffer;
            base++;
        }
        *p++ = *base++;
    }

    *p       = 0;
    return g = try_template_path(c, buffer, name);
}
