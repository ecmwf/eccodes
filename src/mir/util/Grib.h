/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <eccodes.h>

#include <sstream>
#include <string>
#include <vector>

#include "mir/util/Exceptions.h"


inline bool grib_call(int e, const char* call, bool NOT_FOUND_IS_OK = false) {
    if (static_cast<bool>(e)) {
        if (NOT_FOUND_IS_OK && (e == CODES_NOT_FOUND)) {
            return false;
        }

        std::ostringstream os;
        os << call << ": " << codes_get_error_message(e);
        throw mir::exception::SeriousBug(os.str());
    }
    return true;
}


#define GRIB_CALL(a) grib_call(a, #a)
#define GRIB_GET(a) grib_call(a, #a, true)
#define GRIB_ERROR(a, b) grib_call(a, b)


struct grib_info {
    grib_info();

    void extra_set(const char* key, long);
    void extra_set(const char* key, double);
    void extra_set(const char* key, const char*);

    codes_util_grid_spec grid;
    codes_util_packing_spec packing;

private:
    grib_info(const grib_info&)      = delete;
    void operator=(const grib_info&) = delete;

    std::vector<std::string> strings_;
    const size_t extra_settings_size_;
};


class HandleDeleter {
    grib_handle* h_;

public:
    HandleDeleter(grib_handle* h) : h_(h) { ASSERT(h); }
    HandleDeleter(const HandleDeleter&)  = delete;
    void operator=(const HandleDeleter&) = delete;
    ~HandleDeleter() { codes_handle_delete(h_); }
};


class GKeyIteratorDeleter {
    codes_keys_iterator* h_;

public:
    GKeyIteratorDeleter(codes_keys_iterator* h) : h_(h) {}
    GKeyIteratorDeleter(const GKeyIteratorDeleter&) = delete;
    void operator=(const GKeyIteratorDeleter&)      = delete;
    ~GKeyIteratorDeleter() { codes_keys_iterator_delete(h_); }
};


class BKeyIteratorDeleter {
    codes_bufr_keys_iterator* h_;

public:
    BKeyIteratorDeleter(codes_bufr_keys_iterator* h) : h_(h) {}
    BKeyIteratorDeleter(const BKeyIteratorDeleter&) = delete;
    void operator=(const BKeyIteratorDeleter&)      = delete;
    ~BKeyIteratorDeleter() { codes_bufr_keys_iterator_delete(h_); }
};


void grib_reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj);


void grib_get_unique_missing_value(const std::vector<double>& values, double& missingValue);
