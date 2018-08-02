/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Apr 2015


#ifndef Grib_H
#define Grib_H

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

#include <grib_api.h>

template< typename T >
bool same_with_grib1_accuracy(const T& a, const T& b) {
    static const double GRIB1EPSILON = 0.001;
    return eckit::types::is_approximately_equal(a.value(), b.value(), GRIB1EPSILON);
}

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
    ~HandleDeleter() {
        grib_handle_delete(h_);
    }
};

#endif

