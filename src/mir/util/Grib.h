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


#ifndef mir_util_Grib_h
#define mir_util_Grib_h

#include <eccodes.h>

#include <vector>

#include "eckit/exception/Exceptions.h"


inline bool grib_call(int e, const char *call, bool missingOK = false) {
    if (e) {
        if(missingOK && (e == GRIB_NOT_FOUND)) {
            return false;
        }

        std::ostringstream os;
        os << call << ": " << grib_get_error_message(e);
        throw eckit::SeriousBug(os.str());
    }
    return true;
}

#undef GRIB_CALL
#define GRIB_CALL(a) grib_call(a, #a)
#define GRIB_ERROR(a, b) grib_call(a, b)

#define GRIB_GET(a) grib_call(a, #a, true)


struct grib_info {
    grib_util_grid_spec grid;
    grib_util_packing_spec packing;
};


class HandleDeleter {
    grib_handle *h_;
public:
    HandleDeleter(grib_handle *h) : h_(h) {}
    HandleDeleter(const HandleDeleter&) = delete;
    void operator=(const HandleDeleter&) = delete;
    ~HandleDeleter() { grib_handle_delete(h_); }
};


class GKeyIteratorDeleter {
    grib_keys_iterator *h_;
public:
    GKeyIteratorDeleter(grib_keys_iterator* h) : h_(h) {}
    GKeyIteratorDeleter(const GKeyIteratorDeleter&) = delete;
    void operator=(const GKeyIteratorDeleter&) = delete;
    ~GKeyIteratorDeleter() { grib_keys_iterator_delete(h_); }
};


class BKeyIteratorDeleter {
    bufr_keys_iterator *h_;
public:
    BKeyIteratorDeleter(bufr_keys_iterator* h) : h_(h) {}
    BKeyIteratorDeleter(const BKeyIteratorDeleter&) = delete;
    void operator=(const BKeyIteratorDeleter&) = delete;
    ~BKeyIteratorDeleter() { codes_bufr_keys_iterator_delete(h_); }
};


struct GribReorder {
    enum
    {
        iScansNegatively      = 1 << 7,
        jScansPositively      = 1 << 6,
        jPointsAreConsecutive = 1 << 5,
        alternateRowScanning  = 1 << 4
    };

    static void reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj);
};


struct GribExtraSetting {
    static void set(grib_info&, const char* key, long);
    static void set(grib_info&, const char* key, double);
};


#endif
