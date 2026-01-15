/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#undef NDEBUG
#include <assert.h>
#include "eccodes.h"
#include <sys/stat.h>

int main(int argc, char** argv)
{
    int err               = 0;
    size_t len            = 0;
    grib_context* ctx     = grib_context_get_default();

    assert(argc == 5);

    const char* in_filename  = argv[1];  // CCSDS field - not constant
    const char* in_sample    = argv[2];  // CCSDS sample - constant field
    const char* out_binfile  = argv[3];  // binary file with raw bytes
    const char* out_filename = argv[4];  // final output

    FILE* in = fopen(in_filename, "rb");
    assert(in);
    codes_handle* h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    assert(h);
    fclose(in);

    // TODO assert grib edition == 2
    // TODO check isGridded == 1
    // TODO check packingType == grid_ccsds
    // TODO check NO BITMAP

    // Get meta-data from input field
    double referenceValue = 0;
    long bitsPerValue = 0, ccsdsFlags = 0, numberOfValues = 0, numberOfDataPoints = 0;
    long binaryScaleFactor = 0, decimalScaleFactor = 0, ccsdsBlockSize = 0, ccsdsRsi = 0;
    GRIB_CHECK(codes_get_double(h, "referenceValue", &referenceValue), 0);
    GRIB_CHECK(codes_get_long(h, "binaryScaleFactor", &binaryScaleFactor), 0);
    GRIB_CHECK(codes_get_long(h, "decimalScaleFactor", &decimalScaleFactor), 0);
    GRIB_CHECK(codes_get_long(h, "bitsPerValue", &bitsPerValue), 0);
    GRIB_CHECK(codes_get_long(h, "ccsdsFlags", &ccsdsFlags), 0);
    GRIB_CHECK(codes_get_long(h, "ccsdsBlockSize", &ccsdsBlockSize), 0);
    GRIB_CHECK(codes_get_long(h, "ccsdsRsi", &ccsdsRsi), 0);
    GRIB_CHECK(codes_get_long(h, "numberOfValues", &numberOfValues), 0);
    GRIB_CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints), 0);

    // Get input field values as bytes
    unsigned char* byte_val = NULL;
    size_t blen             = 0;
    long sec7Len            = -1;

    // CODES_CHECK(codes_get_length(h, "codedValues", &klen), 0); // no good, always 1025!
    CODES_CHECK(codes_get_long(h, "section7Length", &sec7Len), 0); //grib2 only
    printf("1. codes_get_length of sec7 = %zu\n", sec7Len);

    blen     = sec7Len;
    byte_val = (unsigned char*)malloc(blen * sizeof(unsigned char));
    GRIB_CHECK(codes_get_bytes(h, "codedValues", byte_val, &blen), 0);//magic
    printf("1. codes_get_bytes blen = %zu\n", blen);

    FILE* fout = fopen(out_binfile, "wb");
    if (fwrite(byte_val, 1, blen, fout) != blen) {
        perror("output");
        exit(1);
    }
    printf("Wrote out %s\n", out_binfile);
    fclose(fout);

    // Read direct from bytes in a file
    struct stat finfo;
    size_t fsize        = 0;
    unsigned char* data = NULL;
    unsigned char* p    = NULL;

    FILE* f = fopen(out_binfile, "rb");
    stat(out_binfile, &finfo);
    fsize = finfo.st_size;
    printf("Read bin file: fsize=%zu\n", fsize);

    data = (unsigned char*)malloc(fsize);
    p    = data;
    if (fread(data, 1, fsize, f) != fsize) {
        perror(out_binfile);
        exit(1);
    }
    fclose(f);
    len = fsize;
    in = fopen(in_sample, "rb");
    assert(in);
    codes_handle* h1 = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    assert(h1);
    len = fsize;
    grib_context_set_debug(ctx, -1);
    GRIB_CHECK(codes_set_long(h1, "bitsPerValue", bitsPerValue), 0);
    GRIB_CHECK(codes_set_long(h1, "decimalScaleFactor", decimalScaleFactor), 0);

    // Normally one cannot set the read_only keys like referenceValue and binaryScaleFactor.
    // e.g., GRIB_CHECK( grib_set_double(h1, "referenceValue", referenceValue), 0);
    // Workaround: Resort to trick and force it via grib_set_force_double_array
    grib_set_force_double_array(h1, "referenceValue", &referenceValue, 1);
    double bsf = binaryScaleFactor;
    grib_set_force_double_array(h1, "binaryScaleFactor", &bsf, 1);

    GRIB_CHECK(codes_set_long(h1, "ccsdsFlags", ccsdsFlags), 0);
    GRIB_CHECK(codes_set_long(h1, "ccsdsBlockSize", ccsdsBlockSize), 0);
    GRIB_CHECK(codes_set_long(h1, "ccsdsRsi", ccsdsRsi), 0);
    GRIB_CHECK(codes_set_long(h1, "numberOfValues", numberOfValues), 0);
    GRIB_CHECK(codes_set_long(h1, "numberOfDataPoints", numberOfDataPoints), 0);
    GRIB_CHECK(codes_set_bytes(h1, "codedValues", p, &len), 0);//magic

    CODES_CHECK(codes_write_message(h1, out_filename, "w"), 0);
    grib_context_set_debug(ctx, 0);
    codes_handle_delete(h1);

    free(data);
    free(byte_val);
    codes_handle_delete(h);

    return 0;
}
