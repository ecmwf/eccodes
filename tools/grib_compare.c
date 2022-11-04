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
    { "e", 0, "Edition independent compare. It is used to compare GRIB edition 1 and 2.\n", 0, 1, 0 },
    { "2", 0, "Enable two-way comparison.\n", 0, 1, 0 },
    { "c:", 0, 0, 0, 1, 0 },
    { "S:", "start", "First field to be processed.\n", 0, 1, 0 },
    { "E:", "end", "Last field to be processed.\n", 0, 1, 0 },
    { "a", 0, "-c option modifier. The keys listed with the option -c will be added to the list of keys compared without -c.\n", 0, 1, 0 },
    { "H", 0, "Compare only message headers (everything except data and bitmap). Bit-by-bit compare on. Incompatible with -c option.\n", 0, 1, 0 },
    { "R:", 0, 0, 0, 1, 0 },
    { "A:", 0, 0, 0, 1, 0 },
    { "P", 0, "Compare data values using the packing error as tolerance.\n", 0, 1, 0 },
    { "T:", "factor", "Compare data values using factor multiplied by the tolerance specified in options -P -R -A.\n", 0, 1, 0 },
    { "w:", 0, 0, 0, 1, 0 },
    { "f", 0, 0, 0, 1, 0 },
    { "F", 0, 0, 1, 0, 0 },
    { "q", 0, 0, 1, 0, 0 },
    { "M", 0, 0, 1, 0, 0 },
    { "I", 0, 0, 1, 0, 0 },
    { "V", 0, 0, 0, 1, 0 },
    { "7", 0, 0, 0, 1, 0 },
    { "v", 0, 0, 0, 1, 0 }
};
int grib_options_count = sizeof(grib_options) / sizeof(grib_option);

const char* tool_description =
    "Compare GRIB messages contained in two files."
    "\n\tIf some differences are found it fails returning an error code."
    "\n\tFloating-point values are compared exactly by default, different tolerances can be defined (see -P -A -R)."
    "\n\tDefault behaviour: absolute error=0, bit-by-bit compare, same order in files.";

const char* tool_name  = "grib_compare";
const char* tool_usage = "[options] grib_file1 grib_file2";

typedef double (*compare_double_proc)(const double*, const double*, double);
static int compare_handles(grib_handle* h1, grib_handle* h2, grib_runtime_options* options);

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

static int error     = 0;
static int count     = 0;
static int lastPrint = 0;
static int force     = 0;

/* ECC-651: Boolean 'two_way' set to 1 when '-2' option used */
static int two_way = 0;
/* Boolean 'handles_swapped' relevant in 'two_way' mode:
 *  0 means: h1 is first file,  h2 is second file
 *  1 means: h1 is second file, h2 is first file
 */
static int handles_swapped = 0;

static double maxAbsoluteError = 1e-19;
static int onlyListed          = 1;
static int headerMode          = 0;
static int morein1             = 0;
static int morein2             = 0;
static int listFromCommandLine = 0;
static int editionIndependent  = 0;
static int verbose             = 0;
static double tolerance_factor = 1;

static grib_handle* handle1 = NULL;
static int global_counter   = 0;
static int theStart         = -1;
static int theEnd           = -1;

#define MINIMUM(x, y) ((x) < (y) ? (x) : (y))

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

GRIB_INLINE static int grib_inline_rstrcmp(const char* a, const char* b)
{
    const char* p = a;
    const char* q = b;
    while (*p != 0)
        p++;
    while (*q != 0)
        q++;
    q--;
    p--;
    if (*q != *p)
        return 1;
    while ((p != a && q != b) && *(p) == *(q)) {
        p--;
        q--;
    }
    return (q == b) ? 0 : 1;
}

/* Returns 0 when the values are considered the same */
static double compare_double_absolute(const double* a, const double* b, double tolerance)
{
    double ret = 0;
    double d   = fabs(*a - *b);
    if (d > tolerance) {
        ret = d;
    }
    return ret;
    /* return fabs(*a-*b) > *err ? fabs(*a-*b) : 0; */
}

/* Returns 0 when the values are considered the same */
static double compare_double_relative(const double* a, const double* b, double tolerance)
{
    double relativeError;

    if (fabs(*a) <= maxAbsoluteError || fabs(*b) <= maxAbsoluteError)
        relativeError = fabs(*a - *b);
    else if (fabs(*b) > fabs(*a))
        relativeError = fabs((*a - *b) / *b);
    else
        relativeError = fabs((*a - *b) / *a);

    return relativeError > tolerance ? relativeError : 0;
}

static int blocklisted(const char* name)
{
    grib_string_list* b = blocklist;
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
    int ret = 0;
    int nfiles            = 1;
    const char orderby[]  = "md5Headers";
    grib_context* context = grib_context_get_default();

    options->strict = 1;
    if (grib_options_on("S:"))
        theStart = atoi(grib_options_get_option("S:"));

    if (grib_options_on("E:"))
        theEnd = atoi(grib_options_get_option("E:"));

    if (grib_options_on("f"))
        force = 1;
    else
        force = 0;

    if (grib_options_on("2"))
        two_way = 1;
    else
        two_way = 0;

    verbose = grib_options_on("v");

    listFromCommandLine = 0;
    if (grib_options_on("c:") || grib_options_on("e"))
        listFromCommandLine = 1;

    if (grib_options_on("e"))
        editionIndependent = 1;

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
        int i;
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

    /* Check 1st file is not a directory */
    exit_if_input_is_directory(tool_name, options->infile_extra->name);

    if (grib_options_on("r")) {
        const char* filename[1];
        filename[0]      = options->infile_extra->name; /* First file */
        options->random  = 1;
        options->orderby = strdup(orderby);
        options->idx     = grib_fieldset_new_from_files(context, filename,
                                                    nfiles, 0, 0, 0, orderby, &ret);
        if (ret) {
            fprintf(stderr, "Unable to create index for input file %s (%s)",
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
        int i;
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
        const char* absTolStr = grib_options_get_option("A:");
        char* endPtr          = NULL; /* for error handling */
        if (grib_options_on("R:")) {
            maxAbsoluteError = strtod(absTolStr, &endPtr);
            if (*endPtr) {
                fprintf(stderr, "Invalid absolute error: '%s'\n", absTolStr);
                exit(1);
            }
        }
        else {
            compare_double   = &compare_double_absolute;
            global_tolerance = strtod(absTolStr, &endPtr);
            if (*endPtr) {
                fprintf(stderr, "Invalid absolute error: '%s'\n", absTolStr);
                exit(1);
            }
        }
    }
    if (grib_options_on("P")) {
        packingCompare = 1;
        /* GRIB-972: Do not change the comparator yet. Not all GRIBs have packingError key! */
        /*compare_double= &compare_double_absolute;*/
    }

    if (grib_options_on("T:"))
        tolerance_factor = atof(grib_options_get_option("T:"));

    if (grib_options_on("R:")) {
        char* sarg               = grib_options_get_option("R:");
        options->tolerance_count = MAX_KEYS;
        ret                      = parse_keyval_string(tool_name, sarg, 1, GRIB_TYPE_DOUBLE, options->tolerance, &(options->tolerance_count));
        if (ret == GRIB_INVALID_ARGUMENT) {
            usage();
            exit(1);
        }
    }

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
    char shortName[254] = {0,};
    char levelType[254] = {0,};
    char level[254] = {0,};
    char paramId[254] = {0,};
    char packingType[254] = {0,};
    char gridType[254] = {0,};
    char identifier[254] = {0,};
    size_t len          = 254;
    char stepRange[254] = {0,};
    if (lastPrint == count)
        return;

    len = 254;
    grib_get_string(h, "shortName", shortName, &len);
    len = 254;
    grib_get_string(h, "stepRange", stepRange, &len);
    len = 254;
    grib_get_string(h, "levelType", levelType, &len);
    len = 254;
    grib_get_string(h, "level", level, &len);
    len = 254;
    grib_get_string(h, "paramId", paramId, &len);
    len = 254;
    grib_get_string(h, "packingType", packingType, &len);
    len = 254;
    grib_get_string(h, "gridType", gridType, &len);
    len = 254;
    grib_get_string(h, "identifier", identifier, &len);

    printf("\n-- %s #%d -- shortName=%s paramId=%s stepRange=%s levelType=%s level=%s packingType=%s gridType=%s --\n",
           identifier, count, shortName, paramId, stepRange, levelType, level, packingType, gridType);
    lastPrint = count;
}

static void print_index_key_values(grib_index* index, int counter)
{
    grib_index_key* keys = index->keys;
    printf("== %d == ", counter);
    while (keys) {
        printf("%s=%s ", keys->name, keys->value);
        keys = keys->next;
    }
    printf("\n");
}

/* Note: the grib_handle 'handle2' here is from the 2nd file */
int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* handle2)
{
    int err = 0;
    count++;

    if (options->through_index) {
        grib_index* idx1 = options->index1;
        verbose          = 0;
        global_counter++;

        if (theStart > 0 && global_counter < theStart)
            return 0;
        if (theEnd > 0 && global_counter > theEnd) {
            options->stop = 1;
            return 0;
        }

        grib_index_search_same(idx1, handle2);
        handle1 = grib_handle_new_from_index(idx1, &err);
        if (options->verbose) {
            off_t offset   = 0;
            char* filename = grib_get_field_file(options->index2, &offset);
            printf("file1=\"%s\" ", filename);
            filename = grib_get_field_file(options->index1, &offset);
            printf("file2=\"%s\" \n", filename);
            print_index_key_values(options->index1, global_counter);
        }

        if (!handle1) {
            if (!options->verbose)
                print_index_key_values(idx1, global_counter);
            printf("====== NOT FOUND in %s\n", options->infile->name);
        }

        if (!handle1 || err != GRIB_SUCCESS) {
            morein1++;
            if (handle1)
                grib_handle_delete(handle1);
            return 0;
        }

        if (compare_handles(handle2, handle1, options)) {
            error++;
            if (!force) exit(1);
        }

        grib_handle_delete(handle1);

        return 0;
    }
    else if (options->random)
        handle1 = grib_fieldset_next_handle(options->idx, &err);
    else
        handle1 = grib_handle_new_from_file(handle2->context, options->infile_extra->file, &err);

    if (!handle1 || err != GRIB_SUCCESS) {
        morein2++;
        if (handle1)
            grib_handle_delete(handle1);
        return 0;
    }

    if (compare_handles(handle1, handle2, options)) {
        error++;
        if (!two_way) {
            /* If two_way mode: Don't exit yet. Show further differences */
            if (!force) exit(1);
        }
    }
    if (two_way) {
        /* ECC-651 and ECC-431 */
        handles_swapped = 1;
        if (compare_handles(handle2, handle1, options)) {
            error++;
            if (!force) exit(1);
        }
        else {
            if (error) {
                /* Error from first pass */
                if (!force) exit(1);
            }
        }
    }

    grib_handle_delete(handle1);

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    int err = 0;
    if (!options->through_index && !options->random) {
        handle1 = grib_handle_new_from_file(h->context, options->infile_extra->file, &err);

        if (!handle1 || err != GRIB_SUCCESS)
            morein2++;

        grib_handle_delete(handle1);
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

    /*if (grib_options_on("w:")) return 0;*/

    while ((handle1 = grib_handle_new_from_file(c, options->infile_extra->file, &err))) {
        morein1++;
        grib_handle_delete(handle1);
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

    if (error != 0) exit(1);
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

static int test_bit(long a, long b)
{
    return a & (1 << b);
}

/* If the accessor represents a codeflag key, then return its binary rep in 'result' */
static int codeflag_to_bitstr(grib_accessor* a, long val, char* result)
{
    if (a && grib_inline_strcmp(a->cclass->name, "codeflag") == 0) {
        long i;
        const long bytelen = a->length * 8;
        for (i = 0; i < bytelen; i++) {
            if (test_bit(val, bytelen - i - 1))
                *result = '1';
            else
                *result = '0';
            result++;
        }
        return GRIB_SUCCESS;
    }
    return GRIB_INVALID_TYPE;
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
    int isangle = 0;
    int isMissing1, isMissing2;

    char *sval1 = NULL, *sval2 = NULL;
    unsigned char *uval1 = NULL, *uval2 = NULL;
    double *dval1 = NULL, *dval2 = NULL;
    long *lval1 = NULL, *lval2 = NULL;
    double maxdiff       = 0;
    double packingError1 = 0, packingError2 = 0;
    double value_tolerance = 0;
    grib_context* c        = h1->context;
    const char* first_str  = (handles_swapped == 0 ? "1st" : "2nd");
    const char* second_str = (handles_swapped == 0 ? "2nd" : "1st");

    type1 = type;
    type2 = type;
    if (verbose)
        printf("  comparing %s", name);

    /* If key was blocklisted, then we should not have got here */
    DebugAssert(!blocklisted(name));

    if (type1 == GRIB_TYPE_UNDEFINED && (err = grib_get_native_type(h1, name, &type1)) != GRIB_SUCCESS) {
        printInfo(h1);
        printf("Oops... cannot get type of [%s] in %s field: %s\n", name, first_str, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    if (type2 == GRIB_TYPE_UNDEFINED && (err = grib_get_native_type(h2, name, &type2)) != GRIB_SUCCESS) {
        if (err == GRIB_NOT_FOUND) {
            printInfo(h1);
            printf("[%s] not found in %s field\n", name, second_str);
            save_error(c, name);
            return err;
        }
        printInfo(h1);
        printf("Oops... cannot get type of [%s] in %s field: %s\n", name, second_str, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    /*  if(type1 != type2) {
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
        printf("Oops... cannot get size of [%s] in %s field: %s\n", name, first_str, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    if ((err = grib_get_size(h2, name, &len2)) != GRIB_SUCCESS) {
        if (err == GRIB_NOT_FOUND) {
            printInfo(h1);
            printf("[%s] not found in %s field\n", name, second_str);
            save_error(c, name);
            return err;
        }

        printInfo(h1);
        printf("Oops... cannot get size of [%s] in %s field: %s\n", name, second_str, grib_get_error_message(err));
        save_error(c, name);
        return err;
    }

    if (handles_swapped) {
        /* Comparing a second time with handles swapped. Do not compare keys common to both handles */
        return GRIB_SUCCESS;
    }

    /*  if(len1 != len2 && type1 != GRIB_TYPE_STRING)  {
        printInfo(h1);
        printf("[%s] has different size: 1st field: %ld, 2nd field: %ld\n",name,(long)len1,(long)len2);
        save_error(c,name);
        return GRIB_COUNT_MISMATCH;
    }
     */

    isMissing1 = ((grib_is_missing(h1, name, &err1) == 1) && (err1 == 0)) ? 1 : 0;
    isMissing2 = ((grib_is_missing(h2, name, &err2) == 1) && (err2 == 0)) ? 1 : 0;

    if ((isMissing1 == 1) && (isMissing2 == 1)) {
        if (verbose)
            printf(" is set to missing in both fields\n");
        return GRIB_SUCCESS;
    }

    if (isMissing1 == 1) {
        if (verbose)
            printf(" is set to missing in %s field\n", first_str);
        printInfo(h1);
        printf("%s is set to missing in %s field is not missing in %s field\n", name, first_str, second_str);
        err1 = GRIB_VALUE_MISMATCH;
        save_error(c, name);
        return GRIB_VALUE_MISMATCH;
    }

    if (isMissing2 == 1) {
        if (verbose)
            printf(" is set to missing in %s field\n", first_str);
        printInfo(h1);
        printf("%s is set to missing in %s field is not missing in %s field\n", name, second_str, first_str);
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
                printf("Oops... cannot get string value of [%s] in %s field: %s\n",
                       name, first_str, grib_get_error_message(err1));
                save_error(c, name);
            }

            if ((err2 = grib_get_string(h2, name, sval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get string value of [%s] in %s field: %s\n",
                       name, second_str, grib_get_error_message(err2));
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
                else {
                    /* ECC-136: string reps are the same, but integer values may not be */
                    /* Note: Do not do this during edition-independent compare! */
                    if (!listFromCommandLine) {
                        long v1, v2;
                        if (grib_get_long(h1, name, &v1) == GRIB_SUCCESS &&
                            grib_get_long(h2, name, &v2) == GRIB_SUCCESS) {
                            if (v1 != v2) {
                                printInfo(h1);
                                save_error(c, name);
                                err1 = GRIB_VALUE_MISMATCH;
                                printf("long [%s]: [%ld] != [%ld]\n", name, v1, v2);
                            }
                        }
                    }
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
                printf("Oops... cannot get long value of [%s] in %s field: %s\n",
                       name, first_str, grib_get_error_message(err1));
                save_error(c, name);
            }

            if ((err2 = grib_get_long_array(h2, name, lval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get long value of [%s] in %s field: %s\n",
                       name, second_str, grib_get_error_message(err2));
                save_error(c, name);
            }

            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1 != len2) {
                printInfo(h1);
                printf("Different size for \"%s\"  [%ld]  [%ld]\n", name, (long)len1, (long)len2);
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
                    if (len1 == 1) {
                        char buf1[128] = {0,}; /* buffers to store the binary representation of codeflags */
                        char buf2[128] = {0,};
                        grib_accessor* acc1 = grib_find_accessor(h1, name);
                        grib_accessor* acc2 = grib_find_accessor(h2, name);
                        printf("long [%s]: [%ld] != [%ld]", name, *lval1, *lval2);
                        if (codeflag_to_bitstr(acc1, *lval1, buf1) == GRIB_SUCCESS && codeflag_to_bitstr(acc2, *lval2, buf2) == GRIB_SUCCESS) {
                            printf("    ([%s] != [%s])", buf1, buf2);
                        }
                        printf("\n");
                    }
                    else {
                        printf("long [%s] %d out of %ld different\n", name, countdiff, (long)len1);
                    }
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
                if (packingCompare && !err1 && !err2) {
                    /* packingError specified by user and message supports it */
                    /* GRIB-972: Not all GRIBs have packingError key! */
                    value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
                    compare_double  = &compare_double_absolute;
                    compareAbsolute = 1;
                }
            }
            else if (!grib_inline_strcmp(name, "unpackedValues")) {
                packingError1 = 0;
                packingError2 = 0;
                err1          = grib_get_double(h1, "unpackedError", &packingError1);
                err2          = grib_get_double(h2, "unpackedError", &packingError2);
                if (packingCompare && !err1 && !err2) {
                    /* packingError specified by user and message supports it */
                    /* GRIB-972: Not all GRIBs have unpackedError key! */
                    value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
                    compare_double  = &compare_double_absolute;
                    compareAbsolute = 1;
                }
            }
            else if (!grib_inline_rstrcmp(name, "InDegrees")) {
                packingError1   = 0.0005;
                packingError2   = 0.0005;
                isangle         = 1;
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
                int all_specified = 0; /* =1 if relative comparison with "all" specified */
                for (i = 0; i < options->tolerance_count; i++) {
                    if (!strcmp((options->tolerance[i]).name, "all")) {
                        all_specified   = 1;
                        value_tolerance = (options->tolerance[i]).double_value;
                        break;
                    }
                }
                if (!all_specified) {
                    for (i = 0; i < options->tolerance_count; i++) {
                        if (!strcmp((options->tolerance[i]).name, name)) {
                            value_tolerance = (options->tolerance[i]).double_value;
                            break;
                        }
                    }
                }
            }

            if ((err1 = grib_get_double_array(h1, name, dval1, &len1)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get double value of [%s] in %s field: %s\n",
                       name, first_str, grib_get_error_message(err1));
                save_error(c, name);
            }

            if ((err2 = grib_get_double_array(h2, name, dval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                printf("Oops... cannot get double value of [%s] in %s field: %s\n",
                       name, second_str, grib_get_error_message(err2));
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
                if (verbose) {
                    printf("  (%d values) tolerance=%g \t", (int)len1, value_tolerance);
                    if (compare_double == &compare_double_absolute)
                        printf("using compare_double_absolute");
                    if (compare_double == &compare_double_relative)
                        printf("using compare_double_relative");
                    printf("\n");
                }
                for (i = 0; i < len1; i++) {
                    if ((diff = compare_double(pv1++, pv2++, value_tolerance)) != 0) {
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
                            double max1 = 0, min1 = 0, max2 = 0, min2 = 0;
                            /* Note: some packings like spectral do not have min,max */
                            if (grib_get_double(h1, "max", &max1) == GRIB_SUCCESS &&
                                grib_get_double(h1, "min", &min1) == GRIB_SUCCESS &&
                                grib_get_double(h2, "max", &max2) == GRIB_SUCCESS &&
                                grib_get_double(h2, "min", &min2) == GRIB_SUCCESS) {
                                printf("\n\tvalues max= [%g]  [%g]         min= [%g] [%g]", max1, max2, min1, min2);
                            }
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
            if (len1 < 2)
                len1 = 512;
            if (len2 < 2)
                len2 = 512;
            uval1 = (unsigned char*)grib_context_malloc(h1->context, len1 * sizeof(unsigned char));
            uval2 = (unsigned char*)grib_context_malloc(h2->context, len2 * sizeof(unsigned char));

            if ((err1 = grib_get_bytes(h1, name, uval1, &len1)) != GRIB_SUCCESS) {
                printInfo(h1);
                save_error(c, name);
                printf("Oops... cannot get bytes value of [%s] in %s field: %s\n",
                       name, first_str, grib_get_error_message(err1));
            }

            if ((err2 = grib_get_bytes(h2, name, uval2, &len2)) != GRIB_SUCCESS) {
                printInfo(h1);
                save_error(c, name);
                printf("Oops... cannot get bytes value of [%s] in %s field: %s\n",
                       name, second_str, grib_get_error_message(err2));
            }

            if (err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS) {
                const size_t len_min = MINIMUM(len1, len2);
                if (memcmp(uval1, uval2, len_min) != 0) {
                    for (i = 0; i < len_min; i++)
                        if (uval1[i] != uval2[i]) {
                            printInfo(h1);
                            save_error(c, name);
                            if (len_min == 1)
                                printf("[%s] byte values are different: [%02x] and [%02x]\n",
                                       name, uval1[i], uval2[i]);
                            else
                                printf("[%s] byte value %d of %ld is different: [%02x] and [%02x]\n",
                                       name, i, (long)len_min, uval1[i], uval2[i]);

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
    }

    return GRIB_SUCCESS;
}

static int compare_handles(grib_handle* h1, grib_handle* h2, grib_runtime_options* options)
{
    int err                  = 0;
    int i                    = 0;
    const char* name         = NULL;
    grib_keys_iterator* iter = NULL;

    /* mask only if no -c option or headerMode (-H)*/
    if (blocklist && (!listFromCommandLine || headerMode)) {
        /* See ECC-245, GRIB-573, GRIB-915: Do not change handles in memory */
        /* grib_string_list* nextb=blocklist;
        while (nextb) {
            grib_clear(h1,nextb->value);
            grib_clear(h2,nextb->value);
            nextb=nextb->next;
        } */
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

        iter = grib_keys_iterator_new(h11, GRIB_KEYS_ITERATOR_SKIP_COMPUTED, NULL);

        if (!iter) {
            printf("ERROR: unable to get keys iterator\n");
            exit(1);
        }

        while (grib_keys_iterator_next(iter)) {
            name = grib_keys_iterator_get_name(iter);
            /*printf("----- comparing %s\n",name);*/

            if (blocklisted(name))
                continue;
            if (compare_values(options, h11, h22, name, GRIB_TYPE_UNDEFINED))
                err++;
        }

        grib_keys_iterator_delete(iter);
        grib_handle_delete(h11);
        grib_handle_delete(h22);
        return err;
    }

    if (listFromCommandLine && onlyListed) {
        for (i = 0; i < options->compare_count; i++) {
            if (blocklisted(options->compare[i].name))
                continue;
            if (options->compare[i].type == GRIB_NAMESPACE) {
                int num_keys_in_namespace = 0;
                iter = grib_keys_iterator_new(h1, 0, options->compare[i].name);
                if (!iter) {
                    printf("ERROR: unable to get keys iterator for namespace \"%s\".\n", options->compare[i].name);
                    exit(1);
                }
                while (grib_keys_iterator_next(iter)) {
                    name = grib_keys_iterator_get_name(iter);
                    num_keys_in_namespace++;

                    if (blocklisted(name))
                        continue;
                    if (compare_values(options, h1, h2, name, GRIB_TYPE_UNDEFINED))
                        err++;
                }
                grib_keys_iterator_delete(iter);
                if (num_keys_in_namespace == 0 && !editionIndependent) {
                    printf("ERROR: namespace \"%s\" does not contain any key.\n", options->compare[i].name);
                }
            }
            else {
                if (compare_values(options, h1, h2, options->compare[i].name, options->compare[i].type))
                    err++;
            }
        }
    }
    else {
        const void *msg1 = NULL, *msg2 = NULL;
        size_t size1 = 0, size2 = 0;
        GRIB_CHECK_NOLINE(grib_get_message(h1, &msg1, &size1), 0);
        GRIB_CHECK_NOLINE(grib_get_message(h2, &msg2, &size2), 0);
        if (size1 == size2 && !memcmp(msg1, msg2, size1))
            return 0;

        iter = grib_keys_iterator_new(h1, GRIB_KEYS_ITERATOR_SKIP_COMPUTED, NULL);

        if (!iter) {
            printf("ERROR: unable to get keys iterator\n");
            exit(1);
        }

        while (grib_keys_iterator_next(iter)) {
            name = grib_keys_iterator_get_name(iter);
            /*printf("----- comparing %s\n",name);*/

            if (blocklisted(name))
                continue;
            if (compare_values(options, h1, h2, name, GRIB_TYPE_UNDEFINED))
                err++;
        }

        grib_keys_iterator_delete(iter);

        if (listFromCommandLine) {
            for (i = 0; i < options->compare_count; i++) {
                if (blocklisted(options->compare[i].name))
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
                        if (compare_values(options, h1, h2, name, GRIB_TYPE_UNDEFINED))
                            err++;
                    }
                    grib_keys_iterator_delete(iter);
                }
                else {
                    if (compare_values(options, h1, h2, options->compare[i].name, options->compare[i].type))
                        err++;
                }
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
