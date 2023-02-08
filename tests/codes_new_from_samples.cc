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

typedef struct sample_t {
    const char*  sample_name;
    const char*  expected_ID;
    ProductKind  expected_kind;
} sample_t;

static sample_t samples[] = {
    {"GRIB1",                   "GRIB", PRODUCT_GRIB},
    {"GRIB2",                   "GRIB", PRODUCT_GRIB},
    {"reduced_gg_pl_256_grib1", "GRIB", PRODUCT_GRIB},
    {"reduced_gg_pl_256_grib2", "GRIB", PRODUCT_GRIB},
    {"sh_ml_grib1.tmpl",        "GRIB", PRODUCT_GRIB},
    {"sh_ml_grib2.tmpl",        "GRIB", PRODUCT_GRIB},

    {"diag", "DIAG", PRODUCT_GRIB}, /* pseudo GRIBs */
    {"budg", "BUDG", PRODUCT_GRIB},

    {"BUFR4_local_satellite", "BUFR", PRODUCT_BUFR},
    {"BUFR4_local",           "BUFR", PRODUCT_BUFR},
    {"BUFR4",                 "BUFR", PRODUCT_BUFR},
    {"BUFR3",                 "BUFR", PRODUCT_BUFR}
};

#define NUMBER(a) sizeof(a)/sizeof(a[0])

int main(int argc, char** argv)
{
    grib_handle* h = NULL;
    size_t i = 0;

    for (i=0; i<NUMBER(samples); ++i) {
        const char* name = samples[i].sample_name;
        char identifier[254] = {0,};
        size_t len = 254;

        printf("Testing codes_handle_new_from_samples on %s\n", name);
        h = codes_handle_new_from_samples(0, name);
        Assert(h);
        Assert(samples[i].expected_kind == h->product_kind);

        GRIB_CHECK(grib_get_string(h, "identifier", identifier, &len), 0);
        Assert( strcmp(samples[i].expected_ID, identifier) == 0 );
        grib_handle_delete(h);
    }

    fprintf(stderr,"All done\n");
    return 0;
}
