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


#pragma once

#include "eccodes.h"


namespace eckit::geo
{
class Spec;
}


namespace eccodes::geo
{


using Spec = eckit::geo::Spec;


[[noreturn]] bool codes_check_error(int e, const char* call);
[[noreturn]] void codes_assertion(const char* message);


#define CHECK_ERROR(a, b) codes_check_error(a, b)
#define CHECK_CALL(a)     codes_check_error(a, #a)


}  // namespace eccodes::geo
