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
 * Description: an overview of the speed of the packing of "values" through the API:
 *              codes_get_double_array, codes_get_float_array (decode), codes_set_double_array and
 *              codes_set_float_array (encode), over all messages of a GRIB file held in memory.
 *              Each call runs over all messages three times; the fastest run counts (warm caches).
 *              The implementation is selected by the environment (e.g. ECCODES_USE_SPAEC,
 *              ECCODES_SPAEC_SELECTOR, ECCODES_USE_SP); tests/benchmarks/packing_api.py runs the
 *              variants and prints the tables of the READMEs of projects/spaec and
 *              projects/simple_packing.
 *              usage: packing_api <in.grib> [<out_prefix>]
 *              With out_prefix, the messages encoded from doubles and from floats are written to
 *              <out_prefix>.d.grib and <out_prefix>.f.grib (to compare implementations).
 *              Output per call: "<call> values=<n> seconds=<t> MiB/s=<r> Mvalues/s=<r>".
 */
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "eccodes.h"

using Clock = std::chrono::steady_clock;

static std::vector<std::vector<unsigned char>> readMessages(const char* path)
{
    std::vector<std::vector<unsigned char>> messages;
    FILE* f = std::fopen(path, "rb");
    if (!f) {
        return messages;
    }
    int err = 0;
    while (codes_handle* h = codes_handle_new_from_file(nullptr, f, PRODUCT_GRIB, &err)) {
        const void* m = nullptr;
        size_t size = 0;
        codes_get_message(h, &m, &size);
        messages.emplace_back(static_cast<const unsigned char*>(m), static_cast<const unsigned char*>(m) + size);
        codes_handle_delete(h);
    }
    std::fclose(f);
    return messages;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <in.grib> [<out_prefix>]\n", argv[0]);
        return 2;
    }
    const std::vector<std::vector<unsigned char>> messages = readMessages(argv[1]);
    if (messages.empty()) {
        std::fprintf(stderr, "%s: no GRIB messages\n", argv[1]);
        return 2;
    }
    const std::string prefix = argc > 2 ? argv[2] : "";

    // The values of every message, decoded once (not timed): the input of the encoding
    std::vector<std::vector<double>> doubles(messages.size());
    std::vector<std::vector<float>> floats(messages.size());
    size_t total = 0;
    for (size_t i = 0; i < messages.size(); ++i) {
        codes_handle* h = codes_handle_new_from_message(nullptr, messages[i].data(), messages[i].size());
        size_t n = 0;
        codes_get_size(h, "values", &n);
        doubles[i].resize(n);
        codes_get_double_array(h, "values", doubles[i].data(), &n);
        floats[i].assign(doubles[i].begin(), doubles[i].end());
        total += n;
        codes_handle_delete(h);
    }

    const char* names[4] = { "codes_get_double_array", "codes_get_float_array", "codes_set_double_array", "codes_set_float_array" };
    const size_t bytes[4] = { sizeof(double), sizeof(float), sizeof(double), sizeof(float) };
    for (int call = 0; call < 4; ++call) {
        double best = 1e30;
        for (int run = 0; run < 3; ++run) {
            const bool write = run == 0 && call >= 2 && !prefix.empty();
            FILE* out = write ? std::fopen((prefix + (call == 2 ? ".d.grib" : ".f.grib")).c_str(), "wb") : nullptr;
            double seconds = 0;
            std::vector<double> d;
            std::vector<float> f;
            for (size_t i = 0; i < messages.size(); ++i) {
                // decoding reads the message, encoding changes a copy of it
                codes_handle* h = call < 2 ? codes_handle_new_from_message(nullptr, messages[i].data(), messages[i].size())
                                           : codes_handle_new_from_message_copy(nullptr, messages[i].data(), messages[i].size());
                size_t n = doubles[i].size();
                d.resize(n);
                f.resize(n);
                const auto t0 = Clock::now();
                int err = 0;
                switch (call) {
                    case 0:
                        err = codes_get_double_array(h, "values", d.data(), &n);
                        break;
                    case 1:
                        err = codes_get_float_array(h, "values", f.data(), &n);
                        break;
                    case 2:
                        err = codes_set_double_array(h, "values", doubles[i].data(), n);
                        break;
                    default:
                        err = codes_set_float_array(h, "values", floats[i].data(), n);
                        break;
                }
                seconds += std::chrono::duration<double>(Clock::now() - t0).count();
                if (err != 0) {
                    std::fprintf(stderr, "%s: message %zu: %s\n", names[call], i + 1, codes_get_error_message(err));
                    return 1;
                }
                if (out) {
                    const void* m = nullptr;
                    size_t size = 0;
                    codes_get_message(h, &m, &size);
                    if (std::fwrite(m, 1, size, out) != size) {
                        std::fprintf(stderr, "%s: cannot write the message %zu\n", names[call], i + 1);
                        return 1;
                    }
                }
                codes_handle_delete(h);
            }
            if (out) {
                std::fclose(out);
            }
            best = std::min(best, seconds);
        }
        std::printf("%s values=%zu seconds=%.6f MiB/s=%.0f Mvalues/s=%.0f\n", names[call], total, best,
                    static_cast<double>(total * bytes[call]) / 1048576.0 / best, static_cast<double>(total) / best / 1e6);
    }
    return 0;
}
