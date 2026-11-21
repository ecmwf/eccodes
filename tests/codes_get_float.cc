/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// Reading keys as float must agree with reading them as double, for every numeric key.
//
// usage: codes_get_float <grib_file>
//
// For every numeric key of the first messages:
//  - codes_get_float_array succeeds whenever codes_get_double_array does and gives the same
//    number of values
//  - scalars: the float is exactly (float)double (GRIB_MISSING_DOUBLE -> GRIB_MISSING_FLOAT)
//  - arrays: equal within single precision (data decoders may compute in float)
//  - a second read gives the same result, and a larger buffer gives the same length
//  - codes_get_float gives the first value

#include <cfloat>
#include <cmath>
#include <string>
#include <vector>
#include "eccodes.h"
#include "grib_api_internal.h"

static float expected_float(double d)
{
    return d == CODES_MISSING_DOUBLE ? CODES_MISSING_FLOAT : (float)d;
}

static size_t check_key(codes_handle* h, const char* key)
{
    int type = 0;
    size_t n = 0;
    if (codes_get_native_type(h, key, &type) != 0) return 0;
    if (type != CODES_TYPE_DOUBLE && type != CODES_TYPE_LONG) return 0;
    if (codes_get_size(h, key, &n) != 0 || n == 0) return 0;

    std::vector<double> dv(n);
    size_t nd = n;
    if (codes_get_double_array(h, key, dv.data(), &nd) != 0) return 0;

    double maxabs = 0;
    bool in_range = true;
    for (size_t i = 0; i < nd; ++i) {
        if (dv[i] == CODES_MISSING_DOUBLE || !std::isfinite(dv[i])) continue;
        maxabs = std::max(maxabs, std::fabs(dv[i]));
        if (std::fabs(dv[i]) > FLT_MAX) in_range = false;
    }

    std::vector<float> fv(n);
    size_t nf = n;
    const int err = codes_get_float_array(h, key, fv.data(), &nf);
    if (!in_range) {
        // The generic float reader reports the problem; native float decoders return inf
        if (err != CODES_OUT_OF_RANGE && err != 0) {
            fprintf(stderr, "key %s: values outside the float range: unexpected error %s\n", key, codes_get_error_message(err));
            ECCODES_ASSERT(!"unexpected error");
        }
        printf("  key %s has values outside the float range: %s\n", key, err ? "CODES_OUT_OF_RANGE" : "inf");
        return 0;
    }
    if (err != 0) {
        fprintf(stderr, "key %s: codes_get_float_array failed (%s) but codes_get_double_array works\n",
                key, codes_get_error_message(err));
        ECCODES_ASSERT(!"codes_get_float_array failed");
    }
    if (nf != nd) {
        fprintf(stderr, "key %s: %zu floats but %zu doubles\n", key, nf, nd);
        ECCODES_ASSERT(!"different number of values");
    }
    for (size_t i = 0; i < nd; ++i) {
        const float e = expected_float(dv[i]);
        bool ok = (fv[i] == e) || (std::isnan(fv[i]) && std::isnan(e));
        if (!ok && nd > 1 && dv[i] != CODES_MISSING_DOUBLE) {
            ok = std::fabs((double)fv[i] - dv[i]) <= 1e-5 * maxabs;
        }
        if (!ok) {
            fprintf(stderr, "key %s[%zu]: float=%.9g double=%.17g\n", key, i, fv[i], dv[i]);
            ECCODES_ASSERT(!"float and double values differ");
        }
    }

    // Second read: same result
    std::vector<float> fv2(n);
    size_t nf2 = n;
    ECCODES_ASSERT(codes_get_float_array(h, key, fv2.data(), &nf2) == 0);
    ECCODES_ASSERT(nf2 == nf);
    for (size_t i = 0; i < nf; ++i)
        ECCODES_ASSERT(fv2[i] == fv[i] || (std::isnan(fv2[i]) && std::isnan(fv[i])));

    // Larger buffer: same number of values
    std::vector<float> big(n + 5);
    size_t nb = n + 5;
    ECCODES_ASSERT(codes_get_float_array(h, key, big.data(), &nb) == 0);
    {
        // Same behaviour as the double version
        std::vector<double> dbig(n + 5);
        size_t ndb = n + 5;
        ECCODES_ASSERT(codes_get_double_array(h, key, dbig.data(), &ndb) == 0);
        // (some accessors do not update the length: then the double version returns the buffer size)
        if (nb != ndb && nb != nf) {
            fprintf(stderr, "key %s: larger buffer: %zu floats but %zu doubles (exact buffer: %zu)\n", key, nb, ndb, nf);
            ECCODES_ASSERT(!"different number of values with a larger buffer");
        }
    }

    // Scalar access
    if (nd == 1) {
        float f = 0;
        double d = 0;
        const int ed = codes_get_double(h, key, &d);
        const int ef = codes_get_float(h, key, &f);
        ECCODES_ASSERT(ed == ef);
        if (ed == 0) {
            const float e = expected_float(d);
            if (!(f == e || (std::isnan(f) && std::isnan(e)))) {
                fprintf(stderr, "key %s: codes_get_float=%.9g codes_get_double=%.17g\n", key, f, d);
                ECCODES_ASSERT(!"codes_get_float differs from codes_get_double");
            }
        }
    }
    return nd;
}

int main(int argc, char** argv)
{
    ECCODES_ASSERT(argc == 2);
    int err = 0;
    FILE* in = fopen(argv[1], "rb");
    ECCODES_ASSERT(in);
    codes_handle* h = NULL;
    int count = 0;
    size_t nkeys = 0, nvalues = 0;
    while (count < 3 && (h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL) {
        ++count;
        std::vector<std::string> names;
        codes_keys_iterator* kiter = codes_keys_iterator_new(h, 0, NULL);
        ECCODES_ASSERT(kiter);
        while (codes_keys_iterator_next(kiter))
            names.push_back(codes_keys_iterator_get_name(kiter));
        codes_keys_iterator_delete(kiter);
        for (const auto& name : names) {
            const size_t n = check_key(h, name.c_str());
            if (n) {
                ++nkeys;
                nvalues += n;
            }
        }
        codes_handle_delete(h);
    }
    fclose(in);
    ECCODES_ASSERT(count > 0);
    printf("%s: %d messages, %zu numeric keys, %zu values compared\n", argv[1], count, nkeys, nvalues);
    return 0;
}
