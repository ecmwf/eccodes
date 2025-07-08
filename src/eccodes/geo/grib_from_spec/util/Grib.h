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


bool grib_call(int e, const char* call, bool NOT_FOUND_IS_OK = false);


#define GRIB_CALL(a) grib_call(a, #a)
#define GRIB_GET(a) grib_call(a, #a, true)
#define GRIB_ERROR(a, b) grib_call(a, b)


struct grib_info {
    grib_info();
    grib_info(const grib_info&) = delete;
    grib_info(grib_info&&)      = delete;

    ~grib_info() = default;

    void operator=(grib_info&&)      = delete;
    void operator=(const grib_info&) = delete;

    void extra_set(const char* key, long);
    void extra_set(const char* key, double);
    void extra_set(const char* key, const char*);

    codes_util_grid_spec grid;
    codes_util_packing_spec packing;

private:
    std::vector<std::string> strings_;
    const size_t extra_settings_size_;
};


struct HandleDeleter {
    explicit HandleDeleter(grib_handle*);

    HandleDeleter(const HandleDeleter&) = delete;
    HandleDeleter(HandleDeleter&&)      = delete;

    ~HandleDeleter() { codes_handle_delete(h_); }

    void operator=(const HandleDeleter&) = delete;
    void operator=(HandleDeleter&&)      = delete;

private:
    grib_handle* h_;
};


struct GKeyIteratorDeleter {
    explicit GKeyIteratorDeleter(codes_keys_iterator* h) : h_(h) {}

    GKeyIteratorDeleter(const GKeyIteratorDeleter&) = delete;
    GKeyIteratorDeleter(GKeyIteratorDeleter&&)      = delete;

    ~GKeyIteratorDeleter() { codes_keys_iterator_delete(h_); }

    void operator=(GKeyIteratorDeleter&&)      = delete;
    void operator=(const GKeyIteratorDeleter&) = delete;

private:
    codes_keys_iterator* h_;
};


struct BKeyIteratorDeleter {
    explicit BKeyIteratorDeleter(codes_bufr_keys_iterator* h) : h_(h) {}

    BKeyIteratorDeleter(const BKeyIteratorDeleter&) = delete;
    BKeyIteratorDeleter(BKeyIteratorDeleter&&)      = delete;

    ~BKeyIteratorDeleter() { codes_bufr_keys_iterator_delete(h_); }

    void operator=(BKeyIteratorDeleter&&)      = delete;
    void operator=(const BKeyIteratorDeleter&) = delete;

private:
    codes_bufr_keys_iterator* h_;
};


void grib_reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj);


void grib_get_unique_missing_value(const std::vector<double>& values, double& missingValue);


bool grib_check_is_message_valid();
