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

grib_option grib_options[] = {
    /*  {id, args, help}, on, command_line, value*/
    { "r", 0, "Compare files in which the messages are not in the same order. This option is time expensive.\n", 0, 1, 0 },
    { "b:", 0, 0, 0, 1, 0 },
    { "d", 0, "Write different messages on files\n", 0, 1, 0 },
    { "T:", 0, 0, 1, 0, "M" }, /* METAR */
    { "c:", 0, 0, 0, 1, 0 },
    { "S:", "start", "First field to be processed.\n", 0, 1, 0 },
    { "E:", "end", "Last field to be processed.\n", 0, 1, 0 },
    { "a", 0, "-c option modifier. The keys listed with the option -c will be added to the list of keys compared without -c.\n", 0, 1, 0 },
    { "H", 0, "Compare only message headers. Bit-by-bit compare on. Incompatible with -c option.\n", 0, 1, 0 },
    { "R:", 0, 0, 0, 1, 0 },
    { "A:", 0, 0, 0, 1, 0 },
    { "P", 0, "Compare data values using the packing error as tolerance.\n", 0, 1, 0 },
    { "t:", "factor", "Compare data values using factor multiplied by the tolerance specified in options -P -R -A.\n", 0, 1, 0 },
    { "w:", 0, 0, 0, 1, 0 },
    { "f", 0, 0, 0, 1, 0 },
    { "F", 0, 0, 1, 0, 0 },
    { "q", 0, 0, 1, 0, 0 },
    { "I", 0, 0, 1, 0, 0 },
    { "V", 0, 0, 0, 1, 0 },
    { "7", 0, 0, 0, 1, 0 },
    { "v", 0, 0, 0, 1, 0 }
};

int grib_options_count = sizeof(grib_options) / sizeof(grib_option);

const char* tool_description =
    "Compare METAR messages contained in two files."
    "\n\tIf some differences are found it fails returning an error code."
    "\n\tFloating-point values are compared exactly by default, different tolerance can be defined see -P -A -R."
    "\n\tDefault behaviour: absolute error=0, bit-by-bit compare, same order in files.";

const char* tool_name = "metar_compare";
const char* tool_usage =
    "[options] "
    "file file";


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

typedef double (*compare_double_proc)(const double*, const double*, const double*);

typedef struct grib_error grib_error;
struct grib_error
{
    char* key;
    int count;
    grib_error* next;
};

static grib_error* error_summary;
static compare_double_proc compare_double;
static double global_tolerance     = 0;
static int packingCompare          = 0;
static grib_string_list* blocklist = 0;
static int compareAbsolute         = 1;

static int compare_handles(grib_handle* h1, grib_handle* h2, grib_runtime_options* options);
static int error               = 0;
static int count               = 0;
static int lastPrint           = 0;
static int force               = 0;
static double maxAbsoluteError = 1e-19;
static int onlyListed          = 1;
static int headerMode          = 0;
static int morein1             = 0;
static int morein2             = 0;
static int listFromCommandLine;
static int verbose                = 0;
static double tolerance_factor    = 1;
static int write_error            = 0;
static grib_handle* global_handle = NULL;
static int global_counter         = 0;
static int start                  = -1;
static int end                    = -1;
static int write_count            = 0;

GRIB_INLINE static double compare_double_absolute(const double* a, const double* b, const double* err)
{
    double ret = 0;
    double d   = fabs(*a - *b);
    if (d > *err) {
        ret = d;
    }
    return ret;
    /* return fabs(*a-*b) > *err ? fabs(*a-*b) : 0; */
}

static void write_message(grib_handle* h, const char* str)
{
    const void* m;
    size_t s;
    char fname[1024] = {0,};
    FILE* fh;

    grib_get_message(h, &m, &s);
    sprintf(fname, "%s_%d.metar", str, write_count);

    fh = fopen(fname, "w");
    if (!fh) {
        grib_context_log(h->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                         "Error opening %s", fname);
        exit(GRIB_IO_PROBLEM);
    }

    if (fwrite(m, 1, s, fh) != s) {
        grib_context_log(h->context, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                         "Error writing to %s", fname);
        exit(GRIB_IO_PROBLEM);
    }

    fclose(fh);
}

static void write_messages(grib_handle* h1, grib_handle* h2)
{
    if (!write_error)
        return;
    write_count++;

    write_message(h1, "error1");
    write_message(h2, "error2");
}

static double compare_double_relative(const double* a, const double* b, const double* err)
{
    double relativeError;

    if (fabs(*a) <= maxAbsoluteError || fabs(*b) <= maxAbsoluteError)
        relativeError = fabs(*a - *b);
    else if (fabs(*b) > fabs(*a))
        relativeError = fabs((*a - *b) / *b);
    else
        relativeError = fabs((*a - *b) / *a);

    return relativeError > *err ? relativeError : 0;
}

static int blocklisted(const char* name)
{
    grib_string_list* b = blocklist;
    if (!name) return 0;
    while (b) {
        Assert(b->value);
        if (!strcmp(name, b->value))
            return 1;
        b = b->next;
    }
    return 0;
}

static double relative_error(double a, double b, double err)
{
    double relativeError;

    if (fabs(a) <= maxAbsoluteError || fabs(b) <= maxAbsoluteError)
        relativeError = fabs(a - b);
    else if (fabs(b) > fabs(a))
        relativeError = fabs((a - b) / b);
    else
        relativeError = fabs((a - b) / a);

    return relativeError;
}

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
    int ret               = 0, i;
    int nfiles            = 1;
    char orderby[]        = "md5Headers";
    grib_context* context = grib_context_get_default();

    options->strict = 1;
    if (grib_options_on("S:"))
        start = atoi(grib_options_get_option("S:"));

    if (grib_options_on("E:"))
        end = atoi(grib_options_get_option("E:"));

    if (grib_options_on("f"))
        force = 1;
    else
        force = 0;

    if (grib_options_on("d"))
        write_error = 1;
    else
        write_error = 0;

    verbose = grib_options_on("v");

    listFromCommandLine = 0;
    if (grib_options_on("c:") || grib_options_on("e"))
        listFromCommandLine = 1;

    if (grib_options_on("a"))
        onlyListed = 0;
    else
        onlyListed = 1;

    if (grib_options_on("H"))
        headerMode = 1;
    else
        headerMode = 0;

    if (grib_options_on("H") && grib_options_on("c:")) {
        printf("Error: -H and -c options are incompatible. Choose one of the two please.\n");
        exit(1);
    }
    if (grib_options_on("a") && !grib_options_on("c:")) {
        printf("Error: -a option requires -c option. Please define a list of keys with the -c option.\n");
        exit(1);
    }

    if (grib_options_on("b:")) {
        grib_string_list* next = 0;
        blocklist              = (grib_string_list*)grib_context_malloc_clear(context, sizeof(grib_string_list));
        blocklist->value       = grib_context_strdup(context, options->set_values[0].name);
        next                   = blocklist;
        for (i = 1; i < options->set_values_count; i++) {
            next->next        = (grib_string_list*)grib_context_malloc_clear(context, sizeof(grib_string_list));
            next->next->value = grib_context_strdup(context, options->set_values[i].name);
            next              = next->next;
        }
        context->blocklist = blocklist;
    }

    if (grib_options_on("r")) {
        const char* filename[1];
        filename[0]      = options->infile_extra->name;
        options->random  = 1;
        options->orderby = strdup(orderby);
        options->idx     = grib_fieldset_new_from_files(context, filename,
                                                    nfiles, 0, 0, 0, orderby, &ret);
        if (ret) {
            printf("unable to create index for input file %s (%s)",
                   options->infile_extra->name, grib_get_error_message(ret));
            exit(ret);
        }
    }
    else {
        options->random             = 0;
        options->infile_extra->file = fopen(options->infile_extra->name, "r");

        if (!options->infile_extra->file) {
            perror(options->infile_extra->name);
            exit(1);
        }
    }

    global_tolerance = 0;
    compare_double   = &compare_double_absolute;
    if (grib_options_on("R:")) {
        global_tolerance = 0;
        for (i = 0; i < options->tolerance_count; i++) {
            if (!strcmp((options->tolerance[i]).name, "all")) {
                global_tolerance = (options->tolerance[i]).double_value;
                break;
            }
            if (!strcmp((options->tolerance[i]).name, "global")) {
                global_tolerance = (options->tolerance[i]).double_value;
                break;
            }
        }
        compare_double  = &compare_double_relative;
        compareAbsolute = 0;
    }
    if (grib_options_on("A:")) {
        if (grib_options_on("R:")) {
            maxAbsoluteError = atof(grib_options_get_option("A:"));
        }
        else {
            compare_double   = &compare_double_absolute;
            global_tolerance = atof(grib_options_get_option("A:"));
        }
    }
    if (grib_options_on("P")) {
        packingCompare = 1;
        compare_double = &compare_double_absolute;
    }

    if (grib_options_on("t:"))
        tolerance_factor = atof(grib_options_get_option("t:"));

    {
        /* Check for 2nd file being a directory. If so, we assume user is comparing to a file */
        /* with the same name as first file in that directory */
        grib_tools_file* infile = options->infile; /* the 2nd file in comparison */
        if (infile) {
            if (path_is_directory(infile->name)) {
                /* Take the filename of the 1st file and append to dir */
                char bufr[2048] = {0,};
                /* options->infile_extra->name is the 1st file */
                sprintf(bufr, "%s%c%s",
                        infile->name,
                        get_dir_separator_char(),
                        extract_filename(options->infile_extra->name));
                infile->name = strdup(bufr);
            }
        }
    }

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options, const char* file)
{
    return 0;
}
int grib_tool_new_file_action(grib_runtime_options* options, grib_tools_file* file)
{
    return 0;
}

static void printInfo(grib_handle* h)
{
    printf("== %d == DIFFERENCE == ", count);
    lastPrint = count;
}

static void print_index_key_values(grib_index* index, int counter, const char* error_message)
{
    grib_index_key* keys = index->keys;
    printf("== %d == ", counter);
    if (error_message)
        printf("%s == ", error_message);
    while (keys) {
        printf("%s=%s ", keys->name, keys->value);
        keys = keys->next;
    }
    printf("\n");
}

static grib_handle* metar_handle_new_from_file_x(
    grib_context* c, FILE* f, int mode, int headers_only, int* err)
{
    return codes_handle_new_from_file(c, f, PRODUCT_METAR, err);
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int err = 0;
    count++;

    if (options->through_index) {
        grib_index* idx1 = options->index1;
        verbose          = 0;
        global_counter++;

        if (start > 0 && global_counter < start)
            return 0;
        if (end > 0 && global_counter > end) {
            options->stop = 1;
            return 0;
        }

        grib_index_search_same(idx1, h);
        global_handle = codes_new_from_index(idx1, CODES_METAR, &err);
        if (options->verbose) {
            off_t offset   = 0;
            char* filename = grib_get_field_file(options->index2, &offset);
            printf("file1=\"%s\" ", filename);
            filename = grib_get_field_file(options->index1, &offset);
            printf("file2=\"%s\" \n", filename);
            print_index_key_values(options->index1, global_counter, NULL);
        }

        if (!global_handle) {
            if (!options->verbose)
                print_index_key_values(idx1, global_counter, "NOT FOUND ");
        }

        if (!global_handle || err != GRIB_SUCCESS) {
            morein1++;
            if (global_handle)
                grib_handle_delete(global_handle);
            return 0;
        }

        if (compare_handles(h, global_handle, options)) {
            error++;
            if (!force)
                exit(1);
        }

        grib_handle_delete(global_handle);

        return 0;
    }
    else if (options->random)
        global_handle = grib_fieldset_next_handle(options->idx, &err);
    else
        global_handle = metar_handle_new_from_file_x(h->context, options->infile_extra->file, options->mode, 0, &err);

    if (!global_handle || err != GRIB_SUCCESS) {
        morein2++;
        if (global_handle)
            grib_handle_delete(global_handle);
        return 0;
    }

    if (compare_handles(global_handle, h, options)) {
        error++;
        if (!force)
            exit(1);
    }

    grib_handle_delete(global_handle);

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    int err = 0;
    if (!options->through_index && !options->random) {
        global_handle = metar_new_from_file(h->context, options->infile_extra->file, &err);

        if (!global_handle || err != GRIB_SUCCESS)
            morein2++;

        grib_handle_delete(global_handle);
    }

    grib_handle_delete(h);
    count++;

    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options, grib_handle* h)
{
    grib_print_key_values(options, h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    grib_error* e   = error_summary;
    int err         = 0;
    grib_context* c = grib_context_get_default();

    while ((global_handle = metar_new_from_file(c, options->infile_extra->file, &err))) {
        morein1++;
        if (global_handle)
            grib_handle_delete(global_handle);
    }

    error += morein1 + morein2;
    if (error) {
        printf("\n## ERRORS SUMMARY #######\n");
    }

    if (morein1 > 0) {
        printf("##\n## Different number of messages \n");
        printf("## %d more messages in %s than in %s\n", morein1,
               options->infile_extra->name, options->infile->name);
    }

    if (morein2 > 0) {
        printf("##\n## Different number of messages \n");
        printf("## %d more messages in %s than in %s\n", morein2,
               options->infile->name, options->infile_extra->name);
    }

    if (error) {
        printf("##\n## Summary of different key values \n");
        while (e) {
            printf("## %s ( %d different )\n", e->key, e->count);
            e = e->next;
        }

        printf("##\n## %d different messages out of %d\n\n", error, count);
    }
    if (options->through_index) {
        grib_index_delete(options->index1);
        grib_index_delete(options->index2);
    }

    if (error != 0)
        exit(1);
    return 0;
}

static void save_error(grib_context* c, const char* key)
{
    grib_error* e    = 0;
    grib_error* next = 0;
    int saved        = 0;

    if (!error_summary) {
        error_summary        = (grib_error*)grib_context_malloc_clear(c, sizeof(grib_error));
        error_summary->count = 1;
        error_summary->key   = grib_context_strdup(c, key);
        return;
    }

    e    = error_summary;
    next = e;

    while (next) {
        if (!strcmp(next->key, key)) {
            next->count++;
            saved = 1;
            break;
        }
        e    = next;
        next = next->next;
    }

    if (!saved) {
        e->next        = (grib_error*)grib_context_malloc_clear(c, sizeof(grib_error));
        e->next->count = 1;
        e->next->key   = grib_context_strdup(c, key);
    }
}

static int compare_values(grib_runtime_options* options, grib_handle* h1, grib_handle* h2, const char* name, int type)
{
    size_t len1 = 0;
    size_t len2 = 0;
    int err = 0, i = 0;
    int err1;
    int err2;
    int type1, type2;
    int countdiff;
    int isangle    = 0;
    int isMissing1 = 0, isMissing2 = 0;

    char *sval1 = NULL, *sval2 = NULL;
    unsigned char *uval1 = NULL, *uval2 = NULL;
    double *dval1 = NULL, *dval2 = NULL;
    long *lval1 = NULL, *lval2 = NULL;
    double maxdiff       = 0;
    double packingError1 = 0, packingError2 = 0;
    double value_tolerance = 0;
    grib_context* c        = h1->context;

    type1 = type;
    type2 = type;
    if (verbose)
        printf("  comparing %s", name);

    if (type1 == GRIB_TYPE_UNDEFINED && (err = grib_get_native_type(h1, name, &type1)) != GRIB_SUCCESS) {
        printInfo(h1);
        printf("Oops... cannot get type of [%s] in 1st field: %s\n", name, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    if (type2 == GRIB_TYPE_UNDEFINED && (err = grib_get_native_type(h2, name, &type2)) != GRIB_SUCCESS) {
        if (err == GRIB_NOT_FOUND) {
            printInfo(h1);
            printf("[%s] not found in 2nd field\n", name);
            save_error(c, name);
            return err;
        }
        printInfo(h1);
        printf("Oops... cannot get type of [%s] in 2nd field: %s\n", name, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    /*
  if(type1 != type2)
  {
    printInfo(h1);
    printf("Warning, [%s] has different types: 1st field: [%s], 2nd field: [%s]\n",
        name,grib_get_type_name(type1),grib_get_type_name(type2));
    return GRIB_TYPE_MISMATCH; 
  }
     */

    if (type1 == GRIB_TYPE_LABEL)
        return err;

    if (type1 == GRIB_TYPE_SECTION)
        return err;


    if ((err = grib_get_size(h1, name, &len1)) != GRIB_SUCCESS) {
        printInfo(h1);
        printf("Oops... cannot get size of [%s] in 1st field: %s\n", name, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    if ((err = grib_get_size(h2, name, &len2)) != GRIB_SUCCESS) {
        if (err == GRIB_NOT_FOUND) {
            printInfo(h1);
            printf("[%s] not found in 2nd field\n", name);
            save_error(c, name);
            return err;
        }

        printInfo(h1);
        printf("Oops... cannot get size of [%s] in 2nd field: %s\n", name, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    /*
  if(len1 != len2 && type1 != GRIB_TYPE_STRING)
  {
    printInfo(h1);
    printf("[%s] has different size: 1st field: %ld, 2nd field: %ld\n",name,(long)len1,(long)len2);
    save_error(c,name);
    return GRIB_COUNT_MISMATCH;
  }
     */

    if (options->mode != MODE_METAR) {
        /* TODO: Ignore missing values for keys in METAR. Not yet implemented */
        isMissing1 = ((grib_is_missing(h1, name, &err1) == 1) && (err1 == 0)) ? 1 : 0;
        isMissing2 = ((grib_is_missing(h2, name, &err2) == 1) && (err2 == 0)) ? 1 : 0;
    }

    if ((isMissing1 == 1) && (isMissing2 == 1)) {
        if (verbose)
            printf(" is set to missing in both fields\n");
        return GRIB_SUCCESS;
    }

    if (isMissing1 == 1) {
        if (verbose)
            printf(" is set to missing in 1st field\n");
        printInfo(h1);
        printf("%s is set to missing in 1st field is not missing in 2nd field\n", name);
        err1 = GRIB_VALUE_MISMATCH;
        save_error(c, name);
        return GRIB_VALUE_MISMATCH;
    }

    if (isMissing2 == 1) {
        if (verbose)
            printf(" is set to missing in 1st field\n");
        printInfo(h1);
        printf("%s is set to missing in 2nd field is not missing in 1st field\n", name);
        err1 = GRIB_VALUE_MISMATCH;
        save_error(c, name);
        return GRIB_VALUE_MISMATCH;
    }

    switch (type1) {
        case GRIB_TYPE_STRING:
            if (verbose)
                printf(" as string\n");
            grib_get_string_length(h1, name, &len1);
            grib_get_string_length(h2, name, &len2);
            sval1 = (char*)grib_context_malloc(h1->context, len1 * sizeof(char));
            sval2 = (char*)grib_context_malloc(h2->context, len2 * sizeof(char));

            if ((err1 = grib_get_string(h1, name, sval1, &len1)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get string value of [%s] in 1st field: %s\n",
                       name, grib_get_error_message(err1));
                save_error(c, name);
            }

            if ((err2 = grib_get_string(h2, name, sval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get string value of [%s] in 2nd field: %s\n",
                       name, grib_get_error_message(err2));
                save_error(c, name);
            }

            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS) {
                if (grib_inline_strcmp(sval1, sval2) != 0) {
                    printInfo(h1);
                    printf("string [%s]: [%s] != [%s]\n",
                           name, sval1, sval2);
                    err1 = GRIB_VALUE_MISMATCH;
                    save_error(c, name);
                }
            }

            grib_context_free(h1->context, sval1);
            grib_context_free(h2->context, sval2);

            if (err1)
                return err1;
            if (err2)
                return err2;

            break;

        case GRIB_TYPE_LONG:
            if (verbose)
                printf(" as long\n");

            lval1 = (long*)grib_context_malloc(h1->context, len1 * sizeof(long));
            lval2 = (long*)grib_context_malloc(h2->context, len2 * sizeof(long));

            if ((err1 = grib_get_long_array(h1, name, lval1, &len1)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get long value of [%s] in 1st field: %s\n",
                       name, grib_get_error_message(err1));
                save_error(c, name);
            }

            if ((err2 = grib_get_long_array(h2, name, lval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get long value of [%s] in 2nd field: %s\n",
                       name, grib_get_error_message(err2));
                save_error(c, name);
            }

            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1 != len2) {
                printInfo(h1);
                printf("Different size for \"%s\"  [%ld]  [%ld]\n", name, (long)len1, (long)len2);
                err1 = GRIB_INTERNAL_ERROR;
                save_error(c, name);
            }
            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1 == len2) {
                countdiff = 0;
                for (i = 0; i < len1; i++)
                    if (lval1[i] != lval2[i])
                        countdiff++;

                if (countdiff) {
                    printInfo(h1);
                    save_error(c, name);
                    err1 = GRIB_VALUE_MISMATCH;
                    if (len1 == 1)
                        printf("long [%s]: [%ld] != [%ld]\n",
                               name, *lval1, *lval2);
                    else
                        printf("long [%s] %d out of %ld different\n",
                               name, countdiff, (long)len1);
                }
            }


            grib_context_free(h1->context, lval1);
            grib_context_free(h2->context, lval2);

            if (err1)
                return err1;
            if (err2)
                return err2;
            break;

        case GRIB_TYPE_DOUBLE:
            if (verbose)
                printf(" as double\n");
            dval1 = (double*)grib_context_malloc(h1->context, len1 * sizeof(double));
            dval2 = (double*)grib_context_malloc(h2->context, len2 * sizeof(double));

            isangle         = 0;
            value_tolerance = global_tolerance;
            if (!grib_inline_strcmp(name, "packedValues") || !grib_inline_strcmp(name, "values") || !grib_inline_strcmp(name, "codedValues")) {
                packingError1 = 0;
                packingError2 = 0;
                err1          = grib_get_double(h1, "packingError", &packingError1);
                err2          = grib_get_double(h2, "packingError", &packingError2);
                if (packingCompare)
                    value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
            }
            else if (!grib_inline_strcmp(name, "unpackedValues")) {
                packingError1 = 0;
                packingError2 = 0;
                err1          = grib_get_double(h1, "unpackedError", &packingError1);
                err2          = grib_get_double(h2, "unpackedError", &packingError2);
                if (packingCompare)
                    value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
            }
            else if (!grib_inline_strcmp(name, "referenceValue")) {
                packingError1   = 0;
                packingError2   = 0;
                err1            = grib_get_double(h1, "referenceValueError", &packingError1);
                err2            = grib_get_double(h2, "referenceValueError", &packingError2);
                value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
            }

            if (!compareAbsolute) {
                for (i = 0; i < options->tolerance_count; i++) {
                    if (!strcmp((options->tolerance[i]).name, name)) {
                        value_tolerance = (options->tolerance[i]).double_value;
                        break;
                    }
                }
            }

            if ((err1 = grib_get_double_array(h1, name, dval1, &len1)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get double value of [%s] in 1st field: %s\n",
                       name, grib_get_error_message(err1));
                save_error(c, name);
            }

            if ((err2 = grib_get_double_array(h2, name, dval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get double value of [%s] in 2nd field: %s\n",
                       name, grib_get_error_message(err2));
                save_error(c, name);
            }

            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1 != len2) {
                printInfo(h1);
                printf("Different size for \"%s\"  [%ld]  [%ld]\n", name, (long)len1, (long)len2);
                save_error(c, name);
            }
            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1 == len2) {
                int imaxdiff;
                double diff;
                double *pv1, *pv2, dnew1, dnew2;
                maxdiff   = 0;
                imaxdiff  = 0;
                countdiff = 0;
                pv1       = dval1;
                pv2       = dval2;
                if (isangle) {
                    dnew1 = *dval1;
                    dnew2 = *dval2;
                    pv1   = &dnew1;
                    pv2   = &dnew2;
                    if (*dval1 < 0)
                        dnew1 += 360.0;
                    if (*dval2 < 0)
                        dnew2 += 360.0;
                    if (*dval1 > 360)
                        dnew1 -= 360.0;
                    if (*dval2 > 360)
                        dnew2 -= 360.0;
                }
                value_tolerance *= tolerance_factor;
                if (verbose)
                    printf("  (%d values) tolerance=%g\n", (int)len1, value_tolerance);
                for (i = 0; i < len1; i++) {
                    if ((diff = compare_double(pv1++, pv2++, &value_tolerance)) != 0) {
                        countdiff++;
                        if (maxdiff < diff) {
                            maxdiff  = diff;
                            imaxdiff = i;
                        }
                        err1 = GRIB_VALUE_MISMATCH;
                    }
                }

                if (countdiff) {
                    printInfo(h1);
                    save_error(c, name);
                    if (len1 > 1) {
                        printf("double [%s]: %d out of %ld different\n", name, countdiff, (long)len1);
                        if (compareAbsolute)
                            printf(" max");
                        printf(" absolute diff. = %.16e,", fabs(dval1[imaxdiff] - dval2[imaxdiff]));
                        if (!compareAbsolute)
                            printf(" max");
                        printf(" relative diff. = %g", relative_error(dval1[imaxdiff], dval2[imaxdiff], value_tolerance));
                        printf("\n\tmax diff. element %d: %.20e %.20e",
                               imaxdiff, dval1[imaxdiff], dval2[imaxdiff]);
                        printf("\n\ttolerance=%.16e", value_tolerance);
                        if (packingError2 != 0 || packingError1 != 0)
                            printf(" packingError: [%g] [%g]", packingError1, packingError2);

                        if (!grib_inline_strcmp(name, "packedValues") || !grib_inline_strcmp(name, "values") || !grib_inline_strcmp(name, "codedValues")) {
                            double max1, min1, max2, min2;
                            grib_get_double(h1, "max", &max1);
                            grib_get_double(h1, "min", &min1);
                            grib_get_double(h2, "max", &max2);
                            grib_get_double(h2, "min", &min2);
                            printf("\n\tvalues max= [%g]  [%g]         min= [%g] [%g]", max1, max2, min1, min2);
                        }
                        printf("\n");
                    }
                    else {
                        printf("double [%s]: [%.20e] != [%.20e]\n",
                               name, dval1[0], dval2[0]);
                        printf("\tabsolute diff. = %g,", fabs(dval1[0] - dval2[0]));
                        printf(" relative diff. = %g\n", relative_error(dval1[0], dval2[0], value_tolerance));
                        printf("\ttolerance=%g\n", value_tolerance);
                    }
                }
            }

            grib_context_free(h1->context, dval1);
            grib_context_free(h2->context, dval2);

            if (err1)
                return err1;
            if (err2)
                return err2;
            break;

        case GRIB_TYPE_BYTES:
            if (verbose)
                printf(" as bytes\n");
            if (options->mode == MODE_METAR)
                return 0;
            if (len1 < 2)
                len1 = 512;
            if (len2 < 2)
                len2 = 512;
            uval1 = (unsigned char*)grib_context_malloc(h1->context, len1 * sizeof(unsigned char));
            uval2 = (unsigned char*)grib_context_malloc(h2->context, len2 * sizeof(unsigned char));

            if ((err1 = grib_get_bytes(h1, name, uval1, &len1)) != GRIB_SUCCESS) {
                printInfo(h1);
                save_error(c, name);
                printf("Oops... cannot get bytes value of [%s] in 1st field: %s\n",
                       name, grib_get_error_message(err1));
            }

            if ((err2 = grib_get_bytes(h2, name, uval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                save_error(c, name);
                printf("Oops... cannot get bytes value of [%s] in 2nd field: %s\n",
                       name, grib_get_error_message(err2));
            }

            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS) {
                if (memcmp(uval1, uval2, len1) != 0) {
                    for (i = 0; i < len1; i++)
                        if (uval1[i] != uval2[i]) {
                            printInfo(h1);
                            save_error(c, name);
                            if (len1 == 1)
                                printf("[%s] byte values are different: [%02x] and [%02x]\n",
                                       name, uval1[i], uval2[i]);
                            else
                                printf("[%s] byte value %d of %ld are different: [%02x] and [%02x]\n",
                                       name, i, (long)len1, uval1[i], uval2[i]);

                            err1 = GRIB_VALUE_MISMATCH;
                            break;
                        }
                    err1 = GRIB_VALUE_MISMATCH;
                }
            }

            grib_context_free(h1->context, uval1);
            grib_context_free(h2->context, uval2);

            if (err1)
                return err1;
            if (err2)
                return err2;
            break;

        case GRIB_TYPE_LABEL:
            if (verbose)
                printf(" as label\n");
            break;

        default:
            if (verbose)
                printf("\n");
            printInfo(h1);
            save_error(c, name);
            printf("Cannot compare [%s], unsupported type %d\n", name, type1);
            return GRIB_UNABLE_TO_COMPARE_ACCESSORS;
            break;
    }

    return GRIB_SUCCESS;
}

static int compare_all_dump_keys(grib_handle* h1, grib_handle* h2, grib_runtime_options* options, int* err)
{
    int ret                  = 0;
    const char* name         = NULL;
    grib_keys_iterator* iter = grib_keys_iterator_new(h1, 0, NULL);

    if (!iter) {
        printf("ERROR: unable to get iterator\n");
        exit(1);
    }

    while (grib_keys_iterator_next(iter)) {
        grib_accessor* xa = grib_keys_iterator_get_accessor(iter);
        name              = grib_keys_iterator_get_name(iter);
        /* printf("----- comparing %s\n",name); */

        if (blocklisted(name))
            continue;
        if (xa == NULL || (xa->flags & GRIB_ACCESSOR_FLAG_DUMP) == 0)
            continue;
        if (compare_values(options, h1, h2, name, GRIB_TYPE_UNDEFINED)) {
            err++;
            write_messages(h1, h2);
            ret = 1;
        }
    }

    grib_keys_iterator_delete(iter);
    return ret;
}

static int compare_handles(grib_handle* h1, grib_handle* h2, grib_runtime_options* options)
{
    int err                  = 0;
    int i                    = 0;
    grib_keys_iterator* iter = NULL;
    const char* name         = NULL;

    /* mask only if no -c option or headerMode (-H)*/
    if (blocklist && (!listFromCommandLine || headerMode)) {
        grib_string_list* nextb = blocklist;
        while (nextb) {
            grib_clear(h1, nextb->value);
            grib_clear(h2, nextb->value);
            nextb = nextb->next;
        }
    }

    if (headerMode) {
        const void *msg1 = NULL, *msg2 = NULL;
        size_t size1 = 0, size2 = 0;
        grib_handle *h11, *h22;
        GRIB_CHECK_NOLINE(grib_get_message_headers(h1, &msg1, &size1), 0);
        GRIB_CHECK_NOLINE(grib_get_message_headers(h2, &msg2, &size2), 0);
        if (size1 == size2 && !memcmp(msg1, msg2, size1))
            return 0;

        err = 0;
        h11 = grib_handle_new_from_partial_message(h1->context, msg1, size1);
        h22 = grib_handle_new_from_partial_message(h1->context, msg2, size2);

        iter = grib_keys_iterator_new(h11,
                                      GRIB_KEYS_ITERATOR_SKIP_COMPUTED, NULL);

        if (!iter) {
            printf("ERROR: unable to get iterator\n");
            exit(1);
        }

        while (grib_keys_iterator_next(iter)) {
            name = grib_keys_iterator_get_name(iter);
            /*printf("----- comparing %s\n",name);*/

            if (blocklisted(name))
                continue;
            if (compare_values(options, h11, h22, name, GRIB_TYPE_UNDEFINED)) {
                err++;
                write_messages(h11, h22);
            }
        }

        grib_keys_iterator_delete(iter);
        grib_handle_delete(h11);
        grib_handle_delete(h22);
        return err;
    }

    if (listFromCommandLine && onlyListed) {
        for (i = 0; i < options->compare_count; i++) {
            if (blocklisted((char*)options->compare[i].name))
                continue;
            if (options->compare[i].type == GRIB_NAMESPACE) {
                iter = grib_keys_iterator_new(h1, 0, options->compare[i].name);
                if (!iter) {
                    printf("ERROR: unable to get iterator\n");
                    exit(1);
                }
                while (grib_keys_iterator_next(iter)) {
                    name = grib_keys_iterator_get_name(iter);
                    /*printf("----- comparing %s\n",name);*/

                    if (blocklisted(name))
                        continue;
                    if (compare_values(options, h1, h2, name, GRIB_TYPE_UNDEFINED)) {
                        err++;
                        write_messages(h1, h2);
                    }
                }
                grib_keys_iterator_delete(iter);
            }
            else {
                if (compare_values(options, h1, h2, options->compare[i].name, options->compare[i].type))
                    err++;
                write_messages(h1, h2);
            }
        }
    }
    else {
        const void *msg1 = NULL, *msg2 = NULL;
        size_t size1 = 0, size2 = 0;
        int memcmp_ret = 0;
        /* int ii=0; */
        GRIB_CHECK_NOLINE(grib_get_message(h1, &msg1, &size1), 0);
        GRIB_CHECK_NOLINE(grib_get_message(h2, &msg2, &size2), 0);
        if (size1 == size2 && !(memcmp_ret = memcmp(msg1, msg2, size1))) {
            return 0;
        }
#if 0
        else {
            int lcount=count,ii;
            if (options->current_infile) lcount=options->current_infile->filter_handle_count;
            if (size1 != size2) {
                printf("#%d different size: %d!=%d\n",lcount,(int)size1,(int)size2);
            }
            if (memcmp_ret) {
                unsigned char *m1=(unsigned char*)msg1;
                unsigned char *m2=(unsigned char*)msg2;
                printf("=== list of different bytes for message %d\n",lcount);
                for (ii=0;ii<size1;ii++) {
                    if (memcmp(m1,m2,1)) {
                        printf("  %d 0x%.2X != 0x%.2X\n",ii,*m1,*m2);
                    }
                    m1++; m2++;
                }
            }
            return err;
        }
#endif


        if (listFromCommandLine) {
            for (i = 0; i < options->compare_count; i++) {
                if (blocklisted(name))
                    continue;
                if (options->compare[i].type == GRIB_NAMESPACE) {
                    iter = grib_keys_iterator_new(h1, 0, options->compare[i].name);
                    if (!iter) {
                        printf("ERROR: unable to get iterator for %s\n", options->compare[i].name);
                        exit(1);
                    }
                    while (grib_keys_iterator_next(iter)) {
                        name = grib_keys_iterator_get_name(iter);
                        /*printf("----- comparing %s\n",name);*/

                        if (blocklisted(name))
                            continue;
                        if (compare_values(options, h1, h2, name, GRIB_TYPE_UNDEFINED)) {
                            err++;
                            write_messages(h1, h2);
                            if (compare_all_dump_keys(h1, h2, options, &err)) {
                                err++;
                            }
                        }
                    }
                    grib_keys_iterator_delete(iter);
                }
                else {
                    if (compare_values(options, h1, h2, options->compare[i].name, options->compare[i].type)) {
                        err++;
                        write_messages(h1, h2);
                        if (compare_all_dump_keys(h1, h2, options, &err)) {
                            err++;
                        }
                    }
                }
            }
        }
        else {
            if (compare_all_dump_keys(h1, h2, options, &err)) {
                err++;
            }
        }
    }
    return err;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file, "\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
