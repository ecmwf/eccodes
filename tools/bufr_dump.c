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
 * C Implementation: bufr_dump
 *
 */

#include "grib_tools.h"

grib_option grib_options[] = {
    /*  {id, args, help}, on, command_line, value*/
    { "j:", "s|f|a",
      "\n\t\tJSON mode (JavaScript Object Notation)."
      "\n\t\tOptions: s->structure, f->flat (only data), a->all attributes"
      "\n\t\tDefault mode is structure.\n",
      1, 1, "s" },
    { "D:", "filter|fortran|python|C",
      "\n\t\tDecoding dump. Provides instructions to decode the input message."
      "\n\t\tOptions: filter  -> filter instructions file to decode input BUFR (for bufr_filter)"
      "\n\t\t         fortran -> fortran program to decode the input BUFR"
      "\n\t\t         python  -> python script to decode the input BUFR"
      "\n\t\t         C       -> C program to decode the input BUFR"
      "\n\t\tDefault mode is filter.\n",
      0, 1, "filter" },
    { "E:", "filter|fortran|python|C",
      "\n\t\tEncoding dump. Provides instructions to create the input message."
      "\n\t\tOptions: filter  -> filter instructions file to encode input BUFR (for bufr_filter)"
      "\n\t\t         fortran -> fortran program to encode the input BUFR"
      "\n\t\t         python  -> python script to encode the input BUFR"
      "\n\t\t         C       -> C program to encode the input BUFR"
      "\n\t\tDefault mode is filter.\n",
      0, 1, "filter" },

    /*{"S",0,0,1,0,0},*/
    { "O", 0, "Octet mode. WMO documentation style dump.\n", 0, 1, 0 },
    { "p", 0, "Plain dump (key=value format).\n", 0, 1, 0 },
    /* {"D",0,0,0,1,0},  */ /* See ECC-215 */
    { "d", 0, "Dump the expanded descriptors.\n", 0, 1, 0 },
    /*{"u",0,"Print only some values.\n",0,1,0},*/
    /* {"C",0,0,0,1,0}, */
    { "t", 0, 0, 0, 1, 0 },
    { "f", 0, 0, 0, 1, 0 },
    { "H", 0, 0, 0, 1, 0 },
    { "a", 0, 0, 0, 1, 0 },
    { "w:", 0, 0, 0, 1, 0 },
    { "s:", 0, 0, 0, 1, 0 },
    /* {"M",0,0,0,1,0}, */
    { "T:", 0, 0, 1, 0, "B" },
    { "7", 0, 0, 0, 1, 0 },
    { "V", 0, 0, 0, 1, 0 },
    { "q", 0, 0, 1, 0, 0 },
    { "S:", "subset_number", "\n\t\tDump the given subset\n", 0, 1, 0 },
    { "X:", 0, 0, 0, 1, 0 }
    /* {"x",0,0,0,1,0} */
};

const char* tool_description = "Dump the content of a BUFR file in different formats.";
const char* tool_name        = "bufr_dump";
const char* tool_usage       = "[options] bufr_file bufr_file ...";
static int json                   = 0;
static int dump_descriptors       = 0;
static char* json_option          = 0;
static int first_handle           = 1;
static grib_dumper* dumper        = 0;

int grib_options_count = sizeof(grib_options) / sizeof(grib_option);

/**
 * bufr_dump
 * Dump the content of a BUFR file
 *
 */
int main(int argc, char* argv[])
{
    return grib_tool(argc, argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

static void check_code_gen_dump_mode(const char* language)
{
    grib_context* c = grib_context_get_default();
    const int ok    = (strcmp(language, "C") == 0 ||
                    strcmp(language, "fortran") == 0 ||
                    strcmp(language, "python") == 0 ||
                    strcmp(language, "filter") == 0);
    if (!ok) {
        grib_context_log(c, GRIB_LOG_ERROR, "Invalid language specified. Select one of: filter, fortran, python or C");
    }
}

int grib_tool_init(grib_runtime_options* options)
{
    int opt = grib_options_on("C") + grib_options_on("O");

    options->dump_mode = "default";
    options->strict    = 1; /* Must set here as bufr_dump has its own -S option */

    if (opt > 1) {
        printf("%s: simultaneous j/C/O options not allowed\n", tool_name);
        exit(1);
    }

    if (grib_options_on("j:")) {
        options->dump_mode = "json";
        json_option        = grib_options_get_option("j:");
        if (strlen(json_option) > 1 || (json_option[0] != 's' && json_option[0] != 'f' && json_option[0] != 'a')) {
            printf("%s: Invalid JSON option %s\n", tool_name, json_option);
            exit(1);
        }
        json = 1;
    }

    if (grib_options_on("O")) {
        options->dump_mode  = "wmo";
        json                = 0;
        options->dump_flags = GRIB_DUMP_FLAG_CODED | GRIB_DUMP_FLAG_OCTECT | GRIB_DUMP_FLAG_VALUES | GRIB_DUMP_FLAG_READ_ONLY;
    }

    if (grib_options_on("p")) {
        options->dump_mode = "bufr_simple";
        json               = 0;
    }

    if (grib_options_on("D:")) {
        options->dump_mode = grib_options_get_option("D:");
        check_code_gen_dump_mode(options->dump_mode);
        json = 0;
    }
    if (grib_options_on("E:")) {
        grib_context* c = grib_context_get_default();
        if (c->bufr_multi_element_constant_arrays) {
            grib_context_log(c, GRIB_LOG_ERROR, "Code generation for encoding is not implemented when ECCODES_BUFR_MULTI_ELEMENT_CONSTANT_ARRAYS is enabled");
            exit(1);
        }
        options->dump_mode = grib_options_get_option("E:");
        check_code_gen_dump_mode(options->dump_mode);
        json = 0;
    }

    /* See ECC-215
    if (grib_options_on("D")) {
        options->dump_mode = "debug";
        options->dump_flags = GRIB_DUMP_FLAG_VALUES
                |  GRIB_DUMP_FLAG_READ_ONLY;
    }  */

    if (grib_options_on("a"))
        options->dump_flags |= GRIB_DUMP_FLAG_ALIASES;

    if (grib_options_on("t"))
        options->dump_flags |= GRIB_DUMP_FLAG_TYPE;

    if (grib_options_on("H"))
        options->dump_flags |= GRIB_DUMP_FLAG_HEXADECIMAL;

    if (grib_options_on("d") && !grib_options_on("u")) {
        options->dump_flags |= GRIB_DUMP_FLAG_ALL_DATA;
        dump_descriptors = 1;
        json             = 0;
    }

    /* Turn off GRIB multi-field support mode. Not relevant for BUFR */
    grib_multi_support_off(grib_context_get_default());

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options, const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    if (!options->current_infile->name)
        return 0;

    Assert(file);
    exit_if_input_is_directory(tool_name, file->name);

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
        /* Since there are no BUFR messages, we have to stop tool exiting in case there
         * are more index files */
        options->fail = 0;
        return 0;
    }

    if (json)
        return 0;

    if (grib_options_on("E:") || grib_options_on("D:")) {
        if (!strcmp(options->dump_mode, "filter")) {
            /* Dump filter for one message only. Multi-message BUFR files will not work! */
            int err = 0, numMessages = 0;
            grib_context* c = grib_context_get_default();
            err             = grib_count_in_file(c, options->current_infile->file, &numMessages);
            if (!err && numMessages > 1) {
                fprintf(stderr, "\nERROR: Cannot dump filter for multi-message BUFR files.\n");
                fprintf(stderr, "       (The input file \"%s\" has %d messages)\n", options->current_infile->name, numMessages);
                fprintf(stderr, "       Please split the input file and dump filter for each message separately.\n");
                exit(1);
            }
        }
    }
    else {
        char tmp[1024];
        sprintf(tmp, "FILE: %s ", options->current_infile->name);
        if (!grib_options_on("p"))
            fprintf(stdout, "***** %s\n", tmp);
    }

    return 0;
}

static char* get_dumper_name(grib_runtime_options* options)
{
    if (grib_options_on("E:")) {
        /* Dumper for ENCODING */
        if (strcmp(options->dump_mode, "filter") == 0) {
            return "bufr_encode_filter";
        }
        if (strcmp(options->dump_mode, "fortran") == 0) {
            return "bufr_encode_fortran";
        }
        if (strcmp(options->dump_mode, "C") == 0) {
            return "bufr_encode_C";
        }
        if (strcmp(options->dump_mode, "python") == 0) {
            return "bufr_encode_python";
        }
    }
    else if (grib_options_on("D:")) {
        /* Dumper for DECODING */
        if (strcmp(options->dump_mode, "filter") == 0) {
            return "bufr_decode_filter";
        }
        if (strcmp(options->dump_mode, "fortran") == 0) {
            return "bufr_decode_fortran";
        }
        if (strcmp(options->dump_mode, "C") == 0) {
            return "bufr_decode_C";
        }
        if (strcmp(options->dump_mode, "python") == 0) {
            return "bufr_decode_python";
        }
    }
    return options->dump_mode;
}

static void print_header(grib_runtime_options* options)
{
    if (strcmp(options->dump_mode, "filter") == 0) {
        int print_unpack = 0;
        char generator_name[32];

        if (grib_options_on("D:")) {
            strcpy(generator_name, "-Dfilter");
            print_unpack = 1;
        }
        else if (grib_options_on("E:")) {
            strcpy(generator_name, "-Efilter");
        }

        fprintf(stdout, "#  This filter was automatically generated with bufr_dump %s\n", generator_name);
        fprintf(stdout, "#  Using ecCodes version: ");
        grib_print_api_version(stdout);
        fprintf(stdout, "\n\n");
        if (print_unpack)
            fprintf(stdout, "set unpack=1;\n");
    }
}

static void bufr_dump_descriptors(grib_handle* h)
{
    size_t size_desc = 0, size_names = 0, size_abbrevs = 0, size_units = 0;
    size_t i = 0, j = 0, size_proper = 0;
    long* array_descriptors = NULL;
    char** array_names      = NULL;
    char** array_abbrevs    = NULL;
    char** array_units      = NULL;
    char* the_key           = "expandedDescriptors";

    GRIB_CHECK_NOLINE(grib_get_size(h, the_key, &size_desc), 0);
    array_descriptors = (long*)malloc(size_desc * sizeof(long));
    if (!array_descriptors) {
        fprintf(stderr, "%s: Memory allocation error", the_key);
        exit(GRIB_OUT_OF_MEMORY);
    }
    GRIB_CHECK_NOLINE(grib_get_long_array(h, the_key, array_descriptors, &size_desc), 0);
    size_proper = size_desc;
    /* Exclude the pesky 999999 descriptors as they don't
     * have equivalents in the name, abbreviation and units arrays!
     */
    for (i = 0; i < size_desc; ++i) {
        if (array_descriptors[i] == 999999)
            size_proper--;
    }

    /* The string arrays for keys, names and units should have the same length*/
    the_key = "expandedAbbreviations";
    GRIB_CHECK_NOLINE(grib_get_size(h, the_key, &size_abbrevs), 0);
    array_abbrevs = (char**)malloc(size_abbrevs * sizeof(char*));
    if (!array_abbrevs) {
        fprintf(stderr, "%s: Memory allocation error", the_key);
        exit(GRIB_OUT_OF_MEMORY);
    }
    GRIB_CHECK_NOLINE(grib_get_string_array(h, the_key, array_abbrevs, &size_abbrevs), 0);
    Assert(size_proper == size_abbrevs);

    the_key = "expandedNames";
    GRIB_CHECK_NOLINE(grib_get_size(h, the_key, &size_names), 0);
    array_names = (char**)malloc(size_names * sizeof(char*));
    if (!array_names) {
        fprintf(stderr, "%s: Memory allocation error", the_key);
        exit(GRIB_OUT_OF_MEMORY);
    }
    GRIB_CHECK_NOLINE(grib_get_string_array(h, the_key, array_names, &size_names), 0);
    Assert(size_proper == size_names);

    the_key = "expandedUnits";
    GRIB_CHECK_NOLINE(grib_get_size(h, the_key, &size_units), 0);
    array_units = (char**)malloc(size_units * sizeof(char*));
    if (!array_units) {
        fprintf(stderr, "%s: Memory allocation error", the_key);
        exit(GRIB_OUT_OF_MEMORY);
    }
    GRIB_CHECK_NOLINE(grib_get_string_array(h, the_key, array_units, &size_units), 0);
    Assert(size_proper == size_units);

    i = 0;
    j = 0;
    while (i < size_desc) {
        const long desc = array_descriptors[i];
        if (desc == 999999) {
            printf("%06ld\t\t\n", desc);
        }
        else {
            char* abbr  = array_abbrevs[j];
            char* name  = array_names[j];
            char* units = array_units[j];
            printf("%06ld\t%s\t%s [%s]\n", desc, abbr, name, units);
            ++j;
            free(abbr);
            free(name);
            free(units);
        }
        ++i;
    }

    free(array_descriptors);
    free(array_abbrevs);
    free(array_names);
    free(array_units);
}

static int check_subset_number(const char* user_input, long numberOfSubsets, long* subsetNumber)
{
    long val = 0;
    char* endptr;
    errno = 0; /* To distinguish success/failure after call */
    val   = strtol(user_input, &endptr, 10);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
        perror("strtol");
        return GRIB_INVALID_ARGUMENT;
    }
    if (endptr == user_input) {
        return GRIB_INVALID_ARGUMENT;
    }

    if (val < 1 || val > numberOfSubsets) {
        return GRIB_INVALID_ARGUMENT;
    }
    *subsetNumber = val;
    return GRIB_SUCCESS;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    long length = 0;
    int i, err = 0;
    grib_handle* hclone     = NULL;
    grib_accessor* a        = NULL;
    grib_accessors_list* al = NULL;
    if (grib_get_long(h, "totalLength", &length) != GRIB_SUCCESS)
        length = -9999;

    if (!options->skip) {
        if (options->set_values_count != 0)
            err = grib_set_values(h, options->set_values, options->set_values_count);
        if (err != GRIB_SUCCESS && options->fail)
            exit(err);
    }

    for (i = 0; i < options->print_keys_count; i++)
        grib_set_flag(h, options->print_keys[i].name, GRIB_ACCESSOR_FLAG_DUMP);

    if (grib_options_on("S:")) {
        long numberOfSubsets = 0, subsetNumber = 0;
        char* str = grib_options_get_option("S:");
        err       = grib_get_long(h, "numberOfSubsets", &numberOfSubsets);
        if (err) {
            fprintf(stderr, "ERROR: Failed to get numberOfSubsets.\n");
            exit(1);
        }

        if (check_subset_number(str, numberOfSubsets, &subsetNumber) == GRIB_SUCCESS) {
            if (numberOfSubsets > 1) {
                grib_handle* new_handle = 0;
                grib_handle* h2;
                size_t size        = 0;
                const void* buffer = NULL;

                /* Clone, unpack and extract that particular subset */
                h2 = grib_handle_clone(h);
                Assert(h2);
                GRIB_CHECK_NOLINE(grib_set_long(h2, "unpack", 1), 0);
                GRIB_CHECK_NOLINE(grib_set_long(h2, "extractSubset", subsetNumber), 0);
                GRIB_CHECK_NOLINE(grib_set_long(h2, "doExtractSubsets", 1), 0);

                /* Put result into buffer then form new handle from it */
                GRIB_CHECK_NOLINE(grib_get_message(h2, &buffer, &size), 0);
                new_handle = grib_handle_new_from_message(0, buffer, size);
                Assert(new_handle);
                /* Replace handle with the new one which has only one subset */
                h = new_handle;
                hclone = h2; /* to be deleted later */
            }
        }
        else {
            fprintf(stderr, "ERROR: -S option: Please specify a subset number > 0 and < %ld\n", numberOfSubsets + 1);
            exit(1);
        }
    }

    if (json) {
        /* ECC-233: print comma as separator between messages */
        if (!first_handle && options->handle_count > 1) {
            fprintf(stdout, ",\n");
        }
        if (json && first_handle) {
            fprintf(stdout, "{ \"messages\" : [ \n");
            first_handle = 0;
        }
        switch (json_option[0]) {
            case 'f':
                err = grib_set_long(h, "unpack", 2);
                if (err) {
                    if (options->fail) {
                        fprintf(stderr, "ERROR: unable to unpack data section: %s (message=%d)\n",
                                grib_get_error_message(err), options->handle_count);
                        exit(err);
                    }
                    else {
                        fprintf(stdout, "\"ERROR: unable to unpack data section\"");
                        options->error = err;
                        /*return err; See ECC-723*/
                    }
                }
                a                   = grib_find_accessor(h, "numericValues");
                al                  = accessor_bufr_data_array_get_dataAccessors(a);
                options->dump_flags = GRIB_DUMP_FLAG_ALL_ATTRIBUTES;
                codes_dump_bufr_flat(al, h, stdout, options->dump_mode, options->dump_flags, 0);
                break;
            case 's':
                err = grib_set_long(h, "unpack", 1);
                if (err) {
                    if (options->fail) {
                        fprintf(stderr, "ERROR: unable to unpack data section: %s (message=%d)\n",
                                grib_get_error_message(err), options->handle_count);
                        exit(err);
                    }
                    else {
                        fprintf(stdout, "\"ERROR: unable to unpack data section\"");
                        options->error = err;
                        /*return err; See ECC-723*/
                    }
                }
                grib_dump_content(h, stdout, options->dump_mode, options->dump_flags, 0);
                break;
            case 'a':
                err = grib_set_long(h, "unpack", 1);
                if (err) {
                    if (options->fail) {
                        fprintf(stderr, "ERROR: unable to unpack data section: %s (message=%d)\n",
                                grib_get_error_message(err), options->handle_count);
                        exit(err);
                    }
                    else {
                        fprintf(stdout, "\"ERROR: unable to unpack data section\"");
                        options->error = err;
                        /*return err; See ECC-723*/
                    }
                }
                options->dump_flags = GRIB_DUMP_FLAG_ALL_ATTRIBUTES;
                grib_dump_content(h, stdout, options->dump_mode, options->dump_flags, 0);
                break;
            default:
                printf("Unknown JSON option %s\n", json_option);
                exit(1);
        }
        if (!strcmp(options->dump_mode, "default")) {
            printf("}\n");
        }
    }
    else if (grib_options_on("O")) {
        char tmp[1024];
        sprintf(tmp, "MESSAGE %d ( length=%ld )", options->handle_count, length);
        if (!grib_options_on("C"))
            fprintf(stdout, "#==============   %-38s   ==============\n", tmp);
        grib_dump_content(h, stdout, options->dump_mode, options->dump_flags, 0);
    }
    else if (dump_descriptors) {
        /* Dump out the section 3 descriptors, their keys/units etc */
        bufr_dump_descriptors(h);
    }
    else {
        const char* dumper_name = get_dumper_name(options);
        if (strcmp(dumper_name, "bufr_simple") == 0) {
            /* This speeds up the unpack by skipping attribute keys not used in the dump */
            grib_set_long(h, "skipExtraKeyAttributes", 1);
        }
        err = grib_set_long(h, "unpack", 1);
        if (err) {
            if (options->fail) {
                fprintf(stderr, "ERROR: unable to unpack data section: %s\n", grib_get_error_message(err));
                exit(err);
            }
            else {
                fprintf(stdout, "\"ERROR: unable to unpack data section\"");
                options->error = err;
                /*return err; See ECC-723*/
            }
        }
        print_header(options);
        /* Note: if the dumper passed in is non-NULL, it will be freed up */
        dumper = grib_dump_content_with_dumper(h, dumper, stdout, dumper_name, options->dump_flags, 0);
        if (!dumper)
            exit(1);
        if (grib_options_on("p")) {
            printf("\n"); /* One blank line to separate the messages */
        }
    }

    grib_handle_delete(hclone);
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
    if (json)
        fprintf(stdout, "\n]}\n");

    if (grib_options_on("E:")) {
        /* Code for ENCODING */
        if (!strcmp(options->dump_mode, "filter")) {
            fprintf(stdout, "set pack=1;\nwrite;\n");
        }
        if (!strcmp(options->dump_mode, "fortran")) {
            fprintf(stdout, "end program bufr_encode\n");
        }
        if (!strcmp(options->dump_mode, "C")) {
            fprintf(stdout, "  return 0;\n");
            fprintf(stdout, "}\n");
        }
        if (!strcmp(options->dump_mode, "python")) {
            fprintf(stdout, "\n\n");
            fprintf(stdout, "def main():\n");
            fprintf(stdout, "    try:\n");
            fprintf(stdout, "        bufr_encode()\n");
            fprintf(stdout, "    except CodesInternalError as err:\n");
            fprintf(stdout, "        traceback.print_exc(file=sys.stderr)\n");
            fprintf(stdout, "        return 1\n");
            fprintf(stdout, "\n\n");
            fprintf(stdout, "if __name__ == \"__main__\":\n");
            fprintf(stdout, "    sys.exit(main())\n");
        }
    }
    else if (grib_options_on("D:")) {
        /* Code for DECODING */
        if (!strcmp(options->dump_mode, "filter")) {
        }
        if (!strcmp(options->dump_mode, "fortran")) {
            fprintf(stdout, "\n  call codes_close_file(ifile)\n");
            fprintf(stdout, "end program bufr_decode\n");
        }
        if (!strcmp(options->dump_mode, "C")) {
            fprintf(stdout, "  if (fclose(fin)!=0) {\n");
            fprintf(stdout, "    fprintf(stderr, \"Failed to close file handle.\\n\");\n");
            fprintf(stdout, "    return 1;\n");
            fprintf(stdout, "  }\n");
            fprintf(stdout, "  return 0;\n");
            fprintf(stdout, "}\n");
        }
        if (!strcmp(options->dump_mode, "python")) {
            fprintf(stdout, "\n\n");
            fprintf(stdout, "    f.close()\n\n");
            fprintf(stdout, "def main():\n");
            fprintf(stdout, "    if len(sys.argv) < 2:\n");
            fprintf(stdout, "        print('Usage: ', sys.argv[0], ' BUFR_file', file=sys.stderr)\n");
            fprintf(stdout, "        sys.exit(1)\n\n");
            fprintf(stdout, "    try:\n");
            fprintf(stdout, "        bufr_decode(sys.argv[1])\n");
            fprintf(stdout, "    except CodesInternalError as err:\n");
            fprintf(stdout, "        traceback.print_exc(file=sys.stderr)\n");
            fprintf(stdout, "        return 1\n");
            fprintf(stdout, "\n\n");
            fprintf(stdout, "if __name__ == \"__main__\":\n");
            fprintf(stdout, "    sys.exit(main())\n");
        }
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
    fprintf(dump_file, "\"ERROR: unreadable message\"\n");
    if (options->fail) {
        exit(1);
    }
    return 0;
}
