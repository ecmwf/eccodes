/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_tools.h"
#include <stdlib.h>

#if HAVE_LIBJASPER
/* Remove compiler warnings re macros being redefined */
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#endif

#ifdef ENABLE_FLOATING_POINT_EXCEPTIONS
#define _GNU_SOURCE
#include <fenv.h>
int feenableexcept(int excepts);
#endif

GRIB_INLINE static int grib_inline_strcmp(const char* a, const char* b)
{
    if (*a != *b)
        return 1;
    while ((*a != 0 && *b != 0) && *(a) == *(b)) {
        a++;
        b++;
    }
    return (*a == 0 && *b == 0) ? 0 : 1;
}

static void grib_print_header(grib_runtime_options* options, grib_handle* h);
static void grib_tools_set_print_keys(grib_runtime_options* options, grib_handle* h, const char* ns);
static int grib_tool_with_orderby(grib_runtime_options* options);
static int grib_tool_without_orderby(grib_runtime_options* options);
static int grib_tool_onlyfiles(grib_runtime_options* options);
static int grib_tool_index(grib_runtime_options* options);
static int process(grib_context* c, grib_runtime_options* options, const char* path);
static int scan(grib_context* c, grib_runtime_options* options, const char* dir);

FILE* dump_file;

static grib_runtime_options global_options = {
    0,  /* verbose       */
    0,  /* fail          */
    0,  /* skip          */
    12, /* default_print_width */
    0,  /* print_header */
    0,  /* name_space */
    0,  /* print_number */
    1,  /* print_statistics */
    {{0,},}, /* grib_values requested_print_keys[MAX_KEYS] */
    0, /* requested_print_keys_count */
    {{0,},}, /* grib_values print_keys[MAX_KEYS] */
    0, /* print_keys_count  */
    0, /* strict            */
    0, /* multi_support     */
    0, /* set_values_count  */
    {{0,},}, /* grib_values set_values[MAX_KEYS] */
    {{0,},}, /* grib_values constraints[MAX_KEYS] */
    0, /* constraints_count */
    {{0,},}, /* grib_values compare[MAX_KEYS] */
    0, /* compare_count */
    0, /* handle_count      */
    0, /* filter_handle_count */
    0, /* file_count     */
    0, /* grib_tools_file* infile_extra */
    0, /* grib_tools_file* current_infile */
    0, /* grib_tools_file* infile */
    0, /* grib_tools_file* outfile */
    0, /* grib_action* action */
    0, /* grib_rule* rules */
    0, /* int dump_flags; */
    0, /* char* dump_mode; */
    0, /* repack    */
    0, /* error    */
    0, /* gts    */
    0, /* orderby    */
    0, /* latlon    */
    {0,}, /* double lats[4] */
    {0,}, /* double lons[4] */
    {0,}, /* double values[4] */
    {0,}, /* double distances[4] */
    {0,},  /* int indexes[4] */
    4,  /* int latlon_mode */
    0,  /* char* latlon_mask */
    -1, /* int latlon_idx */
    {0,}, /* double mask_values[4] */
    0, /* index */
    0, /* index_on */
    0, /* constant */
    0, /* dump_filename*/
    0, /* grib_fieldset* idx */
    0, /* random */
    0, /* format */
    0, /* onlyfiles */
    0, /* tolerance_count  */
    0, /* through_index */
    0, /* index1  */
    0, /* index2  */
    0, /* context  */
    0, /* stop  */
    0, /* mode  */
    0, /* headers_only  */
    0, /* skip_all  */
    {{0,},}, /* grib_values tolerance[MAX_KEYS] */
    0, /* infile_offset */
    0  /* JSON output */
};

static grib_handle* grib_handle_new_from_file_x(grib_context* c, FILE* f, int mode, int headers_only, int* err)
{
    if (mode == MODE_GRIB)
        return grib_new_from_file(c, f, headers_only, err);

    if (mode == MODE_BUFR)
        return bufr_new_from_file(c, f, err);

    if (mode == MODE_ANY)
        return any_new_from_file(c, f, err);

    if (mode == MODE_GTS)
        return gts_new_from_file(c, f, err);

    if (mode == MODE_METAR)
        return metar_new_from_file(c, f, err);

    if (mode == MODE_TAF)
        return taf_new_from_file(c, f, err);

    Assert(!"grib_handle_new_from_file_x: unknown mode");
    return NULL;
}

int grib_tool(int argc, char** argv)
{
    int ret                = 0;
    int i = 0;
    grib_context* c        = grib_context_get_default();
    global_options.context = c;

    /* This is a consequence of ECC-440.
     * We want to keep the output file(s) opened as various
     * messages are appended to them. Otherwise they will be opened/closed
     * multiple times.
     */
    if (c->file_pool_max_opened_files == 0)
        c->file_pool_max_opened_files = 200;

#ifdef ENABLE_FLOATING_POINT_EXCEPTIONS
    feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);
#endif

    if (getenv("DOXYGEN_USAGE") && argc == 1)
        usage_doxygen();

    grib_get_runtime_options(argc, argv, &global_options);

    grib_tool_before_getopt(&global_options);

    grib_process_runtime_options(c, argc, argv, &global_options);

    grib_tool_init(&global_options);
    if (global_options.dump_filename) {
        dump_file = fopen(global_options.dump_filename, "w");
        if (!dump_file) {
            perror(global_options.dump_filename);
            exit(1);
        }
    }
    else {
        dump_file = stdout;
    }

    /* ECC-926: Currently only GRIB indexing works. Disable the through_index if BUFR, GTS etc */
    if (global_options.mode == MODE_GRIB &&
        is_index_file(global_options.infile->name) &&
        (global_options.infile_extra && is_index_file(global_options.infile_extra->name))) {
        global_options.through_index = 1;
        return grib_tool_index(&global_options);
    }

    if (global_options.onlyfiles)
        ret = grib_tool_onlyfiles(&global_options);
    else {
        if (global_options.orderby)
            ret = grib_tool_with_orderby(&global_options);
        else
            ret = grib_tool_without_orderby(&global_options);
    }

    if (global_options.dump_filename)
        fclose(dump_file);

    /* Free memory */
    for (i = 0; i < global_options.print_keys_count; i++) {
        if (global_options.print_keys[i].name) {
            free((char*)global_options.print_keys[i].name);
        }
    }
    return ret;
}

static int grib_tool_with_orderby(grib_runtime_options* options)
{
    int err             = 0;
    grib_failed *failed = NULL, *p = NULL;
    grib_handle* h          = NULL;
    grib_tools_file* infile = options->infile;
    char** filenames;
    int files_count    = 0;
    grib_fieldset* set = NULL;
    int i              = 0;
    grib_context* c    = grib_context_get_default();

    if (infile)
        infile->failed = NULL;

    files_count = 0;
    while (infile) {
        files_count++;
        infile = infile->next;
    }

    filenames = (char**)grib_context_malloc_clear(c, files_count * sizeof(char*));

    infile = options->infile;
    for (i = 0; i < files_count; i++) {
        filenames[i] = infile->name;
        infile       = infile->next;
    }

    if (grib_options_on("7"))
        c->no_fail_on_wrong_length = 1;

    set = grib_fieldset_new_from_files(0, filenames, files_count, 0, 0, 0, options->orderby, &err);
    if (err) {
        grib_context_log(c, GRIB_LOG_ERROR, "unable to create index for input file %s (%s)",
                         filenames[0], grib_get_error_message(err));
        exit(err);
    }

    options->handle_count = 0;
    grib_context_set_handle_file_count(c, 0);  /* ECC-873 */
    grib_context_set_handle_total_count(c, 0); /* ECC-873 */
    while (!options->skip_all && ((h = grib_fieldset_next_handle(set, &err)) != NULL || err != GRIB_SUCCESS)) {
        options->handle_count++;
        grib_context_set_handle_file_count(c, options->handle_count);  /* ECC-873 */
        grib_context_set_handle_total_count(c, options->handle_count); /* ECC-873 */
        options->error = err;

        if (!h) {
            grib_no_handle_action(options, err);

            failed        = (grib_failed*)grib_context_malloc_clear(c, sizeof(grib_failed));
            failed->count = infile->handle_count;
            failed->error = err;
            failed->next  = NULL;

            if (!infile->failed) {
                infile->failed = failed;
            }
            else {
                p = infile->failed;
                while (p->next)
                    p = p->next;
                p->next = failed;
            }

            continue;
        }

        if (options->json_output == 0 || options->latlon)
            grib_print_header(options, h);
        else
            grib_tools_set_print_keys(options, h, options->name_space);

        grib_skip_check(options, h);

        if (options->skip && options->strict) {
            grib_tool_skip_handle(options, h);
            continue;
        }

        grib_tool_new_handle_action(options, h);

        grib_tool_print_key_values(options, h);

        grib_handle_delete(h);
    }

    if (set->size==0) fprintf(stderr, "no messages found in fieldset\n");
    grib_tool_finalise_action(options);

    grib_fieldset_delete(set);
    free(filenames);

    return 0;
}

static char iobuf[1024 * 1024];

static int grib_tool_without_orderby(grib_runtime_options* options)
{
    int err = 0;
    /*int nofail=0;*/
    grib_failed *failed = NULL, *p = NULL;
    grib_handle* h          = NULL;
    grib_tools_file* infile = options->infile;

    grib_context* c              = grib_context_get_default();
    options->file_count          = 0;
    options->handle_count        = 0;
    options->filter_handle_count = 0;
    options->current_infile      = options->infile;
    infile->failed               = NULL;

    if (grib_options_on("7"))
        c->no_fail_on_wrong_length = 1;

    while (infile != NULL && infile->name != NULL) {
        if (options->print_statistics && options->verbose && !options->json_output)
            fprintf(dump_file, "%s\n", infile->name);
        if (strcmp(infile->name, "-") == 0)
            infile->file = stdin;
        else
            infile->file = fopen(infile->name, "rb");
        if (!infile->file) {
            perror(infile->name);
            exit(1);
        }
        if (options->infile_offset) {
#ifndef ECCODES_ON_WINDOWS
            /* Check at compile time to ensure our file offset is at least 64 bits */
            COMPILE_TIME_ASSERT(sizeof(options->infile_offset) >= 8);
#endif
            err = fseeko(infile->file, options->infile_offset, SEEK_SET);
            if (err) {
                /*fprintf(stderr, "Invalid file offset: %ld\n", options->infile_offset);*/
                perror("Invalid file offset");
                exit(1);
            }
        }

        setvbuf(infile->file, iobuf, _IOFBF, sizeof(iobuf));

        options->file_count++;
        infile->handle_count        = 0;
        infile->filter_handle_count = 0;

        grib_tool_new_file_action(options, infile);
        /*nofail=grib_options_on("f");*/

        while (!options->skip_all && ((h = grib_handle_new_from_file_x(c, infile->file, options->mode,
                                                                       options->headers_only, &err)) != NULL ||
                                      err != GRIB_SUCCESS)) {
            infile->handle_count++;
            options->handle_count++;

            if (c->no_fail_on_wrong_length && (err == GRIB_PREMATURE_END_OF_FILE || err == GRIB_WRONG_LENGTH))
                err = 0;
            if (!options->error) {
                /* ECC-1086: Do not clear a previous error */
                options->error = err;
            }

            if (!h) {
                /* fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n"); */
                grib_no_handle_action(options, err);

                failed        = (grib_failed*)grib_context_malloc_clear(c, sizeof(grib_failed));
                failed->count = infile->handle_count;
                failed->error = err;
                failed->next  = NULL;

                if (!infile->failed) {
                    infile->failed = failed;
                }
                else {
                    p = infile->failed;
                    while (p->next)
                        p = p->next;
                    p->next = failed;
                }

                continue;
            }

            if (options->json_output == 0 || options->latlon)
                grib_print_header(options, h);
            else
                grib_tools_set_print_keys(options, h, options->name_space);

            grib_skip_check(options, h);

            if (options->skip && options->strict) {
                grib_tool_skip_handle(options, h);
                continue;
            }

            grib_tool_new_handle_action(options, h);

            grib_print_key_values(options, h);

            grib_handle_delete(h);
        }

        grib_print_file_statistics(options, infile);

        if (infile->file)
            fclose(infile->file);

        if (infile->handle_count == 0) {
            fprintf(stderr, "no messages found in %s\n", infile->name);
            if (options->fail)
                exit(1);
        }

        infile                  = infile->next;
        options->current_infile = infile;
    }

    grib_print_full_statistics(options);

    grib_tool_finalise_action(options);

    return options->error;
}

static int navigate(grib_field_tree* fields, grib_runtime_options* options)
{
    int err          = 0;
    int message_type = 0;

    if (!fields || options->stop)
        return 0;

    switch (options->mode) {
        case MODE_GRIB:
            message_type = CODES_GRIB;
            break;
        case MODE_BUFR:
            message_type = CODES_BUFR;
            break;
        default:
            Assert(0);
    }

    if (fields->field) {
        grib_handle* h = codes_index_get_handle(fields->field, message_type, &err);
        if (!options->index2->current)
            options->index2->current = (grib_field_list*)grib_context_malloc_clear(options->context, sizeof(grib_field_list));
        options->index2->current->field = fields->field;
        if (!h)
            return err;
        grib_skip_check(options, h);
        if (options->skip && options->strict) {
            grib_tool_skip_handle(options, h);
        }
        else {
            grib_tool_new_handle_action(options, h);
            grib_handle_delete(h);
        }
    }

    err = navigate(fields->next_level, options);
    if (err)
        return err;

    err = navigate(fields->next, options);

    return err;
}

static int grib_tool_index(grib_runtime_options* options)
{
    int err  = 0;
    char* f1 = options->infile->name;
    char* f2 = options->infile_extra->name;
    grib_index_key *k1, *k2;
    int found = 0;

    grib_context* c = grib_context_get_default();

    options->index1 = grib_index_read(c, f1, &err);
    if (err)
        grib_context_log(c, (GRIB_LOG_FATAL) | (GRIB_LOG_PERROR),
                         "unable to read index from %s", f1);

    options->index2 = grib_index_read(c, f2, &err);
    if (err)
        grib_context_log(c, (GRIB_LOG_FATAL) | (GRIB_LOG_PERROR),
                         "unable to read index from %s", f2);

    k1 = options->index1->keys;
    while (k1) {
        k2    = options->index2->keys;
        found = 0;
        while (k2) {
            if (!strcmp(k1->name, k2->name)) {
                found = 1;
                break;
            }
            k2 = k2->next;
        }
        if (!found) {
            printf("Indexes contained in the input files have different keys\n");
            printf("keys in file %s:\n", f1);
            k1 = options->index1->keys;
            while (k1) {
                printf("\t%s\n", k1->name);
                k1 = k1->next;
            }
            printf("keys in file %s:\n", f2);
            k2 = options->index2->keys;
            while (k2) {
                printf("\t%s\n", k2->name);
                k2 = k2->next;
            }
            exit(1);
        }

        k1->value[0] = 0;
        k1           = k1->next;
    }

    k2 = options->index2->keys;
    while (k2) {
        k1    = options->index1->keys;
        found = 0;
        while (k1) {
            if (!strcmp(k1->name, k2->name)) {
                found = 1;
                break;
            }
            k1 = k1->next;
        }
        if (!found) {
            printf("Indexes contained in the input files have different keys\n");
            printf("keys in file %s:\n", f2);
            k2 = options->index2->keys;
            while (k2) {
                printf("\t%s\n", k2->name);
                k2 = k2->next;
            }
            printf("keys in file %s:\n", f1);
            k1 = options->index1->keys;
            while (k1) {
                printf("\t%s\n", k1->name);
                k1 = k1->next;
            }

            exit(1);
        }

        k2 = k2->next;
    }

    navigate(options->index2->fields, options);

    if (options->index2)
        grib_context_free(c, options->index2->current);

    grib_tool_finalise_action(options);

    return 0;
}

#ifndef ECCODES_ON_WINDOWS
static int scan(grib_context* c, grib_runtime_options* options, const char* dir)
{
    struct dirent* s;
    DIR* d;
    int err = 0;

    d = opendir(dir);
    if (!d) {
        grib_context_log(c, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR), "opendir %s", dir);
        return GRIB_IO_PROBLEM;
    }

    while ((s = readdir(d)) && (err == 0)) {
        if (strcmp(s->d_name, ".") != 0 && strcmp(s->d_name, "..") != 0) {
            char buf[1024];
            sprintf(buf, "%s/%s", dir, s->d_name);
            process(c, options, buf);
        }
    }
    closedir(d);
    return 0;
}
#else
static int scan(grib_context* c, grib_runtime_options* options, const char* dir)
{
    struct _finddata_t fileinfo;
    intptr_t handle;
    char buffer[1024];
    sprintf(buffer, "%s/*", dir);
    if ((handle = _findfirst(buffer, &fileinfo)) != -1) {
        do {
            if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                char buf[1024];
                sprintf(buf, "%s/%s", dir, fileinfo.name);
                process(c, options, buf);
            }
        } while (!_findnext(handle, &fileinfo));

        _findclose(handle);
    }
    else {
        grib_context_log(c, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR), "opendir %s", dir);
        return GRIB_IO_PROBLEM;
    }
    return 0;
}
#endif


static int process(grib_context* c, grib_runtime_options* options, const char* path)
{
    struct stat s;
    int stat_val = 0;

#ifndef ECCODES_ON_WINDOWS
    stat_val = lstat(path, &s);
#else
    stat_val = stat(path, &s);
#endif

    if (stat_val != 0) {
        grib_context_log(c, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR), "Cannot stat %s", path);
        return GRIB_IO_PROBLEM;
    }

    if (S_ISDIR(s.st_mode) && !S_ISLNK(s.st_mode)) {
        scan(c, options, path);
    }
    else {
        grib_tool_new_filename_action(options, path);
    }
    return 0;
}

static int grib_tool_onlyfiles(grib_runtime_options* options)
{
    grib_context* c         = grib_context_get_default();
    grib_tools_file* infile = options->infile;

    while (infile != NULL && infile->name != NULL) {
        process(c, options, infile->name);

        infile = infile->next;
    }

    grib_tool_finalise_action(options);

    return 0;
}

static void grib_print_header(grib_runtime_options* options, grib_handle* h)
{
    size_t strlenkey = 0;
    int width;
    int written_to_dump = 0; /* boolean */
    if (options->json_output && !options->latlon)
        return; /* For JSON output we do not print a single header for all msgs */
    if (options->handle_count != 1)
        return;

    grib_tools_set_print_keys(options, h, options->name_space);

    if (options->verbose && options->print_header) {
        int j = 0;
        for (j = 0; j < options->print_keys_count; j++) {
            strlenkey = strlen(options->print_keys[j].name);
            width     = strlenkey < options->default_print_width ? options->default_print_width + 2 : strlenkey + 2;
            if (options->default_print_width < 0)
                width = strlenkey + 1;

            fprintf(dump_file, "%-*s", (int)width, options->print_keys[j].name);
            written_to_dump = 1;
        }
        if (options->latlon) {
            if (options->latlon_mode == 4) {
                fprintf(dump_file, "       value1 ");
                fprintf(dump_file, " value2 ");
                fprintf(dump_file, " value3 ");
                fprintf(dump_file, " value4 ");
            }
            else {
                fprintf(dump_file, " value ");
            }
            written_to_dump = 1;
        }
        if (options->index_on) {
            fprintf(dump_file, "        value(%d) ", (int)options->index);
            written_to_dump = 1;
        }
        if (written_to_dump) {
            fprintf(dump_file, "\n");
        }
    }
}

static int cmpstringp(const void* p1, const void* p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */
    return strcmp(*(char* const*)p1, *(char* const*)p2);
}

static void grib_tools_set_print_keys(grib_runtime_options* options, grib_handle* h, const char* ns)
{
    int i                     = 0;
    grib_keys_iterator* kiter = NULL;

    options->print_keys_count = 0;

    for (i = 0; i < options->requested_print_keys_count; i++) {
        options->print_keys[options->print_keys_count].name = options->requested_print_keys[i].name;
        if (strlen(options->requested_print_keys[i].name) > options->default_print_width)
            options->default_print_width = (int)strlen(options->requested_print_keys[i].name);
        options->print_keys[options->print_keys_count].type = options->requested_print_keys[i].type;
        options->print_keys_count++;
    }

    if (ns) {
        kiter = grib_keys_iterator_new(h, 0, ns);
        if (!kiter) {
            fprintf(stderr, "ERROR: Unable to create keys iterator\n");
            exit(1);
        }

        while (grib_keys_iterator_next(kiter)) {
            const char* name = grib_keys_iterator_get_name(kiter);

            if (options->print_keys_count >= MAX_KEYS) {
                fprintf(stderr, "ERROR: keys list too long (more than %d keys)\n",
                        options->print_keys_count);
                exit(1);
            }
            if (options->print_keys[options->print_keys_count].name) {
                free((char*)options->print_keys[options->print_keys_count].name);
            }
            options->print_keys[options->print_keys_count].name = strdup(name);
            if (strlen(name) > options->default_print_width)
                options->default_print_width = (int)strlen(name);
            options->print_keys[options->print_keys_count].type = GRIB_TYPE_STRING;
            options->print_keys_count++;
        }

        grib_keys_iterator_delete(kiter);
        if (options->print_keys_count == 0 && options->latlon == 0) {
            int j = 0, k = 0, ns_count = 0;
            const char* all_namespace_vals[1024] = {
                NULL,
            }; /* sorted array containing all namespaces */
            printf("ERROR: namespace \"%s\" does not contain any key.\n", ns);
            printf("Here are the available namespaces in this message:\n");
            for (i = 0; i < ACCESSORS_ARRAY_SIZE; i++) {
                grib_accessor* anAccessor = h->accessors[i];
                if (anAccessor) {
                    for (j = 0; j < MAX_ACCESSOR_NAMES; j++) {
                        const char* a_namespace = anAccessor->all_name_spaces[j];
                        if (a_namespace) {
                            all_namespace_vals[k++] = a_namespace;
                            ns_count++;
                        }
                    }
                }
            }
            qsort(&all_namespace_vals, ns_count, sizeof(char*), cmpstringp);
            for (i = 0; i < ns_count; ++i) {
                if (all_namespace_vals[i]) {
                    int print_it = 1;
                    if (i > 0 && strcmp(all_namespace_vals[i], all_namespace_vals[i - 1]) == 0) {
                        print_it = 0; /* skip duplicate entries */
                    }
                    if (print_it)
                        printf("\t%s\n", all_namespace_vals[i]);
                }
            }
            exit(1);
        }
    }
}

static int to_skip(grib_handle* h, grib_values* v, int* err)
{
    double dvalue              = 0;
    int ret                    = 0;
    long lvalue                = 0;
    char value[MAX_STRING_LEN] = {0,};
    size_t len = MAX_STRING_LEN;
    *err       = 0;

    switch (v->type) {
        case GRIB_TYPE_STRING:
            *err = grib_get_string(h, v->name, value, &len);
            ret  = v->equal ? grib_inline_strcmp(value, v->string_value) : !grib_inline_strcmp(value, v->string_value);
            break;
        case GRIB_TYPE_DOUBLE:
            *err = grib_get_double(h, v->name, &dvalue);
            ret  = v->equal ? (dvalue != v->double_value) : (dvalue == v->double_value);
            break;
        case GRIB_TYPE_LONG:
            *err = grib_get_long(h, v->name, &lvalue);
            ret  = v->equal ? (lvalue != v->long_value) : (lvalue == v->long_value);
            break;
        case GRIB_TYPE_MISSING:
            lvalue = grib_is_missing(h, v->name, err);
            ret    = (lvalue == v->equal) ? 0 : 1;
            break;
        default:
            fprintf(dump_file, "invalid type for %s\n", v->name);
            exit(1);
    }

    return ret;
}

void grib_skip_check(grib_runtime_options* options, grib_handle* h)
{
    int i, ret = 0;
    grib_values* v = NULL;

    /* ECC-1179: bufr_copy/bufr_ls: Allow 'where' clause with Data Section keys */
    if (options->constraints_count > 0 && h->product_kind == PRODUCT_BUFR) {
        for (i = 0; i < options->set_values_count; i++) {
            if (strcmp(options->set_values[i].name, "unpack")==0) {
                grib_set_long(h, "unpack", 1);
                break;
            }
        }
    }

    for (i = 0; i < options->constraints_count; i++) {
        v = &(options->constraints[i]);
        if (v->equal) {
            options->skip = 1;
            while (v) {
                if (!to_skip(h, v, &ret)) {
                    if (!strcmp(v->name, "count") && !v->next)
                        options->skip_all = 1;
                    options->skip = 0;
                    break;
                }
                if (ret != GRIB_SUCCESS && options->fail) {
                    grib_context_log(h->context, GRIB_LOG_ERROR, "unable to get \"%s\" (%s)",
                                     v->name, grib_get_error_message(ret));
                    exit(ret);
                }
                v = v->next;
            }
        }
        else {
            options->skip = 0;
            while (v) {
                if (to_skip(h, v, &ret)) {
                    options->skip = 1;
                    break;
                }
                if (ret != GRIB_SUCCESS && options->fail) {
                    grib_context_log(h->context, GRIB_LOG_ERROR, "unable to get \"%s\" (%s)",
                                     v->name, grib_get_error_message(ret));
                    exit(ret);
                }
                v = v->next;
            }
        }
        if (options->skip == 1)
            break;
    }

    if (!options->skip) {
        options->filter_handle_count++;
        if (options->current_infile)
            options->current_infile->filter_handle_count++;
    }
}

/* TODO: Does not work for 2.7e+01 */
static int is_valid_JSON_number(const char* input)
{
    const char* p = input;
    size_t len    = 0;
    int is_float  = 0;
    if (p == 0 || *p == '\0')
        return 0;
    if (*p == '-')
        p++;

    while (*p) {
        if (*p == '.')
            is_float = 1;
        if (*p != '.' && !isdigit(*p))
            return 0;
        p++;
        len++;
    }
    /*
     * Note: BUFR keys like typicalTime/rdbtimetime can have values
     * like 000000 or 013329 which are invalid JSON numbers.
     * In JSON a leading zero must not be followed by another digit
     */
    if (!is_float && len > 2 && input[0] == '0' && isdigit(input[1]))
        return 0; /* Not a valid JSON number */
    return 1;
}

static void get_value_for_key(grib_handle* h, const char* key_name, int key_type, char* value_str, const char* format)
{
    int ret = 0, type = key_type;
    double dvalue = 0;
    long lvalue   = 0;
    size_t len    = MAX_STRING_LEN;

    if (grib_is_missing(h, key_name, &ret) && ret == GRIB_SUCCESS) {
        sprintf(value_str, "MISSING");
        return;
    }
    if (ret == GRIB_NOT_FOUND) {
        sprintf(value_str, "not_found");
        return;
    }

    if (type == GRIB_TYPE_UNDEFINED) {
        ret = grib_get_native_type(h, key_name, &type);
        if (ret != GRIB_SUCCESS) {
            fprintf(dump_file, "Could not determine type for %s\n", key_name);
            exit(1);
        }
    }

    if (type == GRIB_TYPE_STRING) {
        ret = grib_get_string(h, key_name, value_str, &len);
    }
    else if (type == GRIB_TYPE_DOUBLE) {
        ret = grib_get_double(h, key_name, &dvalue);
        sprintf(value_str, format, dvalue);
    }
    else if (type == GRIB_TYPE_LONG) {
        ret = grib_get_long(h, key_name, &lvalue);
        sprintf(value_str, "%ld", lvalue);
    }
    else if (type == GRIB_TYPE_BYTES) {
        ret = grib_get_string(h, key_name, value_str, &len);
    }
    else {
        fprintf(dump_file, "invalid format option for %s\n", key_name);
        exit(1);
    }

    if (ret != GRIB_SUCCESS) {
        if (ret == GRIB_NOT_FOUND) {
            sprintf(value_str, "not_found");
        } else {
            fprintf(dump_file, "Failed to get value for key %s\n", key_name);
            exit(1);
        }
    }
}

/* See ECC-707 */
static int fix_for_lsdate_needed(grib_handle* h)
{
    long lsdate_bug = 0;
    int err         = grib_get_long(h, "lsdate_bug", &lsdate_bug);
    if (!err && lsdate_bug == 1) {
        return 1;
    }
    return 0;
}

static int get_initial_element_of_array(grib_handle* h, const char* keyName, size_t num_vals, char* value)
{
    int err = 0, type = 0;
    size_t len          = num_vals;
    char* sval          = NULL;
    unsigned char* uval = NULL;
    long* lval          = NULL;
    double* dval        = NULL;
    grib_context* c     = h->context;

    Assert(num_vals > 1); /* This is for array keys */
    if ((err = grib_get_native_type(h, keyName, &type)) != GRIB_SUCCESS)
        return err;
    switch (type) {
        case GRIB_TYPE_STRING:
            grib_get_string_length(h, keyName, &len);
            sval = (char*)grib_context_malloc(c, len * sizeof(char));
            if (!sval)
                return GRIB_OUT_OF_MEMORY;
            if ((err = grib_get_string(h, keyName, sval, &len)) != GRIB_SUCCESS) {
                free(sval);
                return err;
            }
            sprintf(value, "%s", sval);
            free(sval);
            break;
        case GRIB_TYPE_LONG:
            lval = (long*)grib_context_malloc(c, num_vals * sizeof(long));
            if (!lval)
                return GRIB_OUT_OF_MEMORY;
            if ((err = grib_get_long_array(h, keyName, lval, &len)) != GRIB_SUCCESS)
                return err;
            sprintf(value, "%ld...", lval[0]);
            free(lval);
            break;
        case GRIB_TYPE_DOUBLE:
            dval = (double*)grib_context_malloc(c, num_vals * sizeof(double));
            if (!dval)
                return GRIB_OUT_OF_MEMORY;
            if ((err = grib_get_double_array(h, keyName, dval, &len)) != GRIB_SUCCESS)
                return err;
            sprintf(value, "%g...", dval[0]);
            free(dval);
            break;
        case GRIB_TYPE_BYTES:
            uval = (unsigned char*)grib_context_malloc(c, num_vals * sizeof(unsigned char));
            if (!uval)
                return GRIB_OUT_OF_MEMORY;
            if ((err = grib_get_bytes(h, keyName, uval, &len)) != GRIB_SUCCESS)
                return err;
            sprintf(value, "%d...", (short)uval[0]);
            free(uval);
            break;
        default:
            sprintf(value, "%s...", "");
    }
    return GRIB_SUCCESS;
}

void grib_print_key_values(grib_runtime_options* options, grib_handle* h)
{
    int i   = 0;
    int ret = 0, width = 0;
    int strlenvalue = 0;
    double dvalue   = 0;
    long lvalue     = 0;
    char value[MAX_STRING_LEN];
    const char* notfound = "not_found";
    int written_to_dump  = 0; /* boolean */
    grib_accessor* acc   = NULL;
    size_t num_vals      = 0;
    int fix_lsdate       = 0;

    if (!options->verbose)
        return;

    if (options->json_output && !options->latlon) {
        /* fprintf(dump_file, "\"message %d\" : {\n", options->handle_count); */
        fprintf(dump_file, "  {\n");
        for (i = 0; i < options->print_keys_count; i++) {
            fprintf(dump_file, "    \"%s\": ", options->print_keys[i].name);
            get_value_for_key(h, options->print_keys[i].name, options->print_keys[i].type, value, options->format);
            if (is_valid_JSON_number(value))
                fprintf(dump_file, "%s", value);
            else
                fprintf(dump_file, "\"%s\"", value);
            if (i != options->print_keys_count - 1)
                fprintf(dump_file, ",\n");
            else
                fprintf(dump_file, "\n");
        }
        fprintf(dump_file, "  }");
        return;
    }

    fix_lsdate = (fix_for_lsdate_needed(h) && options->name_space && strcmp(options->name_space, "ls") == 0);

    for (i = 0; i < options->print_keys_count; i++) {
        size_t len = MAX_STRING_LEN;
        ret        = GRIB_SUCCESS;

        if (h->product_kind == PRODUCT_BUFR) {
            /* ECC-236: Do not use grib_is_missing for BUFR */
            if (!grib_is_defined(h, options->print_keys[i].name))
                ret = GRIB_NOT_FOUND;
            if (ret == GRIB_SUCCESS) {
                ret = grib_get_size(h, options->print_keys[i].name, &num_vals);
            }
            if (ret == GRIB_SUCCESS) {
                if (options->print_keys[i].type == GRIB_TYPE_UNDEFINED)
                    grib_get_native_type(h, options->print_keys[i].name, &(options->print_keys[i].type));
                switch (options->print_keys[i].type) {
                    case GRIB_TYPE_STRING:
                        acc = grib_find_accessor(h, options->print_keys[i].name);
                        ret = grib_get_string(h, options->print_keys[i].name, value, &len);
                        if (grib_is_missing_string(acc, (unsigned char*)value, len))
                            sprintf(value, "MISSING");
                        break;
                    case GRIB_TYPE_DOUBLE:
                        if (num_vals > 1) {
                            ret = GRIB_ARRAY_TOO_SMALL;
                        } else {
                            ret = grib_get_double(h, options->print_keys[i].name, &dvalue);
                            if (dvalue == GRIB_MISSING_DOUBLE) sprintf(value, "MISSING");
                            else                               sprintf(value, options->format, dvalue);
                        }
                        break;
                    case GRIB_TYPE_LONG:
                        if (num_vals > 1) {
                            ret = GRIB_ARRAY_TOO_SMALL;
                        } else {
                            ret = grib_get_long(h, options->print_keys[i].name, &lvalue);
                            if (lvalue == GRIB_MISSING_LONG) sprintf(value, "MISSING");
                            else                             sprintf(value, "%ld", lvalue);
                        }
                        break;
                    case GRIB_TYPE_BYTES:
                        ret = grib_get_string(h, options->print_keys[i].name, value, &len);
                        break;
                    default:
                        fprintf(dump_file, "Could not determine type for %s\n", options->print_keys[i].name);
                        exit(1);
                }
            }
        }
        else {
            /* Other products e.g. GRIB */
            if (grib_is_missing(h, options->print_keys[i].name, &ret) && ret == GRIB_SUCCESS) {
                sprintf(value, "MISSING");
            }
            else if (ret == GRIB_SUCCESS) {
                const char* pName = NULL;
                if (options->print_keys[i].type == GRIB_TYPE_UNDEFINED)
                    grib_get_native_type(h, options->print_keys[i].name, &(options->print_keys[i].type));
                switch (options->print_keys[i].type) {
                    case GRIB_TYPE_STRING:
                        pName = options->print_keys[i].name;
                        if (fix_lsdate && strcmp(pName, "date") == 0) { /* ECC-707 */
                            pName = "ls.date";
                        }
                        ret = grib_get_size(h, pName, &num_vals);
                        if (ret == GRIB_SUCCESS && num_vals > 1) { /* See ECC-278 */
                            ret = get_initial_element_of_array(h, pName, num_vals, value);
                        }
                        else {
                            ret = grib_get_string(h, pName, value, &len);
                        }
                        break;
                    case GRIB_TYPE_DOUBLE:
                        ret = grib_get_double(h, options->print_keys[i].name, &dvalue);
                        sprintf(value, options->format, dvalue);
                        break;
                    case GRIB_TYPE_LONG:
                        ret = grib_get_long(h, options->print_keys[i].name, &lvalue);
                        sprintf(value, "%ld", lvalue);
                        break;
                    case GRIB_TYPE_BYTES:
                        ret = grib_get_string(h, options->print_keys[i].name, value, &len);
                        break;
                    default:
                        fprintf(dump_file, "invalid format option for %s\n", options->print_keys[i].name);
                        exit(1);
                }
            }
        }

        if (ret != GRIB_SUCCESS) {
            if (options->fail)
                GRIB_CHECK_NOLINE(ret, options->print_keys[i].name);
            if (ret == GRIB_NOT_FOUND)
                strcpy(value, notfound);
            else {
                fprintf(dump_file, "%s %s\n", grib_get_error_message(ret), options->print_keys[i].name);
                exit(ret);
            }
        }

        strlenvalue = (int)strlen(value);

        width = strlenvalue < options->default_print_width ? options->default_print_width + 2 : strlenvalue + 2;

        if (options->default_print_width < 0)
            width = strlenvalue + 1;

        if (options->print_keys_count == i + 1 && options->latlon == 0)
            width--;

        fprintf(dump_file, "%-*s", (int)width, value);
        written_to_dump = 1;
    }

    if (options->latlon) {
        if (options->latlon_mode == 4) {
            int ii = 0;
            for (ii = 0; ii < LATLON_SIZE; ii++) {
                fprintf(dump_file, options->format, options->values[ii]);
                fprintf(dump_file, " ");
            }
            written_to_dump = 1;
        }
        else if (options->latlon_mode == 1) {
            sprintf(value, options->format, options->values[options->latlon_idx]);
            strlenvalue = (int)strlen(value);
            width       = strlenvalue < options->default_print_width ? options->default_print_width + 2 : strlenvalue + 2;
            fprintf(dump_file, "%-*s", (int)width, value);
            written_to_dump = 1;
        }
    }
    if (options->index_on) {
        double v = 0;
        /*if (grib_get_double_element(h,"values",options->index,&v) != GRIB_SUCCESS) {*/
        if (1) {
            size_t size, the_index = 0;
            double* values;
            int err = 0;

            err = grib_get_size(h, "values", &size);
            if (err) {
                sprintf(value, "unknown");
                if (!options->fail)
                    exit(err);
                return;
            }
            values = (double*)grib_context_malloc_clear(h->context, size * sizeof(double));
            grib_get_double_array(h, "values", values, &size);
            the_index = options->index;
            if (the_index >= size) {
                fprintf(dump_file, "\n");
                fprintf(stderr, "invalid index value %d (should be between 0 and %d)\n",
                        options->index, (int)(size - 1));
                exit(1);
            }
            v = values[options->index];
            grib_context_free(h->context, values);
        }

        sprintf(value, options->format, v);
        strlenvalue = (int)strlen(value);
        width       = strlenvalue < options->default_print_width ? options->default_print_width + 2 : strlenvalue + 2;
        fprintf(dump_file, "%-*s", (int)width, value);
        written_to_dump = 1;
    }
    if (written_to_dump) {
        fprintf(dump_file, "\n");
    }
}

void grib_print_file_statistics(grib_runtime_options* options, grib_tools_file* file)
{
    grib_failed* failed = NULL;
    Assert(file);
    if (options->json_output && !options->latlon)
        return;

    failed = file->failed;

    if (!options->print_statistics || !options->verbose)
        return;

    fprintf(dump_file, "%d of %d messages in %s\n\n",
            file->filter_handle_count,
            file->handle_count,
            file->name);
    if (!failed)
        return;
    /*
	   fprintf(dump_file,"Following bad messages found in %s\n",
	   file->name);
	   fprintf(dump_file,"N      Error\n");
	   while (failed){
	   fprintf(dump_file,"%-*d    %s\n",
	   7,failed->count,
	   grib_get_error_message(failed->error));
	   failed=failed->next;
	   }
	   fprintf(dump_file,"\n");
     */
}

void grib_print_full_statistics(grib_runtime_options* options)
{
    if (options->json_output && !options->latlon)
        return;
    if (options->print_statistics && options->verbose)
        fprintf(dump_file, "%d of %d total messages in %d files\n",
                options->filter_handle_count, options->handle_count, options->file_count);
}

static int filenames_equal(const char* f1, const char* f2)
{
    int eq = 0;
    grib_context* c = grib_context_get_default();
    char* resolved1 = codes_resolve_path(c, f1);
    char* resolved2 = codes_resolve_path(c, f2);
    eq = (strcmp(resolved1, resolved2)==0);
    grib_context_free(c, resolved1);
    grib_context_free(c, resolved2);
    return eq;
}

void grib_tools_write_message(grib_runtime_options* options, grib_handle* h)
{
    const void* buffer;
    size_t size;
    grib_file* of       = NULL;
    int err             = 0;
    char filename[1024] = {0,};
    Assert(options->outfile != NULL && options->outfile->name != NULL);

    /* See ECC-1086
     * if (options->error == GRIB_WRONG_LENGTH)
     *   return;
     */
    if ((err = grib_get_message(h, &buffer, &size)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "unable to get binary message\n");
        exit(err);
    }

    err = grib_recompose_name(h, NULL, options->outfile->name, filename, 0);

    // Check outfile is not same as infile
    if (filenames_equal(options->infile->name, filename)) {
        grib_context_log(h->context, GRIB_LOG_ERROR,
                "output file '%s' is the same as input file. Aborting\n", filename);
        exit(GRIB_IO_PROBLEM);
    }

    of = grib_file_open(filename, "w", &err);

    if (!of || !of->handle) {
        grib_context_log(h->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                         "unable to open file %s\n", filename);
        exit(GRIB_IO_PROBLEM);
    }

    if (options->gts && h->gts_header) {
        if (fwrite(h->gts_header, 1, h->gts_header_len, of->handle) != h->gts_header_len) {
            grib_context_log(h->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                             "Error writing GTS header to %s", filename);
            exit(GRIB_IO_PROBLEM);
        }
    }

    if (fwrite(buffer, 1, size, of->handle) != size) {
        grib_context_log(h->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                         "Error writing to %s", filename);
        exit(GRIB_IO_PROBLEM);
    }

    if (options->gts && h->gts_header) {
        char gts_trailer[4] = { '\x0D', '\x0D', '\x0A', '\x03' };
        if (fwrite(gts_trailer, 1, 4, of->handle) != 4) {
            grib_context_log(h->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                             "Error writing GTS trailer to %s", filename);
            exit(GRIB_IO_PROBLEM);
        }
    }

    grib_file_close(filename, 0, &err);

    if (err != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "unable to write message\n");
        exit(err);
    }

    options->outfile->file = NULL;

#if 0
    if (!options->outfile->file)  {
        options->outfile->file = fopen(options->outfile->name,"w");
        if(!options->outfile->file) {
            perror(options->outfile->name);
            exit(1);
        }
    }
    GRIB_CHECK_NOLINE(grib_get_message(h,&buffer,&size),0);
    if (options->gts && h->gts_header)
        fwrite(h->gts_header,1,h->gts_header_len,options->outfile->file);

    if(fwrite(buffer,1,size,options->outfile->file) != size)
    {
        perror(options->outfile->name);
        exit(1);
    }

    if (options->gts && h->gts_header) {
        char gts_trailer[4]={'\x0D','\x0D','\x0A','\x03'};
        fwrite(gts_trailer,1,4,options->outfile->file);
    }
#endif
}
int exit_if_input_is_directory(const char* toolname, const char* filename)
{
    if (path_is_directory(filename)) {
        fprintf(stderr, "%s: ERROR: \"%s\": Is a directory\n", toolname, filename);
        exit(1);
    }
    return 0;
}
