/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/*
 * Description: codes_set_float_array must behave exactly like codes_set_double_array with
 *              d[i] = (double)v[i]: the same return codes and a byte-identical message, for
 *              every packing (the environment selects the implementations: ECCODES_USE_SP and
 *              ECCODES_USE_SPAEC, one program run per mode). The values read back as floats
 *              must be the same after both sets. Also grib_set_force_float_array against
 *              grib_set_force_double_array on the READ-ONLY key codedValues, GRIB_NOT_FOUND for
 *              an unknown key and GRIB_READ_ONLY for codedValues.
 *              usage: grib_set_float_array file...
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eccodes.h"

static int failures = 0;

static void check(int ok, const char* what)
{
    if (!ok) {
        fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

static void compareFields(codes_handle* hf, codes_handle* hd, const char* what)
{
    const void* mf = NULL;
    const void* md = NULL;
    size_t sf = 0, sd = 0;
    const int errf = codes_get_message(hf, &mf, &sf);
    const int errd = codes_get_message(hd, &md, &sd);
    if (errf != GRIB_SUCCESS || errd != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL: %s: cannot get the messages (%d, %d)\n", what, errf, errd);
        ++failures;
        return;
    }
    if (sf != sd || memcmp(mf, md, sf) != 0) {
        fprintf(stderr, "FAIL: %s: the messages differ (%zu vs %zu bytes)\n", what, sf, sd);
        ++failures;
    }
}

static void compareFloatValues(codes_handle* hf, codes_handle* hd, const char* what)
{
    size_t nf = 0, nd = 0;
    if (codes_get_size(hf, "values", &nf) != GRIB_SUCCESS || codes_get_size(hd, "values", &nd) != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL: %s: cannot get the size of values\n", what);
        ++failures;
        return;
    }
    float* gf = (float*)malloc(nf * sizeof(float));
    float* gd = (float*)malloc(nd * sizeof(float));
    size_t lenf = nf, lend = nd;
    const int errf = codes_get_float_array(hf, "values", gf, &lenf);
    const int errd = codes_get_float_array(hd, "values", gd, &lend);
    if (errf != GRIB_SUCCESS || errd != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL: %s: cannot get the values as floats (%d, %d)\n", what, errf, errd);
        ++failures;
    }
    else if (lenf != lend || memcmp(gf, gd, lenf * sizeof(float)) != 0) {
        fprintf(stderr, "FAIL: %s: the values read back as floats differ\n", what);
        ++failures;
    }
    free(gf);
    free(gd);
}

/* Sets the values of one handle as floats and of its clone as doubles and compares the
   results. dsf: decimalScaleFactor to set before (< 0: keep), bpv: bitsPerValue to set before
   (0: keep), constant: set all values to the first one */
static void setBothWays(codes_handle* h, const char* file, const char* variant, int dsf, long bpv, int constant)
{
    codes_handle* hf = codes_handle_clone(h);
    codes_handle* hd = codes_handle_clone(h);
    if (!hf || !hd) {
        fprintf(stderr, "FAIL: %s %s: cannot clone\n", file, variant);
        ++failures;
        return;
    }

    if (dsf >= 0) {
        /* The decimal scale factor only changes the scaling of the decoding: the data stay consistent */
        CODES_CHECK(codes_set_long(hf, "decimalScaleFactor", dsf), 0);
        CODES_CHECK(codes_set_long(hd, "decimalScaleFactor", dsf), 0);
    }

    size_t n = 0;
    if (codes_get_size(hf, "values", &n) != GRIB_SUCCESS || n == 0) {
        /* no values (e.g. an empty sample): nothing to compare */
        codes_handle_delete(hf);
        codes_handle_delete(hd);
        return;
    }
    double* dval = (double*)malloc(n * sizeof(double));
    float* fval = (float*)malloc(n * sizeof(float));
    size_t len = n;
    if (codes_get_double_array(hf, "values", dval, &len) != GRIB_SUCCESS) {
        fprintf(stderr, "FAIL: %s %s: cannot read values\n", file, variant);
        ++failures;
        goto cleanup;
    }
    n = len;
    if (bpv > 0) {
        /* The bits per value is set after reading the values (the length of the data section
           would not match them) and before packing them again with the set */
        CODES_CHECK(codes_set_long(hf, "bitsPerValue", bpv), 0);
        CODES_CHECK(codes_set_long(hd, "bitsPerValue", bpv), 0);
    }
    /* The doubles are the floats converted: codes_set_float_array(v) == codes_set_double_array((double)v) */
    for (size_t i = 0; i < n; ++i) {
        fval[i] = (float)dval[i];
        dval[i] = (double)fval[i];
    }
    if (constant) {
        for (size_t i = 1; i < n; ++i) {
            dval[i] = dval[0];
            fval[i] = fval[0];
        }
    }

    {
        const int errf = codes_set_float_array(hf, "values", fval, n);
        const int errd = codes_set_double_array(hd, "values", dval, n);
        char what[512];
        snprintf(what, sizeof(what), "%s %s: return codes of the set (%d vs %d)", file, variant, errf, errd);
        check(errf == errd, what);
        if (errf == GRIB_SUCCESS && errd == GRIB_SUCCESS) {
            snprintf(what, sizeof(what), "%s %s: the message after the float set must equal the message after the double set", file, variant);
            compareFields(hf, hd, what);
            snprintf(what, sizeof(what), "%s %s: the values after the float set must equal the values after the double set", file, variant);
            compareFloatValues(hf, hd, what);
        }
    }

cleanup:
    free(dval);
    free(fval);
    codes_handle_delete(hf);
    codes_handle_delete(hd);
}

/* The error cases: an unknown key and the READ-ONLY key codedValues */
static void errorCases(codes_handle* h, const char* file)
{
    float fval[2] = { 1.0f, 2.0f };
    size_t n = 2;
    int err = codes_set_float_array(h, "iDontExistKey", fval, n);
    check(err == GRIB_NOT_FOUND, "an unknown key must give GRIB_NOT_FOUND");

    size_t nc = 0;
    if (codes_get_size(h, "codedValues", &nc) != GRIB_SUCCESS || nc == 0) {
        return; /* no codedValues (e.g. GRIB1 without a bitmap) */
    }
    double* cval = (double*)malloc(nc * sizeof(double));
    float* cfval = (float*)malloc(nc * sizeof(float));
    size_t clen = nc;
    if (codes_get_double_array(h, "codedValues", cval, &clen) != GRIB_SUCCESS) {
        free(cval);
        free(cfval);
        return;
    }
    nc = clen;
    for (size_t i = 0; i < nc; ++i) {
        cfval[i] = (float)cval[i];
    }

    err = codes_set_float_array(h, "codedValues", cfval, nc);
    if (err != GRIB_READ_ONLY) {
        fprintf(stderr, "FAIL: %s: codes_set_float_array of codedValues must give GRIB_READ_ONLY (got %d)\n", file, err);
        ++failures;
    }

    codes_handle* hf = codes_handle_clone(h);
    codes_handle* hd = codes_handle_clone(h);
    const int errf = codes_set_force_float_array(hf, "codedValues", cfval, nc);
    const int errd = codes_set_force_double_array(hd, "codedValues", cval, nc);
    char what[512];
    snprintf(what, sizeof(what), "%s: return codes of the forced set of codedValues (%d vs %d)", file, errf, errd);
    check(errf == errd, what);
    if (errf == GRIB_SUCCESS && errd == GRIB_SUCCESS) {
        snprintf(what, sizeof(what), "%s: the message after the forced float set must equal the message after the forced double set", file);
        compareFields(hf, hd, what);
    }
    codes_handle_delete(hf);
    codes_handle_delete(hd);
    free(cval);
    free(cfval);
}

static void checkFile(const char* file)
{
    FILE* f = fopen(file, "rb");
    if (!f) {
        perror(file);
        ++failures;
        return;
    }
    int err = 0;
    codes_handle* h = codes_handle_new_from_file(NULL, f, PRODUCT_GRIB, &err);
    fclose(f);
    if (!h) {
        fprintf(stderr, "FAIL: %s: cannot read the file (%s)\n", file, codes_get_error_message(err));
        ++failures;
        return;
    }

    setBothWays(h, file, "as loaded", -1, 0, 0);
    setBothWays(h, file, "decimalScaleFactor=2", 2, 0, 0);
    setBothWays(h, file, "bitsPerValue=24", -1, 24, 0);
    setBothWays(h, file, "constant", -1, 0, 1);
    setBothWays(h, file, "constant 24 bits", -1, 24, 1);
    errorCases(h, file);

    codes_handle_delete(h);
    if (failures == 0) {
        printf("OK %s\n", file);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s file...\n", argv[0]);
        return 1;
    }
    for (int i = 1; i < argc; ++i) {
        checkFile(argv[i]);
    }
    if (failures != 0) {
        fprintf(stderr, "%d failures\n", failures);
        return 1;
    }
    return 0;
}
