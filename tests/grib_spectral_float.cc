/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// Single-precision get/set of "values" for spectral fields
// (spectral_simple and spectral_complex, GRIB1 and GRIB2).
//
// Checks:
//  1. codes_get_float_array matches codes_get_double_array within float precision
//  2. Encoding with codes_set_float_array gives exactly the same message as
//     encoding the same values (widened to double) with codes_set_double_array
//  3. Decoding the float-encoded message gives the same values as the original
//     field within the packing error
//  4. Error cases: wrong array size, array too small on decode

#include <cfloat>
#include <cmath>
#include <cstring>
#include <vector>
#include "eccodes.h"
#include "grib_api_internal.h"

static void check_get_float_vs_double(codes_handle* h)
{
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    ECCODES_ASSERT(n > 1);

    std::vector<float> fv(n);
    std::vector<double> dv(n);
    size_t nf = n, nd = n;
    CODES_CHECK(codes_get_float_array(h, "values", fv.data(), &nf), 0);
    CODES_CHECK(codes_get_double_array(h, "values", dv.data(), &nd), 0);
    ECCODES_ASSERT(nf == n && nd == n);

    double maxabs = 0;
    for (size_t i = 0; i < n; ++i)
        maxabs = std::max(maxabs, std::fabs(dv[i]));
    for (size_t i = 0; i < n; ++i) {
        const double diff = std::fabs((double)fv[i] - dv[i]);
        // float has a 24-bit mantissa; allow a few ulps relative to the value
        // and a tiny absolute error relative to the largest coefficient
        const double tol = 4 * FLT_EPSILON * std::fabs(dv[i]) + 1e-6 * FLT_EPSILON * maxabs;
        if (diff > tol) {
            fprintf(stderr, "get float vs double: i=%zu float=%.9g double=%.17g diff=%g tol=%g\n",
                    i, fv[i], dv[i], diff, tol);
            ECCODES_ASSERT(!"float and double values differ");
        }
    }

    // Buffer too small
    size_t small = n - 1;
    ECCODES_ASSERT(codes_get_float_array(h, "values", fv.data(), &small) == CODES_ARRAY_TOO_SMALL);
}

static void check_set_float_vs_double(codes_handle* h)
{
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "values", &n), 0);

    std::vector<float> fv(n);
    size_t nf = n;
    CODES_CHECK(codes_get_float_array(h, "values", fv.data(), &nf), 0);
    // Modify the field a little, so the encoder really has to repack
    for (size_t i = 0; i < n; ++i)
        fv[i] = fv[i] * 1.5f;
    std::vector<double> dv(fv.begin(), fv.end());  // exact widening

    codes_handle* hf = codes_handle_clone(h);
    codes_handle* hd = codes_handle_clone(h);
    ECCODES_ASSERT(hf && hd);

    CODES_CHECK(codes_set_float_array(hf, "values", fv.data(), n), 0);
    CODES_CHECK(codes_set_double_array(hd, "values", dv.data(), n), 0);

    const void *mf = NULL, *md = NULL;
    size_t sf = 0, sd = 0;
    CODES_CHECK(codes_get_message(hf, &mf, &sf), 0);
    CODES_CHECK(codes_get_message(hd, &md, &sd), 0);
    if (sf != sd || memcmp(mf, md, sf) != 0) {
        fprintf(stderr, "Messages differ: float-encoded size=%zu double-encoded size=%zu\n", sf, sd);
        ECCODES_ASSERT(!"set float and set double give different messages");
    }

    // Decode the float-encoded message again and compare with the input
    codes_handle* h2 = codes_handle_new_from_message_copy(0, mf, sf);
    ECCODES_ASSERT(h2);
    std::vector<float> back(n);
    size_t nb = n;
    CODES_CHECK(codes_get_float_array(h2, "values", back.data(), &nb), 0);
    ECCODES_ASSERT(nb == n);
    double maxabs = 0, maxdiff = 0;
    for (size_t i = 0; i < n; ++i) {
        maxabs = std::max(maxabs, std::fabs((double)fv[i]));
        maxdiff = std::max(maxdiff, std::fabs((double)back[i] - (double)fv[i]));
    }
    // Lossy packing: the error must be small relative to the field
    if (maxdiff > 1e-2 * maxabs) {
        fprintf(stderr, "Round trip error too large: maxdiff=%g maxabs=%g\n", maxdiff, maxabs);
        ECCODES_ASSERT(!"round trip error too large");
    }

    // Wrong number of values: float and double must behave the same
    // (spectral_complex rejects it, spectral_simple does not check the truncation)
    {
        codes_handle* hf2 = codes_handle_clone(h);
        codes_handle* hd2 = codes_handle_clone(h);
        const int ef = codes_set_float_array(hf2, "values", fv.data(), n - 2);
        const int ed = codes_set_double_array(hd2, "values", dv.data(), n - 2);
        if (ef != ed) {
            fprintf(stderr, "Wrong size: float err=%d double err=%d\n", ef, ed);
            ECCODES_ASSERT(!"float and double differ for wrong array size");
        }
        codes_handle_delete(hf2);
        codes_handle_delete(hd2);
    }

    codes_handle_delete(h2);
    codes_handle_delete(hf);
    codes_handle_delete(hd);
}

int main(int argc, char** argv)
{
    ECCODES_ASSERT(argc == 2);
    const char* filename = argv[1];
    int err = 0;

    FILE* in = fopen(filename, "rb");
    ECCODES_ASSERT(in);

    codes_handle* h = NULL;
    int count = 0;
    while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL) {
        ECCODES_ASSERT(err == 0);
        char packingType[64] = { 0 };
        size_t len = sizeof(packingType);
        long edition = 0;
        CODES_CHECK(codes_get_string(h, "packingType", packingType, &len), 0);
        CODES_CHECK(codes_get_long(h, "edition", &edition), 0);
        printf("%s: message %d: edition=%ld packingType=%s\n", filename, ++count, edition, packingType);

        check_get_float_vs_double(h);
        check_set_float_vs_double(h);

        codes_handle_delete(h);
    }
    ECCODES_ASSERT(count > 0);
    fclose(in);
    return 0;
}
