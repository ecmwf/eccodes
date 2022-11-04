/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_dump
 *
 */

#include "grib_tools.h"

grib_option grib_options[] = {
    /*  {id, args, help}, on, command_line, value*/
    { "S", 0, 0, 1, 0, 0 },
    { "O", 0, "Octet mode. WMO documentation style dump.\n", 0, 1, 0 },
    { "D", 0, 0, 0, 1, 0 },
    { "d", 0, "Print all data values.\n", 0, 1, 0 },
    { "j", 0, 0, 0, 1, 0 },
    /*  {"C",0,0,0,1,0}, */ /* See ECC-234 */
    { "t", 0, 0, 0, 1, 0 },
    { "H", 0, 0, 0, 1, 0 },
    { "a", 0, 0, 0, 1, 0 },
    { "w:", 0, 0, 0, 1, 0 },
    { "s:", 0, 0, 0, 1, 0 },
    { "M", 0, 0, 0, 1, 0 },
    { "T:", 0, 0, 0, 1, 0 },
    { "7", 0, 0, 0, 1, 0 },
    { "V", 0, 0, 0, 1, 0 },
    { "q", 0, 0, 1, 0, 0 },
    { "p:", "key1,key2,...",
      "\n\t\tDeclaration of keys to dump. Only those keys are dumped (not the whole message).\n", 0, 1, 0 },
    { "X:", 0, 0, 0, 1, 0 },
    { "x", 0, 0, 0, 1, 0 }
};

const char* tool_description = "Dump the content of a GRIB file in different formats.";
const char* tool_name        = "grib_dump";
const char* tool_usage       = "[options] grib_file grib_file ...";
static int json              = 0;
static int first_handle      = 1;
static int dump_keys         = 0;

int grib_options_count = sizeof(grib_options) / sizeof(grib_option);

/**
 * grib_dump
 * Dump the contents of a GRIB file
 */
int main(int argc, char* argv[])
{
    return grib_tool(argc, argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    int opt = grib_options_on("C") + grib_options_on("O") + grib_options_on("D") + grib_options_on("j");

    options->dump_mode = (char*)"default";

    if (opt > 1) {
        fprintf(stderr, "%s: simultaneous j/O/D options not allowed\n", tool_name);
        exit(1);
    }

    if (grib_options_on("j")) {
        options->dump_mode = (char*)"json";
        json               = 1;
    }

    /* See ECC-234
    if (grib_options_on("C")) {
        options->dump_mode = "grib_encode_C";
        if (grib_options_on("d"))
            options->dump_flags = 0;
        else
            options->dump_flags = GRIB_DUMP_FLAG_NO_DATA;
    }
     */

    if (grib_options_on("O")) {
        options->dump_mode  = (char*)"wmo";
        options->dump_flags = GRIB_DUMP_FLAG_CODED | GRIB_DUMP_FLAG_OCTET | GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY;
    }

    if (grib_options_on("D")) {
        options->dump_mode  = (char*)"debug";
        options->dump_flags = GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY;
    }

    if (grib_options_on("a"))
        options->dump_flags |= GRIB_DUMP_FLAG_ALIASES;

    if (grib_options_on("t"))
        options->dump_flags |= GRIB_DUMP_FLAG_TYPE;

    if (grib_options_on("H"))
        options->dump_flags |= GRIB_DUMP_FLAG_HEXADECIMAL;

    /* In JSON mode we want to print all data values */
    if (grib_options_on("d") || grib_options_on("j"))
        options->dump_flags |= GRIB_DUMP_FLAG_ALL_DATA;

    dump_keys = grib_options_on("p:");

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options, const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    char tmp[1024];
    if (!options->current_infile->name)
        return 0;
    if (json)
        return 0;

    Assert(file);
    exit_if_input_is_directory(tool_name, file->name);

    sprintf(tmp, "FILE: %s ", options->current_infile->name);
    if (!grib_options_on("C"))
        fprintf(stdout, "***** %s\n", tmp);

    /*
     * Dumping of index files
     */
    if (is_index_file(options->current_infile->name)) {
        int err              = 0;
        grib_context* c      = grib_context_get_default();
        const char* filename = options->current_infile->name;

        err = grib_index_dump_file(stdout, filename);
        if (err) {
            grib_context_log(c, GRIB_LOG_ERROR, "%s: Could not dump index file \"%s\".\n%s\n",
                             tool_name,
                             filename,
                             grib_get_error_message(err));
            exit(1);
        }
        /* Since there are no GRIB messages, we have to stop tool exiting in case there
         * are more index files */
        options->fail = 0;
    }

    return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    long length = 0;
    int i, err = 0;
    grib_context* c = h->context;
    if (grib_get_long(h, "totalLength", &length) != GRIB_SUCCESS)
        length = -9999;

    if (!options->skip) {
        if (options->set_values_count != 0)
            err = grib_set_values(h, options->set_values, options->set_values_count);
        if (err != GRIB_SUCCESS && options->fail)
            exit(err);
    }

    /* ECC-961 */
    if (dump_keys && options->print_keys_count > 0) {
        int errors                = 0; /* true if at least one error */
        const int key_count_total = options->print_keys_count;
        int key_count_valid       = 0; /* Some requested keys may be invalid */
        const char** key_names    = (const char**)grib_context_malloc_clear(c, sizeof(char*) * key_count_total);
        for (i = 0; i < key_count_total; i++) {
            const char* key_name = options->print_keys[i].name;
            err                  = grib_set_flag(h, key_name, GRIB_ACCESSOR_FLAG_DUMP);
            if (err) {
                grib_context_log(c, GRIB_LOG_ERROR, "%s: %s", key_name, grib_get_error_message(err));
                errors = 1;
            }
            else {
                key_names[key_count_valid] = key_name;
                key_count_valid++;
            }
        }
        grib_dump_keys(h, stdout, options->dump_mode, options->dump_flags, 0, key_names, key_count_valid);
        grib_context_free(c, key_names);
        return errors;
    }

    if (json) {
        if (!first_handle && options->handle_count > 1) {
            fprintf(stdout, ",\n");
        }
        if (first_handle) {
            fprintf(stdout, "{ \"messages\" : [\n");
            first_handle = 0;
        }
    }
    else {
        char tmp[1024];
        char identifier[100];
        size_t idlen = 100;
        sprintf(tmp, "MESSAGE %d ( length=%ld )", options->handle_count, length);
        if (!grib_options_on("C"))
            fprintf(stdout, "#==============   %-38s   ==============\n", tmp);
        if (!strcmp(options->dump_mode, "default")) {
            GRIB_CHECK_NOLINE(grib_get_string(h, "identifier", identifier, &idlen), 0);
            printf("%s {\n", identifier);
        }
    }

    grib_dump_content(h, stdout, options->dump_mode, options->dump_flags, 0);

    if (!strcmp(options->dump_mode, "default"))
        printf("}\n");
    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    grib_handle_delete(h);
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options, grib_handle* h)
{
    grib_print_key_values(options, h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    if (json) {
        if (!dump_keys)
            fprintf(stdout, "\n]}\n");
        else
            fprintf(stdout, "\n");
    }
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    if (json) {
        if (first_handle) {
            fprintf(dump_file, "{ \"messages\" : [ \n");
            first_handle = 0;
        }
        else {
            fprintf(dump_file, ",\n");
        }
    }

    fprintf(dump_file, "\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
