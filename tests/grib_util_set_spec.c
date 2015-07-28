/*
 * Copyright 2005-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "eccodes.h"
#include <assert.h>

void test1(const char* input_filename, const char* output_filename)
{
    /* based on copy_spec_from_ksec */
    int err = 0;
    size_t inlen = 0;
    size_t outlen = 0;
    size_t size=0;
    int set_spec_flags=0;
    double* values = NULL;
    FILE* in = NULL;
    FILE* out = NULL;
    const void* buffer = NULL;

    codes_handle *handle = 0;
    codes_handle *finalh = 0;
    grib_util_grid_spec spec={0,};
    grib_util_packing_spec packing_spec={0,};

    in = fopen(input_filename,"r");     assert(in);
    out = fopen(output_filename,"w");   assert(out);

    handle = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    assert(handle);
    CODES_CHECK(codes_get_size(handle,"values",&inlen), 0);
    values = (double*)malloc(sizeof(double)*inlen);
    GRIB_CHECK(codes_get_double_array(handle, "values", values,&inlen), 0);

    spec.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;
    spec.Nj = 14;
    spec.Ni = 17;
    outlen = spec.Nj * spec.Ni;
    spec.iDirectionIncrementInDegrees = 1.5;
    spec.jDirectionIncrementInDegrees = 1.5;
    spec.latitudeOfFirstGridPointInDegrees  = 60.0;
    spec.longitudeOfFirstGridPointInDegrees = -9.0;
    spec.latitudeOfLastGridPointInDegrees   = 40.5;
    spec.longitudeOfLastGridPointInDegrees  = 15.0;
    spec.bitmapPresent = 0;

    packing_spec.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
    packing_spec.bitsPerValue = 24;
    packing_spec.accuracy=GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
    packing_spec.packing=GRIB_UTIL_PACKING_USE_PROVIDED;

    finalh = codes_grib_util_set_spec(
            handle,
            &spec,
            &packing_spec,
            set_spec_flags,
            values,
            outlen,
            &err);
    assert(finalh);
    assert(err == 0);

    /* Write out the message to the output file */
    CODES_CHECK(codes_get_message(finalh, &buffer, &size),0);
    if(fwrite(buffer,1,size,out) != size) {
        assert(0);
    }
    codes_handle_delete(handle);
    codes_handle_delete(finalh);
}

int main(int argc, char *argv[])
{
    const char *infile = argv[1];
    const char *outfile = argv[2];

    test1(infile, outfile);

    return 0;
}
