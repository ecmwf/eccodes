/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// Single-precision access to BUFR numeric keys:
//  1. For every numeric data key: codes_get_float_array == (float)codes_get_double_array,
//     missing values map to CODES_MISSING_FLOAT. Same for codes_get_float on scalars,
//     for "numericValues" and for '/subsetNumber=N/' access.
//  2. Encoding: setting data keys with codes_set_float_array gives the same message
//     as setting the same values with codes_set_double_array.
//  3. CODES_MISSING_FLOAT sets a value to missing.

#include <cfloat>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include "eccodes.h"
#include "grib_api_internal.h"

static float expected_float(double d)
{
    return d == CODES_MISSING_DOUBLE ? CODES_MISSING_FLOAT : (float)d;
}

static double float_to_double(float f)
{
    return f == CODES_MISSING_FLOAT ? CODES_MISSING_DOUBLE : (double)f;
}

static bool is_numeric(codes_handle* h, const char* key)
{
    int type = 0;
    if (codes_get_native_type(h, key, &type) != CODES_SUCCESS)
        return false;
    return type == CODES_TYPE_DOUBLE || type == CODES_TYPE_LONG;
}

// Compare float and double access for one key. Returns number of values compared.
static size_t compare_key(codes_handle* h, const char* key)
{
    size_t n = 0;
    if (codes_get_size(h, key, &n) != CODES_SUCCESS || n == 0)
        return 0;

    std::vector<double> dv(n);
    std::vector<float> fv(n);
    size_t nd = n, nf = n;
    if (codes_get_double_array(h, key, dv.data(), &nd) != CODES_SUCCESS)
        return 0;  // not readable as double: nothing to compare

    bool out_of_range = false;
    for (size_t i = 0; i < nd; ++i)
        if (dv[i] != CODES_MISSING_DOUBLE && std::isfinite(dv[i]) && std::fabs(dv[i]) > FLT_MAX)
            out_of_range = true;

    const int err = codes_get_float_array(h, key, fv.data(), &nf);
    if (out_of_range) {
        ECCODES_ASSERT(err == CODES_OUT_OF_RANGE);
        return 0;
    }
    if (err != CODES_SUCCESS) {
        fprintf(stderr, "codes_get_float_array failed for '%s': %s\n", key, codes_get_error_message(err));
        ECCODES_ASSERT(!"codes_get_float_array failed");
    }
    ECCODES_ASSERT(nf == nd);
    for (size_t i = 0; i < nd; ++i) {
        const float e = expected_float(dv[i]);
        if (!(fv[i] == e || (std::isnan(fv[i]) && std::isnan(e)))) {
            fprintf(stderr, "key '%s'[%zu]: float=%.9g expected=%.9g (double=%.17g)\n", key, i, fv[i], e, dv[i]);
            ECCODES_ASSERT(!"float and double values differ");
        }
    }

    if (nd == 1) {
        float f = 0;
        CODES_CHECK(codes_get_float(h, key, &f), 0);
        ECCODES_ASSERT(f == expected_float(dv[0]));
    }
    return nd;
}

static void check_decode(codes_handle* h)
{
    size_t total = 0, nkeys = 0;
    codes_bufr_keys_iterator* kiter = codes_bufr_keys_iterator_new(h, 0);
    ECCODES_ASSERT(kiter);
    while (codes_bufr_keys_iterator_next(kiter)) {
        const char* name = codes_bufr_keys_iterator_get_name(kiter);
        if (!is_numeric(h, name)) continue;
        total += compare_key(h, name);
        ++nkeys;
    }
    codes_bufr_keys_iterator_delete(kiter);

    total += compare_key(h, "numericValues");

    // Access with a subset condition (accessors list)
    long nsubsets = 0;
    CODES_CHECK(codes_get_long(h, "numberOfSubsets", &nsubsets), 0);
    if (nsubsets > 1) {
        const char* key = "/subsetNumber=1/latitude";
        size_t n = 0;
        if (codes_get_size(h, key, &n) == CODES_SUCCESS && n > 0)
            total += compare_key(h, key);
    }
    printf("  decode: %zu numeric keys, %zu values compared\n", nkeys, total);
}

// Collect up to 'maxkeys' data keys of type double that can be read and written
static std::vector<std::string> data_keys(codes_handle* h, size_t maxkeys)
{
    std::vector<std::string> keys;
    codes_bufr_keys_iterator* kiter = codes_bufr_keys_iterator_new(h, CODES_KEYS_ITERATOR_SKIP_READ_ONLY);
    while (codes_bufr_keys_iterator_next(kiter) && keys.size() < maxkeys) {
        const char* name = codes_bufr_keys_iterator_get_name(kiter);
        int type = 0;
        if (name[0] != '#') continue;      // data section keys only
        if (strstr(name, "->")) continue;  // skip attributes
        if (codes_get_native_type(h, name, &type) || type != CODES_TYPE_DOUBLE) continue;
        keys.push_back(name);
    }
    codes_bufr_keys_iterator_delete(kiter);
    return keys;
}

static void check_encode(codes_handle* h)
{
    codes_handle* hf = codes_handle_clone(h);
    codes_handle* hd = codes_handle_clone(h);
    CODES_CHECK(codes_set_long(hf, "unpack", 1), 0);
    CODES_CHECK(codes_set_long(hd, "unpack", 1), 0);

    std::vector<std::string> keys = data_keys(hf, 50);
    size_t nset = 0;
    for (const auto& key : keys) {
        size_t n = 0;
        CODES_CHECK(codes_get_size(hf, key.c_str(), &n), 0);
        std::vector<float> fv(n);
        size_t nf = n;
        if (codes_get_float_array(hf, key.c_str(), fv.data(), &nf) != CODES_SUCCESS) continue;
        // Change the non-missing values a little
        for (auto& v : fv)
            if (v != CODES_MISSING_FLOAT) v = v * 0.5f;
        std::vector<double> dv(nf);
        for (size_t i = 0; i < nf; ++i)
            dv[i] = float_to_double(fv[i]);
        const int ef = codes_set_float_array(hf, key.c_str(), fv.data(), nf);
        const int ed = codes_set_double_array(hd, key.c_str(), dv.data(), nf);
        ECCODES_ASSERT(ef == ed);
        if (ef == CODES_SUCCESS) ++nset;
    }
    // Scaling can push values out of the coding range: then both must fail the same way
    const int pf = codes_set_long(hf, "pack", 1);
    const int pd = codes_set_long(hd, "pack", 1);
    ECCODES_ASSERT(pf == pd);
    if (pf != CODES_SUCCESS) {
        printf("  encode: %zu keys set, packing failed for float and double alike (%s)\n", nset, codes_get_error_message(pf));
        codes_handle_delete(hf);
        codes_handle_delete(hd);
        return;
    }

    const void *mf = NULL, *md = NULL;
    size_t sf = 0, sd = 0;
    CODES_CHECK(codes_get_message(hf, &mf, &sf), 0);
    CODES_CHECK(codes_get_message(hd, &md, &sd), 0);
    if (sf != sd || memcmp(mf, md, sf) != 0) {
        fprintf(stderr, "Messages differ: float size=%zu double size=%zu\n", sf, sd);
        ECCODES_ASSERT(!"set float and set double give different messages");
    }
    printf("  encode: %zu keys set, messages identical (%zu bytes)\n", nset, sf);

    // Missing value
    if (!keys.empty()) {
        const char* key = keys[0].c_str();
        size_t n = 0;
        CODES_CHECK(codes_get_size(hf, key, &n), 0);
        std::vector<float> miss(n, CODES_MISSING_FLOAT);
        CODES_CHECK(codes_set_long(hf, "unpack", 1), 0);
        CODES_CHECK(codes_set_float_array(hf, key, miss.data(), n), 0);
        CODES_CHECK(codes_set_long(hf, "pack", 1), 0);
        CODES_CHECK(codes_get_message(hf, &mf, &sf), 0);
        codes_handle* h2 = codes_handle_new_from_message_copy(0, mf, sf);
        CODES_CHECK(codes_set_long(h2, "unpack", 1), 0);
        std::vector<double> back(n);
        std::vector<float> fback(n);
        size_t nb = n, nfb = n;
        CODES_CHECK(codes_get_double_array(h2, key, back.data(), &nb), 0);
        CODES_CHECK(codes_get_float_array(h2, key, fback.data(), &nfb), 0);
        for (size_t i = 0; i < nb; ++i) {
            ECCODES_ASSERT(back[i] == CODES_MISSING_DOUBLE);
            ECCODES_ASSERT(fback[i] == CODES_MISSING_FLOAT);
        }
        printf("  missing: '%s' set to CODES_MISSING_FLOAT -> missing after encode/decode\n", key);
        codes_handle_delete(h2);
    }

    codes_handle_delete(hf);
    codes_handle_delete(hd);
}

int main(int argc, char** argv)
{
    ECCODES_ASSERT(argc == 2);
    const char* filename = argv[1];
    int err = 0;

    ECCODES_ASSERT(CODES_MISSING_FLOAT == -FLT_MAX);

    FILE* in = fopen(filename, "rb");
    ECCODES_ASSERT(in);
    codes_handle* h = NULL;
    int count = 0;
    while ((h = codes_handle_new_from_file(0, in, PRODUCT_BUFR, &err)) != NULL) {
        ECCODES_ASSERT(err == 0);
        long compressed = 0;
        CODES_CHECK(codes_set_long(h, "unpack", 1), 0);
        CODES_CHECK(codes_get_long(h, "compressedData", &compressed), 0);
        printf("%s: message %d (compressed=%ld)\n", filename, ++count, compressed);
        check_decode(h);
        check_encode(h);
        codes_handle_delete(h);
    }
    ECCODES_ASSERT(count > 0);
    fclose(in);
    return 0;
}
