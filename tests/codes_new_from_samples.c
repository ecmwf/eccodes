/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <assert.h>
#include "eccodes.h"

typedef struct sample_t {
    const char*  sample_name;
    ProductKind  expected_kind;
} sample_t;

static sample_t samples[] = {
    {"GRIB1", PRODUCT_GRIB},
    {"GRIB2", PRODUCT_GRIB},
    {"reduced_gg_pl_256_grib1", PRODUCT_GRIB},
    {"reduced_gg_pl_256_grib2", PRODUCT_GRIB},
    {"sh_ml_grib1.tmpl", PRODUCT_GRIB},
    {"sh_ml_grib2.tmpl", PRODUCT_GRIB},

    {"diag", PRODUCT_GRIB}, /* pseudo GRIBs */
    {"budg", PRODUCT_GRIB},

    {"BUFR4_local_satellite", PRODUCT_BUFR},
    {"BUFR4_local", PRODUCT_BUFR},
    {"BUFR4", PRODUCT_BUFR},
    {"BUFR3", PRODUCT_BUFR}
};

#define NUMBER(a) sizeof(a)/sizeof(a[0])

int main(int argc, char** argv)
{
    codes_handle* h = NULL;
    size_t i = 0;

    for (i=0; i<NUMBER(samples); ++i) {
        const char* name = samples[i].sample_name;
        char kindstr[254] = {0,};
        size_t len = 254;

        printf("Testing codes_handle_new_from_samples on %s\n", name);
        h = codes_handle_new_from_samples(0, name);
        assert(h);

        CODES_CHECK(codes_get_string(h, "kindOfProduct", kindstr, &len), 0);
        if (samples[i].expected_kind == PRODUCT_GRIB) {
            assert( strcmp(kindstr, "GRIB")==0 ||
                    strcmp(kindstr, "DIAG")==0 ||
                    strcmp(kindstr, "BUDG")==0 );
        }
        if (samples[i].expected_kind == PRODUCT_BUFR) {
            assert( strcmp(kindstr, "BUFR")==0 );
        }
        codes_handle_delete(h);
    }

    fprintf(stderr,"All done\n");
    return 0;
}
