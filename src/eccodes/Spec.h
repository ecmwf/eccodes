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

// complete by including eckit/spec/Spec.h (of eckit, as ecCodes is built with eckit::geo)
namespace eckit::spec
{
class Spec;
}


namespace eccodes
{


using Spec = ::eckit::spec::Spec;


}  // namespace eccodes


// C++ only: a key of a spec (e.g. gridSpec, as eckit::geo::Grid::spec) set or got as a spec, not as its string
int codes_set_spec(codes_handle* h, const char* key, const eccodes::Spec& spec);

// The spec is of the handle, valid until the key is got again (as a spec), the message is changed or the handle deleted;
// throws on error (e.g. the key is not of a spec, or ecCodes isn't built with eckit::geo)
const eccodes::Spec& codes_get_spec(const codes_handle* h, const char* key);
