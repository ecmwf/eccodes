/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_tools.h"

GRIB_INLINE static int grib_inline_strcmp(const char* a,const char* b)
{
    if (*a != *b) return 1;
    while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
    return (*a==0 && *b==0) ? 0 : 1;
}

GRIB_INLINE static int grib_inline_rstrcmp(const char* a,const char* b)
{
    char* p=(char*)a;
    char* q=(char*)b;
    while (*p != 0) p++;
    while (*q != 0) q++;
    q--;p--;
    if (*q != *p) return 1;
    while((p!=a && q!=b) &&  *(p) == *(q) ) {p--;q--;}
    return (q==b) ? 0 : 1;
}

typedef double (*compare_double_proc) (double*,double*,double*);

typedef struct grib_error grib_error;
struct grib_error {
    char* key;
    int count;
    grib_error* next;
};

grib_error* error_summary;

compare_double_proc compare_double;
double global_tolerance=0;
int packingCompare=0;
grib_string_list* blacklist=0;
grib_string_list* keys_list = NULL; /* Used to determine rank of key */
int isLeafKey = 0; /* 0 if key is top-level, 1 if key has no children attributes */
int compareAbsolute=1;

static int compare_handles(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options);
static int compare_values(grib_runtime_options* options, grib_handle* handle1, grib_handle *handle2, const char *name, int type);
static int compare_attributes(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options,
        grib_accessor* a, const char* prefix, int* err);
static int compare_attribute(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options,
        grib_accessor* a, const char* prefix, int* err);

int error=0;
int count=0;
int lastPrint=0;
int force=0;

/* ECC-651: Boolean 'two_way' set to 1 when '-2' option used */
int two_way=0;
/* Boolean 'handles_swapped' relevant in 'two_way' mode:
 *  0 means: h1 is first file,  h2 is second file
 *  1 means: h1 is second file, h2 is first file
 */
int handles_swapped=0;

double maxAbsoluteError = 1e-19;
int onlyListed=1;
int headerMode=0;
int morein1=0;
int morein2=0;
int listFromCommandLine;
int verbose=0;
int tolerance_factor=1;
static int write_error=0;

/* Create the list of keys (global variable keys_list) */
static void new_keys_list()
{
    grib_context* c = grib_context_get_default();
    keys_list=(grib_string_list*)grib_context_malloc_clear(c, sizeof(grib_string_list));
    if (!keys_list) {
        fprintf(stderr, "Failed to allocate memory for keys list");
        exit(1);
    }
}

/* Free the keys list (global variable keys_list) */
static void release_keys_list()
{
    grib_string_list* next=keys_list;
    grib_string_list* cur=keys_list;
    grib_context* c = grib_context_get_default();
    while(next) {
        cur=next;
        next=next->next;
        grib_context_free(c,cur->value);
        grib_context_free(c,cur);
    }
}

GRIB_INLINE static double compare_double_absolute(double *a,double *b,double *err)
{
    double ret=0;
    double d=fabs(*a-*b);
    if (d > *err) {
        ret=d;
    }
    return ret;
    /* return fabs(*a-*b) > *err ? fabs(*a-*b) : 0; */
}

static int write_count=0;

static void write_message(grib_handle* h,const char* str)
{
    const void *m; size_t s;
    char fname[1024]={0,};
    FILE* fh=NULL;

    grib_get_message(h,&m,&s);
    sprintf(fname,"%s_%d.bufr",str,write_count);

    fh= fopen(fname,"w");
    if(!fh) {
        grib_context_log(h->context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),
                "Error opening %s",fname);
        exit(GRIB_IO_PROBLEM);
    }

    if(fwrite(m,1,s,fh) != s) {
        grib_context_log(h->context,(GRIB_LOG_ERROR)|(GRIB_LOG_PERROR),
                "Error writing to %s",fname);
        exit(GRIB_IO_PROBLEM);
    }

    fclose(fh);
}

static void write_messages(grib_handle* handle1, grib_handle* handle2)
{
    if (!write_error) return;
    write_count++;

    write_message(handle1,"error1");
    write_message(handle2,"error2");
}

static double compare_double_relative(double *a,double *b,double *err)
{
    double relativeError;

    if(fabs(*a) <= maxAbsoluteError || fabs(*b) <= maxAbsoluteError)
        relativeError = fabs(*a-*b);
    else if (fabs(*b) > fabs(*a))
        relativeError = fabs((*a-*b) / *b);
    else
        relativeError = fabs((*a-*b) / *a);

    return relativeError > *err ? relativeError : 0;
}

static int blacklisted(const char* name)
{
    grib_string_list* b=blacklist;
    while (b) {
        Assert(b->value);
        if (!strcmp(name,b->value))
            return 1;
        b=b->next;
    }
    return 0;
}

static double relative_error(double a,double b,double err)
{
    double relativeError;

    if(fabs(a) <= maxAbsoluteError || fabs(b) <= maxAbsoluteError)
        relativeError = fabs(a-b);
    else if (fabs(b) > fabs(a))
        relativeError = fabs((a-b) / b);
    else
        relativeError = fabs((a-b) / a);

    return relativeError ;
}

grib_option grib_options[]={
    /*  {id, args, help}, on, command_line, value*/
    /*{"r",0,"Compare files in which the messages are not in the same order. This option is time expensive.\n",0,1,0},*/
    {"b:",0,0,0,1,0},
    {"d",0,"Write different messages on files.\n",0,1,0},
    {"2",0,"Enable two-way comparison.\n",0,1,0},
    {"T:",0,0,1,0,"B"},
    {"c:",0,0,0,1,0},
    {"S:","start","First field to be processed.\n",0,1,0},
    {"E:","end","Last field to be processed.\n",0,1,0},
    {"a",0,"-c option modifier. The keys listed with the option -c will be added to the list of keys compared without -c.\n"
                ,0,1,0},
    {"H",0,"Compare only message headers. Bit-by-bit compare on. Incompatible with -c option.\n",0,1,0},
    {"R:",0,0,0,1,0},
    {"A:",0,0,0,1,0},
/*    {"P",0,"Compare data values using the packing error as tolerance.\n",0,1,0},*/
    {"t:","factor","Compare data values using factor multiplied by the tolerance specified in options -P -R -A.\n",0,1,0},
    {"w:",0,0,0,1,0},
    {"f",0,0,0,1,0},
    {"F",0,0,1,0,0},
    {"q",0,0,1,0,0},
    {"M",0,0,1,0,0},
    {"I",0,0,1,0,0},
    {"V",0,0,0,1,0},
    {"7",0,0,0,1,0},
    {"v",0,0,0,1,0}
};

grib_handle* global_handle=NULL;
int counter=0;
int start=-1;
int end=-1;

char* grib_tool_description=
    "Compare BUFR messages contained in two files."
    "\n\tIf some differences are found it fails returning an error code."
    "\n\tFloating point values are compared exactly by default, different tolerance can be defined see -P -A -R."
    "\n\tDefault behaviour: absolute error=0, bit-by-bit compare, same order in files.";

char* grib_tool_name="bufr_compare";
char* grib_tool_usage="[options] bufr_file1 bufr_file2";

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);

int main(int argc, char *argv[])
{
    return grib_tool(argc,argv);
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    int ret=0,i;
    int nfiles=1;
    char orderby[]="md5Headers";
    grib_context* context=grib_context_get_default();

    options->strict=1;
    if (grib_options_on("S:"))
        start=atoi(grib_options_get_option("S:"));

    if (grib_options_on("E:"))
        end=atoi(grib_options_get_option("E:"));

    if (grib_options_on("f")) force=1;
    else force=0;

    if (grib_options_on("2")) two_way=1;
    else two_way=0;

    if (grib_options_on("d")) write_error=1;
    else write_error=0;

    verbose = grib_options_on("v");

    listFromCommandLine=0;
    if (grib_options_on("c:") || grib_options_on("e"))
        listFromCommandLine=1;

    if (grib_options_on("a")) onlyListed=0;
    else onlyListed=1;

    if (grib_options_on("H")) headerMode=1;
    else headerMode=0;

    if (grib_options_on("H") && grib_options_on("c:")) {
        fprintf(stderr,"Error: -H and -c options are incompatible. Choose one of the two please.\n");
        exit(1);
    }
    if (grib_options_on("a") && !grib_options_on("c:")) {
        fprintf(stderr,"Error: -a option requires -c option. Please define a list of keys with the -c option.\n");
        exit(1);
    }

    if (grib_options_on("b:")) {
        grib_string_list *next=0;
        int ii=0;
        blacklist=(grib_string_list*)grib_context_malloc_clear(context,sizeof(grib_string_list));
        blacklist->value=grib_context_strdup(context,options->set_values[0].name);
        next=blacklist;
        for (ii=1;ii<options->set_values_count;ii++) {
            next->next=(grib_string_list*)grib_context_malloc_clear(context,sizeof(grib_string_list));
            next->next->value=grib_context_strdup(context,options->set_values[ii].name);
            next=next->next;
        }
        context->blacklist=blacklist;
    }

    if (grib_options_on("r")) {
        char* filename[1];
        filename[0]=options->infile_extra->name;
        options->random=1;
        options->orderby=strdup(orderby);
        options->idx=grib_fieldset_new_from_files(context,filename,
                nfiles,0,0,0,orderby,&ret);
        if (ret) {
            fprintf(stderr,"unable to create index for input file %s (%s)",
                    options->infile_extra->name, grib_get_error_message(ret));
            exit(ret);
        }
    } else {
        options->random=0;
        options->infile_extra->file=fopen(options->infile_extra->name,"r");

        if (!options->infile_extra->file) {
            perror(options->infile_extra->name);
            exit(1);
        }
    }

    global_tolerance=0;
    compare_double= &compare_double_absolute;
    if (grib_options_on("R:")) {
        global_tolerance=0;
        for (i=0;i<options->tolerance_count;i++) {
            if (!strcmp((options->tolerance[i]).name,"all")) {
                global_tolerance=(options->tolerance[i]).double_value;
                break;
            }
            if (!strcmp((options->tolerance[i]).name,"global")) {
                global_tolerance=(options->tolerance[i]).double_value;
                break;
            }
        }
        compare_double= &compare_double_relative;
        compareAbsolute=0;
    }
    if (grib_options_on("A:")){
        if (grib_options_on("R:")) {
            maxAbsoluteError = atof(grib_options_get_option("A:"));
        } else {
            compare_double= &compare_double_absolute;
            global_tolerance = atof(grib_options_get_option("A:"));
        }
    }
    if (grib_options_on("P")) {
        packingCompare=1;
        compare_double= &compare_double_absolute;
    }

    if (grib_options_on("t:"))
        tolerance_factor=atof(grib_options_get_option("t:"));

    if (grib_options_on("R:")) {
        char* sarg=grib_options_get_option("R:");
        options->tolerance_count=MAX_KEYS;
        ret=parse_keyval_string(grib_tool_name, sarg,1,GRIB_TYPE_DOUBLE,options->tolerance,&(options->tolerance_count));
        if (ret == GRIB_INVALID_ARGUMENT) {
            usage();
            exit(1);
        }
    }

    {
        /* Check for 2nd file being a directory. If so, we assume user is comparing to a file */
        /* with the same name as first file in that directory */
        struct stat s;
        grib_tools_file* infile = options->infile; /* the 2nd file in comparison */
        if (infile) {
            int stat_val = stat(infile->name, &s);
            if ( stat_val == 0 && S_ISDIR(s.st_mode)) {
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

    /* Turn off GRIB multi-field support mode. Not relevant for BUFR */
    grib_multi_support_off(grib_context_get_default());

    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options,const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file)
{
    return 0;
}

static void printInfo(grib_handle* h)
{
    printf("== %d == DIFFERENCE == ",count);
    lastPrint=count;
}

static void print_index_key_values(grib_index* index,int icounter,const char* error_message)
{
    grib_index_key* keys=index->keys;
    printf("== %d == ",icounter);
    if (error_message) printf("%s == ",error_message);
    while (keys) {
        printf("%s=%s ",keys->name,keys->value);
        keys=keys->next;
    }
    printf("\n");
}

static grib_handle* grib_handle_new_from_file_x(
        grib_context* c,FILE* f,int mode,int headers_only,int *err)
{
    return codes_handle_new_from_file(c,f,PRODUCT_BUFR,err);
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int err=0;
    count++;

    if (options->through_index) {
        grib_index* idx1=options->index1;
        verbose=0;
        counter++;

        if ( start>0 && counter < start ) return 0;
        if ( end>0 && counter > end ) {
            options->stop=1;
            return 0;
        }

        grib_index_search_same(idx1,h);
        global_handle=codes_new_from_index(idx1,CODES_BUFR,&err);
        if (options->verbose) {
            off_t offset=0;
            char* filename=grib_get_field_file(options->index2,&offset);
            printf("file1=\"%s\" ",filename);
            filename=grib_get_field_file(options->index1,&offset);
            printf("file2=\"%s\" \n",filename);
            print_index_key_values(options->index1,counter,NULL);
        }

        if (!global_handle) {
            if (!options->verbose)
                print_index_key_values(idx1,counter,"NOT FOUND ");
        }

        if (!global_handle || err!= GRIB_SUCCESS ) {
            morein1++;
            if (global_handle) grib_handle_delete(global_handle);
            return 0;
        }

        if(compare_handles(h,global_handle,options)) {
            error++;
            if (!force) exit(1);
        }

        grib_handle_delete(global_handle);

        return 0;

    } else if (options->random)
        global_handle = grib_fieldset_next_handle(options->idx,&err);
    else
        global_handle=grib_handle_new_from_file_x(h->context,options->infile_extra->file,options->mode,0,&err);

    if (!global_handle || err!= GRIB_SUCCESS ) {
        morein2++;
        if (global_handle) grib_handle_delete(global_handle);
        return 0;
    }

    if(compare_handles(global_handle,h,options)) {
        error++;
        if (!two_way) {
            /* If two_way mode: Don't exit yet. Show further differences */
            if (!force) exit(1);
        }
    }
    if (two_way) {
        /* ECC-651 and ECC-431 */
        handles_swapped = 1;
        if (verbose) printf("  Swapping handles (two-way mode)\n");
        if(compare_handles(h, global_handle, options)) {
            error++;
            if (!force) exit(1);
        } else {
            if (error) {
                /* Error from first pass */
                if (!force) exit(1);
            }
        }
    }

    grib_handle_delete(global_handle);

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    int err=0;
    if (!options->through_index && !options->random)  {
        global_handle=grib_handle_new_from_file(h->context,options->infile_extra->file,&err);

        if (!global_handle || err!= GRIB_SUCCESS)
            morein2++;

        grib_handle_delete(global_handle);
    }

    grib_handle_delete(h);
    count++;

    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h)
{
    grib_print_key_values(options,h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    grib_error* e = error_summary;
    int err=0;
    grib_context* c=grib_context_get_default();
    error += morein1+morein2;

    /*if (grib_options_on("w:")) return 0;*/

    if (error) {
        printf("\n## ERRORS SUMMARY #######\n");
    }
    while ((global_handle=grib_handle_new_from_file(c,options->infile_extra->file,&err))) {
        morein1++;
        if (global_handle) grib_handle_delete(global_handle);
    }
    if (morein1>0) {
        printf("##\n## Different number of messages \n");
        printf("## %d more messages in %s than in %s\n",morein1,
                options->infile_extra->name,options->infile->name);
    }

    if (morein2>0) {
        printf("##\n## Different number of messages \n");
        printf("## %d more messages in %s than in %s\n",morein2,
                options->infile->name,options->infile_extra->name);
    }

    if (error) {
        printf("##\n## Summary of different key values \n");
        while (e) {
            printf ("## %s ( %d different )\n",e->key,e->count);
            e=e->next;
        }

        printf("##\n## %d different messages out of %d\n\n",error,count);
    }
    if (options->through_index) {
        grib_index_delete(options->index1);
        grib_index_delete(options->index2);
    }
    release_keys_list();
    if (error != 0) exit(1);
    return 0;
}

static void save_error(grib_context* c,const char* key)
{
    grib_error* e=0;
    grib_error* next=0;
    int saved=0;

    if (!error_summary) {
        error_summary=(grib_error*)grib_context_malloc_clear(c,sizeof(grib_error));
        error_summary->count=1;
        error_summary->key=grib_context_strdup(c,key);
        return;
    }

    e=error_summary;
    next=e;

    while (next) {
        if (!strcmp(next->key,key)) {
            next->count++;
            saved=1;
            break;
        }
        e=next;
        next=next->next;
    }

    if (!saved) {
        e->next=(grib_error*)grib_context_malloc_clear(c,sizeof(grib_error));
        e->next->count=1;
        e->next->key=grib_context_strdup(c,key);
    }
}

static char* double_as_string(grib_context* c, double v)
{
    char* sval=(char*)grib_context_malloc_clear(c, sizeof(char)*40);
    if (v == GRIB_MISSING_DOUBLE) sprintf(sval,"MISSING");
    else                          sprintf(sval,"%.20e",v);
    return sval;
}

/* Return the part of the key name without the '#rank#' part */
static char* get_keyname_without_rank(const char* name)
{
    char* p=(char*)name;
    char* end=p;
    char* ret=NULL;

    if (*p=='#') {
        strtol(++p,&end,10);
        if ( *end != '#') {
            DebugAssert(!"Badly formed rank in key");
        } else {
            /* Take everything after 2nd '#' */
            grib_context* c=grib_context_get_default();
            end++;
            ret=grib_context_strdup(c,end);
        }
    }
    return ret;
}

static int compare_values(grib_runtime_options* options, grib_handle* handle1, grib_handle *handle2, const char *name, int type)
{
    size_t len1 = 0;
    size_t len2 = 0;
    int err=0,i=0;
    int err1;
    int err2;
    int type1,type2;
    int countdiff;
    int isangle=0;
    int isMissing1=0,isMissing2=0;

    char *sval1 = NULL,*sval2 = NULL;
    unsigned char *uval1 = NULL,*uval2 = NULL;
    double *dval1 = NULL, *dval2 = NULL;
    long *lval1 = NULL, *lval2 = NULL;
    double maxdiff=0;
    double packingError1=0,packingError2=0;
    double value_tolerance=0;
    grib_context* c=handle1->context;
    const char* first_str = (handles_swapped==0? "1st" : "2nd");
    const char* second_str = (handles_swapped==0? "2nd" : "1st");

    type1=type;
    type2=type;
    if (verbose && !handles_swapped) printf("  comparing %s",name);

    if( type1==GRIB_TYPE_UNDEFINED && (err = grib_get_native_type(handle1,name,&type1)) != GRIB_SUCCESS)
    {
        printInfo(handle1);
        printf("Oops... cannot get type of [%s] in %s field: %s\n",name, first_str, grib_get_error_message(err));
        save_error(c,name);
        return err;
    }

    if(type2==GRIB_TYPE_UNDEFINED && (err = grib_get_native_type(handle2,name,&type2)) != GRIB_SUCCESS)
    {
        if(err == GRIB_NOT_FOUND)
        {
            printInfo(handle1);
            printf("[%s] not found in %s field\n",name, second_str);
            save_error(c,name);
            return err;
        }
        printInfo(handle1);
        printf("Oops... cannot get type of [%s] in %s field: %s\n",name, second_str, grib_get_error_message(err));
        save_error(c,name);
        return err;
    }

    /* if(type1 != type2) {
         printInfo(handle1);
         printf("Warning, [%s] has different types: 1st field: [%s], 2nd field: [%s]\n",
           name,grib_get_type_name(type1),grib_get_type_name(type2));
         return GRIB_TYPE_MISMATCH;
    } */

    if(type1 == GRIB_TYPE_LABEL)
        return err;

    if(type1 == GRIB_TYPE_SECTION)
        return err;

    if((err = grib_get_size(handle1,name,&len1)) != GRIB_SUCCESS)
    {
        printInfo(handle1);
        printf("Oops... cannot get size of [%s] in %s field: %s\n",name, first_str, grib_get_error_message(err));
        save_error(c,name);
        return err;
    }

    if((err = grib_get_size(handle2,name,&len2)) != GRIB_SUCCESS)
    {
        if(err == GRIB_NOT_FOUND)
        {
            printInfo(handle1);
            printf("[%s] not found in %s field\n",name,second_str);
            save_error(c,name);
            return err;
        }

        printInfo(handle1);
        printf("Oops... cannot get size of [%s] in %s field: %s\n",name, second_str, grib_get_error_message(err));
        save_error(c,name);
        return err;
    }
    
    if (handles_swapped) {
        /* Comparing a second time with handles swapped. Do not compare keys common to both handles */
        return GRIB_SUCCESS;
    }
    /* if(len1 != len2 && type1 != GRIB_TYPE_STRING) {
          printInfo(handle1);
          printf("[%s] has different size: 1st field: %ld, 2nd field: %ld\n",name,(long)len1,(long)len2);
          save_error(c,name);
          return GRIB_COUNT_MISMATCH;
        } */

    if (options->mode != MODE_BUFR) {
        /* TODO: Ignore missing values for keys in BUFR. Not yet implemented */
        isMissing1= ( (grib_is_missing(handle1,name,&err1)==1) && (err1 == 0) ) ? 1 : 0;
        isMissing2= ( (grib_is_missing(handle2,name,&err2)==1) && (err2 == 0) ) ? 1 : 0;
    }

    if ((isMissing1==1) && (isMissing2==1)) {
        if (verbose) printf(" is set to missing in both fields\n");
        return GRIB_SUCCESS;
    }

    if (isMissing1==1) {
        if (verbose) printf(" is set to missing in %s field\n", first_str);
        printInfo(handle1);
        printf("%s is set to missing in %s field is not missing in %s field\n",name,first_str,second_str);
        err1 = GRIB_VALUE_MISMATCH;
        save_error(c,name);
        return GRIB_VALUE_MISMATCH;
    }

    if (isMissing2==1) {
        if (verbose) printf(" is set to missing in %s field\n", first_str);
        printInfo(handle1);
        printf("%s is set to missing in %s field is not missing in %s field\n",name, second_str, first_str);
        err1 = GRIB_VALUE_MISMATCH;
        save_error(c,name);
        return GRIB_VALUE_MISMATCH;
    }

    switch(type1)
    {
    case GRIB_TYPE_STRING:
        if (verbose) printf(" as string\n");
        grib_get_string_length(handle1,name,&len1);
        grib_get_string_length(handle2,name,&len2);
        sval1 = (char*)grib_context_malloc(handle1->context,len1*sizeof(char));
        sval2 = (char*)grib_context_malloc(handle2->context,len2*sizeof(char));

        if((err1 = grib_get_string(handle1,name,sval1,&len1)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            printf("Oops... cannot get string value of [%s] in %s field: %s\n",
                    name, first_str, grib_get_error_message(err1));
            save_error(c,name);
        }

        if((err2 = grib_get_string(handle2,name,sval2,&len2)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            printf("Oops... cannot get string value of [%s] in %s field: %s\n",
                    name, second_str, grib_get_error_message(err2));
            save_error(c,name);
        }

        if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS)
        {
            if(grib_inline_strcmp(sval1,sval2) != 0)
            {
                /* Check if strings are 'missing'.
                 * Note: one string could have all its bits=1 and the other empty */
                int equal = 0;
                grib_accessor* a1 = grib_find_accessor(handle1, name);
                grib_accessor* a2 = grib_find_accessor(handle2, name);
                int is_miss_1 = grib_is_missing_string(a1, (unsigned char *)sval1, len1);
                int is_miss_2 = grib_is_missing_string(a2, (unsigned char *)sval2, len2);
                if ( is_miss_1 && is_miss_2 ) {
                    equal = 1;
                }
                if (!equal) {
                    printInfo(handle1);
                    printf("string [%s]: [%s] != [%s]\n", name, sval1, sval2);
                    err1 = GRIB_VALUE_MISMATCH;
                    save_error(c,name);
                }
            }
        }

        grib_context_free(handle1->context,sval1);
        grib_context_free(handle2->context,sval2);

        if(err1) return err1;
        if(err2) return err2;

        break;

    case GRIB_TYPE_LONG:
        if (verbose) printf(" as long\n");

        lval1 = (long*)grib_context_malloc(handle1->context,len1*sizeof(long));
        lval2 = (long*)grib_context_malloc(handle2->context,len2*sizeof(long));

        if((err1 = grib_get_long_array(handle1,name,lval1,&len1)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            printf("Oops... cannot get long value of [%s] in %s field: %s\n",
                    name, first_str, grib_get_error_message(err1));
            save_error(c,name);
        }

        if((err2 = grib_get_long_array(handle2,name,lval2,&len2)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            printf("Oops... cannot get long value of [%s] in %s field: %s\n",
                    name,second_str,grib_get_error_message(err2));
            save_error(c,name);
        }

        if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1!=len2)
        {
            printInfo(handle1);
            printf("Different size for \"%s\"  [%ld]  [%ld]\n",name,(long)len1,(long)len2);
            err1=GRIB_INTERNAL_ERROR;
            save_error(c,name);
        }
        if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1==len2)
        {
            int ii;
            countdiff=0;
            for(ii = 0; ii < len1; ii++)
                if(lval1[ii] != lval2[ii])  countdiff++;

            if (countdiff) {
                printInfo(handle1);
                save_error(c,name);
                err1 = GRIB_VALUE_MISMATCH;
                if(len1 == 1)
                    printf("long [%s]: [%ld] != [%ld]\n", name, *lval1, *lval2);
                else
                    printf("long [%s] %d out of %ld different\n", name, countdiff, (long)len1);
            }
        }

        grib_context_free(handle1->context,lval1);
        grib_context_free(handle2->context,lval2);

        if(err1) return err1;
        if(err2) return err2;
        break;

    case GRIB_TYPE_DOUBLE:
        if (verbose) printf(" as double\n");
        dval1 = (double*)grib_context_malloc(handle1->context,len1*sizeof(double));
        dval2 = (double*)grib_context_malloc(handle2->context,len2*sizeof(double));

        isangle=0;
        value_tolerance=global_tolerance;
        if (!grib_inline_strcmp(name,"packedValues") || !grib_inline_strcmp(name,"values")
                || !grib_inline_strcmp(name,"codedValues")) {
            packingError1=0;
            packingError2=0;
            err1=grib_get_double(handle1,"packingError",&packingError1);
            err2=grib_get_double(handle2,"packingError",&packingError2);
            if (packingCompare)
                value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
        } else if (!grib_inline_strcmp(name,"unpackedValues") ) {
            packingError1=0;
            packingError2=0;
            err1=grib_get_double(handle1,"unpackedError",&packingError1);
            err2=grib_get_double(handle2,"unpackedError",&packingError2);
            if (packingCompare)
                value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
        } else if ( !grib_inline_rstrcmp(name,"InDegrees")) {
            packingError1=0.0005;
            packingError2=0.0005;
            isangle=1;
            value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
        } else if (!grib_inline_strcmp(name,"referenceValue") ) {
            packingError1=0;
            packingError2=0;
            err1=grib_get_double(handle1,"referenceValueError",&packingError1);
            err2=grib_get_double(handle2,"referenceValueError",&packingError2);
            value_tolerance = packingError1 > packingError2 ? packingError1 : packingError2;
        }

        if (!compareAbsolute) {
            int all_specified = 0; /* =1 if relative comparison with "all" specified */
            for (i=0;i<options->tolerance_count;i++) {
                if (!strcmp((options->tolerance[i]).name, "all")) {
                    all_specified = 1;
                    value_tolerance=(options->tolerance[i]).double_value;
                    break;
                }
            }
            if (!all_specified) {
                char* basename = NULL;
                for (i=0;i<options->tolerance_count;i++) {
                    if ( strcmp( (options->tolerance[i]).name, name)==0 ) {
                        value_tolerance=(options->tolerance[i]).double_value;
                        break;
                    } else {
                        /* Check if the key without its rank has a relative tolerance */
                        basename = get_keyname_without_rank(name);
                        if ( basename && strcmp( (options->tolerance[i]).name, basename)==0 ) {
                            value_tolerance=(options->tolerance[i]).double_value;
                            break;
                        }
                    }
                }
                if (basename) grib_context_free(handle1->context, basename);
            }
        }

        if((err1 = grib_get_double_array(handle1,name,dval1,&len1)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            printf("Oops... cannot get double value of [%s] in %s field: %s\n",
                    name, first_str, grib_get_error_message(err1));
            save_error(c,name);
        }

        if((err2 = grib_get_double_array(handle2,name,dval2,&len2)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            printf("Oops... cannot get double value of [%s] in %s field: %s\n",
                    name,second_str,grib_get_error_message(err2));
            save_error(c,name);
        }

        if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1!=len2)
        {
            printInfo(handle1);
            printf("Different size for \"%s\"  [%ld]  [%ld]\n",name,(long)len1,(long)len2);
            save_error(c,name);
        }
        if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS && len1==len2)
        {
            int imaxdiff;
            double diff;
            double *pv1,*pv2,dnew1,dnew2;
            maxdiff=0;
            imaxdiff=0;
            countdiff=0;
            pv1=dval1;
            pv2=dval2;
            if (isangle) {
                dnew1=*dval1; dnew2=*dval2;
                pv1=&dnew1; pv2=&dnew2;
                if (*dval1 < 0 ) dnew1 += 360.0 ;
                if (*dval2 < 0 ) dnew2 += 360.0 ;
                if (*dval1 > 360 ) dnew1 -= 360.0 ;
                if (*dval2 > 360 ) dnew2 -= 360.0 ;
            }
            value_tolerance*=tolerance_factor;
            if (verbose) printf("  (%d values) tolerance=%g\n",(int)len1,value_tolerance);
            for(i = 0; i < len1; i++) {
                if((diff=compare_double(pv1++,pv2++,&value_tolerance))!=0) {
                    countdiff++;
                    if (maxdiff < diff) {maxdiff=diff;imaxdiff=i;}
                    err1 = GRIB_VALUE_MISMATCH;
                }
            }

            if (countdiff) {
                printInfo(handle1);
                save_error(c,name);
                if (len1>1) {
                    printf("double [%s]: %d out of %ld different\n",name,countdiff,(long)len1);
                    if (dval1[imaxdiff] != GRIB_MISSING_DOUBLE && dval2[imaxdiff] != GRIB_MISSING_DOUBLE) {
                        if (compareAbsolute) printf(" max");
                        printf(" absolute diff. = %.16e,",fabs(dval1[imaxdiff]-dval2[imaxdiff]));
                        if (!compareAbsolute) printf(" max");
                        printf(" relative diff. = %g",relative_error(dval1[imaxdiff],dval2[imaxdiff],value_tolerance));
                        printf("\n\tmax diff. element %d: %.20e %.20e",
                                imaxdiff,dval1[imaxdiff],dval2[imaxdiff]);
                        printf("\n\ttolerance=%.16e",value_tolerance);
                    } else {
                        /* One or both values are missing */
                        char* svalA = double_as_string(c, dval1[imaxdiff]);
                        char* svalB = double_as_string(c, dval2[imaxdiff]);
                        printf("\tdiff. element %d: %s %s", imaxdiff, svalA, svalB);
                        grib_context_free(c,svalA);
                        grib_context_free(c,svalB);
                    }
                    if (packingError2!=0 || packingError1!=0)
                        printf(" packingError: [%g] [%g]",packingError1,packingError2);

                    if (!grib_inline_strcmp(name,"packedValues") || !grib_inline_strcmp(name,"values")
                            || !grib_inline_strcmp(name,"codedValues")) {
                        double max1,min1,max2,min2;
                        grib_get_double(handle1,"max",&max1);
                        grib_get_double(handle1,"min",&min1);
                        grib_get_double(handle2,"max",&max2);
                        grib_get_double(handle2,"min",&min2);
                        printf("\n\tvalues max= [%g]  [%g]         min= [%g] [%g]",max1,max2,min1,min2);
                    }
                    printf("\n");
                } else {
                    if (dval1[0] != GRIB_MISSING_DOUBLE && dval2[0] != GRIB_MISSING_DOUBLE) {
                        printf("double [%s]: [%.20e] != [%.20e]\n", name, dval1[0], dval2[0]);
                        printf("\tabsolute diff. = %g,",fabs(dval1[0]-dval2[0]));
                        printf(" relative diff. = %g\n",relative_error(dval1[0],dval2[0],value_tolerance));
                        printf("\ttolerance=%g\n",value_tolerance);
                    } else {
                        /* One or both values are missing */
                        char* svalA = double_as_string(c, dval1[0]);
                        char* svalB = double_as_string(c, dval2[0]);
                        printf("double [%s]: [%s] != [%s]\n", name, svalA, svalB);
                        grib_context_free(c,svalA);
                        grib_context_free(c,svalB);
                    }
                }
            }
        }

        grib_context_free(handle1->context,dval1);
        grib_context_free(handle2->context,dval2);

        if(err1) return err1;
        if(err2) return err2;
        break;

    case GRIB_TYPE_BYTES:
        if (verbose) printf(" as bytes\n");
        if (options->mode==MODE_BUFR) return 0;
        if (len1<2) len1=512;
        if (len2<2) len2=512;
        uval1 = (unsigned char*)grib_context_malloc(handle1->context,len1*sizeof(unsigned char));
        uval2 = (unsigned char*)grib_context_malloc(handle2->context,len2*sizeof(unsigned char));

        if((err1 = grib_get_bytes(handle1,name,uval1,&len1)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            save_error(c,name);
            printf("Oops... cannot get bytes value of [%s] in %s field: %s\n",
                    name,first_str,grib_get_error_message(err1));
        }

        if((err2 = grib_get_bytes(handle2,name,uval2,&len2)) != GRIB_SUCCESS)
        {
            printInfo(handle1);
            save_error(c,name);
            printf("Oops... cannot get bytes value of [%s] in %s field: %s\n",
                    name,second_str,grib_get_error_message(err2));
        }

        if(err1 == GRIB_SUCCESS && err2 == GRIB_SUCCESS)
        {
            if(memcmp(uval1,uval2,len1) != 0)
            {
                for(i = 0; i < len1; i++) {
                    if(uval1[i] != uval2[i])
                    {
                        printInfo(handle1);
                        save_error(c,name);
                        if(len1 == 1)
                            printf("[%s] byte values are different: [%02x] and [%02x]\n",
                                    name,uval1[i],uval2[i]);
                        else
                            printf("[%s] byte value %d of %ld are different: [%02x] and [%02x]\n",
                                    name,i,(long)len1,uval1[i],uval2[i]);

                        err1 = GRIB_VALUE_MISMATCH;
                        break;
                    }
                }
                err1 = GRIB_VALUE_MISMATCH;
            }
        }

        grib_context_free(handle1->context,uval1);
        grib_context_free(handle2->context,uval2);

        if(err1) return err1;
        if(err2) return err2;
        break;

    case GRIB_TYPE_LABEL:
        if (verbose) printf(" as label\n");
        break;

    default:
        if (verbose) printf("\n");
        printInfo(handle1);
        save_error(c,name);
        printf("Cannot compare [%s], unsupported type %d\n",name,type1);
        return GRIB_UNABLE_TO_COMPARE_ACCESSORS;
        break;
    }

    return GRIB_SUCCESS;
}

static int compare_attributes(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options,
        grib_accessor* a, const char* prefix, int* err)
{
    int i=0, ret=0;
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes[i])
    {
        /*long native_type = 0;*/
        grib_accessor* aa = NULL;
        if ( (a->attributes[i]->flags & GRIB_ACCESSOR_FLAG_DUMP)== 0 ) {
            ++i; /* next attribute */
            continue;
        }
        aa = a->attributes[i];
        /*native_type = grib_accessor_get_native_type(aa);   TODO: read only check? */

        isLeafKey = aa->attributes[0]==NULL ? 1 : 0; /* update global variable */

        if (compare_attribute(handle1, handle2, options, aa, prefix, err)) {
            err++;
            write_messages(handle1, handle2);
            ret = 1;
        }

        ++i; /* next attribute */
    }

    return ret;
}

static int compare_attribute(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options,
        grib_accessor* a, const char* prefix, int* err)
{
    int ret = 0;
    grib_context* c = handle1->context;
    char* fullname = (char*)grib_context_malloc_clear( c, sizeof(char)*(strlen(a->name)+strlen(prefix)+5) );
    sprintf(fullname, "%s->%s", prefix, a->name);
    if (compare_values(options, handle1, handle2, fullname, GRIB_TYPE_UNDEFINED)) {
        err++;
        write_messages(handle1, handle2);
        ret=1;
    }
    /* Recurse if this key has children */
    if (!isLeafKey) {
        if (compare_attributes(handle1, handle2, options, a, fullname, err)) {
            ret=1;
        }
    }
    grib_context_free(c, fullname);
    return ret;
}

static int compare_all_dump_keys(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options, int *err)
{
    int ret=0;
    const char* name=NULL;
    grib_keys_iterator* iter  = NULL;
    grib_context* context=handle1->context;

    if (!headerMode) {
        /* See ECC-333: By setting unpack we get ALL the bufr keys. */
        /*              In headerMode we want just the header ones */
        ret = grib_set_long(handle1,"unpack",1);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(context, GRIB_LOG_ERROR, "Failed to unpack 1st message: %s", grib_get_error_message(ret));
            exit(1);
        }
        ret = grib_set_long(handle2,"unpack",1);
        if (ret != GRIB_SUCCESS) {
            grib_context_log(context, GRIB_LOG_ERROR, "Failed to unpack 2nd message: %s", grib_get_error_message(ret));
            exit(1);
        }
    }
    iter=grib_keys_iterator_new(handle1,0,NULL);

    if (!iter) {
        grib_context_log(context, GRIB_LOG_ERROR, "unable to create keys iterator");
        exit(1);
    }

    release_keys_list(); /* The keys list is used to determine the rank */
    new_keys_list();

    while(grib_keys_iterator_next(iter))
    {
        int rank = 0;
        int dofree = 0;
        char* prefix = NULL;
        grib_accessor* xa=grib_keys_iterator_get_accessor(iter);

        isLeafKey = 0; /* clear global variable for each key */
        name=grib_keys_iterator_get_name(iter);
        if (blacklisted(name)) continue;
        if (xa==NULL || ( xa->flags & GRIB_ACCESSOR_FLAG_DUMP )==0 ) continue;

        /* Get full name of key, e.g. '#2#windSpeed' or 'blockNumber' */
        rank = compute_bufr_key_rank(handle1, keys_list, xa->name);
        if (rank != 0) {
            prefix=(char*)grib_context_malloc_clear(context,sizeof(char)*(strlen(xa->name)+10));
            dofree = 1;
            sprintf(prefix,"#%d#%s", rank, xa->name);
        } else {
            prefix = (char*)xa->name;
        }

        if (blacklisted(prefix)) continue;

        /* Compare the key itself */
        if (compare_values(options, handle1, handle2, prefix, GRIB_TYPE_UNDEFINED)) {
            err++;
            write_messages(handle1, handle2);
            ret=1;
        }
        /* Now compare the key attributes (if any) */
        if (compare_attributes(handle1, handle2, options, xa, prefix, err)) {
            err++;
            write_messages(handle1, handle2);
            ret=1;
        }
        if (dofree) grib_context_free(context, prefix);
    }

    grib_keys_iterator_delete(iter);
    return ret;
}

static int compare_handles(grib_handle* handle1, grib_handle* handle2, grib_runtime_options* options)
{
    int err = 0;
    int i=0;
    grib_keys_iterator* iter  = NULL;
    const char* name=NULL;

    /* mask only if no -c option or headerMode (-H)*/
    if (blacklist && ( !listFromCommandLine || headerMode )) {
        /* See ECC-245, GRIB-573, GRIB-915: Do not change handles in memory */
        /*
        grib_string_list* nextb=blacklist;
        while (nextb) {
            grib_clear(handle1,nextb->value);
            grib_clear(handle2,nextb->value);
            nextb=nextb->next;
        }*/
    }

    if ( listFromCommandLine && onlyListed ) {
        for (i=0; i< options->compare_count; i++) {
            if (blacklisted((char*)options->compare[i].name)) continue;
            if (options->compare[i].type == GRIB_NAMESPACE) {
                iter=grib_keys_iterator_new(handle1,0,options->compare[i].name);
                if (!iter) {
                    grib_context_log(handle1->context, GRIB_LOG_ERROR, "unable to get iterator");
                    exit(1);
                }
                while(grib_keys_iterator_next(iter))
                {
                    name=grib_keys_iterator_get_name(iter);
                    /*printf("----- comparing %s\n",name);*/

                    if (blacklisted(name)) continue;
                    if(compare_values(options,handle1,handle2,name,GRIB_TYPE_UNDEFINED))  {
                        err++;
                        write_messages(handle1,handle2);
                    }
                }
                grib_keys_iterator_delete(iter);
            } else {
                if( compare_values(options,handle1,handle2,options->compare[i].name,options->compare[i].type))
                    err++;
                write_messages(handle1,handle2);
            }
        }
    } else {
        const void *msg1=NULL,*msg2=NULL;
        size_t size1=0,size2=0;
        int memcmp_ret=0;
        /* int ii=0; */
        GRIB_CHECK_NOLINE(grib_get_message(handle1,&msg1,&size1),0);
        GRIB_CHECK_NOLINE(grib_get_message(handle2,&msg2,&size2),0);
        if (size1==size2 && !(memcmp_ret=memcmp(msg1,msg2,size1))) {
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

        if ( listFromCommandLine ) {
            for (i=0; i< options->compare_count; i++) {
                if (blacklisted(name)) continue;
                if (options->compare[i].type == GRIB_NAMESPACE) {
                    iter=grib_keys_iterator_new(handle1,0,options->compare[i].name);
                    if (!iter) {
                        grib_context_log(handle1->context, GRIB_LOG_ERROR,
                                "ERROR: unable to get keys iterator for %s",options->compare[i].name);
                        exit(1);
                    }
                    while(grib_keys_iterator_next(iter))
                    {
                        name=grib_keys_iterator_get_name(iter);
                        /*printf("----- comparing %s\n",name);*/

                        if (blacklisted(name)) continue;
                        if(compare_values(options,handle1,handle2,name,GRIB_TYPE_UNDEFINED)) {
                            err++;
                            write_messages(handle1,handle2);
                            if (compare_all_dump_keys(handle1,handle2,options,&err)) {
                                err++;
                            }
                        }
                    }
                    grib_keys_iterator_delete(iter);
                } else {
                    if( compare_values(options,handle1,handle2,options->compare[i].name,options->compare[i].type)) {
                        err++;
                        write_messages(handle1,handle2);
                        if (compare_all_dump_keys(handle1,handle2,options,&err)) {
                            err++;
                        }
                    }
                }
            }
        } else {
            if (compare_all_dump_keys(handle1,handle2,options,&err)) {
                err++;
            }
        }

    }
    return err;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
