/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_get_data
 *
 */

#include "grib_tools.h"

static void print_key_values(grib_values* values,int values_count);
static grib_values* get_key_values(grib_runtime_options* options,grib_handle* h);

grib_option grib_options[]={
/*  {id, args, help}, on, command_line, value */
    {"q",0,0,1,0,0},
    {"S",0,0,1,0,0},
    {"M",0,0,0,1,0},
    {"m:","missingValue","\n\t\tThe missing value is given through this option."
          "\n\t\tAny string is allowed and it is printed in place of the missing"
          "\n\t\tvalues. Default is to skip the missing values.\n",0,1,0},
    {"p:",0,0,0,1,0},
    {"F:","format","\n\t\tC style format for values. Default is \"%.10e\"\n",0,1,0},
    {"w:",0,0,0,1,0},
    {"f",0,0,0,1,0},
    {"G",0,0,0,1,0},
    {"7",0,0,0,1,0},
    {"X:",0,0,0,1,0},
    {"V",0,0,0,1,0}
};

char* grib_tool_description="Print a latitude, longitude, data values list.\n"
                            "\tNote: Rotated grids are first unrotated";
char* grib_tool_name="grib_get_data";
char* grib_tool_usage="[options] grib_file grib_file ...";

extern FILE* dump_file;

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

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int err=0;

    double missingValue = 9999;
    int skip_missing=1;
    char *missing_string=NULL;
    int i=0;
    grib_values* values=NULL;
    grib_iterator* iter = NULL;
    char* format=NULL;
    char* default_format="%.10e";
    int print_keys=grib_options_on("p:");
    long numberOfPoints=0;
    long bitmapPresent = 0;
    long *bitmap = NULL; /* bitmap array */
    size_t bmp_len = 0;
    double *data_values=0,*lats=0,*lons=0;
    int n = 0;
    size_t size=0, num_bytes=0;
    long hasMissingValues = 0;

    if (grib_options_on("m:")) {
        /* User wants to see missing values */
        char* theEnd = NULL;
        double mval=0;
        char* kmiss=grib_options_get_option("m:");
        char* p = kmiss;
        skip_missing=0;
        while (*p != ':' && *p != '\0') p++;
        if (*p == ':' && *(p+1) != '\0') {
            *p='\0';
            missing_string=strdup(p+1);
        } else {
            missing_string=strdup(kmiss);
        }
        mval=strtod(kmiss, &theEnd);
        if (kmiss != theEnd && *theEnd == '\0')
            missingValue = mval;
        grib_set_double(h,"missingValue",missingValue);
        /*missing_string=grib_options_get_option("m:");*/
    }

    if (grib_options_on("F:"))
        format=grib_options_get_option("F:");
    else
        format=default_format;

    if ((err=grib_get_long(h,"numberOfPoints",&numberOfPoints)) !=GRIB_SUCCESS) {
        fprintf(dump_file,"ERROR: unable to get number of points\n");
        return err;
    }

    iter=grib_iterator_new(h,0,&err);

    num_bytes = (numberOfPoints+1)*sizeof(double);
    data_values=(double*)calloc(numberOfPoints+1, sizeof(double));
    if (!data_values) {
        fprintf(dump_file, "ERROR: failed to allocate %ld bytes for data values (number of points=%ld)\n",
                (long)num_bytes, numberOfPoints);
        exit(GRIB_OUT_OF_MEMORY);
    }

    if (iter) {
        double *lat=0,*lon=0,*val=0;
        lats=(double*)calloc(numberOfPoints+1,sizeof(double));
        lons=(double*)calloc(numberOfPoints+1,sizeof(double));
        lat=lats; lon=lons; val=data_values;
        while(grib_iterator_next(iter,lat++,lon++,val++)) {}
    } else if (err==GRIB_NOT_IMPLEMENTED || err==GRIB_SUCCESS){
        size=numberOfPoints;
        err = grib_get_double_array(h,"values",data_values,&size);
        if (err) {
            grib_context_log(h->context,GRIB_LOG_ERROR,"Cannot decode values: %s",
                    grib_get_error_message(err));
            exit(1);
        }
        if (size!=numberOfPoints) {
            if (!grib_options_on("q"))
                fprintf(dump_file,"ERROR: wrong number of points %d\n",(int)numberOfPoints);
            if (grib_options_on("f")) exit(1);
        }
    } else {
        grib_context_log(h->context,GRIB_LOG_ERROR,
                "%s",grib_get_error_message(err));
        exit(err);
    }

    /* Cater for GRIBs which have missing values but no bitmap */
    /* See ECC-511 */
    GRIB_CHECK(grib_get_long(h,"missingValuesPresent",&hasMissingValues),0);
    GRIB_CHECK(grib_get_long(h,"bitmapPresent",&bitmapPresent),0);
    if (bitmapPresent)
    {
        GRIB_CHECK(grib_get_size(h,"bitmap",&bmp_len),0);
        bitmap = (long*)malloc(bmp_len*sizeof(long));
        GRIB_CHECK(grib_get_long_array(h,"bitmap",bitmap,&bmp_len),0);
    }

    if (iter)
        fprintf(dump_file,"Latitude, Longitude, ");

    fprintf(dump_file,"Value");

    if (print_keys)
        for (i=0;i<options->print_keys_count; i++)
            fprintf(dump_file,", %s",options->print_keys[i].name);

    fprintf(dump_file,"\n");

    if (print_keys)
        values=get_key_values(options,h);

    if (skip_missing==0){
        /* Show missing values in data */
        for (i=0;i<numberOfPoints;i++) {
            int is_missing_val = 0;
            if (hasMissingValues) {
                if (bitmapPresent) is_missing_val = (bitmap[i] == 0);
                else is_missing_val = (data_values[i] == missingValue);
            }
            if (iter) fprintf(dump_file,"%9.3f%9.3f ",lats[i],lons[i]);

            if (is_missing_val)
                fprintf(dump_file,"%s",missing_string);
            else
                fprintf(dump_file,format,data_values[i]);

            if (print_keys)
                print_key_values(values,options->print_keys_count);
            fprintf(dump_file,"\n");
            n++;
        }

    } else if ( skip_missing==1 ){
        /* Skip the missing values in data */
        for (i=0;i<numberOfPoints;i++) {
            int is_missing_val = 0;
            if (hasMissingValues) {
                if (bitmapPresent) is_missing_val = (bitmap[i] == 0);
                else is_missing_val = (data_values[i] == missingValue);
            }
            if (!is_missing_val){
                if (iter) fprintf(dump_file,"%9.3f%9.3f ",lats[i],lons[i]);
                fprintf(dump_file,format,data_values[i]);
                if (print_keys)
                    print_key_values(values,options->print_keys_count);
                fprintf(dump_file,"\n");
                n++;
            }
        }
    }

    if (iter) grib_iterator_delete(iter);
    if (bitmap) free(bitmap);

    free(data_values);
    if (iter) {
        free(lats);
        free(lons);
    }

    return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    grib_handle_delete(h);
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h)
{
    grib_print_key_values(options,h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    return 0;
}

static void print_key_values(grib_values* values,int values_count)
{
    int i=0;
    for (i=0; i<values_count; i++) {
        fprintf(dump_file," %s",values[i].string_value);
    }
}

static grib_values* get_key_values(grib_runtime_options* options,grib_handle* h)
{
    int i=0;
    int ret=0;
    char value[MAX_STRING_LEN]={0,};
    char* notfound="not found";

    for (i=0;i<options->print_keys_count;i++) {
        size_t len=MAX_STRING_LEN;
        ret=GRIB_SUCCESS;

        if (grib_is_missing(h,options->print_keys[i].name,&ret) && ret==GRIB_SUCCESS) {
            options->print_keys[i].type=GRIB_TYPE_MISSING;
            sprintf(value,"MISSING");

        } else if ( ret != GRIB_NOT_FOUND ){
            if (options->print_keys[i].type == GRIB_TYPE_UNDEFINED) {
                grib_get_native_type(h,options->print_keys[i].name,&(options->print_keys[i].type));
            }

            switch (options->print_keys[i].type) {
            case GRIB_TYPE_STRING:
                ret=grib_get_string( h,options->print_keys[i].name,value,&len);
                break;
            case GRIB_TYPE_DOUBLE:
                ret=grib_get_double( h,options->print_keys[i].name,
                        &(options->print_keys[i].double_value));
                sprintf(value,"%g",options->print_keys[i].double_value);
                break;
            case GRIB_TYPE_LONG:
                ret=grib_get_long( h,options->print_keys[i].name,
                        &(options->print_keys[i].long_value));
                sprintf(value,"%ld",(long)options->print_keys[i].long_value);
                break;
            default:
                fprintf(dump_file,"invalid type for %s\n",options->print_keys[i].name);
                exit(1);

            }
        }

        if (ret != GRIB_SUCCESS) {
            if (options->fail) GRIB_CHECK_NOLINE(ret,options->print_keys[i].name);
            if (ret == GRIB_NOT_FOUND) strcpy(value,notfound);
            else {
                fprintf(dump_file,"%s %s\n",grib_get_error_message(ret),options->print_keys[i].name);
                exit(ret);
            }
        }
        options->print_keys[i].string_value=strdup(value);
    }
    return options->print_keys;

}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
