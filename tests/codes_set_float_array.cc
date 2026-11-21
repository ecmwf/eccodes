/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// codes_set_float_array must behave like codes_set_double_array for every key.
//
// usage: codes_set_float_array <grib_file>
//
// 1. Sweep: for every key of every message, set the key to its own value (rounded to float)
//    with codes_set_double_array on one clone and with codes_set_float_array on another clone.
//    The return codes, the resulting messages and the value read back (as double and as float)
//    must be the same.
// 2. Bitmap: missing values are recognised, also when missingValue is not exactly
//    representable as a float (a float client then uses (float)missingValue).
// 3. A transient key set as float is seen by double and long readers.
// 4. Reading floats after setting floats gives the new values (no stale cache).

#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include "eccodes.h"
#include "grib_api_internal.h"

static bool same_message(codes_handle* a, codes_handle* b)
{
    const void *ma = NULL, *mb = NULL;
    size_t sa = 0, sb = 0;
    CODES_CHECK(codes_get_message(a, &ma, &sa), 0);
    CODES_CHECK(codes_get_message(b, &mb, &sb), 0);
    return sa == sb && memcmp(ma, mb, sa) == 0;
}

// Values as floats. Not every packing type can decode floats directly: use doubles then.
static std::vector<float> get_float_values(codes_handle* h)
{
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    std::vector<float> v(n);
    size_t len = n;
    if (codes_get_float_array(h, "values", v.data(), &len) != 0) {
        std::vector<double> d(n);
        len = n;
        CODES_CHECK(codes_get_double_array(h, "values", d.data(), &len), 0);
        for (size_t i = 0; i < n; ++i)
            v[i] = (float)d[i];
    }
    return v;
}

static void sweep(codes_handle* h, size_t* nkeys, size_t* nset)
{
    std::vector<std::string> names;
    codes_keys_iterator* kiter = codes_keys_iterator_new(h, 0, NULL);
    ECCODES_ASSERT(kiter);
    while (codes_keys_iterator_next(kiter))
        names.push_back(codes_keys_iterator_get_name(kiter));
    codes_keys_iterator_delete(kiter);

    for (const auto& name : names) {
        const char* key = name.c_str();
        int type = 0;
        size_t n = 0;
        if (codes_get_native_type(h, key, &type) != 0) continue;
        if (type != CODES_TYPE_DOUBLE && type != CODES_TYPE_LONG) continue;
        if (codes_get_size(h, key, &n) != 0 || n == 0) continue;

        std::vector<double> dv(n);
        size_t nd = n;
        if (codes_get_double_array(h, key, dv.data(), &nd) != 0) continue;

        // The same values for both: floats, and these floats widened to double
        std::vector<float> fv(nd);
        bool finite = true;
        for (size_t i = 0; i < nd; ++i) {
            fv[i] = (float)dv[i];
            dv[i] = (double)fv[i];
            if (!std::isfinite(fv[i])) finite = false;
        }
        if (!finite) continue;
        ++(*nkeys);

        codes_handle* hd = codes_handle_clone(h);
        codes_handle* hf = codes_handle_clone(h);
        ECCODES_ASSERT(hd && hf);
        const int ed = codes_set_double_array(hd, key, dv.data(), nd);
        const int ef = codes_set_float_array(hf, key, fv.data(), nd);
        if (ed != ef) {
            fprintf(stderr, "key %s: set double -> %s, set float -> %s\n", key,
                    codes_get_error_message(ed), codes_get_error_message(ef));
            ECCODES_ASSERT(!"different return codes");
        }
        if (ed == CODES_SUCCESS) {
            ++(*nset);
            if (!same_message(hd, hf)) {
                fprintf(stderr, "key %s: messages differ after set double / set float\n", key);
                ECCODES_ASSERT(!"different messages");
            }
            // Read back as double: also catches values kept outside the message (transients)
            std::vector<double> bd(nd), bf(nd);
            size_t s1 = nd, s2 = nd;
            const int e1 = codes_get_double_array(hd, key, bd.data(), &s1);
            const int e2 = codes_get_double_array(hf, key, bf.data(), &s2);
            ECCODES_ASSERT(e1 == e2);
            if (e1 == 0) {
                ECCODES_ASSERT(s1 == s2);
                for (size_t i = 0; i < s1; ++i) {
                    if (bd[i] != bf[i] && !(std::isnan(bd[i]) && std::isnan(bf[i]))) {
                        fprintf(stderr, "key %s[%zu]: after set double=%.17g, after set float=%.17g\n", key, i, bd[i], bf[i]);
                        ECCODES_ASSERT(!"different values read back");
                    }
                }
            }
        }
        codes_handle_delete(hd);
        codes_handle_delete(hf);
    }
}

// Set values with missing points on a clone with a bitmap. Returns numberOfMissing.
template <typename T>
static long set_with_missing(codes_handle* h, double missingValue, codes_handle** out)
{
    codes_handle* c = codes_handle_clone(h);
    size_t n = 0;
    CODES_CHECK(codes_get_size(c, "values", &n), 0);
    const std::vector<float> f = get_float_values(c);
    std::vector<T> v(f.begin(), f.end());
    CODES_CHECK(codes_set_double(c, "missingValue", missingValue), 0);
    CODES_CHECK(codes_set_long(c, "bitmapPresent", 1), 0);
    for (size_t i = 0; i < n; i += 3)
        v[i] = (T)missingValue;
    if (sizeof(T) == sizeof(float))
        CODES_CHECK(codes_set_float_array(c, "values", (const float*)v.data(), n), 0);
    else
        CODES_CHECK(codes_set_double_array(c, "values", (const double*)v.data(), n), 0);
    long numberOfMissing = -1;
    CODES_CHECK(codes_get_long(c, "numberOfMissing", &numberOfMissing), 0);
    *out = c;
    return numberOfMissing;
}

static void check_bitmap(codes_handle* h)
{
    long isSpectral = 0;
    if (codes_get_long(h, "isSpectral", &isSpectral) == 0 && isSpectral) return;
    size_t n = 0;
    CODES_CHECK(codes_get_size(h, "values", &n), 0);
    if (n < 6) return;

    // 9999 is exact in float; 1e20 and 1e-3 are not
    const double missing[] = { 9999, 1e20, 1e-3 };
    for (double m : missing) {
        codes_handle *hd = NULL, *hf = NULL;
        const long nd = set_with_missing<double>(h, m, &hd);
        const long nf = set_with_missing<float>(h, m, &hf);
        const long expected = (long)((n + 2) / 3);
        if (nd != expected || nf != expected) {
            fprintf(stderr, "missingValue=%g: numberOfMissing double=%ld float=%ld expected=%ld\n", m, nd, nf, expected);
            ECCODES_ASSERT(!"missing values not recognised");
        }
        // Same bitmap; the coded values agree within the packing error
        long cd = 0, cf = 0;
        CODES_CHECK(codes_get_long(hd, "numberOfCodedValues", &cd), 0);
        CODES_CHECK(codes_get_long(hf, "numberOfCodedValues", &cf), 0);
        ECCODES_ASSERT(cd == cf);
        size_t bl = n, bl2 = n;
        std::vector<long> b1(n), b2(n);
        CODES_CHECK(codes_get_long_array(hd, "bitmap", b1.data(), &bl), 0);
        CODES_CHECK(codes_get_long_array(hf, "bitmap", b2.data(), &bl2), 0);
        ECCODES_ASSERT(bl == bl2 && b1 == b2);
        codes_handle_delete(hd);
        codes_handle_delete(hf);
    }
}

static void check_transient(codes_handle* h)
{
    codes_handle* c = codes_handle_clone(h);
    const float f = 1234.5f;
    double d = 0;
    float f2 = 0;
    CODES_CHECK(codes_set_float_array(c, "missingValue", &f, 1), 0);
    CODES_CHECK(codes_get_double(c, "missingValue", &d), 0);
    CODES_CHECK(codes_get_float(c, "missingValue", &f2), 0);
    ECCODES_ASSERT(d == 1234.5);
    ECCODES_ASSERT(f2 == 1234.5f);
    // and the other way round
    CODES_CHECK(codes_set_double(c, "missingValue", 4321.25), 0);
    CODES_CHECK(codes_get_float(c, "missingValue", &f2), 0);
    ECCODES_ASSERT(f2 == 4321.25f);
    codes_handle_delete(c);
}

static void check_no_stale_cache(codes_handle* h)
{
    // Not for spectral fields: some coefficients are zero by definition (imaginary part for m=0)
    long isSpectral = 0;
    if (codes_get_long(h, "isSpectral", &isSpectral) == 0 && isSpectral) return;
    codes_handle* c = codes_handle_clone(h);
    size_t n = 0;
    CODES_CHECK(codes_get_size(c, "values", &n), 0);
    std::vector<float> v = get_float_values(c);  // fills a cache, if any
    std::vector<float> w;
    size_t len = n;
    double range = 0;
    for (size_t i = 0; i < n; ++i) {
        range = std::max(range, std::fabs((double)v[i]));
        v[i] = v[i] + (float)((i % 7) * 10.0 + 100.0);
    }
    if (codes_set_float_array(c, "values", v.data(), n) == 0) {
        w = get_float_values(c);
        // The decoded values must be the new ones (within a generous packing error)
        for (size_t i = 0; i < n; ++i) {
            if (std::fabs((double)w[i] - (double)v[i]) > 50.0 + 1e-2 * range) {
                fprintf(stderr, "values[%zu]: set %g, read back %g\n", i, v[i], w[i]);
                ECCODES_ASSERT(!"stale values after codes_set_float_array");
            }
        }
    }
    codes_handle_delete(c);
}

int main(int argc, char** argv)
{
    ECCODES_ASSERT(argc == 2);
    int err = 0;
    FILE* in = fopen(argv[1], "rb");
    ECCODES_ASSERT(in);
    codes_handle* h = NULL;
    int count = 0;
    size_t nkeys = 0, nset = 0;
    while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL && count < 3) {
        ++count;
        sweep(h, &nkeys, &nset);
        check_bitmap(h);
        check_transient(h);
        check_no_stale_cache(h);
        codes_handle_delete(h);
    }
    if (h) codes_handle_delete(h);
    fclose(in);
    ECCODES_ASSERT(count > 0);
    printf("%s: %d messages, %zu numeric keys compared, %zu set successfully\n", argv[1], count, nkeys, nset);
    return 0;
}
