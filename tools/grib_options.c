/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_options
 *
 */
#include "grib_tools.h"

#ifdef ECCODES_ON_WINDOWS
/* Microsoft Windows Visual Studio support */
#include "wingetopt.h"
#endif

char* names[]={"parameter","vertical","geography","data","mars","local"};
int names_count=6;

/* id,args,help */
grib_options_help grib_options_help_list[] ={
  {"a",0,"Dump aliases.\n"},
  {"b:","key,key,...",
     "\n\t\tAll the keys in this list are skipped in the comparison. Bit-by-bit compare on.\n"},
  {"B:","order by directive",
     "\n\t\tOrder by. The output will be ordered according to the order by directive."
     "\n\t\tOrder by example: \"step asc, centre desc\" (step ascending and centre descending)\n"},
  {"c:","key[:i/d/s/n],key[:i/d/s/n],...",
   "\n\t\tOnly the listed keys or namespaces (:n) are compared. The optional letter after the colon is used "
   "\n\t\tto force the type in the comparison: i->integer, d->float, s->string, n->namespace."
   "\n\t\tSee -a option. Incompatible with -H option.\n"},
  {"d:","value",
   "\n\t\tSet all the data values to \"value\".\n"},
  {"e:","tolerance","\n\t\tOnly values whose difference is more than tolerance are considered different.\n"},
  {"f",0,"Force. Force the execution not to fail on error.\n"},
  {"F:","format","\n\t\tC style format for floating point values.\n"},
  {"g",0,"Copy GTS header. \n"},
  {"G",0,"GRIBEX compatibility mode.\n"},
  {"i:","index",
    "\n\t\tData value corresponding to the given index is printed.\n"},
  {"j",0,"JSON mode (JavaScript Object Notation).\n"},
  {"l:","Latitude,Longitude[,MODE,file]",
   "\n\t\tValue close to the point of a Latitude/Longitude."
   "\n\t\tAllowed values for MODE are:"
   "\n\t\t4 (4 values in the nearest points are printed) Default"
   "\n\t\t1 (the value at the nearest point is printed)"
   "\n\t\tfile (file is used as mask. The closer point with mask value>=0.5 is printed)\n"
       },
  {"n:","namespace",
   "\n\t\tAll the keys belonging to namespace are printed.\n"},
  {"m",0,"Mars keys are printed.\n"},
  {"o:","output_file",
   "\n\t\tOutput is written to output_file."
   "\n\t\tIf an output file is required and -o is not used, the"
   " output is written to filter.out\n"},
  {"p:","key[:{s/d/i}],key[:{s/d/i}],...",
   "\n\t\tDeclaration of keys to print."
   "\n\t\tFor each key a string (key:s), a double (key:d) or an integer (key:i)"
   "\n\t\ttype can be requested. Default type is string.\n"},
  {"q",0,"Quiet.\n"},
  {"r",0,"Repack data. Sometimes after setting some keys involving properties"
         "\n\t\tof the packing algorithm a repacking of data is needed."
         "\n\t\tThis repacking is performed setting this -r option.\n"},
  {"s:","key[:{s/d/i}]=value,key[:{s/d/i}]=value,...",
   "\n\t\tKey/values to set."
   "\n\t\tFor each key a string (key:s), a double (key:d) or an integer (key:i)"
   "\n\t\ttype can be defined. By default the native type is set.\n"},
  {"t",0,"Print type information.\n"},
  {"w:","key[:{s/d/i}]{=/!=}value,key[:{s/d/i}]{=/!=}value,...",
   "\n\t\tWhere clause."
   "\n\t\tMessages are processed only if they match all the"
   " key/value constraints."
   "\n\t\tA valid constraint is of type key=value or key!=value."
   "\n\t\tFor each key a string (key:s), a double (key:d) or"
   " an integer (key:i)\n\t\ttype can be specified. Default type is string."
   "\n\t\tIn the value you can also use the forward-slash character '/' to specify an OR condition (i.e. a logical disjunction)\n"
  },
  {"v",0,"Verbose.\n"},
  {"7",0,"Does not fail when the message has wrong length\n"},
  {"A:","absolute error\n",
  "\tCompare floating point values using the absolute error as tolerance.\n\t\tDefault is absolute error=0\n"},
  {"C",0,"C code mode. A C code program generating the message is dumped.\n"},
  {"D",0,"Debug mode.\n"},
  {"H",0,"Print octet content in hexadecimal format.\n"},
  {"M",0,"Multi-field support off. Turn off support for multiple fields in single grib message.\n"},
  {"O",0,"Octet mode. WMO documentation style dump.\n"},
  {"P:","key[:{s/d/i}],key[:{s/d/i}],...",
   "\n\t\tAs -p adding the declared keys to the default list.\n"},
  {"R:","key1=relative_error1,key2=relative_error2,...\n",
   "\tCompare floating point values using the relative error as tolerance."
"\n\t\tkey1=relative_error will compare key1 using relative_error1."
"\n\t\tall=relative_error will compare all the floating point keys using relative_error. Default all=0.\n"},
  {"S",0,"Strict. Only grib messages matching all the constraints are copied to"
   "\n\t\tthe output file\n"},
  {"T:","T | B | M | A","Message type. T->GTS, B->BUFR, M->METAR (Experimental),A->Any (Experimental).\n\t\t\tThe input file is interpreted according to the message type.\n"},
  {"V",0,"Version.\n"},
  {"W:","width","\n\t\tMinimum width of each column in output. Default is 10.\n"},
  {"X:","offset","\n\t\tInput file offset in bytes. Processing of the input file will start from \"offset\".\n"},
  {"x",0,"Fast parsing option, only headers are loaded.\n"},
  {"k:","key1,key2,...","\n\t\tSpecify a list of keys to index on. By default the input files are indexed on the MARS keys."
                        "\n\t\tFor each key a string (key:s) or a double (key:d) or an integer (key:i)"
                        "\n\t\ttype can be requested.\n"}

};

int grib_options_help_count=sizeof(grib_options_help_list)/sizeof(grib_options_help);


void usage()
{
    int i=0;
    printf("\nNAME \t%s\n\n",grib_tool_name);
    printf("DESCRIPTION\n\t%s\n\n",grib_tool_description);
    printf("USAGE \n\t%s %s\n\n",grib_tool_name,grib_tool_usage);
    printf("OPTIONS\n");
    for (i=0;i<grib_options_count;i++) {
        if (grib_options[i].command_line)
            printf("\t-%c %s\t%s",grib_options[i].id[0],
                    grib_options_get_args(grib_options[i].id),
                    grib_options_get_help(grib_options[i].id));
    }
    printf("\n\n");
    exit(1);
}

char* grib_options_get_option(const char* id)
{
    int i=0;
    for (i=0; i<grib_options_count; i++) {
        if (!strcmp(id,grib_options[i].id))
            return grib_options[i].value;
    }
    return NULL;
}

int grib_options_command_line(const char* id)
{
    int i=0;
    for (i=0; i<grib_options_count; i++) {
        if (!strcmp(id,grib_options[i].id))
            return grib_options[i].command_line;
    }
    return 0;
}

int grib_options_on(const char* id)
{
    int i=0;
    for (i=0; i<grib_options_count; i++) {
        if (!strcmp(id,grib_options[i].id))
            return grib_options[i].on;
    }
    return 0;
}

int grib_get_runtime_options(int argc,char** argv,grib_runtime_options* options)
{
    int i=0,c=0;
    char* optstr=(char*)calloc(1,2*grib_options_count*sizeof(char));

    if (!optstr) return GRIB_OUT_OF_MEMORY;

    for (i=0;i<grib_options_count;i++)
        if (grib_options[i].command_line)
            strncat(optstr,grib_options[i].id,2);


    while ((c = getopt (argc, argv, optstr)) != -1) {
        if (c == '?') usage();
        i=0;
        while ( i < grib_options_count && grib_options[i].id[0] != c) i++;
        grib_options[i].on=1;
        if ( grib_options[i].id[1] == ':' )
            grib_options[i].value=optarg;
    }
    free(optstr);
    return 0;
}

int grib_process_runtime_options(grib_context* context,int argc,char** argv,grib_runtime_options* options)
{
    int i=0,ret=0;
    int has_output=0;int has_input_extra=0,nfiles=0;
    char *karg=NULL,*warg=NULL,*sarg=NULL,*barg=NULL;

    if (grib_options_on("V")) {
        printf("\necCodes Version ");
        grib_print_api_version(stdout);
        printf("\n\n");
        exit(0);
    }

    if (grib_options_on("B:"))
        options->orderby=grib_options_get_option("B:");

    if (grib_options_on("x")) options->headers_only=1;
    else options->headers_only=0;

    if (grib_options_on("T:")) {
        char* x=grib_options_get_option("T:");
        if ( *x == 'T' ) options->mode=MODE_GTS;
        else if ( *x == 'B' ) options->mode=MODE_BUFR;
        else if ( *x == 'M' ) options->mode=MODE_METAR;
        else if ( *x == 'A' ) options->mode=MODE_ANY;
        else options->mode=MODE_GRIB;
    }

    if (grib_options_on("F:"))
        options->format=grib_options_get_option("F:");
    else
        options->format=strdup("%g");

    if (grib_options_on("i:")) {
        options->index_on=1;
        options->index=atoi(grib_options_get_option("i:"));
    }

    if (grib_options_on("l:"))
        options->latlon=grib_options_get_option("l:");

    if (grib_options_on("X:"))
        options->infile_offset=atol(grib_options_get_option("X:"));

#ifndef ECCODES_ON_WINDOWS
    /* Check at compile time to ensure our file offset is at least 64 bits */
    COMPILE_TIME_ASSERT( sizeof(options->infile_offset) >= 8 );
#endif

    has_output=grib_options_on("U");
    has_input_extra=grib_options_on("I");
    options->repack=grib_options_on("r");
    options->gts=grib_options_on("g");

    if (grib_options_on("d:")) {
        options->constant=atof(grib_options_get_option("d:"));
        options->repack=1;
    }

    if (grib_options_on("G")) grib_gribex_mode_on(context);

    nfiles=argc-optind;
    if ( nfiles < (1+has_output+has_input_extra) && !options->infile ) usage();

    if (has_input_extra) {
        options->infile_extra=(grib_tools_file*)calloc(1,sizeof(grib_tools_file));
        options->infile_extra->name=argv[optind];
    }

    if (!options->infile) {
        for (i=optind+has_input_extra;i<argc-has_output;i++) {
            grib_tools_file* p=NULL;
            grib_tools_file* infile=(grib_tools_file*)calloc(1,sizeof(grib_tools_file));
            infile->name=argv[i];
            if (!options->infile) options->infile=infile;
            else {
                p=options->infile;
                while(p->next) p=p->next;
                p->next=infile;
            }
        }
    }

    if (has_output) {
        options->outfile=(grib_tools_file*)calloc(1,sizeof(grib_tools_file));
        options->outfile->name=strdup(argv[argc-1]);
    }

    if (grib_options_on("o:")) {
        options->outfile=(grib_tools_file*)calloc(1,sizeof(grib_tools_file));
        options->outfile->name=grib_options_get_option("o:");
    }

    options->print_number=grib_options_on("N");
    options->print_header=grib_options_on("H");
    options->verbose=grib_options_on("v");

    if (grib_options_on("q") && grib_options_command_line("q"))
        options->verbose=0;

    options->fail=!grib_options_on("f");
    if (grib_options_get_option("W:"))
        options->default_print_width=atoi(grib_options_get_option("W:"));

    if (grib_options_on("n:"))
        options->name_space=grib_options_get_option("n:");

    if (grib_options_on("m"))
        options->name_space=strdup("mars");


    if (grib_options_on("P:")) karg=grib_options_get_option("P:");
    else if (grib_options_on("p:")) {
        karg=grib_options_get_option("p:");
        options->name_space=NULL;
    }

    options->requested_print_keys_count=MAX_KEYS;
    ret = parse_keyval_string(grib_tool_name,karg,0,GRIB_TYPE_UNDEFINED,
            options->requested_print_keys,&(options->requested_print_keys_count));
    if (ret == GRIB_INVALID_ARGUMENT) usage();
    GRIB_CHECK_NOLINE(ret,0);

    options->strict=grib_options_on("S");

    if (grib_options_on("M")) grib_multi_support_off(context);
    else grib_multi_support_on(context);

    if (grib_options_on("g")) grib_gts_header_on(context);
    else grib_gts_header_off(context);

    if (grib_options_on("V")) {
        printf("\necCodes Version ");
        grib_print_api_version(stdout);
        printf("\n\n");
    }

    if (grib_options_on("s:")) {
        sarg=grib_options_get_option("s:");
        options->set_values_count=MAX_KEYS;
        ret=parse_keyval_string(grib_tool_name, sarg,1,GRIB_TYPE_UNDEFINED,options->set_values,&(options->set_values_count));
        if (ret == GRIB_INVALID_ARGUMENT) usage();
    }

    if (grib_options_on("b:")) {
        barg=grib_options_get_option("b:");
        options->set_values_count=MAX_KEYS;
        ret=parse_keyval_string(grib_tool_name,barg,0,GRIB_TYPE_STRING,options->set_values,&(options->set_values_count));
        if (ret == GRIB_INVALID_ARGUMENT) usage();
    }

    if (grib_options_on("c:")) {
        sarg=grib_options_get_option("c:");
        options->compare_count=MAX_KEYS;
        ret=parse_keyval_string(grib_tool_name,sarg,0,GRIB_TYPE_UNDEFINED,options->compare,
                &(options->compare_count));
        if (ret == GRIB_INVALID_ARGUMENT) usage();
    }
    if (grib_options_on("e")) {
        for (i=0;i<names_count; i++) {
            options->compare[i+options->compare_count].name=names[i];
            options->compare[i+options->compare_count].type=GRIB_NAMESPACE;
        }
        options->compare_count+=names_count;
    }

    warg=grib_options_get_option("w:");

    options->constraints_count=MAX_KEYS;
    ret=parse_keyval_string(grib_tool_name,warg,1,GRIB_TYPE_UNDEFINED,
            options->constraints,&(options->constraints_count));
    if (ret == GRIB_INVALID_ARGUMENT) usage();

    return GRIB_SUCCESS;
}

char* grib_options_get_help(char* id)
{
    int i=0;
    char msg[]="ERROR: help not found for option ";
    char* err=(char*)calloc(1,sizeof(msg)+3);
    sprintf(err,"%s%c\n",msg,*id);
    for (i=0; i<grib_options_count;i++) {
        if (!strcmp(id,grib_options[i].id)) {
            if (grib_options[i].help != NULL) return grib_options[i].help;
            else break;
        }
    }
    for (i=0; i<grib_options_help_count;i++) {
        if (!strcmp(id,grib_options_help_list[i].id)) {
            return grib_options_help_list[i].help != NULL ? grib_options_help_list[i].help : err;
        }
    }
    return err;
}

char* grib_options_get_args(char* id)
{
    int i=0;
    char empty[]="";
    char msg[]="ERROR: help not found for option -";
    char* err=NULL;
    if (id[1] != ':') return strdup(empty);
    err=(char*)calloc(1,sizeof(msg)+3);
    sprintf(err,"%s%c\n",msg,*id);
    for (i=0; i<grib_options_count;i++) {
        if (!strcmp(id,grib_options[i].id)) {
            if (grib_options[i].args != NULL) return grib_options[i].args;
            else break;
        }
    }
    for (i=0; i<grib_options_help_count;i++) {
        if (!strcmp(id,grib_options_help_list[i].id)) {
            return grib_options_help_list[i].args != NULL ? grib_options_help_list[i].args : err;
        }
    }
    return err;
}

void usage_doxygen()
{
    int i=0;
    printf("/*!  \\page %s %s\n",grib_tool_name,grib_tool_name);
    printf("\\section DESCRIPTION \n %s\n\n",grib_tool_description);
    printf("\\section USAGE \n %s \n %s\n\n",grib_tool_name,grib_tool_usage);
    printf("\\section OPTIONS \n");
    for (i=0;i<grib_options_count;i++) {
        if (grib_options[i].command_line) {
            printf("-%c %s \\n",
                    grib_options[i].id[0],
                    grib_options_get_args(grib_options[i].id));
            printf(" %s \\n \\n ",
                    grib_options_get_help(grib_options[i].id));
        }
    }
    exit(1);
}

#if 0
void usage_doxygen() {
    int i=0;
    printf("/*!  \\page %s %s\n",grib_tool_name,grib_tool_name);
    printf("\\section DESCRIPTION \n%s\n\n",grib_tool_description);
    printf("\\section USAGE \n%s \n%s\n\n",grib_tool_name,grib_tool_usage);
    printf("\\section OPTIONS\n");
    printf("<table frame=void border=0>\n");
    for (i=0;i<grib_options_count;i++) {
        if (grib_options[i].command_line) {
            printf("<tr>\n");
            printf("<td colspan=2>-%c %s</td>\n",
                    grib_options[i].id[0],
                    grib_options_get_args(grib_options[i].id));
            printf("</tr><tr>\n");
            printf("<td width=20></td><td>%s</td>",
                    grib_options_get_help(grib_options[i].id));
            printf("</tr><tr><td></td></tr>\n");
        }
    }
    printf("</table>\n");
    exit(1);
}
#endif

