/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eccodes/geo/Geo.h"

#include <string>

#include "eckit/exception/Exceptions.h"


namespace eccodes::geo
{


bool codes_check_error(int e, const char* call)
{
    if (e != CODES_SUCCESS) {
        throw ::eckit::SeriousBug(std::string(call) + ": " + codes_get_error_message(e));
    }

    return true;
}


void codes_assertion(const char* message)
{
    throw ::eckit::SeriousBug(message);
}


}  // namespace eccodes::geo
