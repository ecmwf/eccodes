/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

typedef void (*getproc)(FILE*, grib_handle*, const char*, const char*);

void get_long(FILE* f, grib_handle* h, const char* name, const char* arg)
{
    const char* key = arg ? arg : name;
    long value;
    GRIB_CHECK(grib_get_long(h, key, &value), key);
    fprintf(f, "\"%s\":%ld\n", name, value);
}

void get_param(FILE* f, grib_handle* h, const char* name, const char* arg)
{
    const char* key = "indicatorOfParameter";
    long value;
    GRIB_CHECK(grib_get_long(h, key, &value), key);
    fprintf(f, "\"%s\":%ld\n", name, value - 131 + 2);
}

void get_iso_ref_date(FILE* f, grib_handle* h, const char* name, const char* arg)
{
    long year;
    long month ;
    long day;
    long hour ;
    long minute ;
    long second;

    GRIB_CHECK(grib_get_long(h, "year", &year), "year");
    GRIB_CHECK(grib_get_long(h, "month", &month), "month");
    GRIB_CHECK(grib_get_long(h, "day", &day), "day");
    GRIB_CHECK(grib_get_long(h, "hour", &hour), "hour");
    GRIB_CHECK(grib_get_long(h, "minute", &minute), "minute");
    GRIB_CHECK(grib_get_long(h, "second", &second), "second");

    fprintf(f, "\"%s\":\"%04ld-%02ld-%02ldT%02ld:%02ld:%02ld.000Z\"", name, year, month, day, hour, minute, second);
}

typedef struct header_keys {
    const char* name;
    getproc proc;
    const char* arg;

} header_keys;

/*
{
 *         "header": {
 *           "refTime": "2013-11-30T18:00:00.000Z",
 *           "parameterNumber": 2,



 *         },
 */
header_keys header[] = {

        {"scanMode",&get_long, "scanningMode",},
        {"la1",&get_long, "latitudeOfFirstGridPointInDegrees",},
        {"lo1",&get_long, "longitudeOfFirstGridPointInDegrees",},

        {"la2",&get_long, "latitudeOfLastGridPointInDegrees",},
        {"lo2",&get_long, "longitudeOfLastGridPointInDegrees",},

        {"forecastTime",&get_long, "step",},

        {"nx",&get_long, "Nx",},
        {"ny",&get_long, "Ny",},

        {"dx",&get_long, "DxInDegrees",},
        {"dy",&get_long, "DyInDegrees",},

        {"parameterNumber", &get_param, "indicatorOfParameter",},
        {"refTime", &get_iso_ref_date, NULL,},

        {NULL,}
};

int main(int argc, char *argv[])
{
    grib_handle* h = NULL;
    FILE* f = NULL;
    size_t i, j = 0;
    int err = 0;
    double* values;
    size_t size, count;
    size_t n = 0;

    printf("[");

    for(i = 1; i < argc; i++)
    {
        f = fopen(argv[i],"r");
        if(!f) {
            perror(argv[i]);
            exit(1);
        }

        while((h = grib_handle_new_from_file(0,f,&err)) != NULL)
        {
            if(++n>1) {
                printf(",");
            }

            printf("{");
            printf("\"header\":{\n");

            j = 0;
            while(header[j].name) {
                if(j) printf(",");
                header[j].proc(stdout, h, header[j].name, header[j].arg);
                j++;
            }

            printf("},\n");
            printf("\"meta\":{\n");
            printf("},\n");
            /*========== data *============*/
            size=0;
            GRIB_CHECK(grib_get_size(h,"values",&size),0);
            values = (double*)malloc(sizeof(double)*size);

            if (!values) {
                fprintf(stderr,"%s: out of memory\n", argv[0]);
                exit(1);
            }
            count=size;
            GRIB_CHECK(grib_get_double_array(h,"values",values,&count),0);
            if (count != size) {
                printf("%s: wrong values count %lu %lu\n",argv[0],count,size);
                exit(1);
            }
            printf("\"data\":[\n");

            for(j = 0; j < count; j ++ ) {
                if(j) printf(",");
                printf("%g", (long)(values[j]*10+0.5)/10.0);
                if(((j+1) % 20) == 0) {
                    printf("\n");
                }
            }
            free(values);

            printf("]");

            printf("}\n");

            grib_handle_delete(h);
        }
        fclose(f);
        if(err)
        {
            fprintf(stderr,"%s\n",grib_get_error_message(err));
            exit(1);
        }
    }
    printf("]\n");
    return 0;
}
